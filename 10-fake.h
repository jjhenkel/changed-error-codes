#include "common.h"

#define HASH_LEN_DECLARE u32 hash; u32 len

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

struct inode {
  unsigned long		i_ino;
};

struct inode *hfs_new_inode(struct inode *dir, struct qstr *name, umode_t mode);
