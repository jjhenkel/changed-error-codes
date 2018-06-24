#include "14-fake.h"

static int
udf_update_inode(struct inode *inode, int do_sync)
{
    struct buffer_head *bh = NULL;
    struct fileEntry *fe;
    struct extendedFileEntry *efe;
    uint64_t        lb_recorded;
    uint32_t        udfperms;
    uint16_t        icbflags;
    uint16_t        crclen;
    int             err = 0;
    struct udf_sb_info *sbi = UDF_SB(inode->i_sb);
    unsigned char   blocksize_bits = inode->i_sb->s_blocksize_bits;
    struct udf_inode_info *iinfo = UDF_I(inode);

    bh = udf_tgetblk(inode->i_sb,
		     udf_get_lb_pblock(inode->i_sb, &iinfo->i_location,
				       0));
    if (!bh) {
	udf_debug("getblk failure\n");
	return -ENOMEM;
    }

    lock_buffer(bh);
    memset(bh->b_data, 0, inode->i_sb->s_blocksize);
    fe = (struct fileEntry *) bh->b_data;
    efe = (struct extendedFileEntry *) bh->b_data;

    if (iinfo->i_use) {
	struct unallocSpaceEntry *use =
	    (struct unallocSpaceEntry *) bh->b_data;

	use->lengthAllocDescs = cpu_to_le32(iinfo->i_lenAlloc);
	memcpy(bh->b_data + sizeof(struct unallocSpaceEntry),
	       iinfo->i_ext.i_data, inode->i_sb->s_blocksize -
	       sizeof(struct unallocSpaceEntry));
	use->descTag.tagIdent = cpu_to_le16(TAG_IDENT_USE);
	use->descTag.tagLocation =
	    cpu_to_le32(iinfo->i_location.logicalBlockNum);
	crclen = sizeof(struct unallocSpaceEntry) +
	    iinfo->i_lenAlloc - sizeof(struct tag);
	use->descTag.descCRCLength = cpu_to_le16(crclen);
	use->descTag.descCRC = cpu_to_le16(crc_itu_t(0, (char *) use +
						     sizeof(struct tag),
						     crclen));
	use->descTag.tagChecksum = udf_tag_checksum(&use->descTag);

	goto out;
    }

    if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_UID_FORGET))
	fe->uid = cpu_to_le32(-1);
    else
	fe->uid = cpu_to_le32(i_uid_read(inode));

    if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_GID_FORGET))
	fe->gid = cpu_to_le32(-1);
    else
	fe->gid = cpu_to_le32(i_gid_read(inode));

    udfperms = ((inode->i_mode & S_IRWXO)) |
	((inode->i_mode & S_IRWXG) << 2) |
	((inode->i_mode & S_IRWXU) << 4);

    udfperms |= (le32_to_cpu(fe->permissions) &
		 (FE_PERM_O_DELETE | FE_PERM_O_CHATTR |
		  FE_PERM_G_DELETE | FE_PERM_G_CHATTR |
		  FE_PERM_U_DELETE | FE_PERM_U_CHATTR));
    fe->permissions = cpu_to_le32(udfperms);

    if (S_ISDIR(inode->i_mode) && inode->i_nlink > 0)
	fe->fileLinkCount = cpu_to_le16(inode->i_nlink - 1);
    else
	fe->fileLinkCount = cpu_to_le16(inode->i_nlink);

    fe->informationLength = cpu_to_le64(inode->i_size);

    if (S_ISCHR(inode->i_mode) || S_ISBLK(inode->i_mode)) {
	struct regid   *eid;
	struct deviceSpec *dsea =
	    (struct deviceSpec *) udf_get_extendedattr(inode, 12, 1);
	if (!dsea) {
	    dsea = (struct deviceSpec *)
		udf_add_extendedattr(inode,
				     sizeof(struct deviceSpec) +
				     sizeof(struct regid), 12, 0x3);
	    dsea->attrType = cpu_to_le32(12);
	    dsea->attrSubtype = 1;
	    dsea->attrLength = cpu_to_le32(sizeof(struct deviceSpec) +
					   sizeof(struct regid));
	    dsea->impUseLength = cpu_to_le32(sizeof(struct regid));
	}
	eid = (struct regid *) dsea->impUse;
	memset(eid, 0, sizeof(struct regid));
	strcpy(eid->ident, UDF_ID_DEVELOPER);
	eid->identSuffix[0] = UDF_OS_CLASS_UNIX;
	eid->identSuffix[1] = UDF_OS_ID_LINUX;
	dsea->majorDeviceIdent = cpu_to_le32(imajor(inode));
	dsea->minorDeviceIdent = cpu_to_le32(iminor(inode));
    }

    if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
	lb_recorded = 0;	/* No extents => no blocks! */
    else
	lb_recorded =
	    (inode->i_blocks + (1 << (blocksize_bits - 9)) - 1) >>
	    (blocksize_bits - 9);

    if (iinfo->i_efe == 0) {
	memcpy(bh->b_data + sizeof(struct fileEntry),
	       iinfo->i_ext.i_data,
	       inode->i_sb->s_blocksize - sizeof(struct fileEntry));
	fe->logicalBlocksRecorded = cpu_to_le64(lb_recorded);

	udf_time_to_disk_stamp(&fe->accessTime, inode->i_atime);
	udf_time_to_disk_stamp(&fe->modificationTime, inode->i_mtime);
	udf_time_to_disk_stamp(&fe->attrTime, inode->i_ctime);
	memset(&(fe->impIdent), 0, sizeof(struct regid));
	strcpy(fe->impIdent.ident, UDF_ID_DEVELOPER);
	fe->impIdent.identSuffix[0] = UDF_OS_CLASS_UNIX;
	fe->impIdent.identSuffix[1] = UDF_OS_ID_LINUX;
	fe->uniqueID = cpu_to_le64(iinfo->i_unique);
	fe->lengthExtendedAttr = cpu_to_le32(iinfo->i_lenEAttr);
	fe->lengthAllocDescs = cpu_to_le32(iinfo->i_lenAlloc);
	fe->checkpoint = cpu_to_le32(iinfo->i_checkpoint);
	fe->descTag.tagIdent = cpu_to_le16(TAG_IDENT_FE);
	crclen = sizeof(struct fileEntry);
    } else {
	memcpy(bh->b_data + sizeof(struct extendedFileEntry),
	       iinfo->i_ext.i_data,
	       inode->i_sb->s_blocksize -
	       sizeof(struct extendedFileEntry));
	efe->objectSize = cpu_to_le64(inode->i_size);
	efe->logicalBlocksRecorded = cpu_to_le64(lb_recorded);

	if (iinfo->i_crtime.tv_sec > inode->i_atime.tv_sec ||
	    (iinfo->i_crtime.tv_sec == inode->i_atime.tv_sec &&
	     iinfo->i_crtime.tv_nsec > inode->i_atime.tv_nsec))
	    iinfo->i_crtime = inode->i_atime;

	if (iinfo->i_crtime.tv_sec > inode->i_mtime.tv_sec ||
	    (iinfo->i_crtime.tv_sec == inode->i_mtime.tv_sec &&
	     iinfo->i_crtime.tv_nsec > inode->i_mtime.tv_nsec))
	    iinfo->i_crtime = inode->i_mtime;

	if (iinfo->i_crtime.tv_sec > inode->i_ctime.tv_sec ||
	    (iinfo->i_crtime.tv_sec == inode->i_ctime.tv_sec &&
	     iinfo->i_crtime.tv_nsec > inode->i_ctime.tv_nsec))
	    iinfo->i_crtime = inode->i_ctime;

	udf_time_to_disk_stamp(&efe->accessTime, inode->i_atime);
	udf_time_to_disk_stamp(&efe->modificationTime, inode->i_mtime);
	udf_time_to_disk_stamp(&efe->createTime, iinfo->i_crtime);
	udf_time_to_disk_stamp(&efe->attrTime, inode->i_ctime);

	memset(&(efe->impIdent), 0, sizeof(struct regid));
	strcpy(efe->impIdent.ident, UDF_ID_DEVELOPER);
	efe->impIdent.identSuffix[0] = UDF_OS_CLASS_UNIX;
	efe->impIdent.identSuffix[1] = UDF_OS_ID_LINUX;
	efe->uniqueID = cpu_to_le64(iinfo->i_unique);
	efe->lengthExtendedAttr = cpu_to_le32(iinfo->i_lenEAttr);
	efe->lengthAllocDescs = cpu_to_le32(iinfo->i_lenAlloc);
	efe->checkpoint = cpu_to_le32(iinfo->i_checkpoint);
	efe->descTag.tagIdent = cpu_to_le16(TAG_IDENT_EFE);
	crclen = sizeof(struct extendedFileEntry);
    }
    if (iinfo->i_strat4096) {
	fe->icbTag.strategyType = cpu_to_le16(4096);
	fe->icbTag.strategyParameter = cpu_to_le16(1);
	fe->icbTag.numEntries = cpu_to_le16(2);
    } else {
	fe->icbTag.strategyType = cpu_to_le16(4);
	fe->icbTag.numEntries = cpu_to_le16(1);
    }

    if (S_ISDIR(inode->i_mode))
	fe->icbTag.fileType = ICBTAG_FILE_TYPE_DIRECTORY;
    else if (S_ISREG(inode->i_mode))
	fe->icbTag.fileType = ICBTAG_FILE_TYPE_REGULAR;
    else if (S_ISLNK(inode->i_mode))
	fe->icbTag.fileType = ICBTAG_FILE_TYPE_SYMLINK;
    else if (S_ISBLK(inode->i_mode))
	fe->icbTag.fileType = ICBTAG_FILE_TYPE_BLOCK;
    else if (S_ISCHR(inode->i_mode))
	fe->icbTag.fileType = ICBTAG_FILE_TYPE_CHAR;
    else if (S_ISFIFO(inode->i_mode))
	fe->icbTag.fileType = ICBTAG_FILE_TYPE_FIFO;
    else if (S_ISSOCK(inode->i_mode))
	fe->icbTag.fileType = ICBTAG_FILE_TYPE_SOCKET;

    icbflags = iinfo->i_alloc_type |
	((inode->i_mode & S_ISUID) ? ICBTAG_FLAG_SETUID : 0) |
	((inode->i_mode & S_ISGID) ? ICBTAG_FLAG_SETGID : 0) |
	((inode->i_mode & S_ISVTX) ? ICBTAG_FLAG_STICKY : 0) |
	(le16_to_cpu(fe->icbTag.flags) &
	 ~(ICBTAG_FLAG_AD_MASK | ICBTAG_FLAG_SETUID |
	   ICBTAG_FLAG_SETGID | ICBTAG_FLAG_STICKY));

    fe->icbTag.flags = cpu_to_le16(icbflags);
    if (sbi->s_udfrev >= 0x0200)
	fe->descTag.descVersion = cpu_to_le16(3);
    else
	fe->descTag.descVersion = cpu_to_le16(2);
    fe->descTag.tagSerialNum = cpu_to_le16(sbi->s_serial_number);
    fe->descTag.tagLocation =
	cpu_to_le32(iinfo->i_location.logicalBlockNum);
    crclen += iinfo->i_lenEAttr + iinfo->i_lenAlloc - sizeof(struct tag);
    fe->descTag.descCRCLength = cpu_to_le16(crclen);
    fe->descTag.descCRC =
	cpu_to_le16(crc_itu_t
		    (0, (char *) fe + sizeof(struct tag), crclen));
    fe->descTag.tagChecksum = udf_tag_checksum(&fe->descTag);

  out:
    set_buffer_uptodate(bh);
    unlock_buffer(bh);

    /*
     * write the data blocks 
     */
    mark_buffer_dirty(bh);
    if (do_sync) {
	sync_dirty_buffer(bh);
	if (buffer_write_io_error(bh)) {
	    udf_warn(inode->i_sb, "IO error syncing udf inode [%08lx]\n",
		     inode->i_ino);
	    err = -EIO;
	}
    }
    brelse(bh);

    return err;
}
