/*
 * yuv2rgb_mlib.c
 * Copyright (C) 2000-2002 Håkan Hjort <d95hjort@dtek.chalmers.se>
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

#ifdef LIBVO_MLIB

#include <stddef.h>
#include <mlib_types.h>
#include <mlib_status.h>
#include <mlib_sys.h>
#include <mlib_video.h>
#include <inttypes.h>

#include "video_out.h"
#include "video_out_internal.h"

static void mlib_YUV2ARGB420_32 (uint8_t * image, uint8_t * py,
                                 uint8_t * pu, uint8_t * pv,
                                 int h_size, int v_size,
                                 int rgb_stride, int y_stride, int uv_stride)
{
    mlib_VideoColorYUV2ARGB420 (image, py, pu, pv, h_size,
                                v_size, rgb_stride, y_stride, uv_stride);
}

static void mlib_YUV2ABGR420_32 (uint8_t * image, uint8_t * py,
                                 uint8_t * pu, uint8_t * pv,
                                 int h_size, int v_size,
                                 int rgb_stride, int y_stride, int uv_stride)
{
    mlib_VideoColorYUV2ABGR420 (image, py, pu, pv, h_size,
                                v_size, rgb_stride, y_stride, uv_stride);
}

static void mlib_YUV2RGB420_24 (uint8_t * image, uint8_t * py,
                                uint8_t * pu, uint8_t * pv,
                                int h_size, int v_size,
                                int rgb_stride, int y_stride, int uv_stride)
{
    mlib_VideoColorYUV2RGB420 (image, py, pu, pv, h_size,
                               v_size, rgb_stride, y_stride, uv_stride);
}

int yuv2rgb_init_mlib (int bpp, int mode)
{
    if ((bpp == 24) && (mode == MODE_RGB)) {
        yuv2rgb = mlib_YUV2RGB420_24;
        return 0;
    } else if (bpp == 32) {
        yuv2rgb =
            (mode == MODE_RGB) ? mlib_YUV2ARGB420_32 : mlib_YUV2ABGR420_32;
        return 0;
    } else
        return 1;       /* Fallback to C */
}

#endif /* LIBVO_MLIB */
