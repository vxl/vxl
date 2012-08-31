#include "boxm2_multi_cache_group.h"
//:
// \file
#include <vgl/vgl_distance.h>
#include <vcl_algorithm.h>

//: Returns indices in visibility order (for group's blocks)
vcl_vector<int> boxm2_multi_cache_group::order_from_cam(vpgl_camera_double_sptr cam)
{
  vgl_point_3d<double> pt;
  if ( cam->type_name() == "vpgl_generic_camera" ) {
    vpgl_generic_camera<double>* gcam = (vpgl_generic_camera<double>*) cam.ptr();
    pt = gcam->max_ray_origin();
  }
  else if ( cam->type_name() == "vpgl_perspective_camera" ) {
    vpgl_perspective_camera<double>* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
    pt = pcam->camera_center();
  }
  else {
    vcl_cout<<"get group order doesn't support camera type "<<cam->type_name()<<vcl_endl;
  }
  vcl_vector<boxm2_dist_pair<int> > distances;
  for (unsigned int i=0; i<bboxes_.size(); ++i)
    distances.push_back(boxm2_dist_pair<int>(vgl_distance(pt, bboxes_[i].centroid()), i));
  vcl_sort(distances.begin(), distances.end());

  //write and return order
  vcl_vector<int> vis_order;
  for (unsigned int i=0; i<distances.size(); ++i)
    vis_order.push_back(distances[i].dat_);
  return vis_order;
}

vcl_ostream& operator<<(vcl_ostream &s, boxm2_multi_cache_group& grp)
{
  s << "boxm2_multi_cache_group [ ";
  for (unsigned int i=0; i<grp.ids().size(); ++i)
    s << grp.id(i) << ' ';
  s << ']';
  return s;
}

