#include <vgl/algo/vgl_rtree.hxx>
#include <vgl/vgl_box_2d.h>
#include <vgl/algo/vgl_rtree_c.h>

typedef vgl_box_2d<float> v;
typedef vgl_bbox_2d<float> b;
typedef vgl_rtree_box_box_2d<float> c;

VGL_RTREE_INSTANTIATE(v, b, c);
