// bcal_camera_node.cpp: implementation of the bcal_camera_node class.
//
//////////////////////////////////////////////////////////////////////

#include "bcal_camera_node.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bcal_camera_node::bcal_camera_node(int id)
{
  cam_ = 0;
  cam_ = new bcal_camera(id);
  num_views_ = 0;
}

bcal_camera_node::~bcal_camera_node()
{
  if (cam_)
    delete cam_;
}

void bcal_camera_node::set_beat(vcl_vector<double> const& new_beat)
{
  num_views_ = new_beat.size();

  // clear motion vector:
  //bcal_camera_node::set_motion(vcl_vector<vcsl_spatial_transformation_sptr>());
}
