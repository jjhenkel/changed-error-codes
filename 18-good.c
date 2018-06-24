#include "18-fake.h"

static int
vidioc_g_chip_ident(struct file *file, void *priv,
		    struct v4l2_dbg_chip_ident *chip)
{
    int             ret;
    struct gspca_dev *gspca_dev = video_drvdata(file);

    if (!gspca_dev->sd_desc->get_chip_ident)
	return -ENOTTY;

    if (mutex_lock_interruptible(&gspca_dev->usb_lock))
	return -ERESTARTSYS;
    gspca_dev->usb_err = 0;
    if (gspca_dev->present)
	ret = gspca_dev->sd_desc->get_chip_ident(gspca_dev, chip);
    else
	ret = -ENODEV;
    mutex_unlock(&gspca_dev->usb_lock);

    return ret;
}
