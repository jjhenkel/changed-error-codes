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

struct block_device {
	int			bd_openers;
	struct inode *		bd_inode;
};

struct bfs_inode_info {
	unsigned long i_dsk_ino; /* inode number from the disk, can be 0 */
	unsigned long i_sblock;
	unsigned long i_eblock;
	struct inode vfs_inode;
};

#define FS_OFS		0x444F5300
#define FS_FFS		0x444F5301
#define FS_INTLOFS	0x444F5302
#define FS_INTLFFS	0x444F5303
#define FS_DCOFS	0x444F5304
#define FS_DCFFS	0x444F5305
#define MUFS_FS		0x6d754653   /* 'muFS' */
#define MUFS_OFS	0x6d754600   /* 'muF\0' */
#define MUFS_FFS	0x6d754601   /* 'muF\1' */
#define MUFS_INTLOFS	0x6d754602   /* 'muF\2' */
#define MUFS_INTLFFS	0x6d754603   /* 'muF\3' */
#define MUFS_DCOFS	0x6d754604   /* 'muF\4' */
#define MUFS_DCFFS	0x6d754605   /* 'muF\5' */

#define T_SHORT		2
#define T_LIST		16
#define T_DATA		8

#define ST_LINKFILE	-4
#define ST_FILE		-3
#define ST_ROOT		1
#define ST_USERDIR	2
#define ST_SOFTLINK	3
#define ST_LINKDIR	4

#define AFFS_ROOT_BMAPS		25

struct affs_date {
	__be32 days;
	__be32 mins;
	__be32 ticks;
};

struct affs_short_date {
	__be16 days;
	__be16 mins;
	__be16 ticks;
};

struct affs_root_head {
	__be32 ptype;
	__be32 spare1;
	__be32 spare2;
	__be32 hash_size;
	__be32 spare3;
	__be32 checksum;
	__be32 hashtable[1];
};

struct affs_root_tail {
	__be32 bm_flag;
	__be32 bm_blk[AFFS_ROOT_BMAPS];
	__be32 bm_ext;
	struct affs_date root_change;
	u8 disk_name[32];
	__be32 spare1;
	__be32 spare2;
	struct affs_date disk_change;
	struct affs_date disk_create;
	__be32 spare3;
	__be32 spare4;
	__be32 dcache;
	__be32 stype;
};

struct affs_head {
	__be32 ptype;
	__be32 key;
	__be32 block_count;
	__be32 spare1;
	__be32 first_data;
	__be32 checksum;
	__be32 table[1];
};

struct affs_tail {
	__be32 spare1;
	__be16 uid;
	__be16 gid;
	__be32 protect;
	__be32 size;
	u8 comment[92];
	struct affs_date change;
	u8 name[32];
	__be32 spare2;
	__be32 original;
	__be32 link_chain;
	__be32 spare[5];
	__be32 hash_chain;
	__be32 parent;
	__be32 extension;
	__be32 stype;
};

struct slink_front
{
	__be32 ptype;
	__be32 key;
	__be32 spare1[3];
	__be32 checksum;
	u8 symname[1];	/* depends on block size */
};

struct affs_data_head
{
	__be32 ptype;
	__be32 key;
	__be32 sequence;
	__be32 size;
	__be32 next;
	__be32 checksum;
	u8 data[1];	/* depends on block size */
};

/* Permission bits */

#define FIBF_OTR_READ		0x8000
#define FIBF_OTR_WRITE		0x4000
#define FIBF_OTR_EXECUTE	0x2000
#define FIBF_OTR_DELETE		0x1000
#define FIBF_GRP_READ		0x0800
#define FIBF_GRP_WRITE		0x0400
#define FIBF_GRP_EXECUTE	0x0200
#define FIBF_GRP_DELETE		0x0100

#define FIBF_HIDDEN		0x0080
#define FIBF_SCRIPT		0x0040
#define FIBF_PURE		0x0020		/* no use under linux */
#define FIBF_ARCHIVED		0x0010		/* never set, always cleared on write */
#define FIBF_NOREAD		0x0008		/* 0 means allowed */
#define FIBF_NOWRITE		0x0004		/* 0 means allowed */
#define FIBF_NOEXECUTE		0x0002		/* 0 means allowed, ignored under linux */
#define FIBF_NODELETE		0x0001		/* 0 means allowed */

#define FIBF_OWNER		0x000F		/* Bits pertaining to owner */
#define FIBF_MASK		0xEE0E		/* Bits modified by Linux */

struct bfs_sb_info *BFS_SB(struct super_block *sb);
struct bfs_inode_info *BFS_I(struct inode *inode);
struct inode *new_inode(struct super_block *sb);

const struct address_space_operations bfs_aops = {};
const struct file_operations bfs_file_operations = {};
const struct inode_operations bfs_file_inops;

/* Ugly macros make the code more pretty. */

#define GET_END_PTR(st,p,sz)		 ((st *)((char *)(p)+((sz)-sizeof(st))))
#define AFFS_GET_HASHENTRY(data,hashkey) be32_to_cpu(((struct dir_front *)data)->hashtable[hashkey])
#define AFFS_BLOCK(sb, bh, blk)		(AFFS_HEAD(bh)->table[AFFS_SB(sb)->s_hashsize-1-(blk)])

#define AFFS_HEAD(bh)		((struct affs_head *)(bh)->b_data)
#define AFFS_TAIL(sb, bh)	((struct affs_tail *)((bh)->b_data+(sb)->s_blocksize-sizeof(struct affs_tail)))
#define AFFS_ROOT_HEAD(bh)	((struct affs_root_head *)(bh)->b_data)
#define AFFS_ROOT_TAIL(sb, bh)	((struct affs_root_tail *)((bh)->b_data+(sb)->s_blocksize-sizeof(struct affs_root_tail)))
#define AFFS_DATA_HEAD(bh)	((struct affs_data_head *)(bh)->b_data)
#define AFFS_DATA(bh)		(((struct affs_data_head *)(bh)->b_data)->data)

#define AFFS_CACHE_SIZE		PAGE_SIZE

#define AFFS_LC_SIZE		(AFFS_CACHE_SIZE/sizeof(u32)/2)
#define AFFS_AC_SIZE		(AFFS_CACHE_SIZE/sizeof(struct affs_ext_key)/2)
#define AFFS_AC_MASK		(AFFS_AC_SIZE-1)

#define AFFSNAMEMAX 30U

struct affs_ext_key {
	u32	ext;				/* idx of the extended block */
	u32	key;				/* block number */
};

/*
 * affs fs inode data in memory
 */
struct affs_inode_info {
	atomic_t i_opencnt;
	// struct semaphore i_link_lock;		/* Protects internal inode access. */
	// struct semaphore i_ext_lock;		/* Protects internal inode access. */
#define i_hash_lock i_ext_lock
	u32	 i_blkcnt;			/* block count */
	u32	 i_extcnt;			/* extended block count */
	u32	*i_lc;				/* linear cache of extended blocks */
	u32	 i_lc_size;
	u32	 i_lc_shift;
	u32	 i_lc_mask;
	struct affs_ext_key *i_ac;		/* associative cache of extended blocks */
	u32	 i_ext_last;			/* last accessed extended block */
	struct buffer_head *i_ext_bh;		/* bh of last extended block */
	loff_t	 mmu_private;
	u32	 i_protect;			/* unused attribute bits */
	u32	 i_lastalloc;			/* last allocated block */
	int	 i_pa_cnt;			/* number of preallocated blocks */
	struct inode vfs_inode;
};

/* short cut to get to the affs specific inode data */
struct affs_inode_info *AFFS_I(struct inode *inode);

/*
 * super-block data in memory
 *
 * Block numbers are adjusted for their actual size
 *
 */

struct affs_bm_info {
	u32 bm_key;			/* Disk block number */
	u32 bm_free;			/* Free blocks in here */
};

typedef struct {

} spinlock_t;

struct delayed_work {

};

struct affs_sb_info {
	int s_partition_size;		/* Partition size in blocks. */
	int s_reserved;			/* Number of reserved blocks. */
	//u32 s_blksize;			/* Initial device blksize */
	u32 s_data_blksize;		/* size of the data block w/o header */
	u32 s_root_block;		/* FFS root block number. */
	int s_hashsize;			/* Size of hash table. */
	unsigned long s_flags;		/* See below. */
	kuid_t s_uid;			/* uid to override */
	kgid_t s_gid;			/* gid to override */
	umode_t s_mode;			/* mode to override */
	struct buffer_head *s_root_bh;	/* Cached root block. */
	struct mutex s_bmlock;		/* Protects bitmap access. */
	struct affs_bm_info *s_bitmap;	/* Bitmap infos. */
	u32 s_bmap_count;		/* # of bitmap blocks. */
	u32 s_bmap_bits;		/* # of bits in one bitmap blocks */
	u32 s_last_bmap;
	struct buffer_head *s_bmap_bh;
	char *s_prefix;			/* Prefix for volumes and assigns. */
	char s_volume[32];		/* Volume prefix for absolute symlinks. */
	spinlock_t symlink_lock;	/* protects the previous two */
	struct super_block *sb;		/* the VFS superblock object */
	int work_queued;		/* non-zero delayed work is queued */
	struct delayed_work sb_work;	/* superblock flush delayed work */
	spinlock_t work_lock;		/* protects sb_work and work_queued */
};

#define AFFS_MOUNT_SF_INTL		0x0001 /* International filesystem. */
#define AFFS_MOUNT_SF_BM_VALID		0x0002 /* Bitmap is valid. */
#define AFFS_MOUNT_SF_IMMUTABLE		0x0004 /* Protection bits cannot be changed */
#define AFFS_MOUNT_SF_QUIET		0x0008 /* chmod errors will be not reported */
#define AFFS_MOUNT_SF_SETUID		0x0010 /* Ignore Amiga uid */
#define AFFS_MOUNT_SF_SETGID		0x0020 /* Ignore Amiga gid */
#define AFFS_MOUNT_SF_SETMODE		0x0040 /* Ignore Amiga protection bits */
#define AFFS_MOUNT_SF_MUFS		0x0100 /* Use MUFS uid/gid mapping */
#define AFFS_MOUNT_SF_OFS		0x0200 /* Old filesystem */
#define AFFS_MOUNT_SF_PREFIX		0x0400 /* Buffer for prefix is allocated */
#define AFFS_MOUNT_SF_VERBOSE		0x0800 /* Talk about fs when mounting */
#define AFFS_MOUNT_SF_NO_TRUNCATE	0x1000 /* Don't truncate filenames */

#define affs_clear_opt(o, opt)    (o &= ~AFFS_MOUNT_##opt)
#define affs_set_opt(o, opt)      (o |= AFFS_MOUNT_##opt)
#define affs_test_opt(o, opt)     ((o) & AFFS_MOUNT_##opt)

/* short cut to get to the affs specific sb data */
struct affs_sb_info *AFFS_SB(struct super_block *sb);

void affs_mark_sb_dirty(struct super_block *sb);

/* amigaffs.c */

extern int	affs_insert_hash(struct inode *inode, struct buffer_head *bh);
extern int	affs_remove_hash(struct inode *dir, struct buffer_head *rem_bh);
extern int	affs_remove_header(struct dentry *dentry);
extern u32	affs_checksum_block(struct super_block *sb, struct buffer_head *bh);
extern void	affs_fix_checksum(struct super_block *sb, struct buffer_head *bh);
// extern void	secs_to_datestamp(time64_t secs, struct affs_date *ds);
extern umode_t	prot_to_mode(u32 prot);
extern void	mode_to_prot(struct inode *inode);
extern void	affs_error(struct super_block *sb, const char *function,
			   const char *fmt, ...);
extern void	affs_warning(struct super_block *sb, const char *function,
			     const char *fmt, ...);
extern bool	affs_nofilenametruncate(const struct dentry *dentry);
extern int	affs_check_name(const unsigned char *name, int len,
				bool notruncate);
extern int	affs_copy_name(unsigned char *bstr, struct dentry *dentry);

/* bitmap. c */

extern u32	affs_count_free_blocks(struct super_block *s);
extern void	affs_free_block(struct super_block *sb, u32 block);
extern u32	affs_alloc_block(struct inode *inode, u32 goal);
extern int	affs_init_bitmap(struct super_block *sb, int *flags);
extern void	affs_free_bitmap(struct super_block *sb);

/* namei.c */

extern int	affs_hash_name(struct super_block *sb, const u8 *name, unsigned int len);
extern struct dentry *affs_lookup(struct inode *dir, struct dentry *dentry, unsigned int);
extern int	affs_unlink(struct inode *dir, struct dentry *dentry);
extern int	affs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool);
extern int	affs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode);
extern int	affs_rmdir(struct inode *dir, struct dentry *dentry);
extern int	affs_link(struct dentry *olddentry, struct inode *dir,
			  struct dentry *dentry);
extern int	affs_symlink(struct inode *dir, struct dentry *dentry,
			     const char *symname);
extern int	affs_rename(struct inode *old_dir, struct dentry *old_dentry,
			    struct inode *new_dir, struct dentry *new_dentry);

/* inode.c */

extern unsigned long		 affs_parent_ino(struct inode *dir);
extern struct inode		*affs_new_inode(struct inode *dir);
// extern int			 affs_notify_change(struct dentry *dentry, struct iattr *attr);
extern void			 affs_evict_inode(struct inode *inode);
extern struct inode		*affs_iget(struct super_block *sb,
					unsigned long ino);
// extern int			 affs_write_inode(struct inode *inode,
					// struct writeback_control *wbc);
extern int			 affs_add_entry(struct inode *dir, struct inode *inode, struct dentry *dentry, s32 type);

/* file.c */

void		affs_free_prealloc(struct inode *inode);
extern void	affs_truncate(struct inode *);
// int		affs_file_fsync(struct file *, loff_t, loff_t, int);

/* dir.c */

extern void   affs_dir_truncate(struct inode *);

/* jump tables */

extern const struct inode_operations	 affs_file_inode_operations;
extern const struct inode_operations	 affs_dir_inode_operations;
extern const struct inode_operations   affs_symlink_inode_operations;
extern const struct file_operations	 affs_file_operations;
extern const struct file_operations	 affs_file_operations_ofs;
extern const struct file_operations	 affs_dir_operations;
extern const struct address_space_operations	 affs_symlink_aops;
extern const struct address_space_operations	 affs_aops;
extern const struct address_space_operations	 affs_aops_ofs;

extern const struct dentry_operations	 affs_dentry_operations;
extern const struct dentry_operations	 affs_intl_dentry_operations;

void
affs_set_blocksize(struct super_block *sb, int size);

struct buffer_head *
affs_bread(struct super_block *sb, int block);

struct buffer_head *
affs_getblk(struct super_block *sb, int block);

struct buffer_head *
affs_getzeroblk(struct super_block *sb, int block);

struct buffer_head *
affs_getemptyblk(struct super_block *sb, int block);

void
affs_brelse(struct buffer_head *bh);

void
affs_adjust_checksum(struct buffer_head *bh, u32 val);

void
affs_adjust_bitmapchecksum(struct buffer_head *bh, u32 val);

void
affs_lock_link(struct inode *inode);

void
affs_unlock_link(struct inode *inode);

void
affs_lock_dir(struct inode *inode);

void
affs_unlock_dir(struct inode *inode);

void
affs_lock_ext(struct inode *inode);

void
affs_unlock_ext(struct inode *inode);

struct buffer_head *
sb_bread(struct super_block *sb, sector_t block);

#define PAGE_SIZE		2048

extern struct dentry * d_make_root(struct inode *);

static const struct super_operations affs_sops = {};
struct work_struct {};
void flush_superblock(struct work_struct *work);