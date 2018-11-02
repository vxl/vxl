#ifndef rgrl_trans_translation_h_
#define rgrl_trans_translation_h_
//:
// \file
// \brief Derived class to represent a translation transformation in arbitrary dimensions.
// \author Charlene Tsai
// \date Dec 2003

#include <iostream>
#include <iosfwd>
#include "rgrl_transformation.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rgrl_trans_translation
  : public rgrl_transformation
{
 public:
  //: Initialize to the identity transformation.
  //
  rgrl_trans_translation(unsigned int dimension = 0);

  //: Construct translation standard transform
  //
  //  The transform is q = p + \a trans.
  //
  rgrl_trans_translation( vnl_vector<double> const& trans,
                          vnl_matrix<double> const& covar );

  //: Construct translation standard transform with unknown covariance matrix
  //
  //  The transform is q = p + \a trans.
  //  The covariance matrix is set to 0 vector.
  //
  rgrl_trans_translation( vnl_vector<double> const& trans );

  //: Construct a centered translation transform.
  //
  //  The transform is q = ( p - \a from_centre ) + \a trans + \a to_centre.
  //
  rgrl_trans_translation( vnl_vector<double> const& trans,
                          vnl_matrix<double> const& covar,
                          vnl_vector<double> const& from_centre,
                          vnl_vector<double> const& to_centre );

  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p  ) const override;

  //: The translation component of the translation transform
  vnl_vector<double> t() const;

  //:  Inverse map with an initial guess
  void inv_map( const vnl_vector<double>& to,
                bool initialize_next,
                const vnl_vector<double>& to_delta,
                vnl_vector<double>& from,
                vnl_vector<double>& from_next_est) const override;

  //:  Inverse map based on the transformation.
  void inv_map( const vnl_vector<double>& to,
                vnl_vector<double>& from ) const override;

  //: is this an invertible transformation?
  bool is_invertible() const override { return true; }

  //: Return an inverse transformation
  //  This function only exist for certain transformations.
  rgrl_transformation_sptr inverse_transform() const override;

  //: Compute jacobian w.r.t. location
  void jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const override;

  //:  transform the transformation for images of different resolution
  rgrl_transformation_sptr scale_by( double scale ) const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_translation, rgrl_transformation );

  // for output
  void write(std::ostream& os ) const override;

  // for input
  bool read(std::istream& is ) override;

  //: make a clone copy
  rgrl_transformation_sptr clone() const override;

 protected:
  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double>      & to ) const override;

  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double>      & to_dir    ) const override;

 private:
  vnl_vector<double> trans_;
  vnl_vector<double> from_centre_;
};


#endif
