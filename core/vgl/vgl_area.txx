// This is core/vgl/vgl_area.txx
#ifndef vgl_area_txx_
#define vgl_area_txx_

#include "vgl_area.h"
#include <vgl/vgl_polygon.h>

template <class T>
T vgl_area_signed(vgl_polygon<T> const& poly)
{
  // Compute the area using Green's theorem
  T area = T(0);
  for ( unsigned int s = 0; s < poly.num_sheets(); ++s )
    for ( unsigned int i = 0, j = poly[s].size()-1; i < poly[s].size(); j=i++ )
      area += poly[s][j].x() * poly[s][i].y() - poly[s][i].x() * poly[s][j].y();

  return area/2;
}

// This function is not implemented inline because the cost of a
// single function call is irrelevant compared to the cost of running
// vgl_area_signed. It is therefore better to have fewer dependencies
// in the header file and implement this function here.
template <class T>
T vgl_area( const vgl_polygon<T>& poly )
{
  T area = vgl_area_signed(poly);
  return area<0 ? -area : area;
}

#undef VGL_AREA_INSTANTIATE
#define VGL_AREA_INSTANTIATE(T) \
template T vgl_area(vgl_polygon<T > const&); \
template T vgl_area_signed(vgl_polygon<T > const&)

#endif // vgl_area_txx_
