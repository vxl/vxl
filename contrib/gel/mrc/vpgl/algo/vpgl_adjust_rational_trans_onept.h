// This is gel/mrc/vpgl/algo/vpgl_adjust_rational_trans_onept.h
#ifndef vpgl_adjust_rational_trans_onept_h_
#define vpgl_adjust_rational_trans_onept_h_
//:
// \file
// \brief Adust image offsets to register a set of rational cameras
// \author J. L. Mundy
// \date July 29, 2007
//

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_comp_rational_camera.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
//: The image offsets of rational cameras typically must be adjusted to 
// compensate for errors in geographic alignment. This algorithm finds 
// a set of minium translations that registers the input set of images. 
// After registration, the images have geographically corresponding rational
// cameras. That is, a visible 3-d point will project into its corresponding
// image location in all the images.

class vpgl_z_search_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  vpgl_z_search_lsqr(vcl_vector<vpgl_rational_camera<double> > const& cams,
                     vcl_vector<vgl_point_2d<double> > const& image_pts,
                     vgl_point_3d<double> const& initial_pt);  
  //: Destructor
  virtual ~vpgl_z_search_lsqr() {}

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  fx has been sized appropriately before the call.
  virtual void f(vnl_vector<double> const& elevation,
                 vnl_vector<double>& projection_error);
  double xm() {return xm_;}
  double ym() {return ym_;}
 protected:
  vpgl_z_search_lsqr();//not valid
  vgl_point_3d<double> initial_pt_;
  vcl_vector<vpgl_rational_camera<double> > cameras_; //cameras
  vcl_vector<vgl_point_2d<double> > image_pts_; //image points 
  double xm_, ym_;
};


class vpgl_adjust_rational_trans_onept
{
 public:

  ~vpgl_adjust_rational_trans_onept(){}

  static bool adjust(vcl_vector<vpgl_rational_camera<double> > const& cams,
                     vcl_vector<vgl_point_2d<double> > const& corrs,
                     vcl_vector<vgl_vector_2d<double> >& cam_translations,
                     vgl_point_3d<double>& intersection);
  
 protected:

  vpgl_adjust_rational_trans_onept();
};



#endif // vpgl_adjust_rational_trans_onept_h_
