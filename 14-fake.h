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
  union {
		const unsigned int i_nlink;
		unsigned int __i_nlink;
	};
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

/* Character set specification (ECMA 167r3 1/7.2.1) */
struct charspec {
	uint8_t		charSetType;
	uint8_t		charSetInfo[63];
} __attribute__ ((packed));

/* Character Set Type (ECMA 167r3 1/7.2.1.1) */
#define CHARSPEC_TYPE_CS0		0x00	/* (1/7.2.2) */
#define CHARSPEC_TYPE_CS1		0x01	/* (1/7.2.3) */
#define CHARSPEC_TYPE_CS2		0x02	/* (1/7.2.4) */
#define CHARSPEC_TYPE_CS3		0x03	/* (1/7.2.5) */
#define CHARSPEC_TYPE_CS4		0x04	/* (1/7.2.6) */
#define CHARSPEC_TYPE_CS5		0x05	/* (1/7.2.7) */
#define CHARSPEC_TYPE_CS6		0x06	/* (1/7.2.8) */
#define CHARSPEC_TYPE_CS7		0x07	/* (1/7.2.9) */
#define CHARSPEC_TYPE_CS8		0x08	/* (1/7.2.10) */

typedef uint8_t		dstring;

/* Timestamp (ECMA 167r3 1/7.3) */
struct timestamp {
	__le16		typeAndTimezone;
	__le16		year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t		minute;
	uint8_t		second;
	uint8_t		centiseconds;
	uint8_t		hundredsOfMicroseconds;
	uint8_t		microseconds;
} __attribute__ ((packed));

/* Type and Time Zone (ECMA 167r3 1/7.3.1) */
#define TIMESTAMP_TYPE_MASK		0xF000
#define TIMESTAMP_TYPE_CUT		0x0000
#define TIMESTAMP_TYPE_LOCAL		0x1000
#define TIMESTAMP_TYPE_AGREEMENT	0x2000
#define TIMESTAMP_TIMEZONE_MASK		0x0FFF

/* Entity identifier (ECMA 167r3 1/7.4) */
struct regid {
	uint8_t		flags;
	uint8_t		ident[23];
	uint8_t		identSuffix[8];
} __attribute__ ((packed));

/* Flags (ECMA 167r3 1/7.4.1) */
#define ENTITYID_FLAGS_DIRTY		0x00
#define ENTITYID_FLAGS_PROTECTED	0x01

/* Volume Structure Descriptor (ECMA 167r3 2/9.1) */
#define VSD_STD_ID_LEN			5
struct volStructDesc {
	uint8_t		structType;
	uint8_t		stdIdent[VSD_STD_ID_LEN];
	uint8_t		structVersion;
	uint8_t		structData[2041];
} __attribute__ ((packed));

/* Standard Identifier (EMCA 167r2 2/9.1.2) */
#define VSD_STD_ID_NSR02		"NSR02"	/* (3/9.1) */

/* Standard Identifier (ECMA 167r3 2/9.1.2) */
#define VSD_STD_ID_BEA01		"BEA01"	/* (2/9.2) */
#define VSD_STD_ID_BOOT2		"BOOT2"	/* (2/9.4) */
#define VSD_STD_ID_CD001		"CD001"	/* (ECMA-119) */
#define VSD_STD_ID_CDW02		"CDW02"	/* (ECMA-168) */
#define VSD_STD_ID_NSR03		"NSR03"	/* (3/9.1) */
#define VSD_STD_ID_TEA01		"TEA01"	/* (2/9.3) */

/* Beginning Extended Area Descriptor (ECMA 167r3 2/9.2) */
struct beginningExtendedAreaDesc {
	uint8_t		structType;
	uint8_t		stdIdent[VSD_STD_ID_LEN];
	uint8_t		structVersion;
	uint8_t		structData[2041];
} __attribute__ ((packed));

/* Terminating Extended Area Descriptor (ECMA 167r3 2/9.3) */
struct terminatingExtendedAreaDesc {
	uint8_t		structType;
	uint8_t		stdIdent[VSD_STD_ID_LEN];
	uint8_t		structVersion;
	uint8_t		structData[2041];
} __attribute__ ((packed));

/* Boot Descriptor (ECMA 167r3 2/9.4) */
struct bootDesc {
	uint8_t			structType;
	uint8_t			stdIdent[VSD_STD_ID_LEN];
	uint8_t			structVersion;
	uint8_t			reserved1;
	struct regid		archType;
	struct regid		bootIdent;
	__le32			bootExtLocation;
	__le32			bootExtLength;
	__le64			loadAddress;
	__le64			startAddress;
	struct timestamp	descCreationDateAndTime;
	__le16			flags;
	uint8_t			reserved2[32];
	uint8_t			bootUse[1906];
} __attribute__ ((packed));

/* Flags (ECMA 167r3 2/9.4.12) */
#define BOOT_FLAGS_ERASE		0x01

/* Extent Descriptor (ECMA 167r3 3/7.1) */
struct extent_ad {
	__le32		extLength;
	__le32		extLocation;
} __attribute__ ((packed));

struct kernel_extent_ad {
	uint32_t	extLength;
	uint32_t	extLocation;
};

/* Descriptor Tag (ECMA 167r3 3/7.2) */
struct tag {
	__le16		tagIdent;
	__le16		descVersion;
	uint8_t		tagChecksum;
	uint8_t		reserved;
	__le16		tagSerialNum;
	__le16		descCRC;
	__le16		descCRCLength;
	__le32		tagLocation;
} __attribute__ ((packed));

/* Tag Identifier (ECMA 167r3 3/7.2.1) */
#define TAG_IDENT_PVD			0x0001
#define TAG_IDENT_AVDP			0x0002
#define TAG_IDENT_VDP			0x0003
#define TAG_IDENT_IUVD			0x0004
#define TAG_IDENT_PD			0x0005
#define TAG_IDENT_LVD			0x0006
#define TAG_IDENT_USD			0x0007
#define TAG_IDENT_TD			0x0008
#define TAG_IDENT_LVID			0x0009

/* NSR Descriptor (ECMA 167r3 3/9.1) */
struct NSRDesc {
	uint8_t		structType;
	uint8_t		stdIdent[VSD_STD_ID_LEN];
	uint8_t		structVersion;
	uint8_t		reserved;
	uint8_t		structData[2040];
} __attribute__ ((packed));

/* Primary Volume Descriptor (ECMA 167r3 3/10.1) */
struct primaryVolDesc {
	struct tag		descTag;
	__le32			volDescSeqNum;
	__le32			primaryVolDescNum;
	dstring			volIdent[32];
	__le16			volSeqNum;
	__le16			maxVolSeqNum;
	__le16			interchangeLvl;
	__le16			maxInterchangeLvl;
	__le32			charSetList;
	__le32			maxCharSetList;
	dstring			volSetIdent[128];
	struct charspec		descCharSet;
	struct charspec		explanatoryCharSet;
	struct extent_ad	volAbstract;
	struct extent_ad	volCopyright;
	struct regid		appIdent;
	struct timestamp	recordingDateAndTime;
	struct regid		impIdent;
	uint8_t			impUse[64];
	__le32			predecessorVolDescSeqLocation;
	__le16			flags;
	uint8_t			reserved[22];
} __attribute__ ((packed));

/* Flags (ECMA 167r3 3/10.1.21) */
#define PVD_FLAGS_VSID_COMMON		0x0001

/* Anchor Volume Descriptor Pointer (ECMA 167r3 3/10.2) */
struct anchorVolDescPtr {
	struct tag		descTag;
	struct extent_ad	mainVolDescSeqExt;
	struct extent_ad	reserveVolDescSeqExt;
	uint8_t	 		reserved[480];
} __attribute__ ((packed));

/* Volume Descriptor Pointer (ECMA 167r3 3/10.3) */
struct volDescPtr {
	struct tag		descTag;
	__le32			volDescSeqNum;
	struct extent_ad	nextVolDescSeqExt;
	uint8_t			reserved[484];
} __attribute__ ((packed));

/* Implementation Use Volume Descriptor (ECMA 167r3 3/10.4) */
struct impUseVolDesc {
	struct tag	descTag;
	__le32		volDescSeqNum;
	struct regid	impIdent;
	uint8_t		impUse[460];
} __attribute__ ((packed));

/* Partition Descriptor (ECMA 167r3 3/10.5) */
struct partitionDesc {
	struct tag descTag;
	__le32 volDescSeqNum;
	__le16 partitionFlags;
	__le16 partitionNumber;
	struct regid partitionContents;
	uint8_t partitionContentsUse[128];
	__le32 accessType;
	__le32 partitionStartingLocation;
	__le32 partitionLength;
	struct regid impIdent;
	uint8_t impUse[128];
	uint8_t reserved[156];
} __attribute__ ((packed));

/* Partition Flags (ECMA 167r3 3/10.5.3) */
#define PD_PARTITION_FLAGS_ALLOC	0x0001

/* Partition Contents (ECMA 167r2 3/10.5.3) */
#define PD_PARTITION_CONTENTS_NSR02	"+NSR02"

/* Partition Contents (ECMA 167r3 3/10.5.5) */
#define PD_PARTITION_CONTENTS_FDC01	"+FDC01"
#define PD_PARTITION_CONTENTS_CD001	"+CD001"
#define PD_PARTITION_CONTENTS_CDW02	"+CDW02"
#define PD_PARTITION_CONTENTS_NSR03	"+NSR03"

/* Access Type (ECMA 167r3 3/10.5.7) */
#define PD_ACCESS_TYPE_NONE		0x00000000
#define PD_ACCESS_TYPE_READ_ONLY	0x00000001
#define PD_ACCESS_TYPE_WRITE_ONCE	0x00000002
#define PD_ACCESS_TYPE_REWRITABLE	0x00000003
#define PD_ACCESS_TYPE_OVERWRITABLE	0x00000004

/* Logical Volume Descriptor (ECMA 167r3 3/10.6) */
struct logicalVolDesc {
	struct tag		descTag;
	__le32			volDescSeqNum;
	struct charspec		descCharSet;
	dstring			logicalVolIdent[128];
	__le32			logicalBlockSize;
	struct regid		domainIdent;
	uint8_t			logicalVolContentsUse[16];
	__le32			mapTableLength;
	__le32			numPartitionMaps;
	struct regid		impIdent;
	uint8_t			impUse[128];
	struct extent_ad	integritySeqExt;
	uint8_t			partitionMaps[0];
} __attribute__ ((packed));

/* Generic Partition Map (ECMA 167r3 3/10.7.1) */
struct genericPartitionMap {
	uint8_t		partitionMapType;
	uint8_t		partitionMapLength;
	uint8_t		partitionMapping[0];
} __attribute__ ((packed));

/* Partition Map Type (ECMA 167r3 3/10.7.1.1) */
#define GP_PARTITION_MAP_TYPE_UNDEF	0x00
#define GP_PARTIITON_MAP_TYPE_1		0x01
#define GP_PARTITION_MAP_TYPE_2		0x02

/* Type 1 Partition Map (ECMA 167r3 3/10.7.2) */
struct genericPartitionMap1 {
	uint8_t		partitionMapType;
	uint8_t		partitionMapLength;
	__le16		volSeqNum;
	__le16		partitionNum;
} __attribute__ ((packed));

/* Type 2 Partition Map (ECMA 167r3 3/10.7.3) */
struct genericPartitionMap2 {
	uint8_t		partitionMapType;
	uint8_t		partitionMapLength;
	uint8_t		partitionIdent[62];
} __attribute__ ((packed));

/* Unallocated Space Descriptor (ECMA 167r3 3/10.8) */
struct unallocSpaceDesc {
	struct tag		descTag;
	__le32			volDescSeqNum;
	__le32			numAllocDescs;
	struct extent_ad	allocDescs[0];
} __attribute__ ((packed));

/* Terminating Descriptor (ECMA 167r3 3/10.9) */
struct terminatingDesc {
	struct tag	descTag;
	uint8_t		reserved[496];
} __attribute__ ((packed));

/* Logical Volume Integrity Descriptor (ECMA 167r3 3/10.10) */
struct logicalVolIntegrityDesc {
	struct tag		descTag;
	struct timestamp	recordingDateAndTime;
	__le32			integrityType;
	struct extent_ad	nextIntegrityExt;
	uint8_t			logicalVolContentsUse[32];
	__le32			numOfPartitions;
	__le32			lengthOfImpUse;
	__le32			freeSpaceTable[0];
	__le32			sizeTable[0];
	uint8_t			impUse[0];
} __attribute__ ((packed));

/* Integrity Type (ECMA 167r3 3/10.10.3) */
#define LVID_INTEGRITY_TYPE_OPEN	0x00000000
#define LVID_INTEGRITY_TYPE_CLOSE	0x00000001

/* Recorded Address (ECMA 167r3 4/7.1) */
struct lb_addr {
	__le32		logicalBlockNum;
	__le16	 	partitionReferenceNum;
} __attribute__ ((packed));

/* ... and its in-core analog */
struct kernel_lb_addr {
	uint32_t		logicalBlockNum;
	uint16_t	 	partitionReferenceNum;
};

/* Short Allocation Descriptor (ECMA 167r3 4/14.14.1) */
struct short_ad {
        __le32		extLength;
        __le32		extPosition;
} __attribute__ ((packed));

/* Long Allocation Descriptor (ECMA 167r3 4/14.14.2) */
struct long_ad {
	__le32		extLength;
	struct lb_addr	extLocation;
	uint8_t		impUse[6];
} __attribute__ ((packed));

struct kernel_long_ad {
	uint32_t		extLength;
	struct kernel_lb_addr	extLocation;
	uint8_t			impUse[6];
};

/* Extended Allocation Descriptor (ECMA 167r3 4/14.14.3) */
struct ext_ad {
	__le32		extLength;
	__le32		recordedLength;
	__le32		informationLength;
	struct lb_addr	extLocation;
} __attribute__ ((packed));

struct kernel_ext_ad {
	uint32_t		extLength;
	uint32_t		recordedLength;
	uint32_t		informationLength;
	struct kernel_lb_addr	extLocation;
};

/* Descriptor Tag (ECMA 167r3 4/7.2 - See 3/7.2) */

/* Tag Identifier (ECMA 167r3 4/7.2.1) */
#define TAG_IDENT_FSD			0x0100
#define TAG_IDENT_FID			0x0101
#define TAG_IDENT_AED			0x0102
#define TAG_IDENT_IE			0x0103
#define TAG_IDENT_TE			0x0104
#define TAG_IDENT_FE			0x0105
#define TAG_IDENT_EAHD			0x0106
#define TAG_IDENT_USE			0x0107
#define TAG_IDENT_SBD			0x0108
#define TAG_IDENT_PIE			0x0109
#define TAG_IDENT_EFE			0x010A

/* File Set Descriptor (ECMA 167r3 4/14.1) */
struct fileSetDesc {
	struct tag		descTag;
	struct timestamp	recordingDateAndTime;
	__le16			interchangeLvl;
	__le16			maxInterchangeLvl;
	__le32			charSetList;
	__le32			maxCharSetList;
	__le32			fileSetNum;
	__le32			fileSetDescNum;
	struct charspec		logicalVolIdentCharSet;
	dstring			logicalVolIdent[128];
	struct charspec		fileSetCharSet;
	dstring			fileSetIdent[32];
	dstring			copyrightFileIdent[32];
	dstring			abstractFileIdent[32];
	struct long_ad		rootDirectoryICB;
	struct regid		domainIdent;
	struct long_ad		nextExt;
	struct long_ad		streamDirectoryICB;
	uint8_t			reserved[32];
} __attribute__ ((packed));

/* Partition Header Descriptor (ECMA 167r3 4/14.3) */
struct partitionHeaderDesc {
	struct short_ad	unallocSpaceTable;
	struct short_ad	unallocSpaceBitmap;
	struct short_ad	partitionIntegrityTable;
	struct short_ad	freedSpaceTable;
	struct short_ad	freedSpaceBitmap;
	uint8_t		reserved[88];
} __attribute__ ((packed));

/* File Identifier Descriptor (ECMA 167r3 4/14.4) */
struct fileIdentDesc {
	struct tag	descTag;
	__le16		fileVersionNum;
	uint8_t		fileCharacteristics;
	uint8_t		lengthFileIdent;
	struct long_ad	icb;
	__le16		lengthOfImpUse;
	uint8_t		impUse[0];
	uint8_t		fileIdent[0];
	uint8_t		padding[0];
} __attribute__ ((packed));

/* File Characteristics (ECMA 167r3 4/14.4.3) */
#define FID_FILE_CHAR_HIDDEN		0x01
#define FID_FILE_CHAR_DIRECTORY		0x02
#define FID_FILE_CHAR_DELETED		0x04
#define FID_FILE_CHAR_PARENT		0x08
#define FID_FILE_CHAR_METADATA		0x10

/* Allocation Ext Descriptor (ECMA 167r3 4/14.5) */
struct allocExtDesc {
	struct tag	descTag;
	__le32		previousAllocExtLocation;
	__le32		lengthAllocDescs;
} __attribute__ ((packed));

/* ICB Tag (ECMA 167r3 4/14.6) */
struct icbtag {
	__le32		priorRecordedNumDirectEntries;
	__le16		strategyType;
	__le16		strategyParameter;
	__le16		numEntries;
	uint8_t		reserved;
	uint8_t		fileType;
	struct lb_addr	parentICBLocation;
	__le16		flags;
} __attribute__ ((packed));

/* Strategy Type (ECMA 167r3 4/14.6.2) */
#define ICBTAG_STRATEGY_TYPE_UNDEF	0x0000
#define ICBTAG_STRATEGY_TYPE_1		0x0001
#define ICBTAG_STRATEGY_TYPE_2		0x0002
#define ICBTAG_STRATEGY_TYPE_3		0x0003
#define ICBTAG_STRATEGY_TYPE_4		0x0004

/* File Type (ECMA 167r3 4/14.6.6) */
#define ICBTAG_FILE_TYPE_UNDEF		0x00
#define ICBTAG_FILE_TYPE_USE		0x01
#define ICBTAG_FILE_TYPE_PIE		0x02
#define ICBTAG_FILE_TYPE_IE		0x03
#define ICBTAG_FILE_TYPE_DIRECTORY	0x04
#define ICBTAG_FILE_TYPE_REGULAR	0x05
#define ICBTAG_FILE_TYPE_BLOCK		0x06
#define ICBTAG_FILE_TYPE_CHAR		0x07
#define ICBTAG_FILE_TYPE_EA		0x08
#define ICBTAG_FILE_TYPE_FIFO		0x09
#define ICBTAG_FILE_TYPE_SOCKET		0x0A
#define ICBTAG_FILE_TYPE_TE		0x0B
#define ICBTAG_FILE_TYPE_SYMLINK	0x0C
#define ICBTAG_FILE_TYPE_STREAMDIR	0x0D

/* Flags (ECMA 167r3 4/14.6.8) */
#define ICBTAG_FLAG_AD_MASK		0x0007
#define ICBTAG_FLAG_AD_SHORT		0x0000
#define ICBTAG_FLAG_AD_LONG		0x0001
#define ICBTAG_FLAG_AD_EXTENDED		0x0002
#define ICBTAG_FLAG_AD_IN_ICB		0x0003
#define ICBTAG_FLAG_SORTED		0x0008
#define ICBTAG_FLAG_NONRELOCATABLE	0x0010
#define ICBTAG_FLAG_ARCHIVE		0x0020
#define ICBTAG_FLAG_SETUID		0x0040
#define ICBTAG_FLAG_SETGID		0x0080
#define ICBTAG_FLAG_STICKY		0x0100
#define ICBTAG_FLAG_CONTIGUOUS		0x0200
#define ICBTAG_FLAG_SYSTEM		0x0400
#define ICBTAG_FLAG_TRANSFORMED		0x0800
#define ICBTAG_FLAG_MULTIVERSIONS	0x1000
#define ICBTAG_FLAG_STREAM		0x2000

/* Indirect Entry (ECMA 167r3 4/14.7) */
struct indirectEntry {
	struct tag	descTag;
	struct icbtag	icbTag;
	struct long_ad	indirectICB;
} __attribute__ ((packed));

/* Terminal Entry (ECMA 167r3 4/14.8) */
struct terminalEntry {
	struct tag	descTag;
	struct icbtag	icbTag;
} __attribute__ ((packed));

/* File Entry (ECMA 167r3 4/14.9) */
struct fileEntry {
	struct tag		descTag;
	struct icbtag		icbTag;
	__le32			uid;
	__le32			gid;
	__le32			permissions;
	__le16			fileLinkCount;
	uint8_t			recordFormat;
	uint8_t			recordDisplayAttr;
	__le32			recordLength;
	__le64			informationLength;
	__le64			logicalBlocksRecorded;
	struct timestamp	accessTime;
	struct timestamp	modificationTime;
	struct timestamp	attrTime;
	__le32			checkpoint;
	struct long_ad		extendedAttrICB;
	struct regid		impIdent;
	__le64			uniqueID;
	__le32			lengthExtendedAttr;
	__le32			lengthAllocDescs;
	uint8_t			extendedAttr[0];
	uint8_t			allocDescs[0];
} __attribute__ ((packed));

/* Permissions (ECMA 167r3 4/14.9.5) */
#define FE_PERM_O_EXEC			0x00000001U
#define FE_PERM_O_WRITE			0x00000002U
#define FE_PERM_O_READ			0x00000004U
#define FE_PERM_O_CHATTR		0x00000008U
#define FE_PERM_O_DELETE		0x00000010U
#define FE_PERM_G_EXEC			0x00000020U
#define FE_PERM_G_WRITE			0x00000040U
#define FE_PERM_G_READ			0x00000080U
#define FE_PERM_G_CHATTR		0x00000100U
#define FE_PERM_G_DELETE		0x00000200U
#define FE_PERM_U_EXEC			0x00000400U
#define FE_PERM_U_WRITE			0x00000800U
#define FE_PERM_U_READ			0x00001000U
#define FE_PERM_U_CHATTR		0x00002000U
#define FE_PERM_U_DELETE		0x00004000U

/* Record Format (ECMA 167r3 4/14.9.7) */
#define FE_RECORD_FMT_UNDEF		0x00
#define FE_RECORD_FMT_FIXED_PAD		0x01
#define FE_RECORD_FMT_FIXED		0x02
#define FE_RECORD_FMT_VARIABLE8		0x03
#define FE_RECORD_FMT_VARIABLE16	0x04
#define FE_RECORD_FMT_VARIABLE16_MSB	0x05
#define FE_RECORD_FMT_VARIABLE32	0x06
#define FE_RECORD_FMT_PRINT		0x07
#define FE_RECORD_FMT_LF		0x08
#define FE_RECORD_FMT_CR		0x09
#define FE_RECORD_FMT_CRLF		0x0A
#define FE_RECORD_FMT_LFCR		0x0B

/* Record Display Attributes (ECMA 167r3 4/14.9.8) */
#define FE_RECORD_DISPLAY_ATTR_UNDEF	0x00
#define FE_RECORD_DISPLAY_ATTR_1	0x01
#define FE_RECORD_DISPLAY_ATTR_2	0x02
#define FE_RECORD_DISPLAY_ATTR_3	0x03

/* Extended Attribute Header Descriptor (ECMA 167r3 4/14.10.1) */
struct extendedAttrHeaderDesc {
	struct tag	descTag;
	__le32		impAttrLocation;
	__le32		appAttrLocation;
} __attribute__ ((packed));

/* Generic Format (ECMA 167r3 4/14.10.2) */
struct genericFormat {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	uint8_t		attrData[0];
} __attribute__ ((packed));

/* Character Set Information (ECMA 167r3 4/14.10.3) */
struct charSetInfo {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		escapeSeqLength;
	uint8_t		charSetType;
	uint8_t		escapeSeq[0];
} __attribute__ ((packed));

/* Alternate Permissions (ECMA 167r3 4/14.10.4) */
struct altPerms {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le16		ownerIdent;
	__le16		groupIdent;
	__le16		permission;
} __attribute__ ((packed));

/* File Times Extended Attribute (ECMA 167r3 4/14.10.5) */
struct fileTimesExtAttr {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		dataLength;
	__le32		fileTimeExistence;
	uint8_t		fileTimes;
} __attribute__ ((packed));

/* FileTimeExistence (ECMA 167r3 4/14.10.5.6) */
#define FTE_CREATION			0x00000001
#define FTE_DELETION			0x00000004
#define FTE_EFFECTIVE			0x00000008
#define FTE_BACKUP			0x00000002

/* Information Times Extended Attribute (ECMA 167r3 4/14.10.6) */
struct infoTimesExtAttr {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		dataLength;
	__le32		infoTimeExistence;
	uint8_t		infoTimes[0];
} __attribute__ ((packed));

/* Device Specification (ECMA 167r3 4/14.10.7) */
struct deviceSpec {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		impUseLength;
	__le32		majorDeviceIdent;
	__le32		minorDeviceIdent;
	uint8_t		impUse[0];
} __attribute__ ((packed));

/* Implementation Use Extended Attr (ECMA 167r3 4/14.10.8) */
struct impUseExtAttr {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		impUseLength;
	struct regid	impIdent;
	uint8_t		impUse[0];
} __attribute__ ((packed));

/* Application Use Extended Attribute (ECMA 167r3 4/14.10.9) */
struct appUseExtAttr {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		appUseLength;
	struct regid	appIdent;
	uint8_t		appUse[0];
} __attribute__ ((packed));

#define EXTATTR_CHAR_SET		1
#define EXTATTR_ALT_PERMS		3
#define EXTATTR_FILE_TIMES		5
#define EXTATTR_INFO_TIMES		6
#define EXTATTR_DEV_SPEC		12
#define EXTATTR_IMP_USE			2048
#define EXTATTR_APP_USE			65536

/* Unallocated Space Entry (ECMA 167r3 4/14.11) */
struct unallocSpaceEntry {
	struct tag	descTag;
	struct icbtag	icbTag;
	__le32		lengthAllocDescs;
	uint8_t		allocDescs[0];
} __attribute__ ((packed));

/* Space Bitmap Descriptor (ECMA 167r3 4/14.12) */
struct spaceBitmapDesc {
	struct tag	descTag;
	__le32		numOfBits;
	__le32		numOfBytes;
	uint8_t		bitmap[0];
} __attribute__ ((packed));

/* Partition Integrity Entry (ECMA 167r3 4/14.13) */
struct partitionIntegrityEntry {
	struct tag		descTag;
	struct icbtag		icbTag;
	struct timestamp	recordingDateAndTime;
	uint8_t			integrityType;
	uint8_t			reserved[175];
	struct regid		impIdent;
	uint8_t			impUse[256];
} __attribute__ ((packed));

/* Short Allocation Descriptor (ECMA 167r3 4/14.14.1) */

/* Extent Length (ECMA 167r3 4/14.14.1.1) */
#define EXT_RECORDED_ALLOCATED		0x00000000
#define EXT_NOT_RECORDED_ALLOCATED	0x40000000
#define EXT_NOT_RECORDED_NOT_ALLOCATED	0x80000000
#define EXT_NEXT_EXTENT_ALLOCDECS	0xC0000000

/* Long Allocation Descriptor (ECMA 167r3 4/14.14.2) */

/* Extended Allocation Descriptor (ECMA 167r3 4/14.14.3) */

/* Logical Volume Header Descriptor (ECMA 167r3 4/14.15) */
struct logicalVolHeaderDesc {
	__le64		uniqueID;
	uint8_t		reserved[24];
} __attribute__ ((packed));

/* Path Component (ECMA 167r3 4/14.16.1) */
struct pathComponent {
	uint8_t		componentType;
	uint8_t		lengthComponentIdent;
	__le16		componentFileVersionNum;
	dstring		componentIdent[0];
} __attribute__ ((packed));

/* File Entry (ECMA 167r3 4/14.17) */
struct extendedFileEntry {
	struct tag		descTag;
	struct icbtag		icbTag;
	__le32			uid;
	__le32			gid;
	__le32			permissions;
	__le16			fileLinkCount;
	uint8_t			recordFormat;
	uint8_t			recordDisplayAttr;
	__le32			recordLength;
	__le64			informationLength;
	__le64			objectSize;
	__le64			logicalBlocksRecorded;
	struct timestamp	accessTime;
	struct timestamp	modificationTime;
	struct timestamp	createTime;
	struct timestamp	attrTime;
	__le32			checkpoint;
	__le32			reserved;
	struct long_ad		extendedAttrICB;
	struct long_ad		streamDirectoryICB;
	struct regid		impIdent;
	__le64			uniqueID;
	__le32			lengthExtendedAttr;
	__le32			lengthAllocDescs;
	uint8_t			extendedAttr[0];
	uint8_t			allocDescs[0];
} __attribute__ ((packed));

/* OSTA CS0 Charspec (UDF 2.50 2.1.2) */
#define UDF_CHAR_SET_TYPE		0
#define UDF_CHAR_SET_INFO		"OSTA Compressed Unicode"

/* Entity Identifier (UDF 2.50 2.1.5) */
/* Identifiers (UDF 2.50 2.1.5.2) */
#define UDF_ID_DEVELOPER		"*Linux UDFFS"
#define	UDF_ID_COMPLIANT		"*OSTA UDF Compliant"
#define UDF_ID_LV_INFO			"*UDF LV Info"
#define UDF_ID_FREE_EA			"*UDF FreeEASpace"
#define UDF_ID_FREE_APP_EA		"*UDF FreeAppEASpace"
#define UDF_ID_DVD_CGMS			"*UDF DVD CGMS Info"
#define UDF_ID_OS2_EA			"*UDF OS/2 EA"
#define UDF_ID_OS2_EA_LENGTH		"*UDF OS/2 EALength"
#define UDF_ID_MAC_VOLUME		"*UDF Mac VolumeInfo"
#define UDF_ID_MAC_FINDER		"*UDF Mac FinderInfo"
#define UDF_ID_MAC_UNIQUE		"*UDF Mac UniqueIDTable"
#define UDF_ID_MAC_RESOURCE		"*UDF Mac ResourceFork"
#define UDF_ID_VIRTUAL			"*UDF Virtual Partition"
#define UDF_ID_SPARABLE			"*UDF Sparable Partition"
#define UDF_ID_ALLOC			"*UDF Virtual Alloc Tbl"
#define UDF_ID_SPARING			"*UDF Sparing Table"
#define UDF_ID_METADATA			"*UDF Metadata Partition"

/* Identifier Suffix (UDF 2.50 2.1.5.3) */
#define IS_DF_HARD_WRITE_PROTECT	0x01
#define IS_DF_SOFT_WRITE_PROTECT	0x02

struct UDFIdentSuffix {
	__le16		UDFRevision;
	uint8_t		OSClass;
	uint8_t		OSIdentifier;
	uint8_t		reserved[4];
} __attribute__ ((packed));

struct impIdentSuffix {
	uint8_t		OSClass;
	uint8_t		OSIdentifier;
	uint8_t		reserved[6];
} __attribute__ ((packed));

struct appIdentSuffix {
	uint8_t		impUse[8];
} __attribute__ ((packed));

/* Logical Volume Integrity Descriptor (UDF 2.50 2.2.6) */
/* Implementation Use (UDF 2.50 2.2.6.4) */
struct logicalVolIntegrityDescImpUse {
	struct regid	impIdent;
	__le32		numFiles;
	__le32		numDirs;
	__le16		minUDFReadRev;
	__le16		minUDFWriteRev;
	__le16		maxUDFWriteRev;
	uint8_t		impUse[0];
} __attribute__ ((packed));

/* Implementation Use Volume Descriptor (UDF 2.50 2.2.7) */
/* Implementation Use (UDF 2.50 2.2.7.2) */
struct impUseVolDescImpUse {
	struct charspec	LVICharset;
	dstring		logicalVolIdent[128];
	dstring		LVInfo1[36];
	dstring		LVInfo2[36];
	dstring		LVInfo3[36];
	struct regid	impIdent;
	uint8_t		impUse[128];
} __attribute__ ((packed));

struct udfPartitionMap2 {
	uint8_t		partitionMapType;
	uint8_t		partitionMapLength;
	uint8_t		reserved1[2];
	struct regid	partIdent;
	__le16		volSeqNum;
	__le16		partitionNum;
} __attribute__ ((packed));

/* Virtual Partition Map (UDF 2.50 2.2.8) */
struct virtualPartitionMap {
	uint8_t		partitionMapType;
	uint8_t		partitionMapLength;
	uint8_t		reserved1[2];
	struct regid	partIdent;
	__le16		volSeqNum;
	__le16		partitionNum;
	uint8_t		reserved2[24];
} __attribute__ ((packed));

/* Sparable Partition Map (UDF 2.50 2.2.9) */
struct sparablePartitionMap {
	uint8_t partitionMapType;
	uint8_t partitionMapLength;
	uint8_t reserved1[2];
	struct regid partIdent;
	__le16 volSeqNum;
	__le16 partitionNum;
	__le16 packetLength;
	uint8_t numSparingTables;
	uint8_t reserved2[1];
	__le32 sizeSparingTable;
	__le32 locSparingTable[4];
} __attribute__ ((packed));

/* Metadata Partition Map (UDF 2.4.0 2.2.10) */
struct metadataPartitionMap {
	uint8_t		partitionMapType;
	uint8_t		partitionMapLength;
	uint8_t		reserved1[2];
	struct regid	partIdent;
	__le16		volSeqNum;
	__le16		partitionNum;
	__le32		metadataFileLoc;
	__le32		metadataMirrorFileLoc;
	__le32		metadataBitmapFileLoc;
	__le32		allocUnitSize;
	__le16		alignUnitSize;
	uint8_t		flags;
	uint8_t		reserved2[5];
} __attribute__ ((packed));

/* Virtual Allocation Table (UDF 1.5 2.2.10) */
struct virtualAllocationTable15 {
	__le32		VirtualSector[0];
	struct regid	vatIdent;
	__le32		previousVATICBLoc;
} __attribute__ ((packed));

#define ICBTAG_FILE_TYPE_VAT15		0x00U

/* Virtual Allocation Table (UDF 2.50 2.2.11) */
struct virtualAllocationTable20 {
	__le16		lengthHeader;
	__le16		lengthImpUse;
	dstring		logicalVolIdent[128];
	__le32		previousVATICBLoc;
	__le32		numFiles;
	__le32		numDirs;
	__le16		minReadRevision;
	__le16		minWriteRevision;
	__le16		maxWriteRevision;
	__le16		reserved;
	uint8_t		impUse[0];
	__le32		vatEntry[0];
} __attribute__ ((packed));

#define ICBTAG_FILE_TYPE_VAT20		0xF8U

/* Sparing Table (UDF 2.50 2.2.12) */
struct sparingEntry {
	__le32		origLocation;
	__le32		mappedLocation;
} __attribute__ ((packed));

struct sparingTable {
	struct tag	descTag;
	struct regid	sparingIdent;
	__le16		reallocationTableLen;
	__le16		reserved;
	__le32		sequenceNum;
	struct sparingEntry
			mapEntry[0];
} __attribute__ ((packed));

/* Metadata File (and Metadata Mirror File) (UDF 2.50 2.2.13.1) */
#define ICBTAG_FILE_TYPE_MAIN		0xFA
#define ICBTAG_FILE_TYPE_MIRROR		0xFB
#define ICBTAG_FILE_TYPE_BITMAP		0xFC

/* struct struct long_ad ICB - ADImpUse (UDF 2.50 2.2.4.3) */
struct allocDescImpUse {
	__le16		flags;
	uint8_t		impUse[4];
} __attribute__ ((packed));

#define AD_IU_EXT_ERASED		0x0001

/* Real-Time Files (UDF 2.50 6.11) */
#define ICBTAG_FILE_TYPE_REALTIME	0xF9U

/* Implementation Use Extended Attribute (UDF 2.50 3.3.4.5) */
/* FreeEASpace (UDF 2.50 3.3.4.5.1.1) */
struct freeEaSpace {
	__le16		headerChecksum;
	uint8_t		freeEASpace[0];
} __attribute__ ((packed));

/* DVD Copyright Management Information (UDF 2.50 3.3.4.5.1.2) */
struct DVDCopyrightImpUse {
	__le16		headerChecksum;
	uint8_t		CGMSInfo;
	uint8_t		dataType;
	uint8_t		protectionSystemInfo[4];
} __attribute__ ((packed));

/* Application Use Extended Attribute (UDF 2.50 3.3.4.6) */
/* FreeAppEASpace (UDF 2.50 3.3.4.6.1) */
struct freeAppEASpace {
	__le16		headerChecksum;
	uint8_t		freeEASpace[0];
} __attribute__ ((packed));

/* UDF Defined System Stream (UDF 2.50 3.3.7) */
#define UDF_ID_UNIQUE_ID		"*UDF Unique ID Mapping Data"
#define UDF_ID_NON_ALLOC		"*UDF Non-Allocatable Space"
#define UDF_ID_POWER_CAL		"*UDF Power Cal Table"
#define UDF_ID_BACKUP			"*UDF Backup"

/* Operating System Identifiers (UDF 2.50 6.3) */
#define UDF_OS_CLASS_UNDEF		0x00U
#define UDF_OS_CLASS_DOS		0x01U
#define UDF_OS_CLASS_OS2		0x02U
#define UDF_OS_CLASS_MAC		0x03U
#define UDF_OS_CLASS_UNIX		0x04U
#define UDF_OS_CLASS_WIN9X		0x05U
#define UDF_OS_CLASS_WINNT		0x06U
#define UDF_OS_CLASS_OS400		0x07U
#define UDF_OS_CLASS_BEOS		0x08U
#define UDF_OS_CLASS_WINCE		0x09U

#define UDF_OS_ID_UNDEF			0x00U
#define UDF_OS_ID_DOS			0x00U
#define UDF_OS_ID_OS2			0x00U
#define UDF_OS_ID_MAC			0x00U
#define UDF_OS_ID_MAX_OSX		0x01U
#define UDF_OS_ID_UNIX			0x00U
#define UDF_OS_ID_AIX			0x01U
#define UDF_OS_ID_SOLARIS		0x02U
#define UDF_OS_ID_HPUX			0x03U
#define UDF_OS_ID_IRIX			0x04U
#define UDF_OS_ID_LINUX			0x05U
#define UDF_OS_ID_MKLINUX		0x06U
#define UDF_OS_ID_FREEBSD		0x07U
#define UDF_OS_ID_WIN9X			0x00U
#define UDF_OS_ID_WINNT			0x00U
#define UDF_OS_ID_OS400			0x00U
#define UDF_OS_ID_BEOS			0x00U
#define UDF_OS_ID_WINCE			0x00U


struct extent_position {
	struct buffer_head *bh;
	uint32_t offset;
	struct kernel_lb_addr block;
};

struct udf_ext_cache {
	/* Extent position */
	struct extent_position epos;
	/* Start logical offset in bytes */
	loff_t lstart;
};

/*
 * The i_data_sem and i_mutex serve for protection of allocation information
 * of a regular files and symlinks. This includes all extents belonging to
 * the file/symlink, a fact whether data are in-inode or in external data
 * blocks, preallocation, goal block information... When extents are read,
 * i_mutex or i_data_sem must be held (for reading is enough in case of
 * i_data_sem). When extents are changed, i_data_sem must be held for writing
 * and also i_mutex must be held.
 *
 * For directories i_mutex is used for all the necessary protection.
 */

struct udf_inode_info {
	struct timespec		i_crtime;
	/* Physical address of inode */
	struct kernel_lb_addr		i_location;
	__u64			i_unique;
	__u32			i_lenEAttr;
	__u32			i_lenAlloc;
	__u64			i_lenExtents;
	__u32			i_next_alloc_block;
	__u32			i_next_alloc_goal;
	__u32			i_checkpoint;
	unsigned		i_alloc_type : 3;
	unsigned		i_efe : 1;	/* extendedFileEntry */
	unsigned		i_use : 1;	/* unallocSpaceEntry */
	unsigned		i_strat4096 : 1;
	unsigned		reserved : 26;
	union {
		struct short_ad	*i_sad;
		struct long_ad		*i_lad;
		__u8		*i_data;
	} i_ext;
	// struct rw_semaphore	i_data_sem;
	struct udf_ext_cache cached_extent;
	/* Spinlock for protecting extent cache */
	// spinlock_t i_extent_cache_lock;
	struct inode vfs_inode;
};

struct udf_inode_info *UDF_I(struct inode *inode);

#define UDF_SUPER_MAGIC			0x15013346

#define UDF_MAX_READ_VERSION		0x0250
#define UDF_MAX_WRITE_VERSION		0x0201

#define UDF_FLAG_USE_EXTENDED_FE	0
#define UDF_VERS_USE_EXTENDED_FE	0x0200
#define UDF_FLAG_USE_STREAMS		1
#define UDF_VERS_USE_STREAMS		0x0200
#define UDF_FLAG_USE_SHORT_AD		2
#define UDF_FLAG_USE_AD_IN_ICB		3
#define UDF_FLAG_USE_FILE_CTIME_EA	4
#define UDF_FLAG_STRICT			5
#define UDF_FLAG_UNDELETE		6
#define UDF_FLAG_UNHIDE			7
#define UDF_FLAG_VARCONV		8
#define UDF_FLAG_NLS_MAP		9
#define UDF_FLAG_UTF8			10
#define UDF_FLAG_UID_FORGET     11    /* save -1 for uid to disk */
#define UDF_FLAG_UID_IGNORE     12    /* use sb uid instead of on disk uid */
#define UDF_FLAG_GID_FORGET     13
#define UDF_FLAG_GID_IGNORE     14
#define UDF_FLAG_UID_SET	15
#define UDF_FLAG_GID_SET	16
#define UDF_FLAG_SESSION_SET	17
#define UDF_FLAG_LASTBLOCK_SET	18
#define UDF_FLAG_BLOCKSIZE_SET	19

#define UDF_PART_FLAG_UNALLOC_BITMAP	0x0001
#define UDF_PART_FLAG_UNALLOC_TABLE	0x0002
#define UDF_PART_FLAG_FREED_BITMAP	0x0004
#define UDF_PART_FLAG_FREED_TABLE	0x0008
#define UDF_PART_FLAG_READ_ONLY		0x0010
#define UDF_PART_FLAG_WRITE_ONCE	0x0020
#define UDF_PART_FLAG_REWRITABLE	0x0040
#define UDF_PART_FLAG_OVERWRITABLE	0x0080

#define UDF_MAX_BLOCK_LOADED	8

#define UDF_TYPE1_MAP15			0x1511U
#define UDF_VIRTUAL_MAP15		0x1512U
#define UDF_VIRTUAL_MAP20		0x2012U
#define UDF_SPARABLE_MAP15		0x1522U
#define UDF_METADATA_MAP25		0x2511U

#define UDF_INVALID_MODE		((umode_t)-1)

struct udf_meta_data {
	__u32	s_meta_file_loc;
	__u32	s_mirror_file_loc;
	__u32	s_bitmap_file_loc;
	__u32	s_alloc_unit_size;
	__u16	s_align_unit_size;
	int	s_flags;
	struct inode *s_metadata_fe;
	struct inode *s_mirror_fe;
	struct inode *s_bitmap_fe;
};

struct udf_sparing_data {
	__u16	s_packet_len;
	struct buffer_head *s_spar_map[4];
};

struct udf_virtual_data {
	__u32	s_num_entries;
	__u16	s_start_offset;
};

struct udf_bitmap {
	__u32			s_extPosition;
	int			s_nr_groups;
	struct buffer_head 	*s_block_bitmap[0];
};

struct udf_part_map {
	union {
		struct udf_bitmap	*s_bitmap;
		struct inode		*s_table;
	} s_uspace;
	union {
		struct udf_bitmap	*s_bitmap;
		struct inode		*s_table;
	} s_fspace;
	__u32	s_partition_root;
	__u32	s_partition_len;
	__u16	s_partition_type;
	__u16	s_partition_num;
	union {
		struct udf_sparing_data s_sparing;
		struct udf_virtual_data s_virtual;
		struct udf_meta_data s_metadata;
	} s_type_specific;
	__u32	(*s_partition_func)(struct super_block *, __u32, __u16, __u32);
	__u16	s_volumeseqnum;
	__u16	s_partition_flags;
};

#pragma pack()

struct udf_sb_info {
	struct udf_part_map	*s_partmaps;
	__u8			s_volume_ident[32];

	/* Overall info */
	__u16			s_partitions;
	__u16			s_partition;

	/* Sector headers */
	__s32			s_session;
	__u32			s_anchor;
	__u32			s_last_block;

	struct buffer_head	*s_lvid_bh;

	/* Default permissions */
	umode_t			s_umask;
	kgid_t			s_gid;
	kuid_t			s_uid;
	umode_t			s_fmode;
	umode_t			s_dmode;
	/* Lock protecting consistency of above permission settings */
	// rwlock_t		s_cred_lock;

	/* Root Info */
	struct timespec		s_record_time;

	/* Fileset Info */
	__u16			s_serial_number;

	/* highest UDF revision we have recorded to this media */
	__u16			s_udfrev;

	/* Miscellaneous flags */
	unsigned long		s_flags;

	/* Encoding info */
	struct nls_table	*s_nls_map;

	/* VAT inode */
	struct inode		*s_vat_inode;

	struct mutex		s_alloc_mutex;
	/* Protected by s_alloc_mutex */
	unsigned int		s_lvid_dirty;
};

struct udf_sb_info *UDF_SB(struct super_block *sb);
extern struct genericFormat *udf_get_extendedattr(struct inode *, uint32_t,
						  uint8_t);
extern struct buffer_head *udf_tgetblk(struct super_block *, int);

extern struct genericFormat *udf_add_extendedattr(struct inode *, uint32_t,
						  uint32_t, uint8_t);
extern struct genericFormat *udf_get_extendedattr(struct inode *, uint32_t,
						  uint8_t);