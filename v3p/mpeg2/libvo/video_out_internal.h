/*
 * video_out_internal.h
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

extern uint32_t vo_mm_accel;

int libvo_common_alloc_frames (vo_instance_t * instance, int width, int height,
			       int frame_size,
			       void (* copy) (vo_frame_t *, uint8_t **),
			       void (* field) (vo_frame_t *, int),
			       void (* draw) (vo_frame_t *));
void libvo_common_free_frames (vo_instance_t * instance);
vo_frame_t * libvo_common_get_frame (vo_instance_t * instance, int prediction);

#define MODE_RGB  0x1
#define MODE_BGR  0x2

extern void (* yuv2rgb) (uint8_t * image, uint8_t * py,
                         uint8_t * pu, uint8_t * pv, int h_size, int v_size,
                         int rgb_stride, int y_stride, int uv_stride);

void yuv2rgb_init (int bpp, int mode);
int yuv2rgb_init_mmxext (int bpp, int mode);
int yuv2rgb_init_mmx (int bpp, int mode);
int yuv2rgb_init_mlib (int bpp, int mode);
