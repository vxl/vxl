// This is gel/mrc/vpgl/vpgl_basic_camera.cxx
#include "vpgl_basic_camera.h"
//:
// \file
// The code is taken and adapted from BasicCamera.c of TargetJr for VxL photogrammetry library.

vpgl_basic_camera::vpgl_basic_camera():
  init_pt(3)
{
  for (int i=0; i<3; i++) init_pt[i]=0.0;
}

vpgl_basic_camera::~vpgl_basic_camera()
{
}

//---------------------------------------------------------------------
//  Various flavours of world_to_image
//  Routines that call with double arguments are converted to float
//  so as to pick up existing virtual routines that use float.
//

void vpgl_basic_camera::world_to_image(vnl_vector<double> const& xyz, double &u, double &v, double t)
{
  world_to_image (xyz[0], xyz[1], xyz[2], u, v, t);
}

void vpgl_basic_camera::world_to_image(double,double,double, double&, double&, double)
{
  vcl_cerr <<" Can't compute world_to_image()\n";
}

//------------------------------------------------------------------------

void vpgl_basic_camera::image_to_world(vnl_vector<double>& ,vnl_vector<double>& , double , double )
{
  vcl_cerr << " Can't compute ImageToWorld" ;
}

void vpgl_basic_camera::set_init_pt(const vnl_vector<double>& pt)
{
  init_pt = pt;
}

void vpgl_basic_camera::get_init_pt(vnl_vector<double>& pt)
{
  for (int i=0; i<3; i++) pt[i]= init_pt[i];
}
