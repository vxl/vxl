// This is core/vil/vil_nearest_neighbour_interp.h
#ifndef vil_nearest_neighbour_interp_h_
#define vil_nearest_neighbour_interp_h_
//:
// \file
// \brief nearest neighbour interpolation functions for 2D images
// \author Ian Scott
//
// The vil_nearest_neighbour_interp source files were derived from the corresponding
// vil_bilin_interp files, thus the bilin/nearest_neighbour/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bicub and bilin files that would likely also benefit from
// the same change.

#include <vil/vil_image_view.h>
#include <vcl_cassert.h>

//: Compute nearest neighbour interpolation at (x,y), with bound checks
//  If (x,y) is outside interpolatable image region, zero is returned.
//  The safe interpolatable region is [-0.5,view.ni()-0.5)*[0,view.nj()-0.5).
// \relates vil_image_view
template<class T>
inline double vil_nearest_neighbour_interp_safe(
  const vil_image_view<T> &view, double x, double y, unsigned p=0)
{
  int ix = int(x + 0.5);
  int iy = int(y + 0.5);
  if (ix >= 0 && ix < view.ni() && iy >= 0 && iy < view.nj())
    return view(ix, iy, p);
  else return 0.0;
}


//: Compute nearest_neighbour interpolation at (x,y), with minimal bound checks
//  Image is nx * ny array of Ts. x,y element is data[ystep*y+xstep*x]
//  If (x,y) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [-0.5,view.ni()-0.5)*[0.5,view.nj()-0.5).
template<class T>
inline double vil_nearest_neighbour_interp(
  const vil_image_view<T> &view, double x, double y, unsigned p=0)
{
  int ix = int(x + 0.5);
  int iy = int(y + 0.5);
  assert (ix>=0);
  assert (iy>=0);
  assert (ix<=view.ni()-1);
  assert (iy<=view.nj()-1);
  return view(ix, iy, p);
}



//: Compute nearest_neighbourear interpolation at (x,y), with bound checks
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [-0.5,view.ni()-0.5)*[-0.5,view.nj()-0.5).
// \relates vil_image_view
template<class T>
inline double vil_nearest_neighbour_interp_safe_extend(
  const vil_image_view<T> &view, double x, double y, unsigned p=0)
{
  int ix = int(x + 0.5);
  int iy = int(y + 0.5);
  if (ix<0)
    ix= 0.0;
  else
    if (ix>=view.ni()) ix=view.ni()-1;

  if (iy<0)
    iy= 0.0;
  else
    if (iy>=view.nj()) iy=view.nj()-1;

  return view(ix, iy, p);
}

#endif // vil_nearest_neighbour_interp_h_
