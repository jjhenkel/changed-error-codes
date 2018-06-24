#include "1-fake.h"

static int
vidioc_s_jpegcomp(struct file *file, void *priv,
		  struct v4l2_jpegcompression *jpegcomp)
{
    struct gspca_dev *gspca_dev = video_drvdata(file);
    int             ret;

    if (!gspca_dev->sd_desc->set_jcomp)
	return -ENOTTY;
    if (mutex_lock_interruptible(&gspca_dev->usb_lock))
	return -ERESTARTSYS;
    gspca_dev->usb_err = 0;
    if (gspca_dev->present)
	ret = gspca_dev->sd_desc->set_jcomp(gspca_dev, jpegcomp);
    else
	ret = -ENODEV;
    mutex_unlock(&gspca_dev->usb_lock);
    return ret;
}
