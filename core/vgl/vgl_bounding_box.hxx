#include "vgl_bounding_box.h"
#include "vgl_polygon.h"
#include "vgl_sphere_3d.h"
#include "vgl_pointset_3d.h"
#include "vgl_cubic_spline_3d.h"
#include "vgl_box_3d.h"
#include "vgl_box_2d.h"

template <class T>
vgl_box_3d<T>
vgl_bounding_box(const vgl_sphere_3d<T> & sph)
{
  T r = sph.radius();
  vgl_point_3d<T> c = sph.centre();
  T cx = c.x(), cy = c.y(), cz = c.z();
  vgl_point_3d<T> p0(cx - r, cy - r, cz - r);
  vgl_point_3d<T> p1(cx + r, cy + r, cz + r);
  vgl_box_3d<T> bb;
  bb.add(p0);
  bb.add(p1);
  return bb;
}

template <class T>
vgl_box_3d<T>
vgl_bounding_box(const vgl_pointset_3d<T> & ptset)
{
  unsigned n = ptset.npts();
  vgl_box_3d<T> bb;
  for (unsigned i = 0; i < n; i++)
    bb.add(ptset.p(i));
  return bb;
}

template <class T>
vgl_box_3d<T>
vgl_bounding_box(const vgl_cubic_spline_3d<T> & spline)
{
  vgl_box_3d<T> ret;
  std::vector<vgl_point_3d<T>> knots = spline.knots();
  for (typename std::vector<vgl_point_3d<T>>::const_iterator kit = knots.begin(); kit != knots.end(); ++kit)
    ret.add(*kit);
  return ret;
}
template <class T>
vgl_box_2d<T>
vgl_bounding_box(const vgl_polygon<T> & poly)
{
  vgl_box_2d<T> ret;
  for (size_t s = 0; s < poly.num_sheets(); ++s)
  {
    for (size_t i = 0; i < poly[s].size(); ++i)
      ret.add(poly[s][i]);
  }
  return ret;
}
#define VGL_BOUNDING_BOX_INSTANTIATE(T)                                    \
  template vgl_box_3d<T> vgl_bounding_box(vgl_sphere_3d<T> const &);       \
  template vgl_box_3d<T> vgl_bounding_box(vgl_pointset_3d<T> const &);     \
  template vgl_box_3d<T> vgl_bounding_box(vgl_cubic_spline_3d<T> const &); \
  template vgl_box_2d<T> vgl_bounding_box(vgl_polygon<T> const &)
