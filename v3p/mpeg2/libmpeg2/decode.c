/*
 * decode.c
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
#include <string.h>	/* memcpy/memset, try to remove */
#include <stdlib.h>
#include <inttypes.h>

#include "video_out.h"
#include "mpeg2.h"
#include "mpeg2_internal.h"

#ifdef HAVE_MEMALIGN
/* some systems have memalign() but no declaration for it */
void * memalign (size_t align, size_t size);
#else
/* assume malloc alignment is sufficient */
#define memalign(align,size) malloc (size)
#endif

#define BUFFER_SIZE (1194 * 1024)

void mpeg2_init (mpeg2dec_t * mpeg2dec, uint32_t mm_accel,
		 vo_instance_t * output)
{
    static int do_init = 1;

    if (do_init) {
	do_init = 0;
	mpeg2_cpu_state_init (mm_accel);
	mpeg2_idct_init (mm_accel);
	mpeg2_mc_init (mm_accel);
    }

    mpeg2dec->chunk_buffer = memalign (16, BUFFER_SIZE + 4);
    mpeg2dec->picture = memalign (16, sizeof (picture_t));

    mpeg2dec->shift = 0xffffff00;
    mpeg2dec->is_sequence_needed = 1;
    mpeg2dec->drop_flag = 0;
    mpeg2dec->drop_frame = 0;
    mpeg2dec->in_slice = 0;
    mpeg2dec->output = output;
    mpeg2dec->chunk_ptr = mpeg2dec->chunk_buffer;
    mpeg2dec->code = 0xb4;

    memset (mpeg2dec->picture, 0, sizeof (picture_t));

    /* initialize substructures */
    mpeg2_header_state_init (mpeg2dec->picture);
}

static inline int parse_chunk (mpeg2dec_t * mpeg2dec, int code,
			       uint8_t * buffer)
{
    picture_t * picture;
    int is_frame_done;

    /* wait for sequence_header_code */
    if (mpeg2dec->is_sequence_needed && (code != 0xb3))
	return 0;

    mpeg2_stats (code, buffer);

    picture = mpeg2dec->picture;
    is_frame_done = 0;

    if (mpeg2dec->in_slice && ((!code) || (code >= 0xb0))) {
	mpeg2dec->in_slice = 0;

	if (((picture->picture_structure == FRAME_PICTURE) ||
	     (picture->second_field)) &&
	    (!(mpeg2dec->drop_frame))) {
	    is_frame_done = 1;
	    vo_draw ((picture->picture_coding_type == B_TYPE) ?
		     picture->current_frame :
		     picture->forward_reference_frame);
	}
    }

    switch (code) {
    case 0x00:	/* picture_start_code */
	if (mpeg2_header_picture (picture, buffer)) {
	    fprintf (stderr, "bad picture header\n");
	    exit (1);
	}
	mpeg2dec->drop_frame =
	    mpeg2dec->drop_flag && (picture->picture_coding_type == B_TYPE);
	break;

    case 0xb3:	/* sequence_header_code */
	if (mpeg2_header_sequence (picture, buffer)) {
	    fprintf (stderr, "bad sequence header\n");
	    exit (1);
	}
	if (mpeg2dec->is_sequence_needed) {
	    mpeg2dec->is_sequence_needed = 0;
	    if (vo_setup (mpeg2dec->output, picture->coded_picture_width,
			  picture->coded_picture_height)) {
		fprintf (stderr, "display setup failed\n");
		exit (1);
	    }
	    picture->forward_reference_frame =
		vo_get_frame (mpeg2dec->output,
			      VO_PREDICTION_FLAG | VO_BOTH_FIELDS);
	    picture->backward_reference_frame =
		vo_get_frame (mpeg2dec->output,
			      VO_PREDICTION_FLAG | VO_BOTH_FIELDS);
	}
	mpeg2dec->frame_rate_code = picture->frame_rate_code;	/* FIXME */
	break;

    case 0xb5:	/* extension_start_code */
	if (mpeg2_header_extension (picture, buffer)) {
	    fprintf (stderr, "bad extension\n");
	    exit (1);
	}
	break;

    default:
	if (code >= 0xb9)
	    fprintf (stderr, "stream not demultiplexed ?\n");

	if (code >= 0xb0)
	    break;

	if (!(mpeg2dec->in_slice)) {
	    mpeg2dec->in_slice = 1;

	    if (picture->second_field)
		vo_field (picture->current_frame, picture->picture_structure);
	    else {
		vo_frame_t * frame;

		if (picture->picture_coding_type == B_TYPE)
		    picture->current_frame =
			vo_get_frame (mpeg2dec->output,
				      picture->picture_structure);
		else {
		    picture->current_frame =
			vo_get_frame (mpeg2dec->output,
				      (VO_PREDICTION_FLAG |
				       picture->picture_structure));
		    picture->forward_reference_frame =
			picture->backward_reference_frame;
		    picture->backward_reference_frame = picture->current_frame;
		}

		/* hopefully vektor will be happy */
		frame = picture->current_frame;
		frame->aspect_ratio = picture->aspect_ratio_information;
		frame->frame_rate_code = picture->frame_rate_code;
		frame->bitrate = picture->bitrate;
		frame->progressive_sequence = picture->progressive_sequence;
		frame->progressive_frame = picture->progressive_frame;
		frame->top_field_first = picture->top_field_first;
		frame->repeat_first_field = picture->repeat_first_field;
		frame->picture_coding_type = picture->picture_coding_type;
		frame->pts = mpeg2dec->pts;
	    }
	}

	if (!(mpeg2dec->drop_frame))
	    mpeg2_slice (picture, code, buffer);
    }

    return is_frame_done;
}

static inline uint8_t * copy_chunk (mpeg2dec_t * mpeg2dec,
				    uint8_t * current, uint8_t * end)
{
    uint32_t shift;
    uint8_t * chunk_ptr;
    uint8_t * limit;
    uint8_t byte;

    shift = mpeg2dec->shift;
    chunk_ptr = mpeg2dec->chunk_ptr;
    limit = current + (mpeg2dec->chunk_buffer + BUFFER_SIZE - chunk_ptr);
    if (limit > end)
	limit = end;

    do {
	byte = *current++;
	if (shift == 0x00000100)
	    goto startcode;
	shift = (shift | byte) << 8;
	*chunk_ptr++ = byte;
    } while (current < limit);

    mpeg2dec->bytes_since_pts += chunk_ptr - mpeg2dec->chunk_ptr;
    mpeg2dec->shift = shift;
    if (current == end) {
	mpeg2dec->chunk_ptr = chunk_ptr;
	return NULL;
    } else {
	/* we filled the chunk buffer without finding a start code */
	mpeg2dec->chunk_ptr = mpeg2dec->chunk_buffer;
	mpeg2dec->code = 0xb4;	/* sequence_error_code */
	return current;
    }

startcode:
    mpeg2dec->bytes_since_pts += chunk_ptr + 1 - mpeg2dec->chunk_ptr;
    mpeg2dec->chunk_ptr = mpeg2dec->chunk_buffer;
    mpeg2dec->shift = 0xffffff00;
    mpeg2dec->code = byte;
    if (!byte) {
	if (!mpeg2dec->num_pts)
	    mpeg2dec->pts = 0;	/* none */
	else if (mpeg2dec->bytes_since_pts >= 4) {
	    mpeg2dec->num_pts = 0;
	    mpeg2dec->pts = mpeg2dec->pts_current;
	} else if (mpeg2dec->num_pts > 1) {
	    mpeg2dec->num_pts = 1;
	    mpeg2dec->pts = mpeg2dec->pts_previous;
	} else
	    mpeg2dec->pts = 0;	/* none */
    }
    return current;
}

int mpeg2_decode_data (mpeg2dec_t * mpeg2dec, uint8_t * current, uint8_t * end)
{
    int ret;
    uint8_t code;

    ret = 0;

    while (current != end) {
	code = mpeg2dec->code;
	current = copy_chunk (mpeg2dec, current, end);
	if (current == NULL)
	    return ret;
	ret += parse_chunk (mpeg2dec, code, mpeg2dec->chunk_buffer);
    }
    return ret;
}

void mpeg2_pts (mpeg2dec_t * mpeg2dec, uint32_t pts)
{
    mpeg2dec->pts_previous = mpeg2dec->pts_current;
    mpeg2dec->pts_current = pts;
    mpeg2dec->num_pts++;
    mpeg2dec->bytes_since_pts = 0;
}

void mpeg2_close (mpeg2dec_t * mpeg2dec)
{
    static uint8_t finalizer[] = {0,0,1,0xb4};

    mpeg2_decode_data (mpeg2dec, finalizer, finalizer+4);

    if (! (mpeg2dec->is_sequence_needed))
	vo_draw (mpeg2dec->picture->backward_reference_frame);

    free (mpeg2dec->chunk_buffer);
    free (mpeg2dec->picture);
}

void mpeg2_drop (mpeg2dec_t * mpeg2dec, int flag)
{
    mpeg2dec->drop_flag = flag;
}
