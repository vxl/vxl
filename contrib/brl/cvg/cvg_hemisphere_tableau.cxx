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


//: Constructor
cvg_hemisphere_tableau::cvg_hemisphere_tableau(vil_image_view_base const& img)
 : vgui_image_tableau(img, 0) 
{}
cvg_hemisphere_tableau::cvg_hemisphere_tableau(vil_image_resource_sptr const& img)
 : vgui_image_tableau(img, 0) 
{}
cvg_hemisphere_tableau::cvg_hemisphere_tableau(vil_image_resource_sptr const& img, 
                                               vbl_array_2d<vcl_string> images)
 : vgui_image_tableau(img, 0)
{
  curr_row_ = 0;
  curr_col_ = 0;
  images_ = images;
}
cvg_hemisphere_tableau::cvg_hemisphere_tableau(vil_image_resource_sptr const& img, 
                                                vsph_view_sphere<vsph_view_point<vcl_string> > sphere)
 : vgui_image_tableau(img, 0)
{
  img_sphere_ = sphere;
  vsph_view_point<vcl_string> first_view = img_sphere_.begin()->second;
  curr_point_ = first_view.view_point();
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
      this->set_expected_image();
  }
  else if (e.key == vgui_CURSOR_RIGHT || e.key == vgui_key('d')) {
      vcl_cout<<"Moving Right "<<vcl_endl;
      curr_point_.phi_ += (vnl_math::pi/18.0);
      this->set_expected_image();
  }  
  else if (e.key == vgui_CURSOR_UP || e.key == vgui_key('w')) {
      vcl_cout<<"Rotating Up "<<vcl_endl;
      curr_point_.theta_ -= (vnl_math::pi/36.0);
      this->set_expected_image();
  }  
  else if (e.key == vgui_CURSOR_DOWN || e.key == vgui_key('s')) {
      vcl_cout<<"Rotating Down "<<vcl_endl;
      curr_point_.theta_ += (vnl_math::pi/36.0);
      this->set_expected_image();
  } 

  //Otherwise let the VGUI_IMAGE_TABLEAU handle the draw
  if (vgui_image_tableau::handle(e))
    return true;

  return false;
}

void cvg_hemisphere_tableau::set_expected_image(int row, int col)
{
  //make sure the right image is displayed
  vil_image_resource_sptr im = vil_load_image_resource( images_(row, col).c_str() );
  if ( !im ) {
    vcl_cerr << "Could not load " << images_(row, col) << '\n';
    return;
  }
  this->set_image_resource(im); 
  this->post_redraw();  
}

void cvg_hemisphere_tableau::set_expected_image()
{
  //convert to cartesian (as method is only in cartesian for some reason)
  vgl_point_3d<double> cart_point = img_sphere_.cart_coord(curr_point_);

  //get closest view
  int uid; double dist;
  vsph_view_point<vcl_string> curr_view = img_sphere_.find_closest(cart_point, uid, dist); 
  if(uid == -1) {
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
  this->set_image_resource(im); 
  this->post_redraw();  
}

double cvg_hemisphere_tableau::compress_range(double rad) 
{
  if(rad >  vnl_math::pi) return rad - 2*vnl_math::pi; 
  if(rad < -vnl_math::pi) return rad + 2*vnl_math::pi; 
}

