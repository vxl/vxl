// bcal_camera_node.cpp: implementation of the bcal_camera_node class.
//
//////////////////////////////////////////////////////////////////////

#include "bcal_camera_node.h"
#include "bcal_camera.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bcal_camera_node::bcal_camera_node(int id)
{
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

vnl_double_3x3 bcal_camera_node::get_intrinsic() const
{
  return cam_->get_intrisic_matrix();
}

void bcal_camera_node::set_intrinsic(vnl_double_3x3 k)
{
  cam_->set_intrisic_matrix(k);
}

int bcal_camera_node::get_id() const
{
  return cam_->getID();
}
