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

struct block_device;

struct super_block {
  struct block_device	*s_bdev;
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
  loff_t			i_size;
};

struct inode *hfsplus_new_inode(struct super_block *sb, umode_t mode);

struct hfsplus_vh;
struct hfs_btree;

struct hfsplus_sb_info {
	void *s_vhdr_buf;
	struct hfsplus_vh *s_vhdr;
	void *s_backup_vhdr_buf;
	struct hfsplus_vh *s_backup_vhdr;
	struct hfs_btree *ext_tree;
	struct hfs_btree *cat_tree;
	struct hfs_btree *attr_tree;
	atomic_t attr_tree_state;
	struct inode *alloc_file;
	struct inode *hidden_dir;
	struct nls_table *nls;

	/* Runtime variables */
	u32 blockoffset;
	sector_t part_start;
	sector_t sect_count;
	int fs_shift;

	/* immutable data from the volume header */
	u32 alloc_blksz;
	int alloc_blksz_shift;
	u32 total_blocks;
	u32 data_clump_blocks, rsrc_clump_blocks;

	/* mutable data from the volume header, protected by alloc_mutex */
	u32 free_blocks;
	struct mutex alloc_mutex;

	/* mutable data from the volume header, protected by vh_mutex */
	u32 next_cnid;
	u32 file_count;
	u32 folder_count;
	struct mutex vh_mutex;

	/* Config options */
	u32 creator;
	u32 type;

	umode_t umask;
	kuid_t uid;
	kgid_t gid;

	int part, session;
	unsigned long flags;

	int work_queued;               /* non-zero delayed work is queued */
	// struct delayed_work sync_work; /* FS sync delayed work */
	// spinlock_t work_lock;          /* protects sync_work and work_queued */
};

struct hfsplus_sb_info *HFSPLUS_SB(struct super_block *sb);