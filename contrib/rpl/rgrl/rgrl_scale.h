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


#include <vnl/vnl_matrix.h>

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

  //:
  bool has_geometric_scale() const;

  //: Valid iff has_geometric_scale() is true.
  double geometric_scale( ) const;

  //:
  bool has_signature_covar() const;

  //:  Valid iff has_signature_covariance() is true.
  vnl_matrix<double> const& signature_covar() const;

  //:  Set all scale information
  void set_scales( double                     geometric_scale,
                   vnl_matrix<double>  const& signature_covar );

  //:  Set the geometric scale
  void set_geometric_scale( double scale );

  //:  Set the signature covariance matrix
  void set_signature_covar( vnl_matrix<double> const& covar );

  // Defines type-related functions
  rgrl_type_macro( rgrl_scale, rgrl_object );

private:
  bool                has_geometric_scale_;
  double              geometric_scale_;
  bool                has_signature_covar_;
  vnl_matrix<double>  signature_covar_;
};

#endif
