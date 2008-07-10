// core/vil/vil_nearest_interp.txx
#ifndef vil_nearest_interp_txx_
#define vil_nearest_interp_txx_
//:
// \file
// \brief nearest neighbour interpolation functions for 2D images
//
// The vil nearest source files were derived from the corresponding
// vil bilin files, thus the vil bilin/nearest/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bilin/bicub file that would likely also benefit from
// the same change.
//
// see:-
// vil_bilin_interp.txx file, vil_bilin_interp.h
// vil_bicub_interp.txx file, vil_bicub_interp.h 

#include "vil_nearest_interp.h"
#include <vcl_compiler.h>

// vil_nearest_interp.h defines only inline functions, but some of the
// corresponding vil_nearest_interp functions are a little big to be
// inline.  Plus, on one platform, msvc 6.0 with /O2 optimization
// compiled the vil_nearest_interp functions without a peep but gave
// incorrect numerical results when these functions were inline and
// defined in vil_nearest_interp.h.
template<class T>
double vil_nearest_interp_unsafe(double x, double y, const T* data,
                               vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
  int p1x=int(x+0.5);
  int p1y=int(y+0.5);
    
  const T* pix1 = data + p1y*ystep + p1x*xstep;

  return pix1[0];
}

// See the comments where this variable is used below.  If it is
// necessary to get rid of this static variable we can try using a
// volatile automatic variable defined in vil_nearest_interp_raw()
// instead.  That should have the same effect.
#ifdef VCL_VC_6
static double vil_nearest_interp_raw_temp_hack = 0.0;
#endif

template<class T>
double vil_nearest_interp_raw(double x, double y, const T* data,
                            vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
  int p1x=int(x+0.5);
  int p1y=int(y+0.5);

  const T* pix1 = data + p1y*ystep + p1x*xstep;

  return pix1[0];
}

#define VIL_NEAREST_INTERP_INSTANTIATE(T) \
template double \
vil_nearest_interp_unsafe (double x, double y, const T* data, \
                         vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep); \
template double \
vil_nearest_interp_raw (double x, double y, const T* data, \
                      vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)

#endif // vil_nearest_interp_txx_
