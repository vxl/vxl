#ifndef rgrl_trans_mixed_spline_h_
#define rgrl_trans_mixed_spline_h_
//:
// \file
#include <iostream>
#include <utility>
#include <vector>
#include "rgrl_trans_spline.h"
#include "rgrl_transformation.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rgrl_trans_mixed_spline
  : public rgrl_transformation
{
 public:
  //: Constructor
  rgrl_trans_mixed_spline( std::vector< rgrl_trans_spline >  trans_splines )
    : trans_splines_(std::move( trans_splines )){}

  ~rgrl_trans_mixed_spline() override = default;

  void add_trans( rgrl_trans_spline trans_spline )
  {
    trans_splines_.push_back( trans_spline );
  }

  vnl_matrix<double>
  transfer_error_covar( vnl_vector<double> const& p ) const override;

  //: Compute jacobian w.r.t. location
  void jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const override;

  //: make a clone copy
  rgrl_transformation_sptr clone() const override;

  //:  transform the transformation for images of different resolution
  rgrl_transformation_sptr scale_by( double scale ) const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_mixed_spline, rgrl_transformation );

 protected:
  void map_loc( vnl_vector< double > const& from,
                vnl_vector< double > & to ) const override;

  void map_dir( vnl_vector< double > const& from_loc,
                vnl_vector< double > const& from_dir,
                vnl_vector< double > & to_dir ) const override;

 private:
  // each rgrl_trans_spline defines its own region transformation
  // the transformation is the sum of these rgrl_trans_spline
  std::vector< rgrl_trans_spline > trans_splines_;
};

#endif
