/*
 * video_out_pgm.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "video_out.h"
#include "video_out_internal.h"

typedef struct pgm_instance_s {
    vo_instance_t vo;
    int prediction_index;
    vo_frame_t * frame_ptr[3];
    vo_frame_t frame[3];
    int width;
    int height;
    int framenum;
    char header[1024];
    char filename[128];
} pgm_instance_t;

static void internal_draw_frame (pgm_instance_t * instance, FILE * file,
                                 vo_frame_t * frame)
{
    int i;

    fwrite (instance->header, strlen (instance->header), 1, file);
    fwrite (frame->base[0], instance->width, instance->height, file);
    for (i = 0; i < instance->height >> 1; i++) {
        fwrite (frame->base[1]+i*instance->width/2, instance->width/2, 1,
                file);
        fwrite (frame->base[2]+i*instance->width/2, instance->width/2, 1,
                file);
    }
}

static int internal_setup (vo_instance_t * instance_, int width, int height,
                           void (* draw_frame) (vo_frame_t *))
{
    pgm_instance_t * instance;

    instance = (pgm_instance_t *) instance_;

    instance->vo.close = libvo_common_free_frames;
    instance->vo.get_frame = libvo_common_get_frame;
    instance->width = width;
    instance->height = height;
    sprintf (instance->header, "P5\n\n%d %d\n255\n", width, height * 3 / 2);
    return libvo_common_alloc_frames ((vo_instance_t *) instance,
                                      width, height, sizeof (vo_frame_t),
                                      NULL, NULL, draw_frame);
}

static void pgm_draw_frame (vo_frame_t * frame)
{
    pgm_instance_t * instance;
    FILE * file;

    instance = (pgm_instance_t *) frame->instance;
    if (++(instance->framenum) < 0)
        return;
    sprintf (instance->filename, "%d.pgm", instance->framenum);
    file = fopen (instance->filename, "wb");
    if (!file)
        return;
    internal_draw_frame (instance, file, frame);
    fclose (file);
}

static int pgm_setup (vo_instance_t * instance, int width, int height)
{
    return internal_setup (instance, width, height, pgm_draw_frame);
}

vo_instance_t * vo_pgm_open (void)
{
    pgm_instance_t * instance;

    instance = malloc (sizeof (pgm_instance_t));
    if (instance == NULL)
        return NULL;

    instance->vo.setup = pgm_setup;
    instance->framenum = -2;
    return (vo_instance_t *) instance;
}

static void pgmpipe_draw_frame (vo_frame_t * frame)
{
    pgm_instance_t * instance;

    instance = (pgm_instance_t *)frame->instance;
    if (++(instance->framenum) >= 0)
        internal_draw_frame (instance, stdout, frame);
}

static int pgmpipe_setup (vo_instance_t * instance, int width, int height)
{
    return internal_setup (instance, width, height, pgmpipe_draw_frame);
}

vo_instance_t * vo_pgmpipe_open (void)
{
    pgm_instance_t * instance;

    instance = malloc (sizeof (pgm_instance_t));
    if (instance == NULL)
        return NULL;

    instance->vo.setup = pgmpipe_setup;
    instance->framenum = -2;
    return (vo_instance_t *) instance;
}

static void md5_draw_frame (vo_frame_t * frame)
{
    pgm_instance_t * instance;
    char buf[100];

    instance = (pgm_instance_t *) frame->instance;
    pgm_draw_frame (frame);
    if (instance->framenum < 0)
        return;
    sprintf (buf, "md5sum -b %s", instance->filename);
    system (buf);
    remove (instance->filename);
}

static int md5_setup (vo_instance_t * instance, int width, int height)
{
    return internal_setup (instance, width, height, md5_draw_frame);
}

vo_instance_t * vo_md5_open (void)
{
    pgm_instance_t * instance;

    instance = malloc (sizeof (pgm_instance_t));
    if (instance == NULL)
        return NULL;

    instance->vo.setup = md5_setup;
    instance->framenum = -2;
    return (vo_instance_t *) instance;
}
