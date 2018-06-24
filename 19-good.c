#include "19-fake.h"

static int
vidioc_querymenu(struct file *file, void *priv,
		 struct v4l2_querymenu *qmenu)
{
    struct gspca_dev *gspca_dev = video_drvdata(file);

    if (!gspca_dev->sd_desc->querymenu)
	return -ENOTTY;
    return gspca_dev->sd_desc->querymenu(gspca_dev, qmenu);
}
