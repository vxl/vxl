#include <vgl/algo/vgl_rtree.hxx>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/algo/vgl_rtree_c.h>

typedef vgl_point_2d<float> pt;
typedef vgl_box_2d<float> box;
typedef vgl_rtree_point_box_2d<float> c;

VGL_RTREE_INSTANTIATE(pt, box, c);
