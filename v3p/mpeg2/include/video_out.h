/*
 * video_out.h
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

#ifndef VIDEO_OUT_H
#define VIDEO_OUT_H

typedef struct vo_frame_s vo_frame_t;
typedef struct vo_instance_s vo_instance_t;

struct vo_frame_s {
    uint8_t * base[3];	/* pointer to 3 planes */
    void (* copy) (vo_frame_t * frame, uint8_t ** src);
    void (* field) (vo_frame_t * frame, int flags);
    void (* draw) (vo_frame_t * frame);
    vo_instance_t * instance;

    /* ONLY for 0.2.x release - will not stay there later */
    int aspect_ratio;
    int frame_rate_code;
    int bitrate;
    int progressive_sequence;
    int progressive_frame;
    int top_field_first;
    int repeat_first_field;
    int picture_coding_type;
    uint32_t pts;
};

typedef vo_instance_t * vo_open_t (void);

struct vo_instance_s {
    int (* setup) (vo_instance_t * instance, int width, int height);
    void (* close) (vo_instance_t * instance);
    vo_frame_t * (* get_frame) (vo_instance_t * instance, int flags);
};

typedef struct vo_driver_s {
    char * name;
    vo_open_t * open;
} vo_driver_t;

void vo_accel (uint32_t accel);

/* return NULL terminated array of all drivers */
vo_driver_t * vo_drivers (void);

static inline vo_instance_t * vo_open (vo_open_t * open)
{
    return open ();
}

static inline int vo_setup (vo_instance_t * instance, int width, int height)
{
    return instance->setup (instance, width, height);
}

static inline void vo_close (vo_instance_t * instance)
{
    if (instance->close)
	instance->close (instance);
}

#define VO_TOP_FIELD 1
#define VO_BOTTOM_FIELD 2
#define VO_BOTH_FIELDS (VO_TOP_FIELD | VO_BOTTOM_FIELD)
#define VO_PREDICTION_FLAG 4

static inline vo_frame_t * vo_get_frame (vo_instance_t * instance, int flags)
{
    return instance->get_frame (instance, flags);
}

static inline void vo_field (vo_frame_t * frame, int flags)
{
    if (frame->field)
	frame->field (frame, flags);
}

static inline void vo_draw (vo_frame_t * frame)
{
    frame->draw (frame);
}

#endif /* VIDEO_OUT_H */
