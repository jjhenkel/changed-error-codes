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
struct address_space;
typedef void (bh_end_io_t)(struct buffer_head *bh, int uptodate);

struct super_block {
	unsigned char		s_blocksize_bits;
	unsigned long		s_blocksize;
};

#define ADFS_FPLUS_NAME_LEN	255

#define BIGDIRSTARTNAME ('S' | 'B' << 8 | 'P' << 16 | 'r' << 24)
#define BIGDIRENDNAME	('o' | 'v' << 8 | 'e' << 16 | 'n' << 24)

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
