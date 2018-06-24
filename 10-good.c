#include "10-fake.h"

static int
hfs_create(struct inode *dir, struct dentry *dentry, umode_t mode,
	   bool excl)
{
    struct inode   *inode;
    int             res;

    inode = hfs_new_inode(dir, &dentry->d_name, mode);
    if (!inode)
	return -ENOMEM;

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
