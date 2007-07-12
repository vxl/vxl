// This is core/vgl/vgl_area.txx
#ifndef vgl_area_txx_
#define vgl_area_txx_

#include "vgl_area.h"

#include <vcl_vector.h>

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

//: The orientation enforced area of a polygon.
// \note This method assumes that the polygon is simple (i.e. no crossings)
//  and the correct orientation is 'enforced' on the polygon (i.e. holes are
//  given negative area) to ensure that the resultant area is correct
// \sa vgl_area
// \relates vgl_polygon
template <class T> T vgl_area_enforce_orientation(vgl_polygon<T> const& poly)
{
  T area = T(0);

  //now check containment and enforce correct signs
  //if a sheet is inside an odd number of other sheets then it's a hole
  for (unsigned t = 0; t < poly.num_sheets(); ++t)
  {
    const vgl_polygon<T>::sheet_t& test_pgon = poly[t];
    T t_area = T(0);

    //first calculate all t_pgon's area using Green's theorem
    for ( unsigned int i = 0, j = test_pgon.size()-1; i < test_pgon.size(); j=i++ )
      t_area += test_pgon[j].x() * test_pgon[i].y() - test_pgon[i].x() * test_pgon[j].y();

    //test if one of t's points is inside the other sheets
    //assume sheets don't  overlap!
    bool is_hole = false;
    T x = test_pgon[0].x();
    T y = test_pgon[0].y();
    for (unsigned s = 0; s < poly.num_sheets(); ++s)
    {
      //dont check a sheet against itself
      if(s==t)
        continue;

      vgl_polygon<T>::sheet_t const& pgon = poly[s];
      unsigned n = pgon.size();
      bool c = false;
      for (unsigned i = 0, j = n-1; i < n; j = i++)
        // invert c for each edge crossing
        if ((((pgon[i].y() <= y) && (y < pgon[j].y())) || ((pgon[j].y() <= y) && (y < pgon[i].y()))) &&
            (x < (pgon[j].x() - pgon[i].x()) * (y - pgon[i].y()) / (pgon[j].y() - pgon[i].y()) + pgon[i].x()))
          c = !c;

      if(c)
        is_hole = !is_hole;
    }

    //if it's orientated in the wrong direction then reverse it
    if( (!is_hole && t_area < 0) || (is_hole && t_area > 0))
      t_area = -t_area;

    area += t_area;
  }
 
  return area/2;
}

#undef VGL_AREA_INSTANTIATE
#define VGL_AREA_INSTANTIATE(T) \
template T vgl_area(vgl_polygon<T > const&); \
template T vgl_area_signed(vgl_polygon<T > const&); \
template T vgl_area_enforce_orientation(vgl_polygon<T> const&)

#endif // vgl_area_txx_
