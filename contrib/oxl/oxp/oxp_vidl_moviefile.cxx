// This is oxl/oxp/oxp_vidl_moviefile.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "oxp_vidl_moviefile.h"

#include <vcl_compiler.h>
#include <vcl_cassert.h>

#include <vidl_vil1/vidl_vil1_movie_sptr.h>
#include <vidl_vil1/vidl_vil1_movie.h>
#include <vidl_vil1/vidl_vil1_frame.h>
#include <vidl_vil1/vidl_vil1_io.h>

#ifdef VCL_WIN32
#include <vidl_vil1/vidl_vil1_avicodec.h>
#endif
#if defined(HAS_MPEG2)
#include <oxp/oxp_vidl_mpeg_codec.h>
#endif

struct oxp_vidl_moviefile_privates {
  vidl_vil1_movie_sptr m;
};

/////////////////////////////////////////////////////////////////////////////

static bool init = false;

void ensure_initialized()
{
  if (init) return;

  // Register video codec
#ifdef VCL_WIN32
  vidl_vil1_io::register_codec(new vidl_vil1_avicodec);
#endif
#if defined(HAS_MPEG2)
  vidl_vil1_io::register_codec(new oxp_vidl_mpeg_codec);
#endif
  init = true;
}

/////////////////////////////////////////////////////////////////////////////


oxp_vidl_moviefile::oxp_vidl_moviefile(char const* f)
{
  ensure_initialized();

  p = new oxp_vidl_moviefile_privates;
  p->m = vidl_vil1_io::load_movie(f);
}

oxp_vidl_moviefile::~oxp_vidl_moviefile()
{
  delete p;
}

int oxp_vidl_moviefile::GetLength()
{
  return p->m->length();
}

vil1_image oxp_vidl_moviefile::GetImage(int frame)
{
  return p->m->get_frame(frame)->get_image();
}


int oxp_vidl_moviefile::GetSizeX(int frame)
{
  return p->m->get_frame(frame)->width();
}

int oxp_vidl_moviefile::GetSizeY(int frame)
{
  return p->m->get_frame(frame)->height();
}

int oxp_vidl_moviefile::GetBitsPixel()
{
  return p->m->get_frame(0)->get_bits_pixel();
}

bool oxp_vidl_moviefile::IsInterlaced()
{
  assert(0);
#if 0
  return p->m->interlaced != 0;
#else
  return 0;
#endif
}

bool oxp_vidl_moviefile::HasFrame(int frame_index)
{
  return (0 <= frame_index) && (frame_index < GetLength());
}

bool oxp_vidl_moviefile::GetFrame(int frame_index, void* buffer)
{
  vidl_vil1_frame_sptr f = p->m->get_frame(frame_index);
  return f->get_section(buffer, 0,0, f->width(), f->height());
}

bool oxp_vidl_moviefile::GetField(int /*field_index*/, void* /*buffer*/)
{
  return false;
}

