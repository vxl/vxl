#ifndef rgrl_trans_spline_h_
#define rgrl_trans_spline_h_
//:
// \file
// \brief Here I only implement it as a cubic B-spline.
// \author Ying-Lin Bess Lee
// \date   Sept 2003

#include <vector>
#include <iostream>
#include <iosfwd>
#include "rgrl_transformation.h"
#include "rgrl_spline.h"
#include "rgrl_spline_sptr.h"
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rgrl_trans_spline
  : public rgrl_transformation
{
 public:
  //: Constructor
  //  should not be used by anything other than reader
  //  use the following two constructors instead.
  rgrl_trans_spline(unsigned int dim = 0);

  //: Constructor
  rgrl_trans_spline( std::vector<rgrl_spline_sptr>  splines,
                     vnl_vector< double > const& x0, vnl_vector< double > const& delta,
                     const rgrl_transformation_sptr& xform = nullptr );
  //: Constructor
  rgrl_trans_spline( std::vector<rgrl_spline_sptr>  splines,
                     vnl_vector< double > const& x0, vnl_vector< double > const& delta,
                     vnl_matrix< double > const& covar,
                     const rgrl_transformation_sptr& xform = nullptr );

  ~rgrl_trans_spline() override = default;

  vnl_vector< double > const& get_delta() const { return delta_; }
  void set_covar( vnl_matrix<double> const& cov ) { covar_ = cov; }

  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p ) const override;

  rgrl_transformation_sptr get_global_xform( ) const { return xform_; }
  rgrl_spline_sptr get_spline( unsigned i ) const { return splines_[i]; }

  //: Compute jacobian w.r.t. location
  void jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const override;

  // for tester to access the private members
  friend class test_rgrl_trans_spline;

  //: for output
  void write( std::ostream& os ) const override;

  //: for input
  bool read( std::istream& is ) override;

  //: make a clone copy
  rgrl_transformation_sptr clone() const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_spline, rgrl_transformation);

 protected:
  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double> & to ) const override;

  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double> & to_dir) const override;

 private:
  void point_in_knots( vnl_vector< double > const& point, vnl_vector< double > & spline_pt ) const;

  // This is used for transform the data first and then spline is used
  // for estimate the displacement.
  rgrl_transformation_sptr xform_;

  // The displacement function has this form(in 3D): [x y z]^T = D( [x y z]^T )
  // This variable contains [ D_x D_y D_z ], each of which is an independent spline
  std::vector<rgrl_spline_sptr> splines_;

  // The covariance of control points of splines. The displacement in
  // x, y, z are independent to each other, the covariance between
  // each other's variables are 0.  So I don't save the covariance
  // between them.  However, the covariance between the parameters is
  // the same for each spline. So here, only one covariance is saved.
  vnl_vector<double> x0_;
  vnl_vector<double> delta_;

  // TODO - pure virtual functions of rgrl_transformation
  void inv_map(vnl_vector<double> const&, bool,
                       vnl_vector<double> const&, vnl_vector<double>&, vnl_vector<double>&) const override;
  void inv_map(vnl_vector<double> const&, vnl_vector<double>&) const override;
  rgrl_transformation_sptr inverse_transform() const override;
  rgrl_transformation_sptr scale_by(double) const override;
};

#endif
