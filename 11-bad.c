#include "11-fake.h"

static int
hfs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode)
{
    struct inode   *inode;
    int             res;

    inode = hfs_new_inode(dir, &dentry->d_name, S_IFDIR | mode);
    if (!inode)
	return -ENOSPC;

    res = hfs_cat_create(inode->i_ino, dir, &dentry->d_name, inode);
    if (res) {
	clear_nlink(inode);
	hfs_delete_inode(inode);
	iput(inode);
	return res;
    }
    d_instantiate(dentry, inode);
    mark_inode_dirty(inode);
    return 0;
}
