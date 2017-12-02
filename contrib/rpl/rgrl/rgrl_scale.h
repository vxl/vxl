#ifndef rgrl_scale_h_
#define rgrl_scale_h_
//:
// \file
// \brief  Class to represent error scale estimates.
// \author Chuck Stewart
// \date   14 Nov 2002
//
// This class must be extended if covariance matrix for the geometric
// error is to be added!
// \verbatim
//  Modifications
//   Gehua Yang   Feb. 2006   add a flag to indicate whether the scale is prior or an estimate
// \endverbatim


#include <iostream>
#include <iosfwd>
#include <vnl/vnl_matrix.h>
#include <vcl_compiler.h>

#include "rgrl_scale_sptr.h"
#include "rgrl_object.h"

//:
// Represents error scale estimates.
//
class rgrl_scale
  : public rgrl_object
{
 public:
  rgrl_scale( );

  //: scale type
  enum type { prior, estimate };

  //:
  bool has_geometric_scale() const;

  //: Valid iff has_geometric_scale() is true.
  double geometric_scale( ) const;

  //:
  bool has_signature_inv_covar() const;

  //:  Valid iff has_signature_inv_covariance() is true.
  vnl_matrix<double> const& signature_inv_covar() const;

  //:  Set all scale information
  void set_scales( double                     geometric_scale,
                   vnl_matrix<double>  const& signature_inv_covar,
                   type                       new_type = estimate );

  //:  Set the geometric scale
  void set_geometric_scale( double scale, type new_type = estimate );

  //:  Set the signature covariance matrix
  void set_signature_inv_covar( vnl_matrix<double> const& covar );

  // Defines type-related functions
  rgrl_type_macro( rgrl_scale, rgrl_object );

  //: return scale type
  type geo_scale_type() const { return geo_scale_type_; }

  //: set scale type
  void set_geo_scale_type(type new_type)
  { geo_scale_type_ = new_type; }

  //: write to io stream
  void write( std::ostream& os ) const;

  //: read from io stream
  bool read( std::istream& is );

 private:
  bool                has_geometric_scale_;
  double              geometric_scale_;
  bool                has_signature_inv_covar_;
  vnl_matrix<double>  signature_inv_covar_;

  type                geo_scale_type_;
};

//: output operator
std::ostream&
operator<<( std::ostream& ofs, rgrl_scale const& scale );

//: input operator
std::istream&
operator>>( std::istream& ifs, rgrl_scale& scale );

#endif // rgrl_scale_h_
