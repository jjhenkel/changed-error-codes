#include "common.h"

#define HASH_LEN_DECLARE u32 hash; u32 len
#define bytemask_from_count(cnt)	(~(~0ul << (cnt)*8))

struct qstr {
	union {
		struct {
			HASH_LEN_DECLARE;
		};
		u64 hash_len;
	};
	const unsigned char *name;
};

struct dentry_operations {};
struct dentry {
  struct qstr d_name;
};
struct super_operations {};

struct super_block {
	unsigned char		s_blocksize_bits;
	unsigned long		s_blocksize;
	const struct super_operations	*s_op;
  	char s_id[32];				/* Informational name */
	u8 s_uuid[16];				/* UUID */
  void 			*s_fs_info;	/* Filesystem private info */
	unsigned long		s_flags;
	unsigned long		s_iflags;	/* internal SB_I_* flags */
	unsigned long		s_magic;
	struct dentry		*s_root;
  const struct dentry_operations *s_d_op; /* default d_op for dentries */
};

struct mutex {};
struct inode_operations {};
struct file_operations {};

struct bfs_sb_info {
	unsigned long si_blocks;
	unsigned long si_freeb;
	unsigned long si_freei;
	unsigned long si_lf_eblk;
	unsigned long si_lasti;
	unsigned long *si_imap;
	struct mutex bfs_lock;
};

struct address_space_operations {};

struct address_space {
  const struct address_space_operations *a_ops;	/* methods */
};

struct inode {
	umode_t			i_mode;
	unsigned short		i_opflags;
	uid_t			i_uid;
	gid_t			i_gid;
	unsigned int		i_flags;
  unsigned long		i_ino;
	unsigned short          i_bytes;
	unsigned int		i_blkbits;
	blkcnt_t		i_blocks;
  struct super_block	*i_sb;
  struct timespec		i_atime;
	struct timespec		i_mtime;
	struct timespec		i_ctime;	
  const struct inode_operations	*i_op;
  const struct file_operations	*i_fop;
  struct address_space	*i_mapping;
};

struct bfs_inode_info {
	unsigned long i_dsk_ino; /* inode number from the disk, can be 0 */
	unsigned long i_sblock;
	unsigned long i_eblock;
	struct inode vfs_inode;
};

struct bfs_sb_info *BFS_SB(struct super_block *sb);
struct bfs_inode_info *BFS_I(struct inode *inode);
struct inode *new_inode(struct super_block *sb);

const struct address_space_operations bfs_aops = {};
const struct file_operations bfs_file_operations = {};
const struct inode_operations bfs_file_inops;
