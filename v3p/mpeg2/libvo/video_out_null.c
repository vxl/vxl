/* 
 * video_out_null.c
 * Copyright (C) 2000-2002 Michel Lespinasse <walken@zoy.org>
 * Copyright (C) 1999-2000 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
 *
 * This file is part of mpeg2dec, a free MPEG-2 video stream decoder.
 * See http://libmpeg2.sourceforge.net/ for updates.
 *
 * mpeg2dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * mpeg2dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include <stdlib.h>
#include <inttypes.h>

#include "video_out.h"
#include "video_out_internal.h"

typedef struct null_frame_s {
    vo_frame_t vo;
    uint8_t * rgb_ptr;
    int rgb_stride;
    int yuv_stride;
} null_frame_t;

typedef struct null_instance_s {
    vo_instance_t vo;
    int prediction_index;
    vo_frame_t * frame_ptr[3];
    null_frame_t frame[3];
    uint8_t * rgbdata;
    int rgbstride;
    int width;
    int bpp;
} null_instance_t;

static void null_draw_frame (vo_frame_t * frame)
{
}

static int null_setup (vo_instance_t * instance, int width, int height)
{
    return libvo_common_alloc_frames (instance, width, height,
				      sizeof (null_frame_t),
				      NULL, NULL, null_draw_frame);
}

vo_instance_t * vo_null_open (void)
{
    null_instance_t * instance;

    instance = malloc (sizeof (null_instance_t));
    if (instance == NULL)
	return NULL;

    instance->vo.setup = null_setup;
    instance->vo.close = libvo_common_free_frames;
    instance->vo.get_frame = libvo_common_get_frame;

    return (vo_instance_t *) instance;
}

static void null_copy_slice (vo_frame_t * frame, uint8_t ** src)
{
}

static int nullslice_setup (vo_instance_t * instance, int width, int height)
{
    return libvo_common_alloc_frames (instance, width, height,
				      sizeof (null_frame_t),
				      null_copy_slice, NULL, null_draw_frame);
}

vo_instance_t * vo_nullslice_open (void)
{
    null_instance_t * instance;

    instance = malloc (sizeof (null_instance_t));
    if (instance == NULL)
	return NULL;

    instance->vo.setup = nullslice_setup;
    instance->vo.close = libvo_common_free_frames;
    instance->vo.get_frame = libvo_common_get_frame;

    return (vo_instance_t *) instance;
}

static vo_frame_t * rgb_get_frame (vo_instance_t * _instance, int flags)
{
    null_instance_t * instance;
    null_frame_t * frame;

    instance = (null_instance_t *) _instance;
    frame =
	(null_frame_t *) libvo_common_get_frame ((vo_instance_t *) instance,
						 flags);

    frame->rgb_ptr = instance->rgbdata;
    frame->rgb_stride = instance->rgbstride;
    frame->yuv_stride = instance->width;
    if ((flags & VO_TOP_FIELD) == 0)
        frame->rgb_ptr += frame->rgb_stride;
    if ((flags & VO_BOTH_FIELDS) != VO_BOTH_FIELDS) {
        frame->rgb_stride <<= 1;
        frame->yuv_stride <<= 1;
    }

    return (vo_frame_t *) frame;
}

static void rgb_copy_slice (vo_frame_t * _frame, uint8_t ** src)
{
    null_frame_t * frame;
    null_instance_t * instance;

    frame = (null_frame_t *) _frame;
    instance = (null_instance_t *) frame->vo.instance;

    yuv2rgb (frame->rgb_ptr, src[0], src[1], src[2], instance->width, 16,
	     frame->rgb_stride, frame->yuv_stride, frame->yuv_stride >> 1);
    frame->rgb_ptr += frame->rgb_stride << 4;
}

static void rgb_field (vo_frame_t * _frame, int flags)
{
    null_frame_t * frame;
    null_instance_t * instance;

    frame = (null_frame_t *) _frame;
    instance = (null_instance_t *) frame->vo.instance;

    frame->rgb_ptr = instance->rgbdata;
    if ((flags & VO_TOP_FIELD) == 0)
	frame->rgb_ptr += instance->rgbstride;
}

static int nullrgb_setup (vo_instance_t * _instance, int width, int height)
{
    null_instance_t * instance;

    instance = (null_instance_t *) _instance;

    instance->width = width;
    instance->rgbstride = width * instance->bpp / 8;
    instance->rgbdata = malloc (instance->rgbstride * height);

    yuv2rgb_init (instance->bpp, MODE_RGB);

    return libvo_common_alloc_frames ((vo_instance_t *) instance,
				      width, height, sizeof (null_frame_t),
				      rgb_copy_slice, rgb_field,
				      null_draw_frame);
}

vo_instance_t * vo_nullrgb16_open (void)
{
    null_instance_t * instance;

    instance = malloc (sizeof (null_instance_t));
    if (instance == NULL)
	return NULL;

    instance->vo.setup = nullrgb_setup;
    instance->vo.close = libvo_common_free_frames;
    instance->vo.get_frame = rgb_get_frame;
    instance->bpp = 16;

    return (vo_instance_t *) instance;
}

vo_instance_t * vo_nullrgb32_open (void)
{
    null_instance_t * instance;

    instance = malloc (sizeof (null_instance_t));
    if (instance == NULL)
	return NULL;

    instance->vo.setup = nullrgb_setup;
    instance->vo.close = libvo_common_free_frames;
    instance->vo.get_frame = rgb_get_frame;
    instance->bpp = 32;

    return (vo_instance_t *) instance;
}
