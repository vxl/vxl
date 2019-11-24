#include <vgl/algo/vgl_rtree.hxx>
#include "vgl/vgl_box_2d.h"
#include <vgl/algo/vgl_rtree_c.h>

using v = vgl_box_2d<float>;
using b = vgl_bbox_2d<float>;
using c = vgl_rtree_box_box_2d<float>;

VGL_RTREE_INSTANTIATE(v, b, c);
