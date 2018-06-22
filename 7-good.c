#include "7-fake.h"

static int
adfs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct adfs_discrecord *dr;
    struct buffer_head *bh;
    struct object_info root_obj;
    unsigned char  *b_data;
    struct adfs_sb_info *asb;
    struct inode   *root;
    int             ret = -EINVAL;

    sb->s_flags |= MS_NODIRATIME;

    asb = kzalloc(sizeof(*asb), GFP_KERNEL);
    if (!asb)
	return -ENOMEM;
    sb->s_fs_info = asb;

    /*
     * set default options 
     */
    asb->s_uid = GLOBAL_ROOT_UID;
    asb->s_gid = GLOBAL_ROOT_GID;
    asb->s_owner_mask = ADFS_DEFAULT_OWNER_MASK;
    asb->s_other_mask = ADFS_DEFAULT_OTHER_MASK;
    asb->s_ftsuffix = 0;

    if (parse_options(sb, data))
	goto error;

    sb_set_blocksize(sb, BLOCK_SIZE);
    if (!(bh = sb_bread(sb, ADFS_DISCRECORD / BLOCK_SIZE))) {
	adfs_error(sb, "unable to read superblock");
	ret = -EIO;
	goto error;
    }

    b_data = bh->b_data + (ADFS_DISCRECORD % BLOCK_SIZE);

    if (adfs_checkbblk(b_data)) {
	if (!silent)
	    printk("VFS: Can't find an adfs filesystem on dev "
		   "%s.\n", sb->s_id);
	ret = -EINVAL;
	goto error_free_bh;
    }

    dr = (struct adfs_discrecord *) (b_data + ADFS_DR_OFFSET);

    /*
     * Do some sanity checks on the ADFS disc record
     */
    if (adfs_checkdiscrecord(dr)) {
	if (!silent)
	    printk("VPS: Can't find an adfs filesystem on dev "
		   "%s.\n", sb->s_id);
	ret = -EINVAL;
	goto error_free_bh;
    }

    brelse(bh);
    if (sb_set_blocksize(sb, 1 << dr->log2secsize)) {
	bh = sb_bread(sb, ADFS_DISCRECORD / sb->s_blocksize);
	if (!bh) {
	    adfs_error(sb, "couldn't read superblock on " "2nd try.");
	    ret = -EIO;
	    goto error;
	}
	b_data = bh->b_data + (ADFS_DISCRECORD % sb->s_blocksize);
	if (adfs_checkbblk(b_data)) {
	    adfs_error(sb, "disc record mismatch, very weird!");
	    ret = -EINVAL;
	    goto error_free_bh;
	}
	dr = (struct adfs_discrecord *) (b_data + ADFS_DR_OFFSET);
    } else {
	if (!silent)
	    printk(KERN_ERR "VFS: Unsupported blocksize on dev "
		   "%s.\n", sb->s_id);
	ret = -EINVAL;
	goto error;
    }

    /*
     * blocksize on this device should now be set to the ADFS log2secsize
     */

    sb->s_magic = ADFS_SUPER_MAGIC;
    asb->s_idlen = dr->idlen;
    asb->s_map_size = dr->nzones | (dr->nzones_high << 8);
    asb->s_map2blk = dr->log2bpmb - dr->log2secsize;
    asb->s_size = adfs_discsize(dr, sb->s_blocksize_bits);
    asb->s_version = dr->format_version;
    asb->s_log2sharesize = dr->log2sharesize;

    asb->s_map = adfs_read_map(sb, dr);
    if (IS_ERR(asb->s_map)) {
	ret = PTR_ERR(asb->s_map);
	goto error_free_bh;
    }

    brelse(bh);

    /*
     * set up enough so that we can read an inode
     */
    sb->s_op = &adfs_sops;

    dr = (struct adfs_discrecord *) (asb->s_map[0].dm_bh->b_data + 4);

    root_obj.parent_id = root_obj.file_id = le32_to_cpu(dr->root);
    root_obj.name_len = 0;
    /*
     * Set root object date as 01 Jan 1987 00:00:00 
     */
    root_obj.loadaddr = 0xfff0003f;
    root_obj.execaddr = 0xec22c000;
    root_obj.size = ADFS_NEWDIR_SIZE;
    root_obj.attr = ADFS_NDA_DIRECTORY | ADFS_NDA_OWNER_READ |
	ADFS_NDA_OWNER_WRITE | ADFS_NDA_PUBLIC_READ;
    root_obj.filetype = -1;

    /*
     * If this is a F+ disk with variable length directories,
     * get the root_size from the disc record.
     */
    if (asb->s_version) {
	root_obj.size = le32_to_cpu(dr->root_size);
	asb->s_dir = &adfs_fplus_dir_ops;
	asb->s_namelen = ADFS_FPLUS_NAME_LEN;
    } else {
	asb->s_dir = &adfs_f_dir_ops;
	asb->s_namelen = ADFS_F_NAME_LEN;
    }
    /*
     * ,xyz hex filetype suffix may be added by driver
     * to files that have valid RISC OS filetype
     */
    if (asb->s_ftsuffix)
	asb->s_namelen += 4;

    sb->s_d_op = &adfs_dentry_operations;
    root = adfs_iget(sb, &root_obj);
    sb->s_root = d_make_root(root);
    if (!sb->s_root) {
	int             i;
	for (i = 0; i < asb->s_map_size; i++)
	    brelse(asb->s_map[i].dm_bh);
	kfree(asb->s_map);
	adfs_error(sb, "get root inode failed\n");
	ret = -EIO;
	goto error;
    }
    return 0;

  error_free_bh:
    brelse(bh);
  error:
    sb->s_fs_info = NULL;
    kfree(asb);
    return ret;
}
