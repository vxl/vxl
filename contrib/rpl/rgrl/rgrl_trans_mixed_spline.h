#ifndef rgrl_trans_mixed_spline_h_
#define rgrl_trans_mixed_spline_h_
//:
// \file
#include "rgrl_transformation.h"
#include "rgrl_trans_spline.h"
#include <vcl_vector.h>

class rgrl_trans_mixed_spline
  : public rgrl_transformation
{
 public:
  //: Constructor
  rgrl_trans_mixed_spline( vcl_vector< rgrl_trans_spline > const& trans_splines )
    : trans_splines_( trans_splines ){}

  ~rgrl_trans_mixed_spline() {}

  void add_trans( rgrl_trans_spline trans_spline )
  {
    trans_splines_.push_back( trans_spline );
  }

  vnl_matrix<double>
  transfer_error_covar( vnl_vector<double> const& p ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_mixed_spline, rgrl_transformation );

 protected:
  void map_loc( vnl_vector< double > const& from,
                vnl_vector< double > & to ) const;

  void map_dir( vnl_vector< double > const& from_loc,
                vnl_vector< double > const& from_dir,
                vnl_vector< double > & to_dir ) const;

 private:
  // each rgrl_trans_spline defines its own region transformation
  // the transformation is the sum of these rgrl_trans_spline
  vcl_vector< rgrl_trans_spline > trans_splines_;
};

#endif
