#include <iostream>
#include <sstream>
#include "cvg_hemisphere_tableau.h"
//:
// \file
#include <vgui/internals/trackball.h>
#include <vgui/vgui_modifier.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_crop.h>
#include <vil/vil_resample_bilin.h>
#include <vil/vil_load.h>

//: Constructor
cvg_hemisphere_tableau::cvg_hemisphere_tableau(vil_image_view_base const& img)
 : vgui_image_tableau(img, nullptr)
{}
cvg_hemisphere_tableau::cvg_hemisphere_tableau(vil_image_resource_sptr const& img)
 : vgui_image_tableau(img, nullptr)
{}
cvg_hemisphere_tableau::cvg_hemisphere_tableau(vil_image_resource_sptr const& img,
                                               vsph_view_sphere<vsph_view_point<std::string> > sphere)
 : vgui_image_tableau(img, nullptr), curr_pyramid_(nullptr)
{
  img_sphere_ = sphere;
  vsph_view_point<std::string> first_view = img_sphere_.begin()->second;
  curr_point_ = first_view.view_point();
  curr_level_ = PYRAMID_MAX_LEVEL-1;
}

cvg_hemisphere_tableau::cvg_hemisphere_tableau(vil_image_view_base const& img,
                                               vsph_view_sphere<vsph_view_point<std::string> > sphere)
 : vgui_image_tableau(img, nullptr), curr_pyramid_(nullptr)
{
  img_sphere_ = sphere;
  vsph_view_point<std::string> first_view = img_sphere_.begin()->second;
  curr_point_ = first_view.view_point();
  curr_level_ = PYRAMID_MAX_LEVEL-1;

  //load all images to disk - BARF
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
  if (e.type == vgui_KEY_PRESS && (e.key == vgui_CURSOR_LEFT || e.key == vgui_key('a')) ) {
      std::cout<<"Going Left"<<std::endl;
      curr_point_.phi_ -= (vnl_math::pi/18.0);
      this->set_expected_pyramid();
  }
  else if (e.key == vgui_CURSOR_RIGHT || e.key == vgui_key('d')) {
      std::cout<<"Moving Right"<<std::endl;
      curr_point_.phi_ += (vnl_math::pi/18.0);
      this->set_expected_pyramid();
  }
  else if (e.key == vgui_CURSOR_UP || e.key == vgui_key('w')) {
      std::cout<<"Rotating Up"<<std::endl;
      curr_point_.theta_ -= (vnl_math::pi/36.0);
      this->set_expected_pyramid();
  }
  else if (e.key == vgui_CURSOR_DOWN || e.key == vgui_key('s')) {
      std::cout<<"Rotating Down"<<std::endl;
      curr_point_.theta_ += (vnl_math::pi/36.0);
      this->set_expected_pyramid();
  }
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('i')) {
      std::cout<<"Zooming In"<<std::endl;
      curr_level_--;
      if (curr_level_ < 0) curr_level_ = 0;
      this->set_expected_image();
  }
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('o')) {
      std::cout<<"Zooming out"<<std::endl;
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
  vsph_view_point<std::string> curr_view = img_sphere_.find_closest(cart_point, uid, dist);
  if (uid == -1) {
    std::cout<<"View could not find a nearest point, something is wrong"<<std::endl;
    return;
  }

  //get string path
  std::string* img_path = curr_view.metadata();
  vil_image_resource_sptr im = vil_load_image_resource(img_path->c_str());
  if ( !im ) {
    std::cerr << "Could not load " << img_path->c_str() << '\n';
    return;
  }

  //IF THIS PYRAMID has been created, then just get it from cache
  if (pyramids_.find(uid) != pyramids_.end()) {
    curr_pyramid_ = pyramids_[uid];
  }
  else {    //create a new one
    vil_image_view<vxl_byte> img = im->get_view();
    curr_pyramid_ = new vil_pyramid_image_view<vxl_byte>(img);
    double scale = PYRAMID_SCALE;
    for (int i=1; i<PYRAMID_MAX_LEVEL; ++i, scale*=PYRAMID_SCALE) {
      int sni = (int) (scale * img.ni());
      int snj = (int) (scale * img.nj());
      vil_image_view<vxl_byte>* scaled = new vil_image_view<vxl_byte>(sni, snj);
      vil_resample_bilin(img, *scaled, sni, snj);
      vil_image_view_base_sptr scaled_sptr = scaled;
      curr_pyramid_->add_view(scaled_sptr, scale);
    }

    //store pyramid in the pyramid map
    pyramids_[uid] = curr_pyramid_;
  }

  //set the image
  this->set_expected_image();
}

void cvg_hemisphere_tableau::set_expected_image()
{
  //get minimum scale
  double min_scale;
  vil_image_view_base_sptr minview = curr_pyramid_->get_view(PYRAMID_MAX_LEVEL-1, min_scale);

  //get current scale
  double scale;
  vil_image_view_base_sptr scaled = curr_pyramid_->get_view(curr_level_, scale);

  //if curr level is a bit bigger, crop the image...
  int ni = minview->ni();
  int nj = minview->nj();

  //get difference
  int dni = scaled->ni() - ni;
  int dnj = scaled->nj() - nj;
  int i0 = dni/2;
  int j0 = dnj/2;
  vil_image_view<vxl_byte> cropped = vil_crop<vxl_byte>(*scaled, i0, ni, j0, nj);
  this->set_image_view(cropped);
  this->post_redraw();
  std::cout<<"Tableau width/height"<<this->width()<<','<<this->height()<<std::endl;
}

double cvg_hemisphere_tableau::compress_range(double rad)
{
  return vnl_math::angle_minuspi_to_pi(rad);
}
