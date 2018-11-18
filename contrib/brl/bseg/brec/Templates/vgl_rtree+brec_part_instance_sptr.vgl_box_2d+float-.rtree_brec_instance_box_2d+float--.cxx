#include <vgl/algo/vgl_rtree.hxx>
#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_rtree_c.h>
#include <brec/brec_part_base_sptr.h>
#include <brec/brec_part_hierarchy_detector.h>

typedef brec_part_instance_sptr pt;
typedef vgl_box_2d<float> container;
typedef rtree_brec_instance_box_2d<float> c;
VGL_RTREE_INSTANTIATE(pt, container, c);
