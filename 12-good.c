#include "12-fake.h"

static int
hfsplus_symlink(struct inode *dir, struct dentry *dentry,
		const char *symname)
{
    struct hfsplus_sb_info *sbi = HFSPLUS_SB(dir->i_sb);
    struct inode   *inode;
    int             res = -ENOMEM;

    mutex_lock(&sbi->vh_mutex);
    inode = hfsplus_new_inode(dir->i_sb, S_IFLNK | S_IRWXUGO);
    if (!inode)
	goto out;

    res = page_symlink(inode, symname, strlen(symname) + 1);
    if (res)
	goto out_err;

    res = hfsplus_create_cat(inode->i_ino, dir, &dentry->d_name, inode);
    if (res)
	goto out_err;

    res = hfsplus_init_inode_security(inode, dir, &dentry->d_name);
    if (res == -EOPNOTSUPP)
	res = 0;		/* Operation is not supported. */
    else if (res) {
	/*
	 * Try to delete anyway without error analysis. 
	 */
	hfsplus_delete_cat(inode->i_ino, dir, &dentry->d_name);
	goto out_err;
    }

    hfsplus_instantiate(dentry, inode, inode->i_ino);
    mark_inode_dirty(inode);
    goto out;

  out_err:
    clear_nlink(inode);
    hfsplus_delete_inode(inode);
    iput(inode);
  out:
    mutex_unlock(&sbi->vh_mutex);
    return res;
}
