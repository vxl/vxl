// camera_node.cpp: implementation of the camera_node class.
//
//////////////////////////////////////////////////////////////////////

#include "camera_node.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

camera_node::camera_node(int id)
{
  pCam_ = 0;
  pCam_ = new camera(id);
  nViews_ = 0;
}

camera_node::~camera_node()
{
  if (pCam_)
    delete pCam_;
}

void camera_node::set_beat(vcl_vector<double> const& new_beat)
{
  nViews_ = new_beat.size();

  // clear motion vector:
  //camera_node::set_motion(vcl_vector<vcsl_spatial_transformation_sptr>());
}

vcl_ostream& operator<<(vcl_ostream& s, camera_node const& camera)
{
  return s << "camera: "<< camera.get_id()<<"\n intrinsic is"<<camera.get_intrinsic();
}