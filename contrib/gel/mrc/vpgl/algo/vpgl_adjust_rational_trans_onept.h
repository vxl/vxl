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

//:
// The image offsets of rational cameras typically must be adjusted to
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
  double xm() const {return xm_;}
  double ym() const {return ym_;}
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
  ~vpgl_adjust_rational_trans_onept() {}

  static bool adjust(vcl_vector<vpgl_rational_camera<double> > const& cams,
                     vcl_vector<vgl_point_2d<double> > const& corrs,
                     vcl_vector<vgl_vector_2d<double> >& cam_translations,
                     vgl_point_3d<double>& intersection);

 protected:
  vpgl_adjust_rational_trans_onept();
};

//:
// This algorithm finds a set of minium translations that registers the input set of images using multiple correspondences
// e.g. multiple 3D points projecting to a set of 2D correspondences in each image
// After registration, the images have geographically corresponding rational
// cameras. That is, a visible 3-d point will project into its corresponding
// image location in all the images.

class vpgl_cam_trans_search_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  vpgl_cam_trans_search_lsqr(vcl_vector<vpgl_rational_camera<double> > const& cams,
                             vcl_vector< vcl_vector<vgl_point_2d<double> > > const& image_pts,  // for each 3D corr, an array of 2D corrs for each camera
                             vcl_vector< vgl_point_3d<double> > const& initial_pts);
  //: Destructor
  virtual ~vpgl_cam_trans_search_lsqr() {}

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  fx has been sized appropriately before the call.
  virtual void f(vnl_vector<double> const& translation,   // size is 2*cams.size()
                 vnl_vector<double>& projection_errors);  // size is cams.size()*image_pts.size() --> compute a residual for each 3D corr point in each image
  
  void get_finals(vcl_vector<vgl_point_3d<double> >& finals);

 protected:
  vpgl_cam_trans_search_lsqr();//not valid
  vcl_vector<vgl_point_3d<double> > initial_pts_;
  vcl_vector<vpgl_rational_camera<double> > cameras_; //cameras
  vcl_vector<vcl_vector<vgl_point_2d<double> > > corrs_; 
  vcl_vector<vgl_point_3d<double> > finals_;
};

class vpgl_adjust_rational_trans_multiple_pts
{
public:
  ~vpgl_adjust_rational_trans_multiple_pts() {}

  //: exhaustively searches the parameter space to find the best parameter setting
  static bool adjust(vcl_vector<vpgl_rational_camera<double> > const& cams,
                     vcl_vector<vcl_vector< vgl_point_2d<double> > > const& corrs,  // a vector of correspondences for each cam
                     double radius, int n,       // divide radius into n intervals to generate camera translation space
                     vcl_vector<vgl_vector_2d<double> >& cam_translations,          // output translations for each cam
                     vcl_vector<vgl_point_3d<double> >& intersections);             // output 3d locations for each correspondence

  //: run Lev-Marq optimization to search the param space to find the best parameter setting
  static bool adjust_lev_marq(vcl_vector<vpgl_rational_camera<double> > const& cams,
                     vcl_vector<vcl_vector< vgl_point_2d<double> > > const& corrs,  // a vector of correspondences for each cam
                     vcl_vector<vgl_vector_2d<double> >& cam_translations,          // output translations for each cam
                     vcl_vector<vgl_point_3d<double> >& intersections);             // output 3d locations for each correspondence

protected:
  vpgl_adjust_rational_trans_multiple_pts();
};


#endif // vpgl_adjust_rational_trans_onept_h_
