#include <vgl/algo/vgl_rtree.hxx>
#include "vgl/vgl_point_2d.h"
#include "vgl/vgl_box_2d.h"
#include <vgl/algo/vgl_rtree_c.h>

using pt = vgl_point_2d<float>;
using box = vgl_box_2d<float>;
using c = vgl_rtree_point_box_2d<float>;

VGL_RTREE_INSTANTIATE(pt, box, c);
