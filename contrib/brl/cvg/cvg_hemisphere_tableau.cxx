#include "cvg_hemisphere_tableau.h"
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgui/internals/trackball.h>
#include <vgui/vgui_modifier.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_sstream.h>

#include <vil/vil_crop.h>


//: Constructor
cvg_hemisphere_tableau::cvg_hemisphere_tableau(vil_image_view_base const& img)
 : vgui_image_tableau(img, 0)
{}
cvg_hemisphere_tableau::cvg_hemisphere_tableau(vil_image_resource_sptr const& img)
 : vgui_image_tableau(img, 0)
{}
cvg_hemisphere_tableau::cvg_hemisphere_tableau(vil_image_resource_sptr const& img,
                                               vsph_view_sphere<vsph_view_point<vcl_string> > sphere)
 : vgui_image_tableau(img, 0), curr_pyramid_(0)
{
  img_sphere_ = sphere;
  vsph_view_point<vcl_string> first_view = img_sphere_.begin()->second;
  curr_point_ = first_view.view_point();
  curr_level_ = PYRAMID_MAX_LEVEL-1;
}


//: initialize tableau properties
bool cvg_hemisphere_tableau::init()
{
  return true;
}


//: Handles tableau events (drawing and keys)
bool cvg_hemisphere_tableau::handle(vgui_event const &e)
{
  //handle cursor calls
  if (e.key == vgui_CURSOR_LEFT || e.key == vgui_key('a')) {
      vcl_cout<<"Going Left "<<vcl_endl;
      curr_point_.phi_ -= (vnl_math::pi/18.0);
      this->set_expected_pyramid();
  }
  else if (e.key == vgui_CURSOR_RIGHT || e.key == vgui_key('d')) {
      vcl_cout<<"Moving Right "<<vcl_endl;
      curr_point_.phi_ += (vnl_math::pi/18.0);
      this->set_expected_pyramid();
  }
  else if (e.key == vgui_CURSOR_UP || e.key == vgui_key('w')) {
      vcl_cout<<"Rotating Up "<<vcl_endl;
      curr_point_.theta_ -= (vnl_math::pi/36.0);
      this->set_expected_pyramid();
  }
  else if (e.key == vgui_CURSOR_DOWN || e.key == vgui_key('s')) {
      vcl_cout<<"Rotating Down "<<vcl_endl;
      curr_point_.theta_ += (vnl_math::pi/36.0);
      this->set_expected_pyramid();
  }
  else if (e.key == vgui_key('i')) {
      vcl_cout<<"Zooming In "<<vcl_endl;
      curr_level_--;
      if (curr_level_ < 0) curr_level_ = 0;
      this->set_expected_image();
  }
  else if (e.key == vgui_key('o')) {
      vcl_cout<<"Zooming out "<<vcl_endl;
      curr_level_++;
      if (curr_level_ >= PYRAMID_MAX_LEVEL) curr_level_ = PYRAMID_MAX_LEVEL-1;
      this->set_expected_image();
  }

  //Otherwise let the VGUI_IMAGE_TABLEAU handle the draw
  if (vgui_image_tableau::handle(e))
    return true;

  return false;
}

void cvg_hemisphere_tableau::set_expected_pyramid()
{
  //convert to cartesian (as method is only in cartesian for some reason)
  vgl_point_3d<double> cart_point = img_sphere_.cart_coord(curr_point_);

  //get closest view
  int uid; double dist;
  vsph_view_point<vcl_string> curr_view = img_sphere_.find_closest(cart_point, uid, dist);
  if (uid == -1) {
    vcl_cout<<"View could not find a nearest point, something is wrong "<<vcl_endl;
    return;
  }

  //get string path
  vcl_string* img_path = curr_view.metadata();
  vil_image_resource_sptr im = vil_load_image_resource(img_path->c_str());
  if ( !im ) {
    vcl_cerr << "Could not load " << img_path->c_str() << '\n';
    return;
  }
  if (curr_pyramid_) delete curr_pyramid_;

  //create the current pyramid
  curr_pyramid_ = new vil_pyramid_image_view<vxl_byte>(im->get_view(), PYRAMID_MAX_LEVEL);

  //set the image
  this->set_expected_image();
}

void cvg_hemisphere_tableau::set_expected_image()
{
  double scale;
  vil_image_view_base_sptr scaled = curr_pyramid_->get_view(curr_level_, scale);

  //if curr level is a bit bigger, crop the image...
  if (curr_level_ == 0) {
    int ni = scaled->ni()/2;
    int nj = scaled->nj()/2;
    int i0 = ni/2;
    int j0 = nj/2;
    vil_image_view<vxl_byte> cropped = vil_crop<vxl_byte>(*scaled, i0, ni, j0, nj);
    this->set_image_view(cropped);
    this->post_redraw();
    return;
  }
  vcl_cout<<"Tableau width/height"<<this->width()<<','<<this->height()<<vcl_endl;

  this->set_image_view(*scaled);
  this->post_redraw();
}

double cvg_hemisphere_tableau::compress_range(double rad)
{
  while (rad >  vnl_math::pi) rad -= 2*vnl_math::pi;
  while (rad < -vnl_math::pi) rad += 2*vnl_math::pi;
  return rad;
}

