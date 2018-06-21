#include "common.h"

enum i2400m_pt {
  I2400M_PT_DATA = 0,
  I2400M_PT_CTRL,
  I2400M_PT_TRACE,        /* For device debug */
  I2400M_PT_RESET_WARM,   /* device reset */
  I2400M_PT_RESET_COLD,   /* USB[transport] reset, like reconnect */
  I2400M_PT_EDATA,        /* Extended RX data */
  I2400M_PT_ILLEGAL
};

/* Misc constants */
enum {
    I2400M_PL_ALIGN = 16,   /* Payload data size alignment */
    I2400M_PL_SIZE_MAX = 0x3EFF,
    I2400M_MAX_PLS_IN_MSG = 60,
    /* protocol barkers: sync sequences; for notifications they
     * are sent in groups of four. */
    I2400M_H2D_PREVIEW_BARKER = 0xcafe900d,
    I2400M_COLD_RESET_BARKER = 0xc01dc01d,
    I2400M_WARM_RESET_BARKER = 0x50f750f7,
    I2400M_NBOOT_BARKER = 0xdeadbeef,
    I2400M_SBOOT_BARKER = 0x0ff1c1a1,
    I2400M_SBOOT_BARKER_6050 = 0x80000001,
    I2400M_ACK_BARKER = 0xfeedbabe,
    I2400M_D2H_MSG_BARKER = 0xbeefbabe,
};

enum {
    /**
     * TX Buffer size
     *
     * Doc says maximum transaction is 16KiB. If we had 16KiB en
     * route and 16KiB being queued, it boils down to needing
     * 32KiB.
     * 32KiB is insufficient for 1400 MTU, hence increasing
     * tx buffer size to 64KiB.
     */
    I2400M_TX_BUF_SIZE = 65536,
    /**
     * Message header and payload descriptors have to be 16
     * aligned (16 + 4 * N = 16 * M). If we take that average sent
     * packets are MTU size (~1400-~1500) it follows that we could
     * fit at most 10-11 payloads in one transaction. To meet the
     * alignment requirement, that means we need to leave space
     * for 12 (64 bytes). To simplify, we leave space for that. If
     * at the end there are less, we pad up to the nearest
     * multiple of 16.
     */
    /*
    * According to Intel Wimax i3200, i5x50 and i6x50 specification
    * documents, the maximum number of payloads per message can be
    * up to 60. Increasing the number of payloads to 60 per message
    * helps to accommodate smaller payloads in a single transaction.
    */
    I2400M_TX_PLD_MAX = 60,
   
    I2400M_TX_SKIP = 0x80000000,
    /*
    * According to Intel Wimax i3200, i5x50 and i6x50 specification
    * documents, the maximum size of each message can be up to 16KiB.
    */
    I2400M_TX_MSG_SIZE = 16384,
};

#define TAIL_FULL ((void *)~(unsigned long)NULL)

struct i2400m_msg_hdr {
    int num_pls;
    int size;
    struct {
        int val;
    } * pld;
};

struct i2400m {
    void * tx_buf;
    struct i2400m_msg_hdr * tx_msg;
    int bus_tx_block_size;
    void * tx_lock;
    void (*bus_tx_kick)(struct i2400m *);
};

struct device {

};

struct device* i2400m_dev(struct i2400m *);
void * i2400m_tx_fifo_push(struct i2400m *, size_t, int);
