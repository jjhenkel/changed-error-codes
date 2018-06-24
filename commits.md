# Commits Used for Logs

## 2 - `iwl_dump_nic_event_log`

```
commit 937c397eb633b804d9a806d08c395ecfc42b1fec
Author: Wey-Yi Guy <wey-yi.w.guy@intel.com>
Date:   Fri Jan 15 13:43:36 2010 -0800

    iwlwifi: correct return code for log_event
    
    When dumping event log in debugfs, iwl_dump_nic_event_log()
    should return the correct error code instead of let the calling
    function makes it own assumption.
    
    Signed-off-by: Wey-Yi Guy <wey-yi.w.guy@intel.com>
    Signed-off-by: Reinette Chatre <reinette.chatre@intel.com>
    Signed-off-by: John W. Linville <linville@tuxdriver.com>

diff --git a/drivers/net/wireless/iwlwifi/iwl-agn.c b/drivers/net/wireless/iwlwifi/iwl-agn.c
index c780633..eaa309d 100644
--- a/drivers/net/wireless/iwlwifi/iwl-agn.c
+++ b/drivers/net/wireless/iwlwifi/iwl-agn.c
@@ -1965,7 +1965,7 @@ int iwl_dump_nic_event_log(struct iwl_priv *priv, bool full_log,
                IWL_ERR(priv,
                        "Invalid event log pointer 0x%08X for %s uCode\n",
                        base, (priv->ucode_type == UCODE_INIT) ? "Init" : "RT");
-               return pos;
+               return -EINVAL;
        }
 
        /* event log header */
@@ -2013,7 +2013,7 @@ int iwl_dump_nic_event_log(struct iwl_priv *priv, bool full_log,
                        bufsz = size * 48;
                *buf = kmalloc(bufsz, GFP_KERNEL);
                if (!*buf)
-                       return pos;
+                       return -ENOMEM;
        }
        if ((iwl_get_debug_level(priv) & IWL_DL_FW_ERRORS) || full_log) {
```

## 3 - `iwl3945_dump_nic_event_log`

```
commit 937c397eb633b804d9a806d08c395ecfc42b1fec
Author: Wey-Yi Guy <wey-yi.w.guy@intel.com>
Date:   Fri Jan 15 13:43:36 2010 -0800

    iwlwifi: correct return code for log_event
    
    When dumping event log in debugfs, iwl_dump_nic_event_log()
    should return the correct error code instead of let the calling
    function makes it own assumption.
    
    Signed-off-by: Wey-Yi Guy <wey-yi.w.guy@intel.com>
    Signed-off-by: Reinette Chatre <reinette.chatre@intel.com>
    Signed-off-by: John W. Linville <linville@tuxdriver.com>

diff --git a/drivers/net/wireless/iwlwifi/iwl3945-base.c b/drivers/net/wireless/iwlwifi/iwl3945-base.c
index 10b0aa8..8bbcc54 100644
--- a/drivers/net/wireless/iwlwifi/iwl3945-base.c
+++ b/drivers/net/wireless/iwlwifi/iwl3945-base.c
@@ -1644,7 +1644,7 @@ int iwl3945_dump_nic_event_log(struct iwl_priv *priv, bool full_log,
        base = le32_to_cpu(priv->card_alive.log_event_table_ptr);
        if (!iwl3945_hw_valid_rtc_data_addr(base)) {
                IWL_ERR(priv, "Invalid event log pointer 0x%08X\n", base);
-               return pos;
+               return  -EINVAL;
        }
 
        /* event log header */
@@ -1693,7 +1693,7 @@ int iwl3945_dump_nic_event_log(struct iwl_priv *priv, bool full_log,
                        bufsz = size * 48;
                *buf = kmalloc(bufsz, GFP_KERNEL);
                if (!*buf)
-                       return pos;
+                       return -ENOMEM;
        }
        if ((iwl_get_debug_level(priv) & IWL_DL_FW_ERRORS) || full_log) {
                /* if uCode has wrapped back to top of log,
```

## 4 - `iwl_dbgfs_chain_tx_power_read`

```
commit 0af0d04b8db7712498558541cbedeeebdbabf70f
Author: Wey-Yi Guy <wey-yi.w.guy@intel.com>
Date:   Mon Apr 26 16:08:27 2010 -0700

    iwlwifi: give correct return information for tx power debugfs
    
    Return -EAGAIN when request tx power information and uCode is not ready;
    so it will not confuse with tx power information not available.
    
    Signed-off-by: Wey-Yi Guy <wey-yi.w.guy@intel.com>
    Signed-off-by: Reinette Chatre <reinette.chatre@intel.com>

diff --git a/drivers/net/wireless/iwlwifi/iwl-debugfs.c b/drivers/net/wireless/iwlwifi/iwl-debugfs.c
index df34c31..06905bb 100644
--- a/drivers/net/wireless/iwlwifi/iwl-debugfs.c
+++ b/drivers/net/wireless/iwlwifi/iwl-debugfs.c
@@ -1225,7 +1225,7 @@ static ssize_t iwl_dbgfs_chain_tx_power_read(struct file *file,
        struct statistics_tx *tx;
 
        if (!iwl_is_alive(priv))
-               pos += scnprintf(buf + pos, bufsz - pos, "N/A\n");
+               return -EAGAIN;
        else {
                tx = &priv->statistics.tx;
                if (tx->tx_power.ant_a ||
```

## 5 - `i2400m_tx`

```
commit 4818d14de36228a749f714b450a85c1b35542e6f
Author: Prasanna S.Panchamukhi <prasannax.s.panchamukhi@intel.com>
Date:   Mon Jan 18 14:28:23 2010 -0800

    wimax/i2400m: fix incorrect return -ESHUTDOWN when there is no Tx buffer available
    
    i2400m_tx() routine was returning -ESHUTDOWN even when there was no Tx buffer
    available. This patch fixes the i2400m_tx() to return -ESHUTDOWN only when
    the device is down(i2400m->tx_buf is NULL) and also to return -ENOSPC
    when there is no Tx buffer. Error seen in the kernel log.
    kernel: i2400m_sdio mmc0:0001:1: can't send message 0x5606: -108
    kernel: i2400m_sdio mmc0:0001:1: Failed to issue 'Enter power save'command: -108
    
    Signed-off-by: Prasanna S.Panchamukhi <prasannax.s.panchamukhi@intel.com>

diff --git a/drivers/net/wimax/i2400m/tx.c b/drivers/net/wimax/i2400m/tx.c
index 6db909e..fab27e4 100644
--- a/drivers/net/wimax/i2400m/tx.c
+++ b/drivers/net/wimax/i2400m/tx.c
@@ -643,9 +643,11 @@ int i2400m_tx(struct i2400m *i2400m, const void *buf, size_t buf_len,
         * current one is out of payload slots or we have a singleton,
         * close it and start a new one */
        spin_lock_irqsave(&i2400m->tx_lock, flags);
-       result = -ESHUTDOWN;
-       if (i2400m->tx_buf == NULL)
+       /* If tx_buf is NULL, device is shutdown */
+       if (i2400m->tx_buf == NULL) {
+               result = -ESHUTDOWN;
                goto error_tx_new;
+       }
 try_new:
        if (unlikely(i2400m->tx_msg == NULL))
                i2400m_tx_new(i2400m);
```

## 6 - `adfs_fplus_read`

```
commit b796410630a0f090864d7595c6bffbc0136f0f8c
Author: Sanidhya Kashyap <sanidhya.gatech@gmail.com>
Date:   Thu Apr 16 12:48:13 2015 -0700

    adfs: return correct return values
    
    Fix the wrong values returned by various functions such as EIO and ENOMEM.
    
    Signed-off-by: Sanidhya Kashyap <sanidhya.gatech@gmail.com>
    Cc: Fabian Frederick <fabf@skynet.be>
    Cc: Joe Perches <joe@perches.com>
    Cc: Taesoo kim <taesoo@gatech.edu>
    Signed-off-by: Andrew Morton <akpm@linux-foundation.org>
    Signed-off-by: Linus Torvalds <torvalds@linux-foundation.org>

diff --git a/fs/adfs/dir_fplus.c b/fs/adfs/dir_fplus.c
index f2ba88a..82d14cd 100644
--- a/fs/adfs/dir_fplus.c
+++ b/fs/adfs/dir_fplus.c
@@ -61,6 +61,7 @@
                        kcalloc(size, sizeof(struct buffer_head *),
                                GFP_KERNEL);
                if (!bh_fplus) {
+                       ret = -ENOMEM;
                        adfs_error(sb, "not enough memory for"
                                        " dir object %X (%d blocks)", id, size);
                        goto out;
```

## 7 - `adfs_fill_super`

```
commit b796410630a0f090864d7595c6bffbc0136f0f8c
Author: Sanidhya Kashyap <sanidhya.gatech@gmail.com>
Date:   Thu Apr 16 12:48:13 2015 -0700

    adfs: return correct return values
    
    Fix the wrong values returned by various functions such as EIO and ENOMEM.
    
    Signed-off-by: Sanidhya Kashyap <sanidhya.gatech@gmail.com>
    Cc: Fabian Frederick <fabf@skynet.be>
    Cc: Joe Perches <joe@perches.com>
    Cc: Taesoo kim <taesoo@gatech.edu>
    Signed-off-by: Andrew Morton <akpm@linux-foundation.org>
    Signed-off-by: Linus Torvalds <torvalds@linux-foundation.org>

@@ -370,6 +370,7 @@ static int adfs_fill_super(struct super_block *sb, void *data, int silent)
        unsigned char *b_data;
        struct adfs_sb_info *asb;
        struct inode *root;
+       int ret = -EINVAL;
 
        sb->s_flags |= MS_NODIRATIME;
 
@@ -391,6 +392,7 @@ static int adfs_fill_super(struct super_block *sb, void *data, int silent)
        sb_set_blocksize(sb, BLOCK_SIZE);
        if (!(bh = sb_bread(sb, ADFS_DISCRECORD / BLOCK_SIZE))) {
                adfs_error(sb, "unable to read superblock");
+               ret = -EIO;
                goto error;
        }
 
@@ -400,6 +402,7 @@ static int adfs_fill_super(struct super_block *sb, void *data, int silent)
                if (!silent)
                        printk("VFS: Can't find an adfs filesystem on dev "
                                "%s.\n", sb->s_id);
+               ret = -EINVAL;
                goto error_free_bh;
        }
 
@@ -412,6 +415,7 @@ static int adfs_fill_super(struct super_block *sb, void *data, int silent)
                if (!silent)
                        printk("VPS: Can't find an adfs filesystem on dev "
                                "%s.\n", sb->s_id);
+               ret = -EINVAL;
                goto error_free_bh;
        }
 
@@ -421,11 +425,13 @@ static int adfs_fill_super(struct super_block *sb, void *data, int silent)
                if (!bh) {
                        adfs_error(sb, "couldn't read superblock on "
                                "2nd try.");
+                       ret = -EIO;
                        goto error;
                }
                b_data = bh->b_data + (ADFS_DISCRECORD % sb->s_blocksize);
                if (adfs_checkbblk(b_data)) {
                        adfs_error(sb, "disc record mismatch, very weird!");
+                       ret = -EINVAL;
                        goto error_free_bh;
                }
                dr = (struct adfs_discrecord *)(b_data + ADFS_DR_OFFSET);
@@ -433,6 +439,7 @@ static int adfs_fill_super(struct super_block *sb, void *data, int silent)
                if (!silent)
                        printk(KERN_ERR "VFS: Unsupported blocksize on dev "
                                "%s.\n", sb->s_id);
+               ret = -EINVAL;
                goto error;
        }
 
@@ -447,10 +454,12 @@ static int adfs_fill_super(struct super_block *sb, void *data, int silent)
        asb->s_size             = adfs_discsize(dr, sb->s_blocksize_bits);
        asb->s_version          = dr->format_version;
        asb->s_log2sharesize    = dr->log2sharesize;
-       
+
        asb->s_map = adfs_read_map(sb, dr);
-       if (!asb->s_map)
+       if (IS_ERR(asb->s_map)) {
+               ret =  PTR_ERR(asb->s_map);
                goto error_free_bh;
+       }
 
        brelse(bh);
 
@@ -499,6 +508,7 @@ static int adfs_fill_super(struct super_block *sb, void *data, int silent)
                        brelse(asb->s_map[i].dm_bh);
                kfree(asb->s_map);
                adfs_error(sb, "get root inode failed\n");
+               ret = -EIO;
                goto error;
        }
        return 0;
@@ -508,7 +518,7 @@ static int adfs_fill_super(struct super_block *sb, void *data, int silent)
 error:
        sb->s_fs_info = NULL;
        kfree(asb);
-       return -EINVAL;
+       return ret;
 }
```

## 8 - `bfs_create`

```
commit c3fe5872eb3f5f9e027d61d8a3f5d092168fdbca
Author: Sanidhya Kashyap <sanidhya.gatech@gmail.com>
Date:   Thu Apr 16 12:48:32 2015 -0700

    bfs: correct return values
    
    In case of failed memory allocation, the return should be ENOMEM instead
    of ENOSPC.
    
    Return -EIO when sb_bread() fails.
    
    Signed-off-by: Sanidhya Kashyap <sanidhya.gatech@gmail.com>
    Cc: Tigran Aivazian <tigran@aivazian.fsnet.co.uk>
    Signed-off-by: Andrew Morton <akpm@linux-foundation.org>
    Signed-off-by: Linus Torvalds <torvalds@linux-foundation.org>

diff --git a/fs/bfs/dir.c b/fs/bfs/dir.c
index 08063ae..7a818277 100644
--- a/fs/bfs/dir.c
+++ b/fs/bfs/dir.c
@@ -86,7 +86,7 @@ static int bfs_create(struct inode *dir, struct dentry *dentry, umode_t mode,
 
        inode = new_inode(s);
        if (!inode)
-               return -ENOSPC;
+               return -ENOMEM;
        mutex_lock(&info->bfs_lock);
        ino = find_first_zero_bit(info->si_imap, info->si_lasti + 1);
        if (ino > info->si_lasti) {
@@ -293,7 +293,7 @@ static int bfs_add_entry(struct inode *dir, const unsigned char *name,
        for (block = sblock; block <= eblock; block++) {
                bh = sb_bread(dir->i_sb, block);
                if (!bh)
-                       return -ENOSPC;
+                       return -EIO;
                for (off = 0; off < BFS_BSIZE; off += BFS_DIRENT_SIZE) {
                        de = (struct bfs_dirent *)(bh->b_data + off);
                        if (!de->ino) {
```

## 9 - `affs_fill_super`

```
commit c8f33d0bec999a4f2b5c3f9380361b88ce6f6ab0
Author: Sanidhya Kashyap <sanidhya.gatech@gmail.com>
Date:   Thu Apr 16 12:48:26 2015 -0700

    affs: kstrdup() memory handling
    
    There is a possibility of kstrdup() failure upon memory pressure.
    Therefore, returning ENOMEM even for new_opts.
    
    [akpm@linux-foundation.org: cleanup]
    Signed-off-by: Sanidhya Kashyap <sanidhya.gatech@gmail.com>
    Cc: Taesoo kim <taesoo@gatech.edu>
    Cc: Fabian Frederick <fabf@skynet.be>
    Signed-off-by: Andrew Morton <akpm@linux-foundation.org>
    Signed-off-by: Linus Torvalds <torvalds@linux-foundation.org>

diff --git a/fs/affs/super.c b/fs/affs/super.c
index 6819d04..3f89c9e 100644
--- a/fs/affs/super.c
+++ b/fs/affs/super.c
@@ -521,10 +521,14 @@ static int affs_fill_super(struct super_block *sb, void *data, int silent)
        int                      root_block;
        unsigned long            mount_flags;
        int                      res = 0;
-       char                    *new_opts = kstrdup(data, GFP_KERNEL);
+       char                    *new_opts;
        char                     volume[32];
        char                    *prefix = NULL;
 
+       new_opts = kstrdup(data, GFP_KERNEL);
+       if (!new_opts)
+               return -ENOMEM;
+
        pr_debug("%s(flags=0x%x,opts=\"%s\")\n", __func__, *flags, data);
 
        sync_filesystem(sb);
```

