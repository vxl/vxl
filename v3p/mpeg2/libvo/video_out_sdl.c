/*
 * video_out_sdl.c
 *
 * Copyright (C) 2000-2002 Ryan C. Gordon <icculus@lokigames.com> and
 *                         Dominik Schnitzer <aeneas@linuxvideo.org>
 *
 * SDL info, source, and binaries can be found at http://www.libsdl.org/
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

#ifdef LIBVO_SDL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <inttypes.h>

#include "video_out.h"
#include "video_out_internal.h"

typedef struct sdl_frame_s {
    vo_frame_t vo;
    SDL_Overlay * overlay;
} sdl_frame_t;

typedef struct sdl_instance_s {
    vo_instance_t vo;
    int prediction_index;
    vo_frame_t * frame_ptr[3];
    sdl_frame_t frame[3];

    SDL_Surface * surface;
    Uint32 sdlflags;
    Uint8 bpp;
} sdl_instance_t;

static vo_frame_t * sdl_get_frame (vo_instance_t * _instance, int flags)
{
    sdl_instance_t * instance;
    sdl_frame_t * frame;

    instance = (sdl_instance_t *) _instance;
    frame = (sdl_frame_t *) libvo_common_get_frame ((vo_instance_t *) instance,
						    flags);
    SDL_LockYUVOverlay (frame->overlay);
    return (vo_frame_t *) frame;
}

static void check_events (sdl_instance_t * instance)
{
    SDL_Event event;

    while (SDL_PollEvent (&event))
	if (event.type == SDL_VIDEORESIZE)
	    instance->surface =
		SDL_SetVideoMode (event.resize.w, event.resize.h,
				  instance->bpp, instance->sdlflags);
}

static void sdl_draw_frame (vo_frame_t * _frame)
{
    sdl_frame_t * frame;
    sdl_instance_t * instance;

    frame = (sdl_frame_t *) _frame;
    instance = (sdl_instance_t *) frame->vo.instance;

    SDL_UnlockYUVOverlay (frame->overlay);
    SDL_DisplayYUVOverlay (frame->overlay, &(instance->surface->clip_rect));
    check_events (instance);
}

static int sdl_alloc_frames (sdl_instance_t * instance, int width, int height)
{
    int i;

    for (i = 0; i < 3; i++) {
	instance->frame[i].overlay =
	    SDL_CreateYUVOverlay (width, height, SDL_YV12_OVERLAY,
				  instance->surface);
	if (instance->frame[i].overlay == NULL)
	    return 1;
	instance->frame_ptr[i] = (vo_frame_t *) (instance->frame + i);
	instance->frame[i].vo.base[0] = instance->frame[i].overlay->pixels[0];
	instance->frame[i].vo.base[1] = instance->frame[i].overlay->pixels[2];
	instance->frame[i].vo.base[2] = instance->frame[i].overlay->pixels[1];
	instance->frame[i].vo.copy = NULL;
	instance->frame[i].vo.field = NULL;
	instance->frame[i].vo.draw = sdl_draw_frame;
	instance->frame[i].vo.instance = (vo_instance_t *) instance;
    }

    return 0;
}

static void sdl_close (vo_instance_t * _instance)
{
    sdl_instance_t * instance;
    int i;

    instance = (sdl_instance_t *) _instance;
    for (i = 0; i < 3; i++)
	SDL_FreeYUVOverlay (instance->frame[i].overlay);
    SDL_FreeSurface (instance->surface);
    SDL_QuitSubSystem (SDL_INIT_VIDEO);
}

static int sdl_setup (vo_instance_t * _instance, int width, int height)
{
    sdl_instance_t * instance;

    instance = (sdl_instance_t *) _instance;

    instance->surface = SDL_SetVideoMode (width, height, instance->bpp,
					  instance->sdlflags);
    if (! (instance->surface)) {
	fprintf (stderr, "sdl could not set the desired video mode\n");
	return 1;
    }

    if (sdl_alloc_frames (instance, width, height)) {
	fprintf (stderr, "sdl could not allocate frame buffers\n");
	return 1;
    }

    return 0;
}

vo_instance_t * vo_sdl_open (void)
{
    sdl_instance_t * instance;
    const SDL_VideoInfo * vidInfo;

    instance = malloc (sizeof (sdl_instance_t));
    if (instance == NULL)
	return NULL;

    instance->vo.setup = sdl_setup;
    instance->vo.close = sdl_close;
    instance->vo.get_frame = sdl_get_frame;

    instance->surface = NULL;
    instance->sdlflags = SDL_HWSURFACE | SDL_RESIZABLE;

    putenv("SDL_VIDEO_YUV_HWACCEL=1");
    putenv("SDL_VIDEO_X11_NODIRECTCOLOR=1");

    if (SDL_Init (SDL_INIT_VIDEO)) {
	fprintf (stderr, "sdl video initialization failed.\n");
	return NULL;
    }

    vidInfo = SDL_GetVideoInfo ();
    if (!SDL_ListModes (vidInfo->vfmt, SDL_HWSURFACE | SDL_RESIZABLE)) {
	instance->sdlflags = SDL_RESIZABLE;
	if (!SDL_ListModes (vidInfo->vfmt, SDL_RESIZABLE)) {
	    fprintf (stderr, "sdl couldn't get any acceptable video mode\n");
	    return NULL;
	}
    }
    instance->bpp = vidInfo->vfmt->BitsPerPixel;
    if (instance->bpp < 16) {
	fprintf(stderr, "sdl has to emulate a 16 bit surfaces, "
		"that will slow things down.\n");
	instance->bpp = 16;
    }

    return (vo_instance_t *) instance;
}
#endif
