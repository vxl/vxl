// This is oxl/mvl/FMPlanarNonLinFun.h
#ifndef FMPlanarNonLinFun_h_
#define FMPlanarNonLinFun_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief a class that contains the functions required for FMPlanarComputeLinear.
//
// \author
//     Martin Armstrong, Oxford 21/11/96
//
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_double_3x3.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgMetric.h>
#include <mvl/HomgNorm2D.h>

class FMatrixPlanar;
class ImageMetric;

class FMPlanarNonLinFun : public vnl_least_squares_function
{
  // Data Members--------------------------------------------------------------
  int _data_size;

  vcl_vector<HomgPoint2D>& _points1;
  vcl_vector<HomgPoint2D>& _points2;

  HomgNorm2D _normalized;

  vnl_double_3x3 _denorm_matrix;
  vnl_double_3x3 _denorm_matrix_inv;


  double _outlier_distance_squared;
  int _terminate_count;

  HomgMetric _image_metric1;
  HomgMetric _image_metric2;

 public:

  // Constructors/Destructors--------------------------------------------------
  FMPlanarNonLinFun(const ImageMetric*, const ImageMetric*,
                    double outlier_distance_squared,
                    vcl_vector<HomgPoint2D>& points1,
                    vcl_vector<HomgPoint2D>& points2);

  // Computations--------------------------------------------------------------

  bool compute(FMatrixPlanar* F);

  // The virtual function from vnl_levenberg_marquardt
  void f(const vnl_vector<double>& x, vnl_vector<double>& fx);

 private:
  // Helpers-------------------------------------------------------------------
  void fmatrix_to_params(const FMatrixPlanar& F, vnl_vector<double>& params);
  FMatrixPlanar params_to_fmatrix(const vnl_vector<double>& params);

  void fmatrix_to_params_awf(const FMatrixPlanar& F, vnl_vector<double>& params);
  FMatrixPlanar params_to_fmatrix_awf(const vnl_vector<double>& params);

  void fmatrix_to_params_mna(const FMatrixPlanar& F, vnl_vector<double>& params);
  FMatrixPlanar params_to_fmatrix_mna(const vnl_vector<double>& params);
};

#endif // FMPlanarNonLinFun_h_
