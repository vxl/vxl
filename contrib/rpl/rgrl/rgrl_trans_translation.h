#ifndef rgrl_trans_translation_h_
#define rgrl_trans_translation_h_
//:
// \file
// \brief Derived class to represent an translation transformation in arbitrary dimensions.
// \author Charlene Tsai
// \date Dec 2003

#include "rgrl_transformation.h"
#include <vcl_iosfwd.h>

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

  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p  ) const;

  //: The translation component of the translation transform
  vnl_vector<double> t() const;

  //:  Inverse map with an initial guess
  void inv_map( const vnl_vector<double>& to,
                bool initialize_next,
                const vnl_vector<double>& to_delta,
                vnl_vector<double>& from,
                vnl_vector<double>& from_next_est) const;

  //:  Inverse map based on the transformation.
  void inv_map( const vnl_vector<double>& to,
                vnl_vector<double>& from ) const;

  //: is this an invertible transformation?
  virtual bool is_invertible() const { return true; }

  //: Return an inverse transformation
  //  This function only exist for certain transformations.
  virtual rgrl_transformation_sptr inverse_transform() const;

  //: Compute jacobian w.r.t. location
  virtual void jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const;

  //:  transform the transformation for images of different resolution
  rgrl_transformation_sptr scale_by( double scale ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_translation, rgrl_transformation );

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
  vnl_vector<double> trans_;
  vnl_vector<double> from_centre_;
};


#endif
