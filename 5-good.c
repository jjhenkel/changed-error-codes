#include "5-fake.h"

int
i2400m_tx(struct i2400m *i2400m, const void *buf, size_t buf_len,
          enum i2400m_pt pl_type)
{
    int             result = -ENOSPC;
    struct device  *dev = i2400m_dev(i2400m);
    unsigned long   flags;
    size_t          padded_len;
    void           *ptr;
    unsigned        is_singleton = pl_type == I2400M_PT_RESET_WARM
        || pl_type == I2400M_PT_RESET_COLD;

    d_fnstart(3, dev, "(i2400m %p skb %p [%zu bytes] pt %u)\n",
              i2400m, buf, buf_len, pl_type);
    padded_len = ALIGN(buf_len, I2400M_PL_ALIGN);
    d_printf(5, dev, "padded_len %zd buf_len %zd\n", padded_len, buf_len);
    /*
     * If there is no current TX message, create one; if the current one
     * is out of payload slots or we have a singleton, close it and start
     * a new one
     */
    spin_lock_irqsave(&i2400m->tx_lock, flags);
    /*
     * If tx_buf is NULL, device is shutdown
     */
    if (i2400m->tx_buf == NULL) {
        result = -ESHUTDOWN;
        goto error_tx_new;
    }
  try_new:
    if (unlikely(i2400m->tx_msg == NULL))
        i2400m_tx_new(i2400m);
    else if (unlikely(!i2400m_tx_fits(i2400m)
                      || (is_singleton && i2400m->tx_msg->num_pls != 0))) {
        d_printf(2, dev, "closing TX message (fits %u singleton "
                 "%u num_pls %u)\n", i2400m_tx_fits(i2400m),
                 is_singleton, i2400m->tx_msg->num_pls);
        i2400m_tx_close(i2400m);
        i2400m_tx_new(i2400m);
    }
    if (i2400m->tx_msg == NULL)
        goto error_tx_new;
    if (i2400m->tx_msg->size + padded_len > I2400M_TX_BUF_SIZE / 2) {
        d_printf(2, dev, "TX: message too big, going new\n");
        i2400m_tx_close(i2400m);
        i2400m_tx_new(i2400m);
    }
    if (i2400m->tx_msg == NULL)
        goto error_tx_new;
    /*
     * So we have a current message header; now append space for the
     * message -- if there is not enough, try the head
     */
    ptr = i2400m_tx_fifo_push(i2400m, padded_len,
                              i2400m->bus_tx_block_size);
    if (ptr == TAIL_FULL) {     /* Tail is full, try head */
        d_printf(2, dev, "pl append: tail full\n");
        i2400m_tx_close(i2400m);
        i2400m_tx_skip_tail(i2400m);
        goto try_new;
    } else if (ptr == NULL) {   /* All full */
        result = -ENOSPC;
        d_printf(2, dev, "pl append: all full\n");
    } else {                    /* Got space, copy it, set padding */
        struct i2400m_msg_hdr *tx_msg = i2400m->tx_msg;
        unsigned        num_pls = le16_to_cpu(tx_msg->num_pls);
        memcpy(ptr, buf, buf_len);
        memset(ptr + buf_len, 0xad, padded_len - buf_len);
        i2400m_pld_set(&tx_msg->pld[num_pls], buf_len, pl_type);
        d_printf(3, dev, "pld 0x%08x (type 0x%1x len 0x%04zx\n",
                 le32_to_cpu(tx_msg->pld[num_pls].val), pl_type, buf_len);
        tx_msg->num_pls = le16_to_cpu(num_pls + 1);
        tx_msg->size += padded_len;
        d_printf(2, dev, "TX: appended %zu b (up to %u b) pl #%u\n",
                 padded_len, tx_msg->size, num_pls + 1);
        d_printf(2, dev,
                 "TX: appended hdr @%zu %zu b pl #%u @%zu %zu/%zu b\n",
                 (void *) tx_msg - i2400m->tx_buf, (size_t) tx_msg->size,
                 num_pls + 1, ptr - i2400m->tx_buf, buf_len, padded_len);
        result = 0;
        if (is_singleton)
            i2400m_tx_close(i2400m);
    }
  error_tx_new:
    spin_unlock_irqrestore(&i2400m->tx_lock, flags);
    /*
     * kick in most cases, except when the TX subsys is down, as it might
     * free space
     */
    if (likely(result != -ESHUTDOWN))
        i2400m->bus_tx_kick(i2400m);
    d_fnend(3, dev, "(i2400m %p skb %p [%zu bytes] pt %u) = %d\n",
            i2400m, buf, buf_len, pl_type, result);
    return result;
}