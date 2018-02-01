// This is core/vgl/vgl_area.hxx
#ifndef vgl_area_hxx_
#define vgl_area_hxx_

#include "vgl_area.h"
//:
// \file
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_oriented_box_2d.h>

template <class T>
T vgl_area_signed(vgl_polygon<T> const& poly)
{
  // Compute the area using Green's theorem
  T area = T(0);
  for (unsigned int s = 0; s < poly.num_sheets(); ++s )
    for (unsigned int i = 0, j = (unsigned int)(poly[s].size()-1); i < poly[s].size(); j=i++ )
      area += poly[s][j].x() * poly[s][i].y() - poly[s][i].x() * poly[s][j].y();

  return area/2;
}

//: The area weighted center of a polygon
//  In general this is different than the mean of the polygon's vertices
template <class T>
vgl_point_2d<T> vgl_centroid(vgl_polygon<T> const& poly)
{
  T area = vgl_area_signed(poly);
  T x = T(0), y = T(0);
  for (unsigned int s = 0; s < poly.num_sheets(); ++s ){
    for (unsigned int i = 0, j = (unsigned int)(poly[s].size()-1); i < poly[s].size(); j=i++ ){
      T w = poly[s][j].x() * poly[s][i].y() - poly[s][i].x() * poly[s][j].y();
      x += (poly[s][j].x() + poly[s][i].x())*w;
      y += (poly[s][j].y() + poly[s][i].y())*w;
    }
  }
  x /= 6*area;
  y /= 6*area;
  return vgl_point_2d<T>(x,y);
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
// \relatesalso vgl_polygon
template <class T> T vgl_area_enforce_orientation(vgl_polygon<T> const& poly)
{
  T area = T(0);

  // now check containment and enforce correct signs
  // if a sheet is inside an odd number of other sheets then it's a hole
  for (unsigned int t = 0; t < poly.num_sheets(); ++t)
  {
    const typename vgl_polygon<T>::sheet_t & test_pgon= poly[t];
    T t_area = T(0);

    // first calculate all test_pgon's area using Green's theorem
    for (unsigned int i = 0, j = (unsigned int)(test_pgon.size()-1); i < test_pgon.size(); j=i++ )
      t_area += test_pgon[j].x() * test_pgon[i].y() - test_pgon[i].x() * test_pgon[j].y();

    // test if one of t's points is inside the other sheets
    // assume sheets don't overlap!
    bool is_hole = false;
    T x = test_pgon[0].x();
    T y = test_pgon[0].y();
    for (unsigned int s = 0; s < poly.num_sheets(); ++s)
    {
      // don't check a sheet against itself
      if (s==t)
        continue;

      typename vgl_polygon<T>::sheet_t const& pgon = poly[s];
      unsigned int n = (unsigned int)(pgon.size());
      bool c = false;
      for (unsigned int i = 0, j = n-1; i < n; j = i++)
        // invert c for each edge crossing
        if ((((pgon[i].y() <= y) && (y < pgon[j].y())) || ((pgon[j].y() <= y) && (y < pgon[i].y()))) &&
            (x < (pgon[j].x() - pgon[i].x()) * (y - pgon[i].y()) / (pgon[j].y() - pgon[i].y()) + pgon[i].x()))
          c = !c;

      if (c)
        is_hole = !is_hole;
    }

    // if it's oriented in the wrong direction then reverse it
    if ( (!is_hole && t_area < 0) || (is_hole && t_area > 0))
      t_area = -t_area;

    area += t_area;
  }

  return area/2;
}
template <class T>
T vgl_area( const vgl_box_2d<T>& box ){
  return box.width()*box.height(); }

template <class T>
T vgl_area( const vgl_oriented_box_2d<T>& obox ){
  std::pair<T,T> wh = obox.width_height();
    return wh.first * wh.second;
}

#undef VGL_AREA_INSTANTIATE
#define VGL_AREA_INSTANTIATE(T) \
template T vgl_area(vgl_polygon<T > const&); \
template T vgl_area_signed(vgl_polygon<T > const&); \
template T vgl_area_enforce_orientation(vgl_polygon<T > const&); \
template vgl_point_2d<T > vgl_centroid(vgl_polygon<T > const&); \
template T vgl_area( const vgl_box_2d<T>& box ); \
template T vgl_area( const vgl_oriented_box_2d<T>& obox )
#endif // vgl_area_hxx_
