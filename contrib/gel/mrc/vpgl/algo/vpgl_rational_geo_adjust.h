// This is gel/mrc/vpgl/algo/vpgl_rational_geo_adjust.h
#ifndef vpgl_rational_geo_adjust_h_
#define vpgl_rational_geo_adjust_h_
//:
// \file
// \brief Adust 3-d offset and scale to align rational cameras to geolocations
// \author J. L. Mundy
// \date August 06, 2007
//

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

//:
// The 3-d offset and scale parameters of rational cameras typically
// must be adjusted to compensate for errors in geographic alignment.
// This algorithm adjusts these parameters to give the smallest
// projection error. That is, the error between the true image location
// and the projected 3-d world point corresponding to that location.

class vpgl_adjust_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  // \note image points are not homogeneous because require finite points to measure projection error
  vpgl_adjust_lsqr(vpgl_rational_camera<double>  const& rcam,
                   vcl_vector<vgl_point_2d<double> > const& img_pts,
                   vcl_vector<vgl_point_3d<double> > const& geo_pts,
                   unsigned num_unknowns, unsigned num_residuals);

  //: Destructor
  virtual ~vpgl_adjust_lsqr() {}

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  fx has been sized appropriately before the call.
  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

#if 0
  //: Called after each LM iteration to print debugging etc.
  virtual void trace(int iteration, vnl_vector<double> const& x, vnl_vector<double> const& fx);
#endif

 protected:
  unsigned num_corrs_;
  vpgl_rational_camera<double> rcam_;
  vcl_vector<vgl_point_2d<double> > img_pts_;
  vcl_vector<vgl_point_3d<double> > geo_pts_;
};

class vpgl_rational_geo_adjust
{
 public:
  ~vpgl_rational_geo_adjust(){}

 static bool adjust(vpgl_rational_camera<double> const& initial_rcam,
                    vcl_vector<vgl_point_2d<double> > img_pts,
                    vcl_vector<vgl_point_3d<double> > geo_pts,
                    vpgl_rational_camera<double> & adj_rcam);
 protected:
 vpgl_rational_geo_adjust();
};


#endif // vpgl_rational_geo_adjust_h_
