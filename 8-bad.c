#include "8-fake.h"

static int
bfs_create(struct inode *dir, struct dentry *dentry, umode_t mode,
	   bool excl)
{
    int             err;
    struct inode   *inode;
    struct super_block *s = dir->i_sb;
    struct bfs_sb_info *info = BFS_SB(s);
    unsigned long   ino;

    inode = new_inode(s);
    if (!inode)
	return -ENOSPC;
    mutex_lock(&info->bfs_lock);
    ino = find_first_zero_bit(info->si_imap, info->si_lasti + 1);
    if (ino > info->si_lasti) {
	mutex_unlock(&info->bfs_lock);
	iput(inode);
	return -ENOSPC;
    }
    set_bit(ino, info->si_imap);
    info->si_freei--;
    inode_init_owner(inode, dir, mode);
    inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME_SEC;
    inode->i_blocks = 0;
    inode->i_op = &bfs_file_inops;
    inode->i_fop = &bfs_file_operations;
    inode->i_mapping->a_ops = &bfs_aops;
    inode->i_ino = ino;
    BFS_I(inode)->i_dsk_ino = ino;
    BFS_I(inode)->i_sblock = 0;
    BFS_I(inode)->i_eblock = 0;
    insert_inode_hash(inode);
    mark_inode_dirty(inode);
    bfs_dump_imap("create", s);

    err = bfs_add_entry(dir, dentry->d_name.name, dentry->d_name.len,
			inode->i_ino);
    if (err) {
	inode_dec_link_count(inode);
	mutex_unlock(&info->bfs_lock);
	iput(inode);
	return err;
    }
    mutex_unlock(&info->bfs_lock);
    d_instantiate(dentry, inode);
    return 0;
}
