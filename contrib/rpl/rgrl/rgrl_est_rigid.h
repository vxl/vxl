#ifndef rgrl_est_rigid_h_
#define rgrl_est_rigid_h_

//:
// \file
// \author Tomasz Malisiewicz
// \date   March 2004

#include "rgrl_estimator.h"
#include <vcl_vector.h>
#include "rgrl_trans_rigid.h"

//: Rigid transform estimator
//
class rgrl_est_rigid
  : public rgrl_estimator
{
public:
  // Constructor which takes the dimension of the dataset that the
  // estimator will be applied on.
  rgrl_est_rigid( unsigned int dimension);

  //: Estimates an rigid transform.
  //
  // The return pointer is to a rgrl_trans_rigid object.
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
            rgrl_transformation const& cur_transform ) const;


  //: Estimates a rigid transform.
  //
  // The return pointer is to a rgrl_trans_rigid object.
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_match_set_sptr matches,
            rgrl_transformation const& cur_transform ) const;

  //: Type of transformation estimated by this estimator.
  const vcl_type_info& transformation_type() const;

  //: Determine the covariance matrix of this rigid xform given the matches
  void determine_covariance( rgrl_set_of<rgrl_match_set_sptr> const& matches, rgrl_transformation_sptr current_trans) const;

  //: Return the estimation statistics
  //  for each iteration, the stats are 
  //    "determinant of DeltaR before orthonormalization", 
  //    "fro norm of orthonormalized DeltaR - I",
  //    "fro norm of DeltaTrans"
  inline vcl_vector<vcl_vector<double> > const& get_stats() { return this->stats; }

  // Defines type-related functions
  rgrl_type_macro( rgrl_est_rigid, rgrl_estimator );

protected:
  vcl_vector<vcl_vector<double> > stats;
};

#endif // rgrl_est_rigid_h_
