// Instantiation of vgl_rtree<vgl_point_2d<double>, vgl_box_2d<double>, dummy>
// Treat this template instantiation as a "dummy" instantiation, for purposes of
// testing the vgl_rtree implementation on potential compile errors or warnings.
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/algo/vgl_rtree.hxx>

typedef vgl_point_2d<double> V;
typedef vgl_box_2d<double>   B;

struct dummy // dummy "namespace", used by some of the rtree methods
{
  static void  init  (B &, V const &) {}
  static void  update(B &, V const &) {}
  static void  update(B &, B const &) {}
  static bool  meet  (B const &, V const &) { return false; }
  static bool  meet  (B const &, B const &) { return false; }
  static float volume(B const &) { return 0.0f; }
};

VGL_RTREE_INSTANTIATE(V, B, dummy);
