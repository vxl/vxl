#ifndef rgrl_trans_rigid_h_
#define rgrl_trans_rigid_h_
//:
// \file
// \author Tomasz Malisiewicz
// \date   March 2004

#include "rgrl_transformation.h"
#include <vcl_iosfwd.h>

//: Represents a rigid transformation.
//
//  A rigid transform is q = R p + t where R
//  is an n-dimensional rotation matrix, and t is an n-dimensional
//  translation vector.  This is just like a similarity transformation with scale=1.
//  Note that R has {9,4} parameters, but only {3,1} degrees of freedom in {3D,2D}.
//  t has {3,2} degrees of freedom in {3D,2D}.
//  This can be most easily seen from the fact that rotation matrices are orthonormal.
//  The rigid transform has {6,3} total degrees of freedom (rotation + translation) in {3D,2D}.
//  The relationship between dimensionality(d) and dof(degrees of freedom) is: dof = 3(d-1)
class rgrl_trans_rigid
  : public rgrl_transformation
{
 public:
  //: Initialize to the identity transformation.
  //
  rgrl_trans_rigid( unsigned int dimension );

  //: Constructor based on an initial transformation and covar estimate
  //
  rgrl_trans_rigid( vnl_matrix<double> const& rot,
                    vnl_vector<double> const& trans,
                    vnl_matrix<double> const& covar );

  //: Constructor based on an initial transformation and unknown covar
  //
  //  The  covariance matrix is a zero matrix.
  rgrl_trans_rigid( vnl_matrix<double> const& rot,
                    vnl_vector<double> const& trans );

  //: Sets the translation component of this transformation
  //  \note This method is valid for only the 3d version
  void set_translation(double tx, double ty, double tz);

  //: Sets the translation component of this transformation
  //  \note This method is valid for only the 2d version
  void set_translation(double tx, double ty);

  //: Sets the rotation part of this transformation
  //  \note This method is valid for only the 3d version
  void set_rotation(double theta, double alpha, double phi);

  //: Sets the rotation part of this transformation
  //  \note This method is valid for only the 2d version
  void set_rotation(double theta);

  //: Given a rotation matrix which is the product of three independent rotations, extract the angles
  //  \note This method is valid for only the 3d version
  void determine_angles( double& phi, double& alpha, double& theta) const;

  //: Extract the angles
  //  \note This method is valid for only the 2d version
  void determine_angles( double& theta ) const;


  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p  ) const;

  //: The rotation component of this transform
  vnl_matrix<double> const& R() const;

  //: The translation component of the transform
  vnl_vector<double> t() const;

  //:  Inverse map with an initial guess
  void inv_map( const vnl_vector<double>& to,
                bool initialize_next,
                const vnl_vector<double>& to_delta,
                vnl_vector<double>& from,
                vnl_vector<double>& from_next_est) const;

  //:  Inverse map based on the transformation.
  //   The inverse mapping for R(p)+ t = q is p = A^-1(q-t)
  void inv_map( const vnl_vector<double>& to,
                vnl_vector<double>& from ) const;

  //: is this an invertible transformation?
  virtual bool is_invertible() const { return true; }
  
  //: Return an inverse transformation
  //  This function only exist for certain transformations.
  virtual rgrl_transformation_sptr inverse_transform() const;

  //: Return the jacobian of the transform
  vnl_matrix<double> jacobian( vnl_vector<double> const& from_loc ) const;

  //:  transform the transformation for images of different resolution
  rgrl_transformation_sptr scale_by( double scale ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_rigid, rgrl_transformation )

  // for output
  virtual void write(vcl_ostream& os ) const;

  // for input
  virtual void read(vcl_istream& is );

 protected:
  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double>      & to ) const;

  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double>      & to_dir    ) const;

 private:
  vnl_matrix<double> R_;
  vnl_vector<double> trans_;
};


#endif
