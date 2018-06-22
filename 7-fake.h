#include "common.h"


#define MAX_BUF_PER_PAGE (PAGE_SIZE / 512)

enum bh_state_bits {
	BH_Uptodate,	/* Contains valid data */
	BH_Dirty,	/* Is dirty */
	BH_Lock,	/* Is locked */
	BH_Req,		/* Has been submitted for I/O */
	BH_Uptodate_Lock,/* Used by the first bh in a page, to serialise
			  * IO completion of other buffers in the page
			  */

	BH_Mapped,	/* Has a disk mapping */
	BH_New,		/* Disk mapping was newly created by get_block */
	BH_Async_Read,	/* Is under end_buffer_async_read I/O */
	BH_Async_Write,	/* Is under end_buffer_async_write I/O */
	BH_Delay,	/* Buffer is not yet allocated on disk */
	BH_Boundary,	/* Block is followed by a discontiguity */
	BH_Write_EIO,	/* I/O error on write */
	BH_Unwritten,	/* Buffer is allocated on disk but not written */
	BH_Quiet,	/* Buffer Error Prinks to be quiet */
	BH_Meta,	/* Buffer contains metadata */
	BH_Prio,	/* Buffer should be submitted with REQ_PRIO */
	BH_Defer_Completion, /* Defer AIO completion to workqueue */

	BH_PrivateStart,/* not a state bit, but the first bit available
			 * for private allocation by other entities
			 */
};

struct page;
struct buffer_head;
struct address_space;
typedef void (bh_end_io_t)(struct buffer_head *bh, int uptodate);

struct buffer_head {
	unsigned long b_state;		/* buffer state bitmap (see above) */
	struct buffer_head *b_this_page;/* circular list of page's buffers */
	struct page *b_page;		/* the page this bh is mapped to */

	sector_t b_blocknr;		/* start block number */
	size_t b_size;			/* size of mapping */
	char *b_data;			/* pointer to data within the page */

	struct block_device *b_bdev;
	bh_end_io_t *b_end_io;		/* I/O completion */
 	void *b_private;		/* reserved for b_end_io */
	struct list_head b_assoc_buffers; /* associated with another mapping */
	struct address_space *b_assoc_map;	/* mapping this buffer is
						   associated with */
	atomic_t b_count;		/* users using this buffer_head */
};

struct dentry_operations {};
struct dentry {};
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

#define ADFS_FPLUS_NAME_LEN	255

#define BIGDIRSTARTNAME ('S' | 'B' << 8 | 'P' << 16 | 'r' << 24)
#define BIGDIRENDNAME	('o' | 'v' << 8 | 'e' << 16 | 'n' << 24)
#define ADFS_F_NAME_LEN 10
#define ADFS_NDA_DIRECTORY	(1 << 3)
#define ADFS_NEWDIR_SIZE	2048
#define ADFS_NDA_OWNER_READ	(1 << 0)
#define ADFS_NDA_OWNER_WRITE	(1 << 1)
#define ADFS_NDA_LOCKED		(1 << 2)
#define ADFS_NDA_EXECUTE	(1 << 4)
#define ADFS_NDA_PUBLIC_READ	(1 << 5)
#define ADFS_NDA_PUBLIC_WRITE	(1 << 6)

struct adfs_bigdirheader {
	__u8	startmasseq;
	__u8	bigdirversion[3];
	__le32	bigdirstartname;
	__le32	bigdirnamelen;
	__le32	bigdirsize;
	__le32	bigdirentries;
	__le32	bigdirnamesize;
	__le32	bigdirparent;
	char	bigdirname[1];
};

struct adfs_bigdirentry {
	__le32	bigdirload;
	__le32	bigdirexec;
	__le32	bigdirlen;
	__le32	bigdirindaddr;
	__le32	bigdirattr;
	__le32	bigdirobnamelen;
	__le32	bigdirobnameptr;
};

struct adfs_bigdirtail {
	__le32	bigdirendname;
	__u8	bigdirendmasseq;
	__u8	reserved[2];
	__u8	bigdircheckbyte;
};

struct adfs_dir {
  int nr_buffers;
  u64 parent_id;
  struct buffer_head **bh_fplus;
  struct buffer_head **bh;
  struct super_block *sb;
};

struct buffer_head *
sb_bread(struct super_block *sb, sector_t block);

#define ADFS_DEFAULT_OWNER_MASK S_IRWXU
#define ADFS_DEFAULT_OTHER_MASK (S_IRWXG | S_IRWXO)
/*
 * Disc Record at disc address 0xc00
 */
struct adfs_discrecord {
    __u8  log2secsize;
    __u8  secspertrack;
    __u8  heads;
    __u8  density;
    __u8  idlen;
    __u8  log2bpmb;
    __u8  skew;
    __u8  bootoption;
    __u8  lowsector;
    __u8  nzones;
    __le16 zone_spare;
    __le32 root;
    __le32 disc_size;
    __le16 disc_id;
    __u8  disc_name[10];
    __le32 disc_type;
    __le32 disc_size_high;
    __u8  log2sharesize:4;
    __u8  unused40:4;
    __u8  big_flag:1;
    __u8  unused41:1;
    __u8  nzones_high;
    __le32 format_version;
    __le32 root_size;
    __u8  unused52[60 - 52];
};

#define ADFS_DISCRECORD		(0xc00)
#define ADFS_DR_OFFSET		(0x1c0)
#define ADFS_DR_SIZE		 60
#define ADFS_DR_SIZE_BITS	(ADFS_DR_SIZE << 3)


struct adfs_discmap {
	struct buffer_head	*dm_bh;
	__u32			dm_startblk;
	unsigned int		dm_startbit;
	unsigned int		dm_endbit;
};

struct adfs_dir_ops {

};

struct adfs_sb_info {
union { struct {
		struct adfs_discmap *s_map;	/* bh list containing map */
		const struct adfs_dir_ops *s_dir; /* directory operations */
		};
		struct rcu_head rcu;	/* used only at shutdown time	 */
	};
	uid_t		s_uid;		/* owner uid */
	gid_t		s_gid;		/* owner gid */
	umode_t		s_owner_mask;	/* ADFS owner perm -> unix perm */
	umode_t		s_other_mask;	/* ADFS other perm -> unix perm	*/
	int		s_ftsuffix;	/* ,xyz hex filetype suffix option */

	__u32		s_ids_per_zone;	/* max. no ids in one zone */
	__u32		s_idlen;	/* length of ID in map */
	__u32		s_map_size;	/* sector size of a map	*/
	unsigned long	s_size;		/* total size (in blocks) of this fs */
	signed int	s_map2blk;	/* shift left by this for map->sector*/
	unsigned int	s_log2sharesize;/* log2 share size */
	__le32		s_version;	/* disc format version */
	unsigned int	s_namelen;	/* maximum number of characters in name	 */
};

#define ADFS_MAX_NAME_LEN	(256 + 4) /* +4 for ,xyz hex filetype suffix */
struct object_info {
	__u32		parent_id;		/* parent object id	*/
	__u32		file_id;		/* object id		*/
	__u32		loadaddr;		/* load address		*/
	__u32		execaddr;		/* execution address	*/
	__u32		size;			/* size			*/
	__u8		attr;			/* RISC OS attributes	*/
	unsigned int	name_len;		/* name length		*/
	char		name[ADFS_MAX_NAME_LEN];/* file name		*/

	/* RISC OS file type (12-bit: derived from loadaddr) */
	__u16		filetype;
};


struct inode {

};

const struct dentry_operations adfs_dentry_operations = { 
};

const struct adfs_dir_ops adfs_f_dir_ops = {
};

const struct adfs_dir_ops adfs_fplus_dir_ops = {
};

const struct super_operations adfs_sops = {};

struct adfs_discmap *adfs_read_map(struct super_block *sb, struct adfs_discrecord *dr);
struct inode *adfs_iget(struct super_block *sb, struct object_info *obj);
extern struct dentry * d_make_root(struct inode *);
