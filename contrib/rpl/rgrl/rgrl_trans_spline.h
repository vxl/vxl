#ifndef rgrl_trans_spline_h_
#define rgrl_trans_spline_h_

#include "rgrl_transformation.h"
#include "rgrl_spline.h"
#include "rgrl_spline_sptr.h"
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>

//:
// \file
// \brief Here I only implement it as a cubic B-spline.
// \author Ying-Lin Bess Lee
// \date   Sept 2003
   
class rgrl_trans_spline
  : public rgrl_transformation
{
public:
  //: Constructor
  //  should not be used by anything other than reader
  //  use the following two constructors insead. 
  rgrl_trans_spline( int dim );
  
  //: Constructor
  rgrl_trans_spline( vcl_vector<rgrl_spline_sptr> const& splines, 
                     vnl_vector< double > const& x0, vnl_vector< double > const& delta, 
                     rgrl_transformation_sptr xform = 0 );
  //: Constructor
  rgrl_trans_spline( vcl_vector<rgrl_spline_sptr> const& splines, 
                     vnl_vector< double > const& x0, vnl_vector< double > const& delta, 
                     vnl_matrix< double > const& covar,
                     rgrl_transformation_sptr xform = 0 );

  ~rgrl_trans_spline() {}

  vnl_vector< double > const& get_delta() const { return delta_; }
  void set_covar( vnl_matrix<double> const& cov ) { covar_ = cov; }

  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p ) const;

  vnl_matrix<double> covar() const;

  rgrl_transformation_sptr get_global_xform( ) const { return xform_; }
  rgrl_spline_sptr get_spline( unsigned i ) const { return splines_[i]; }

  //: Return the jacobian of the transform
  vnl_matrix<double> jacobian( vnl_vector<double> const& from_loc ) const;

  // for tester to access the private members
  friend class test_rgrl_trans_spline;

  //: for output
  virtual void write( vcl_ostream& os ) const;
   
  //: for input
  virtual void read( vcl_istream& is );
  
  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_spline, rgrl_transformation);

protected:
  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double> & to ) const;

  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double> & to_dir) const;

private:
  void point_in_knots( vnl_vector< double > const& point, vnl_vector< double > & spline_pt ) const;

  // This is used for transform the data first and then spline is used
  // for estimate the displacement.
  rgrl_transformation_sptr xform_;

  // The displacement function has this form(in 3D): [x y z]^T = D( [x y z]^T )
  // This variable contains [ D_x D_y D_z ], each of which is an independent spline
  vcl_vector<rgrl_spline_sptr> splines_;
  
  // The covariance of control points of splines. The displacement in
  // x, y, z are independent to each other, the covariance between
  // each other's variables are 0.  So I don't save the covariance
  // between them.  However, the covariance between the parameters is
  // the same for each spline. So here, only one covariance is saved.
  vnl_matrix<double> covar_;
  vnl_vector< double >  x0_;
  vnl_vector<double> delta_;
};

#endif
