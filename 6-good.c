#include "6-fake.h"

static int
adfs_fplus_read(struct super_block *sb, unsigned int id, unsigned int sz,
		struct adfs_dir *dir)
{
    struct adfs_bigdirheader *h;
    struct adfs_bigdirtail *t;
    unsigned long   block;
    unsigned int    blk,
                    size;
    int             i,
                    ret = -EIO;

    dir->nr_buffers = 0;

    /*
     * start off using fixed bh set - only alloc for big dirs 
     */
    dir->bh_fplus = &dir->bh[0];

    block = __adfs_block_map(sb, id, 0);
    if (!block) {
	adfs_error(sb, "dir object %X has a hole at offset 0", id);
	goto out;
    }

    dir->bh_fplus[0] = sb_bread(sb, block);
    if (!dir->bh_fplus[0])
	goto out;
    dir->nr_buffers += 1;

    h = (struct adfs_bigdirheader *) dir->bh_fplus[0]->b_data;
    size = le32_to_cpu(h->bigdirsize);
    if (size != sz) {
	printk(KERN_WARNING "adfs: adfs_fplus_read:"
	       " directory header size %X\n"
	       " does not match directory size %X\n", size, sz);
    }

    if (h->bigdirversion[0] != 0 || h->bigdirversion[1] != 0 ||
	h->bigdirversion[2] != 0 || size & 2047 ||
	h->bigdirstartname != cpu_to_le32(BIGDIRSTARTNAME)) {
	printk(KERN_WARNING "adfs: dir object %X has"
	       " malformed dir header\n", id);
	goto out;
    }

    size >>= sb->s_blocksize_bits;
    if (size > ARRAY_SIZE(dir->bh)) {
	/*
	 * this directory is too big for fixed bh set, must allocate 
	 */
	struct buffer_head **bh_fplus =
	    kcalloc(size, sizeof(struct buffer_head *),
		    GFP_KERNEL);
	if (!bh_fplus) {
	    ret = -ENOMEM;
	    adfs_error(sb, "not enough memory for"
		       " dir object %X (%d blocks)", id, size);
	    goto out;
	}
	dir->bh_fplus = bh_fplus;
	/*
	 * copy over the pointer to the block that we've already read 
	 */
	dir->bh_fplus[0] = dir->bh[0];
    }

    for (blk = 1; blk < size; blk++) {
	block = __adfs_block_map(sb, id, blk);
	if (!block) {
	    adfs_error(sb, "dir object %X has a hole at offset %d", id,
		       blk);
	    goto out;
	}

	dir->bh_fplus[blk] = sb_bread(sb, block);
	if (!dir->bh_fplus[blk]) {
	    adfs_error(sb,
		       "dir object %x failed read for offset %d, mapped block %lX",
		       id, blk, block);
	    goto out;
	}

	dir->nr_buffers += 1;
    }

    t = (struct adfs_bigdirtail *)
	(dir->bh_fplus[size - 1]->b_data + (sb->s_blocksize - 8));

    if (t->bigdirendname != cpu_to_le32(BIGDIRENDNAME) ||
	t->bigdirendmasseq != h->startmasseq ||
	t->reserved[0] != 0 || t->reserved[1] != 0) {
	printk(KERN_WARNING "adfs: dir object %X has "
	       "malformed dir end\n", id);
	goto out;
    }

    dir->parent_id = le32_to_cpu(h->bigdirparent);
    dir->sb = sb;
    return 0;

  out:
    if (dir->bh_fplus) {
	for (i = 0; i < dir->nr_buffers; i++)
	    brelse(dir->bh_fplus[i]);

	if (&dir->bh[0] != dir->bh_fplus)
	    kfree(dir->bh_fplus);

	dir->bh_fplus = NULL;
    }

    dir->nr_buffers = 0;
    dir->sb = NULL;
    return ret;
}
