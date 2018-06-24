#include "common.h"

/* GSPCA debug codes */

#define D_PROBE  1
#define D_CONF   2
#define D_STREAM 3
#define D_FRAM   4
#define D_PACK   5
#define D_USBI   6
#define D_USBO   7

#define ERESTARTSYS	512
#define ERESTARTNOINTR	513
#define ERESTARTNOHAND	514	/* restart if no handler.. */
#define ENOIOCTLCMD	515	/* No ioctl command */
#define ERESTART_RESTARTBLOCK 516 /* restart by calling sys_restart_syscall */
#define EPROBE_DEFER	517	/* Driver requests probe retry */
#define EOPENSTALE	518	/* open found a stale dentry */

/* Defined for the NFSv3 protocol */
#define EBADHANDLE	521	/* Illegal NFS file handle */
#define ENOTSYNC	522	/* Update synchronization mismatch */
#define EBADCOOKIE	523	/* Cookie is stale */
#define ENOTSUPP	524	/* Operation is not supported */
#define ETOOSMALL	525	/* Buffer or request is too small */
#define ESERVERFAULT	526	/* An untranslatable error occurred */
#define EBADTYPE	527	/* Type not supported by server */
#define EJUKEBOX	528	/* Request initiated, but will not complete before timeout */
#define EIOCBQUEUED	529	/* iocb queued, will get completion event */


extern int gspca_debug;

struct file {};
struct mutex {};
struct v4l2_buffer {};
struct usb_interface {};
struct v4l2_ctrl_handler {};
struct v4l2_pix_format {};
struct video_device {};
struct v4l2_device {};
struct usb_device_id {};
struct v4l2_jpegcompression {};
struct v4l2_streamparm {};
struct v4l2_dbg_register {};
struct v4l2_dbg_chip_info {};
struct v4l2_frmsizeenum {};
struct v4l2_format {};

#define PDEBUG(level, fmt, ...) \
	v4l2_dbg(level, gspca_debug, &gspca_dev->v4l2_dev, fmt, ##__VA_ARGS__)

#define PERR(fmt, ...) \
	v4l2_err(&gspca_dev->v4l2_dev, fmt, ##__VA_ARGS__)

#define GSPCA_MAX_FRAMES 16	/* maximum number of video frame buffers */
/* image transfers */
#define MAX_NURBS 4		/* max number of URBs */


/* used to list framerates supported by a camera mode (resolution) */
struct framerates {
	const u8 *rates;
	int nrates;
};

/* device information - set at probe time */
struct cam {
	const struct v4l2_pix_format *cam_mode;	/* size nmodes */
	const struct framerates *mode_framerates; /* must have size nmodes,
						   * just like cam_mode */
	u32 bulk_size;		/* buffer size when image transfer by bulk */
	u32 input_flags;	/* value for ENUM_INPUT status flags */
	u8 nmodes;		/* size of cam_mode */
	u8 no_urb_create;	/* don't create transfer URBs */
	u8 bulk_nurbs;		/* number of URBs in bulk mode
				 * - cannot be > MAX_NURBS
				 * - when 0 and bulk_size != 0 means
				 *   1 URB and submit done by subdriver */
	u8 bulk;		/* image transfer by 0:isoc / 1:bulk */
	u8 npkt;		/* number of packets in an ISOC message
				 * 0 is the default value: 32 packets */
	u8 needs_full_bandwidth;/* Set this flag to notify the bandwidth calc.
				 * code that the cam fills all image buffers to
				 * the max, even when using compression. */
};

struct gspca_dev;
struct gspca_frame;

/* subdriver operations */
typedef int (*cam_op) (struct gspca_dev *);
typedef void (*cam_v_op) (struct gspca_dev *);
typedef int (*cam_cf_op) (struct gspca_dev *, const struct usb_device_id *);
typedef int (*cam_get_jpg_op) (struct gspca_dev *,
				struct v4l2_jpegcompression *);
typedef int (*cam_set_jpg_op) (struct gspca_dev *,
				const struct v4l2_jpegcompression *);
typedef int (*cam_get_reg_op) (struct gspca_dev *,
				struct v4l2_dbg_register *);
typedef int (*cam_set_reg_op) (struct gspca_dev *,
				const struct v4l2_dbg_register *);
typedef int (*cam_chip_info_op) (struct gspca_dev *,
				struct v4l2_dbg_chip_info *);
typedef void (*cam_streamparm_op) (struct gspca_dev *,
				  struct v4l2_streamparm *);
typedef void (*cam_pkt_op) (struct gspca_dev *gspca_dev,
				u8 *data,
				int len);
typedef int (*cam_int_pkt_op) (struct gspca_dev *gspca_dev,
				u8 *data,
				int len);
typedef void (*cam_format_op) (struct gspca_dev *gspca_dev,
				struct v4l2_format *fmt);
typedef int (*cam_frmsize_op) (struct gspca_dev *gspca_dev,
				struct v4l2_frmsizeenum *fsize);

/* subdriver description */
struct sd_desc {
/* information */
	const char *name;	/* sub-driver name */
/* mandatory operations */
	cam_cf_op config;	/* called on probe */
	cam_op init;		/* called on probe and resume */
	cam_op init_controls;	/* called on probe */
	cam_op start;		/* called on stream on after URBs creation */
	cam_pkt_op pkt_scan;
/* optional operations */
	cam_op isoc_init;	/* called on stream on before getting the EP */
	cam_op isoc_nego;	/* called when URB submit failed with NOSPC */
	cam_v_op stopN;		/* called on stream off - main alt */
	cam_v_op stop0;		/* called on stream off & disconnect - alt 0 */
	cam_v_op dq_callback;	/* called when a frame has been dequeued */
	cam_get_jpg_op get_jcomp;
	cam_set_jpg_op set_jcomp;
	cam_streamparm_op get_streamparm;
	cam_streamparm_op set_streamparm;
	cam_format_op try_fmt;
	cam_frmsize_op enum_framesizes;
// #ifdef CONFIG_VIDEO_ADV_DEBUG
	cam_set_reg_op set_register;
	cam_get_reg_op get_register;
	cam_chip_info_op get_chip_ident;
// #endif
};

/* packet types when moving from iso buf to frame buf */
enum gspca_packet_type {
	DISCARD_PACKET,
	FIRST_PACKET,
	INTER_PACKET,
	LAST_PACKET
};

struct gspca_frame {
	__u8 *data;			/* frame buffer */
	int vma_use_count;
	struct v4l2_buffer v4l2_buf;
};

struct gspca_dev {
	struct video_device vdev;	/* !! must be the first item */
	struct module *module;		/* subdriver handling the device */
	struct v4l2_device v4l2_dev;
	struct usb_device *dev;
	struct file *capt_file;		/* file doing video capture */
					/* protected by queue_lock */
// #if IS_ENABLED(CONFIG_INPUT)
	struct input_dev *input_dev;
	char phys[64];			/* physical device path */
// #endif

	struct cam cam;				/* device information */
	const struct sd_desc *sd_desc;		/* subdriver description */
	struct v4l2_ctrl_handler ctrl_handler;

	/* autogain and exposure or gain control cluster, these are global as
	   the autogain/exposure functions in autogain_functions.c use them */
	struct {
		struct v4l2_ctrl *autogain;
		struct v4l2_ctrl *exposure;
		struct v4l2_ctrl *gain;
		int exp_too_low_cnt, exp_too_high_cnt;
	};

#define USB_BUF_SZ 64
	__u8 *usb_buf;				/* buffer for USB exchanges */
	struct urb *urb[MAX_NURBS];
// #if IS_ENABLED(CONFIG_INPUT)
	struct urb *int_urb;
// #endif

	__u8 *frbuf;				/* buffer for nframes */
	struct gspca_frame frame[GSPCA_MAX_FRAMES];
	u8 *image;				/* image beeing filled */
	__u32 frsz;				/* frame size */
	u32 image_len;				/* current length of image */
	atomic_t fr_q;				/* next frame to queue */
	atomic_t fr_i;				/* frame being filled */
	signed char fr_queue[GSPCA_MAX_FRAMES];	/* frame queue */
	char nframes;				/* number of frames */
	u8 fr_o;				/* next frame to dequeue */
	__u8 last_packet_type;
	__s8 empty_packet;		/* if (-1) don't check empty packets */
	__u8 streaming;			/* protected by both mutexes (*) */

	__u8 curr_mode;			/* current camera mode */
	struct v4l2_pix_format pixfmt;	/* current mode parameters */
	__u32 sequence;			/* frame sequence number */

	// wait_queue_head_t wq;		/* wait queue */
	struct mutex usb_lock;		/* usb exchange protection */
	// struct mutex queue_lock;	/* ISOC queue protection */
	int usb_err;			/* USB error - protected by usb_lock */
	u16 pkt_size;			/* ISOC packet size */
#ifdef CONFIG_PM
	char frozen;			/* suspend - resume */
#endif
	char present;			/* device connected */
	char nbufread;			/* number of buffers for read() */
	char memory;			/* memory type (V4L2_MEMORY_xxx) */
	__u8 iface;			/* USB interface number */
	__u8 alt;			/* USB alternate setting */
	int xfer_ep;			/* USB transfer endpoint address */
	u8 audio;			/* presence of audio device */

	/* (*) These variables are proteced by both usb_lock and queue_lock,
	   that is any code setting them is holding *both*, which means that
	   any code getting them needs to hold at least one of them */
};

int gspca_dev_probe(struct usb_interface *intf,
		const struct usb_device_id *id,
		const struct sd_desc *sd_desc,
		int dev_size,
		struct module *module);
int gspca_dev_probe2(struct usb_interface *intf,
		const struct usb_device_id *id,
		const struct sd_desc *sd_desc,
		int dev_size,
		struct module *module);
void gspca_disconnect(struct usb_interface *intf);
void gspca_frame_add(struct gspca_dev *gspca_dev,
			enum gspca_packet_type packet_type,
			const u8 *data,
			int len);
#ifdef CONFIG_PM
int gspca_suspend(struct usb_interface *intf, pm_message_t message);
int gspca_resume(struct usb_interface *intf);
#endif
int gspca_expo_autogain(struct gspca_dev *gspca_dev, int avg_lum,
	int desired_avg_lum, int deadzone, int gain_knee, int exposure_knee);
int gspca_coarse_grained_expo_autogain(struct gspca_dev *gspca_dev,
	int avg_lum, int desired_avg_lum, int deadzone);

struct video_device *video_devdata(struct file *file);
void *video_drvdata(struct file *file);
