#include "13-fake.h"

static int
hfsplus_mknod(struct inode *dir, struct dentry *dentry,
	      umode_t mode, dev_t rdev)
{
    struct hfsplus_sb_info *sbi = HFSPLUS_SB(dir->i_sb);
    struct inode   *inode;
    int             res = -ENOSPC;

    mutex_lock(&sbi->vh_mutex);
    inode = hfsplus_new_inode(dir->i_sb, mode);
    if (!inode)
	goto out;

    if (S_ISBLK(mode) || S_ISCHR(mode) || S_ISFIFO(mode) || S_ISSOCK(mode))
	init_special_inode(inode, mode, rdev);

    res = hfsplus_create_cat(inode->i_ino, dir, &dentry->d_name, inode);
    if (res)
	goto failed_mknod;

    res = hfsplus_init_inode_security(inode, dir, &dentry->d_name);
    if (res == -EOPNOTSUPP)
	res = 0;		/* Operation is not supported. */
    else if (res) {
	/*
	 * Try to delete anyway without error analysis. 
	 */
	hfsplus_delete_cat(inode->i_ino, dir, &dentry->d_name);
	goto failed_mknod;
    }

    hfsplus_instantiate(dentry, inode, inode->i_ino);
    mark_inode_dirty(inode);
    goto out;

  failed_mknod:
    clear_nlink(inode);
    hfsplus_delete_inode(inode);
    iput(inode);
  out:
    mutex_unlock(&sbi->vh_mutex);
    return res;
}
