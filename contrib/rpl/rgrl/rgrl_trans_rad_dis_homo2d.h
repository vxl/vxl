#ifndef rgrl_trans_rad_dis_homo2d_h_
#define rgrl_trans_rad_dis_homo2d_h_
//:
// \file
// \author Gehua Yang
// \date   Feb 2005

#include "rgrl_transformation.h"
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iosfwd.h>

//: Represents a 2D homography transformation with radial lens distortion
//
//  From image Distortion: 
//    Xu = (1+k1*R^2) Xd
//  To image Distortion
//    Yd = (1+k1*R^2) Yu
//  Homography transformation
//    Y=H(X)
//
// NOTE: everything is in 2D unhomogenous coordinate,
//       therefore it is not linear

class rgrl_trans_rad_dis_homo2d
  : public rgrl_transformation
{
 public:
  //: Initialize to the identity matrix
  rgrl_trans_rad_dis_homo2d();

  //: Constructor based on an initial transformation and covar estimate
  //
  //rgrl_trans_rad_dis_homo2d( vnl_matrix<double> const& H,
  //                           double k1_from,
  //                           double k1_to,
  //                           vnl_matrix<double> const& covar );

  //: Constructor based on an initial transformation and unknown covar
  //
  //  The  covariance matrix is a zero matrix.
  rgrl_trans_rad_dis_homo2d( vnl_matrix<double> const& H,
                             double k1_from, 
                             double k1_to,
                             vnl_vector<double> const& from_centre,
                             vnl_vector<double> const& to_centre );

  //: Construct a centered transform.
  //
  rgrl_trans_rad_dis_homo2d( vnl_matrix<double> const& H,
                             double k1_from,
                             double k1_to,
                             vnl_matrix<double> const& covar,
                             vnl_vector<double> const& from_centre,
                             vnl_vector<double> const& to_centre );

  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p  ) const;

  //: The H matrix of the transform
  vnl_matrix_fixed<double, 3, 3> const& H() const { return H_; }

  //: radial distortion term on From image
  double k1_from() const { return k1_from_; }

  //: radial distortion term on To image
  double k1_to() const { return k1_to_; }

  //: From center
  vnl_vector<double> from_centre() const { return from_centre_.as_ref(); }
  
  //: To center
  vnl_vector<double> to_centre() const { return to_centre_.as_ref(); }
  
  //: Inverse map using pseudo-inverse of H_.
  void inv_map( const vnl_vector<double>& to,
                vnl_vector<double>& from ) const;

  //:  Inverse map with an initial guess
  void inv_map( const vnl_vector<double>& to,
                bool initialize_next,
                const vnl_vector<double>& to_delta,
                vnl_vector<double>& from,
                vnl_vector<double>& from_next_est) const;

  //: is this an invertible transformation?
  bool is_invertible() const { return false; }

  //: Return an inverse transformation of the uncentered transform
  rgrl_transformation_sptr inverse_transform() const;

  //: Return the jacobian of the transform w/ respect to inhomogenous coordinate
  //  This is a 2x2 matrix
  vnl_matrix<double> jacobian( vnl_vector<double> const& from_loc ) const;

  //:  transform the transformation for images of different resolution
  rgrl_transformation_sptr scale_by( double scale ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_rad_dis_homo2d, rgrl_transformation );

  // for output UNCENTERED transformation, with the origin as the center.
  void write(vcl_ostream& os ) const;

  // for input
  void read(vcl_istream& is );

  // uncenter H matrix
  vnl_matrix_fixed<double, 3, 3>  
  uncenter_H_matrix( ) const;

 protected:
  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double>      & to ) const;

  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double>      & to_dir    ) const;

 private:
  vnl_matrix_fixed<double, 3, 3> H_;
  vnl_vector_fixed<double, 2> from_centre_;
  vnl_vector_fixed<double, 2> to_centre_;

  double k1_from_;
  double k1_to_;
};

#endif
