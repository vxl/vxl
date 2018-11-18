#include <vgl/algo/vgl_rtree.hxx>
#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_rtree_c.h>
#include <sdet/sdet_region.h>
#include <sdet/sdet_region_classifier.h>

typedef sdet_region_sptr r;
typedef vgl_box_2d<float> container;
typedef sdet_rtree_region_box_2d<float> c;
VGL_RTREE_INSTANTIATE(r, container, c);
