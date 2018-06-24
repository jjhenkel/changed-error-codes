#include "15-fake.h"

static int
__qbuf_userptr(struct vb2_buffer *vb, struct v4l2_buffer *b)
{
    struct v4l2_plane planes[VIDEO_MAX_PLANES];
    struct vb2_queue *q = vb->vb2_queue;
    void           *mem_priv;
    unsigned int    plane;
    int             ret;
    int             write = !V4L2_TYPE_IS_OUTPUT(q->type);

    /*
     * Verify and copy relevant information provided by the userspace 
     */
    ret = __fill_vb2_buffer(vb, b, planes);
    if (ret)
	return ret;

    for (plane = 0; plane < vb->num_planes; ++plane) {
	/*
	 * Skip the plane if already verified 
	 */
	if (vb->v4l2_planes[plane].m.userptr == planes[plane].m.userptr
	    && vb->v4l2_planes[plane].length == planes[plane].length)
	    continue;

	dprintk(3, "qbuf: userspace address for plane %d changed, "
		"reacquiring memory\n", plane);

	/*
	 * Check if the provided plane buffer is large enough 
	 */
	if (planes[plane].length < q->plane_sizes[plane]) {
	    ret = EINVAL;
	    goto err;
	}

	/*
	 * Release previously acquired memory if present 
	 */
	if (vb->planes[plane].mem_priv)
	    call_memop(vb, put_userptr, vb->planes[plane].mem_priv);

	vb->planes[plane].mem_priv = NULL;
	vb->v4l2_planes[plane].m.userptr = 0;
	vb->v4l2_planes[plane].length = 0;

	/*
	 * Acquire each plane's memory 
	 */
	if (q->mem_ops->get_userptr) {
	    mem_priv = q->mem_ops->get_userptr(q->alloc_ctx[plane],
					       planes[plane].m.userptr,
					       planes[plane].length,
					       write);
	    if (IS_ERR(mem_priv)) {
		dprintk(1, "qbuf: failed acquiring userspace "
			"memory for plane %d\n", plane);
		ret = PTR_ERR(mem_priv);
		goto err;
	    }
	    vb->planes[plane].mem_priv = mem_priv;
	}
    }

    /*
     * Call driver-specific initialization on the newly acquired buffer,
     * if provided.
     */
    ret = call_qop(q, buf_init, vb);
    if (ret) {
	dprintk(1, "qbuf: buffer initialization failed\n");
	goto err;
    }

    /*
     * Now that everything is in order, copy relevant information
     * provided by userspace.
     */
    for (plane = 0; plane < vb->num_planes; ++plane)
	vb->v4l2_planes[plane] = planes[plane];

    return 0;
  err:
    /*
     * In case of errors, release planes that were already acquired 
     */
    for (plane = 0; plane < vb->num_planes; ++plane) {
	if (vb->planes[plane].mem_priv)
	    call_memop(vb, put_userptr, vb->planes[plane].mem_priv);
	vb->planes[plane].mem_priv = NULL;
	vb->v4l2_planes[plane].m.userptr = 0;
	vb->v4l2_planes[plane].length = 0;
    }

    return ret;
}
