// This is core/vgl/vgl_convex.txx
#ifndef vgl_convex_txx_
#define vgl_convex_txx_
//:
// \file
#include "vgl_convex.h"
#include <vcl_limits.h>
#include <vcl_cmath.h>
#include <vcl_list.h>


//: Calculate the negative cosine of the angle from dir to next - current.
// If next and current are on top of each other, pretend it is a high angle.
// Use cosine because it is quicker to calculate and monotonic with angle
// over [0 pi].
template <class T>
static T get_nc_angle(const vgl_vector_2d<T> &last_dir,
                      const vgl_point_2d<T> &current,
                      const vgl_point_2d<T> &next)
{
  vgl_vector_2d<T> v = next - current;
  double eps = vcl_sqrt(current.x() * current.x() + current.y() * current.y())
    * vcl_numeric_limits<T>::epsilon();

  //if the two points are on top of each other, pretend it is a very bad angle.
  // Use an illegal cosine value to indicate this.
  if (v.length() <= eps) return (T)2.0;
  return -cos_angle(last_dir, v);
}

template <class T>
vgl_polygon<T> vgl_convex_hull(vcl_vector<vgl_point_2d<T> > const& points)
{
  vgl_polygon<T> hull(1);
  if (points.empty()) return hull;

  typedef typename vcl_list<vgl_point_2d<T> >::iterator ITER;

  // A list of points still not used.
  vcl_list<vgl_point_2d<T> > pts(points.begin(), points.end());

  // Find left most point.
  ITER start = pts.begin();
  for (ITER i=pts.begin(); i != pts.end(); ++i)
    if (i->x() < start->x()) start=i;

  vgl_point_2d<T> first = *start; // Closing point on hull.
  vgl_point_2d<T> current = first; // Current point on hull.
  vgl_vector_2d<T> last_dir(0,1); // Direction of last segment added to hull.
  hull.push_back(first);
  pts.erase(start);
  bool not_starting = false;
  // iterate through remaining points
  while (true)
  {
    // if we are out of points, then our poly is the answer.
    if (pts.empty()) return hull;

    // Calculate angles to closing point, and all the remaining points.
    double nc_angle_to_first = get_nc_angle(last_dir, current, first);

    // If the current point is not the closing point, but is very close to it,
    // then we are done.
    if (not_starting && nc_angle_to_first > 1.5) return hull;
    not_starting=true;

    double best_nc_angle = 1.5;
    ITER best;

    for (ITER i=pts.begin(); i != pts.end(); ++i)
    {
      double nc_angle = get_nc_angle(last_dir, current, *i);
      if (nc_angle < best_nc_angle)
      {
        best_nc_angle = nc_angle;
        best = i;
      }
    }

    // If the closing angle is lowest, the our hull is the answer.
    if (nc_angle_to_first <= best_nc_angle) return hull;


    // Add best point to hull, remove from list, update invariant.
    hull.push_back(*best);
    last_dir=*best - current;
    current = * best;
    pts.erase(best);
  }
}

#undef VGL_CONVEX_INSTANTIATE
#define VGL_CONVEX_INSTANTIATE(T) \
template vgl_polygon<T > vgl_convex_hull(const vcl_vector<vgl_point_2d<T > >&)


#endif // vgl_convex_txx_
