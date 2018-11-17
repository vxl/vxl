// This is brl/bseg/sdet/sdet_gauss_fit.h
#ifndef sdet_gauss_fit_h_
#define sdet_gauss_fit_h_
//:
// \file
// \brief Fit 2d gaussian to a small peak in image intensity
// \author J. Green
// patterned after Joe Mundy's vpgl_rational_adjust
// \date August 12, 2007
//

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vgl/vgl_point_3d.h>
#include <vsol/vsol_point_2d_sptr.h>

//: Class to fit a 2-d Gaussian to small peaks in a polygon drawn on image.
//  This class started out to fit one peak in a polygon area, but has been
//  expanded to fit up to 4 peaks.  The approximate locations of the peaks
//  (1-4) are picked using the pick_point_set() method that was added to
//  the bgui_picker_tableau class.

class sdet_adjust_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  sdet_adjust_lsqr( std::vector<vgl_point_3d<double> >  img_pts,
                    unsigned int num_unknowns, unsigned int num_residuals,
                    int n_peaks);

  // Destructor
  ~sdet_adjust_lsqr() override = default;

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;

#if 0
  //: Called after each LM iteration to print debugging etc.
  virtual void trace(int iteration, vnl_vector<double> const& x,
                     vnl_vector<double> const& fx);
#endif

 protected:
 // 3 value vector;  1&2 are x&y indices of pixel, 3 is intensity of pixel;
  std::vector<vgl_point_3d<double> > img_pts_;
  unsigned num_pixels_;
  int n_peaks_;
};

class sdet_gauss_fit
{
 public:
  ~sdet_gauss_fit()= default;

  static vnl_vector<double> adjust(const std::vector<vgl_point_3d<double> >& img_pts,
                                   std::vector<vsol_point_2d_sptr> ps_list,
                                   int n_peaks,
                                   std::ofstream& outfile,
                                   double xmin, double ymin);

  static vnl_vector<double> calculate_ellipse(vnl_vector<double> result,
                                              float x,
                                              float y,
                                              int i,
                                              int n_peaks,
                                              std::ofstream& outfile);

 protected:
  sdet_gauss_fit();
};

#endif // sdet_gauss_fit_h_
