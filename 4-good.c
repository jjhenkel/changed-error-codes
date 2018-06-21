#include "4-fake.h"

static ssize_t
iwl_dbgfs_chain_tx_power_read(struct file *file,
                              char * user_buf,
                              size_t count, loff_t * ppos)
{

    struct iwl_priv *priv = file->private_data;
    char            buf[128];
    int             pos = 0;
    const size_t    bufsz = sizeof(buf);
    struct statistics_tx *tx;

    if (!iwl_is_alive(priv))
        return -EAGAIN;
    else {
        tx = &priv->statistics.tx;
        if (tx->tx_power.ant_a || tx->tx_power.ant_b || tx->tx_power.ant_c) {
            pos += scnprintf(buf + pos, bufsz - pos,
                             "tx power: (1/2 dB step)\n");
            if ((priv->cfg->valid_tx_ant & ANT_A) && tx->tx_power.ant_a)
                pos += scnprintf(buf + pos, bufsz - pos,
                                 "\tantenna A: 0x%X\n",
                                 tx->tx_power.ant_a);
            if ((priv->cfg->valid_tx_ant & ANT_B) && tx->tx_power.ant_b)
                pos += scnprintf(buf + pos, bufsz - pos,
                                 "\tantenna B: 0x%X\n",
                                 tx->tx_power.ant_b);
            if ((priv->cfg->valid_tx_ant & ANT_C) && tx->tx_power.ant_c)
                pos += scnprintf(buf + pos, bufsz - pos,
                                 "\tantenna C: 0x%X\n",
                                 tx->tx_power.ant_c);
        } else
            pos += scnprintf(buf + pos, bufsz - pos, "N/A\n");
    }
    return simple_read_from_buffer(user_buf, count, ppos, buf, pos);
}