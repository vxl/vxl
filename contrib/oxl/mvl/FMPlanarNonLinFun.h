#ifndef FMPlanarNonLinFun_h_
#define FMPlanarNonLinFun_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	FMPlanarNonLinFun
// .LIBRARY	MViewCompute
// .HEADER	MultiView Package
// .INCLUDE	mvl/FMPlanarNonLinFun.h
// .FILE	FMPlanarNonLinFun.cxx
//
// .SECTION Description
//    FMPlanarNonLinFun is a class that contains the functions required for
//  FMPlanarComputeLinear.
//
// .SECTION Author
//     Martin Armstrong, Oxford 21/11/96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_double_3x3.h>
#include <mvl/HomgPoint2D.h>
#if defined(VCL_GCC_27)
# include <mvl/HomgLine2D.h>
#endif
#include <mvl/HomgMetric.h>
#include <mvl/HomgNorm2D.h>

class FMatrixPlanar;
class ImageMetric;

class FMPlanarNonLinFun : public vnl_least_squares_function {
public:

  // Constructors/Destructors--------------------------------------------------
  FMPlanarNonLinFun(const ImageMetric*, const ImageMetric*,
			double outlier_distance_squared,
			vcl_vector<HomgPoint2D>& points1,
			vcl_vector<HomgPoint2D>& points2);

  // Operations----------------------------------------------------------------

  // Computations--------------------------------------------------------------

  bool compute(FMatrixPlanar* F);

// The virtual function from vnl_levenberg_marquardt
  void f(const vnl_vector<double>& x, vnl_vector<double>& fx);

  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

private:
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

  // Helpers-------------------------------------------------------------------
  void fmatrix_to_params(const FMatrixPlanar& F, vnl_vector<double>& params);
  FMatrixPlanar params_to_fmatrix(const vnl_vector<double>& params);

  void fmatrix_to_params_awf(const FMatrixPlanar& F, vnl_vector<double>& params);
  FMatrixPlanar params_to_fmatrix_awf(const vnl_vector<double>& params);

  void fmatrix_to_params_mna(const FMatrixPlanar& F, vnl_vector<double>& params);
  FMatrixPlanar params_to_fmatrix_mna(const vnl_vector<double>& params);
};

#endif // FMPlanarNonLinFun_h_
