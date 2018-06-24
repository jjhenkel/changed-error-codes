#include "16-fake.h"

static int
vidioc_g_register(struct file *file, void *priv,
		  struct v4l2_dbg_register *reg)
{
    int             ret;
    struct gspca_dev *gspca_dev = video_drvdata(file);

    if (!gspca_dev->sd_desc->get_chip_ident)
	return -ENOTTY;

    if (!gspca_dev->sd_desc->get_register)
	return -ENOTTY;

    if (mutex_lock_interruptible(&gspca_dev->usb_lock))
	return -ERESTARTSYS;
    gspca_dev->usb_err = 0;
    if (gspca_dev->present)
	ret = gspca_dev->sd_desc->get_register(gspca_dev, reg);
    else
	ret = -ENODEV;
    mutex_unlock(&gspca_dev->usb_lock);

    return ret;
}
