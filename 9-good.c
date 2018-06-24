#include "9-fake.h"

static int
affs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct affs_sb_info *sbi;
    struct buffer_head *root_bh = NULL;
    struct buffer_head *boot_bh;
    struct inode   *root_inode = NULL;
    s32             root_block;
    int             size,
                    blocksize;
    u32             chksum;
    int             num_bm;
    int             i,
                    j;
    kuid_t          uid;
    kgid_t          gid;
    int             reserved;
    unsigned long   mount_flags;
    int             tmp_flags;	/* fix remount prototype... */
    u8              sig[4];
    int             ret;

    save_mount_options(sb, data);

    pr_debug("read_super(%s)\n",
	     data ? (const char *) data : "no options");

    sb->s_magic = AFFS_SUPER_MAGIC;
    sb->s_op = &affs_sops;
    sb->s_flags |= MS_NODIRATIME;

    sbi = kzalloc(sizeof(struct affs_sb_info), GFP_KERNEL);
    if (!sbi)
	return -ENOMEM;

    sb->s_fs_info = sbi;
    sbi->sb = sb;
    mutex_init(&sbi->s_bmlock);
    spin_lock_init(&sbi->symlink_lock);
    spin_lock_init(&sbi->work_lock);
    INIT_DELAYED_WORK(&sbi->sb_work, flush_superblock);

    if (!parse_options(data, &uid, &gid, &i, &reserved, &root_block,
		       &blocksize, &sbi->s_prefix,
		       sbi->s_volume, &mount_flags)) {
	pr_err("Error parsing options\n");
	return -EINVAL;
    }
    /*
     * N.B. after this point s_prefix must be released 
     */

    sbi->s_flags = mount_flags;
    sbi->s_mode = i;
    sbi->s_uid = uid;
    sbi->s_gid = gid;
    sbi->s_reserved = reserved;

    /*
     * Get the size of the device in 512-byte blocks. If we later see that 
     * the partition uses bigger blocks, we will have to change it. 
     */

    size = sb->s_bdev->bd_inode->i_size >> 9;
    pr_debug("initial blocksize=%d, #blocks=%d\n", 512, size);

    affs_set_blocksize(sb, PAGE_SIZE);
    /*
     * Try to find root block. Its location depends on the block size. 
     */

    i = 512;
    j = 4096;
    if (blocksize > 0) {
	i = j = blocksize;
	size = size / (blocksize / 512);
    }
    for (blocksize = i; blocksize <= j; blocksize <<= 1, size >>= 1) {
	sbi->s_root_block = root_block;
	if (root_block < 0)
	    sbi->s_root_block = (reserved + size - 1) / 2;
	pr_debug("setting blocksize to %d\n", blocksize);
	affs_set_blocksize(sb, blocksize);
	sbi->s_partition_size = size;

	/*
	 * The root block location that was calculated above is not
	 * correct if the partition size is an odd number of 512- byte
	 * blocks, which will be rounded down to a number of 1024-byte
	 * blocks, and if there were an even number of reserved blocks.
	 * Ideally, all partition checkers should report the real number
	 * of blocks of the real blocksize, but since this just cannot be
	 * done, we have to try to find the root block anyways. In the
	 * above case, it is one block behind the calculated one. So we
	 * check this one, too. 
	 */
	for (num_bm = 0; num_bm < 2; num_bm++) {
	    pr_debug("Dev %s, trying root=%u, bs=%d, "
		     "size=%d, reserved=%d\n",
		     sb->s_id,
		     sbi->s_root_block + num_bm,
		     blocksize, size, reserved);
	    root_bh = affs_bread(sb, sbi->s_root_block + num_bm);
	    if (!root_bh)
		continue;
	    if (!affs_checksum_block(sb, root_bh) &&
		be32_to_cpu(AFFS_ROOT_HEAD(root_bh)->ptype) == T_SHORT &&
		be32_to_cpu(AFFS_ROOT_TAIL(sb, root_bh)->stype) ==
		ST_ROOT) {
		sbi->s_hashsize = blocksize / 4 - 56;
		sbi->s_root_block += num_bm;
		goto got_root;
	    }
	    affs_brelse(root_bh);
	    root_bh = NULL;
	}
    }
    if (!silent)
	pr_err("No valid root block on device %s\n", sb->s_id);
    return -EINVAL;

    /*
     * N.B. after this point bh must be released 
     */
  got_root:
    /*
     * Keep super block in cache 
     */
    sbi->s_root_bh = root_bh;
    root_block = sbi->s_root_block;

    /*
     * Find out which kind of FS we have 
     */
    boot_bh = sb_bread(sb, 0);
    if (!boot_bh) {
	pr_err("Cannot read boot block\n");
	return -EINVAL;
    }
    memcpy(sig, boot_bh->b_data, 4);
    brelse(boot_bh);
    chksum = be32_to_cpu(*(__be32 *) sig);

    /*
     * Dircache filesystems are compatible with non-dircache ones when
     * reading. As long as they aren't supported, writing is not
     * recommended. 
     */
    if ((chksum == FS_DCFFS || chksum == MUFS_DCFFS || chksum == FS_DCOFS
	 || chksum == MUFS_DCOFS) && !(sb->s_flags & MS_RDONLY)) {
	pr_notice("Dircache FS - mounting %s read only\n", sb->s_id);
	sb->s_flags |= MS_RDONLY;
    }
    switch (chksum) {
    case MUFS_FS:
    case MUFS_INTLFFS:
    case MUFS_DCFFS:
	affs_set_opt(sbi->s_flags, SF_MUFS);
	/*
	 * fall thru 
	 */
    case FS_INTLFFS:
    case FS_DCFFS:
	affs_set_opt(sbi->s_flags, SF_INTL);
	break;
    case MUFS_FFS:
	affs_set_opt(sbi->s_flags, SF_MUFS);
	break;
    case FS_FFS:
	break;
    case MUFS_OFS:
	affs_set_opt(sbi->s_flags, SF_MUFS);
	/*
	 * fall thru 
	 */
    case FS_OFS:
	affs_set_opt(sbi->s_flags, SF_OFS);
	sb->s_flags |= MS_NOEXEC;
	break;
    case MUFS_DCOFS:
    case MUFS_INTLOFS:
	affs_set_opt(sbi->s_flags, SF_MUFS);
    case FS_DCOFS:
    case FS_INTLOFS:
	affs_set_opt(sbi->s_flags, SF_INTL);
	affs_set_opt(sbi->s_flags, SF_OFS);
	sb->s_flags |= MS_NOEXEC;
	break;
    default:
	pr_err("Unknown filesystem on device %s: %08X\n",
	       sb->s_id, chksum);
	return -EINVAL;
    }

    if (affs_test_opt(mount_flags, SF_VERBOSE)) {
	u8              len = AFFS_ROOT_TAIL(sb, root_bh)->disk_name[0];
	pr_notice
	    ("Mounting volume \"%.*s\": Type=%.3s\\%c, Blocksize=%d\n",
	     len > 31 ? 31 : len, AFFS_ROOT_TAIL(sb,
						 root_bh)->disk_name + 1,
	     sig, sig[3] + '0', blocksize);
    }

    sb->s_flags |= MS_NODEV | MS_NOSUID;

    sbi->s_data_blksize = sb->s_blocksize;
    if (affs_test_opt(sbi->s_flags, SF_OFS))
	sbi->s_data_blksize -= 24;

    tmp_flags = sb->s_flags;
    ret = affs_init_bitmap(sb, &tmp_flags);
    if (ret)
	return ret;
    sb->s_flags = tmp_flags;

    /*
     * set up enough so that it can read an inode 
     */

    root_inode = affs_iget(sb, root_block);
    if (IS_ERR(root_inode))
	return PTR_ERR(root_inode);

    if (affs_test_opt(AFFS_SB(sb)->s_flags, SF_INTL))
	sb->s_d_op = &affs_intl_dentry_operations;
    else
	sb->s_d_op = &affs_dentry_operations;

    sb->s_root = d_make_root(root_inode);
    if (!sb->s_root) {
	pr_err("AFFS: Get root inode failed\n");
	return -ENOMEM;
    }

    pr_debug("s_flags=%lX\n", sb->s_flags);
    return 0;
}
