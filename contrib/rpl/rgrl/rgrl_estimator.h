#ifndef rgrl_estimator_h_
#define rgrl_estimator_h_

//:
// \file
// \author Amitha Perera
// \date   Feb 2003
//
// \verbatim
//   2003-09 Charlene Tsai. Added the functions to support random sampling.
// \endverbatim
//

#include <vcl_cassert.h>

#include "rgrl_set_of.h"
#include "rgrl_match_set_sptr.h"
#include "rgrl_transformation.h"
#include "rgrl_object.h"

#include "rgrl_estimator_sptr.h"

//: Interface for the transform estimators
//
// An estimator creates a transform object from a set of matches.
//
class rgrl_estimator
  : public rgrl_object
{
public:
  //: Default constructor
  //
  //  Does nothing. 
  rgrl_estimator();

  //: Constructor.
  //
  // See the comments for param_dof(). The parameter is required by
  // some algorithms such as random sampling and DBICP.
  rgrl_estimator( unsigned int param_dof );

  virtual
  ~rgrl_estimator();

  //: Estimate the transform
  //
  // Given a collectsion of match sets in \a matches and the current
  // transform estimate (from the previous iteration, for example),
  // this function will estimate the transformation parameters and
  // return a transform object that captures the estimated transform.
  //
  virtual
  rgrl_transformation_sptr estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
                                     rgrl_transformation const& cur_transform ) const = 0;


  //: Estimate the transform
  //
  // Given a set of matches in \a matches and the current transform
  // estimate (from the previous iteration, for example), this
  // function will estimate the transformation parameters and return a
  // transform object that captures the estimated transform.
  //
  // The default implementation in the abstract base class will simply
  // construct a size one collection of match sets and call the other
  // estimate function.
  //
  virtual
  rgrl_transformation_sptr estimate( rgrl_match_set_sptr matches,
                                     rgrl_transformation const& cur_transform ) const = 0;

  //: The degrees of freedom in the parameter set. 
  unsigned int param_dof() const { assert (dof_); return dof_; }

  //: Set the degrees of freedom
  void set_param_dof(unsigned int dof) { dof_ = dof; }

  //: The degrees of freedom in the residual.
  //
  //  Most of the time, this would be 1 since the residual comes from
  //  a single random variable. In some problems, however, the error
  //  is the combination of more than one random variable. (For
  //  example, if the residual is a 2d Euclidean distance with
  //  possible error in both coordinates, the degrees of freedom in
  //  the error will be 2.)
  virtual unsigned int residual_dof() const { return 1; }

  //: Type of transformation estimated by this estimator.
  virtual
  const vcl_type_info& transformation_type() const = 0;
  
  // Defines type-related functions
  rgrl_type_macro( rgrl_estimator, rgrl_object );

private:
  unsigned int dof_;
};

#endif // rgrl_estimator_h_
