// camera_node.cpp: implementation of the camera_node class.
//
//////////////////////////////////////////////////////////////////////

#include "camera_node.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

camera_node::camera_node(int id)
{
  cam_ = 0;
  cam_ = new camera(id);
  num_views_ = 0;
}

camera_node::~camera_node()
{
  if (cam_)
    delete cam_;
}

void camera_node::set_beat(vcl_vector<double> const& new_beat)
{
  num_views_ = new_beat.size();

  // clear motion vector:
  //camera_node::set_motion(vcl_vector<vcsl_spatial_transformation_sptr>());
}
