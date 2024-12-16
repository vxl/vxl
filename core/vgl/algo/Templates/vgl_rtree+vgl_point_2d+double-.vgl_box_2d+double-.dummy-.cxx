// Instantiation of vgl_rtree<vgl_point_2d<double>, vgl_box_2d<double>, dummy>
// Treat this template instantiation as a "dummy" instantiation, for purposes of
// testing the vgl_rtree implementation on potential compile errors or warnings.
#include "vgl/vgl_point_2d.h"
#include "vgl/vgl_box_2d.h"
#include <vgl/algo/vgl_rtree.hxx>

using V = vgl_point_2d<double>;
using B = vgl_box_2d<double>;

struct dummy // dummy "namespace", used by some of the rtree methods
{
  static void
  init(B &, const V &)
  {}
  static void
  update(B &, const V &)
  {}
  static void
  update(B &, const B &)
  {}
  static bool
  meet(const B &, const V &)
  {
    return false;
  }
  static bool
  meet(const B &, const B &)
  {
    return false;
  }
  static float
  volume(const B &)
  {
    return 0.0f;
  }
};

VGL_RTREE_INSTANTIATE(V, B, dummy);
