#ifndef rgrl_est_spline_h_
#define rgrl_est_spline_h_
//:
// \file
// \author Lee, Ying-Lin (Bess)
// \date   Sept 2003

#include "rgrl_estimator.h"
#include "rgrl_estimator_sptr.h"
#include "rgrl_mask.h"

enum rgrl_optimize_method
{
  RGRL_LEVENBERG_MARQUARDT=0,
  RGRL_CONJUGATE_GRADIENT=1,
  RGRL_AMOEBA=2,
  RGRL_LBFGS=3,
  RGRL_NONE=4,
  RGRL_POWELL=5
};

class rgrl_est_spline
  : public rgrl_estimator
{
 public:
  //: Constructor
  //
  // Only B-spline estimation is performed. Neither is the global
  // estimation performed and nor is the global xformation used.
  rgrl_est_spline( unsigned dof, rgrl_mask_box const& roi, vnl_vector<double> const& delta,
                   vnl_vector< unsigned > const& m,
                   bool use_thin_plate = false, double lambda = 0.001 );

  //: Constructor
  //
  // global_xform is used as the global xformation. No global
  // estimation will be performed.
  rgrl_est_spline( unsigned dof, rgrl_transformation_sptr global_xform,
                   rgrl_mask_box const& roi, vnl_vector<double> const& delta,
                   vnl_vector< unsigned > const& m,
                   bool use_thin_plate = false, double lambda = 0.001 );

  rgrl_transformation_sptr
  estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
            rgrl_transformation const& cur_transform ) const;

  rgrl_transformation_sptr
  estimate( rgrl_match_set_sptr matches,
            rgrl_transformation const& cur_transform ) const;

  //: Type of transformation estimated by this estimator.
  const vcl_type_info& transformation_type() const;

  void set_optimize_method( int optimize_method )
    { optimize_method_ = optimize_method; }

  // Defines type-related functions
  rgrl_type_macro( rgrl_est_spline, rgrl_estimator );

 private:
  void point_in_knots( vnl_vector< double > const& point, vnl_vector< double > & spline_pt ) const;

  rgrl_mask_box roi_;
  vnl_vector<double> delta_;
  vnl_vector<unsigned> m_;
  bool use_thin_plate_;
  // the multiplier for the thin-plate constraint
  double lambda_;
  // determine which method is used to estimate the parameters.
  int optimize_method_;

  //: A global transformation.
  //  If it's null, then the spline is used without other prior transforation.
  rgrl_transformation_sptr global_xform_;
};

#endif

