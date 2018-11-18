#include <iostream>
#include <algorithm>
#include "boxm2_multi_cache_group.h"
//:
// \file
#include <vgl/vgl_distance.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Returns indices in visibility order (for group's blocks)
std::vector<int> boxm2_multi_cache_group::order_from_cam(const vpgl_camera_double_sptr& cam)
{
  vgl_point_3d<double> pt;
  if ( cam->type_name() == "vpgl_generic_camera" ) {
    auto* gcam = (vpgl_generic_camera<double>*) cam.ptr();
    pt = gcam->max_ray_origin();
  }
  else if ( cam->type_name() == "vpgl_perspective_camera" ) {
    auto* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
    pt = pcam->camera_center();
  }
  else {
    std::cout<<"get group order doesn't support camera type "<<cam->type_name()<<std::endl;
  }
  std::vector<boxm2_dist_pair<int> > distances;
  for (unsigned int i=0; i<bboxes_.size(); ++i)
    distances.emplace_back(vgl_distance(pt, bboxes_[i].centroid()), i);
  std::sort(distances.begin(), distances.end());

  //write and return order
  std::vector<int> vis_order;
  vis_order.reserve(distances.size());
for (auto & distance : distances)
    vis_order.push_back(distance.dat_);
  return vis_order;
}

std::ostream& operator<<(std::ostream &s, boxm2_multi_cache_group& grp)
{
  s << "boxm2_multi_cache_group [ ";
  for (unsigned int i=0; i<grp.ids().size(); ++i)
    s << grp.id(i) << ' ';
  s << ']';
  return s;
}
