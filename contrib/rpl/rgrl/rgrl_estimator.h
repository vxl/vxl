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

  //: whether this method is iterative or non-iterative
  virtual 
  bool 
  is_iterative_method() const = 0;
  

  //: Estimate the transform
  //
  // Given a collectsion of match sets in \a matches and the current
  // transform estimate (from the previous iteration, for example),
  // this function will estimate the transformation parameters and
  // return a transform object that captures the estimated transform.
  //
  virtual
  rgrl_transformation_sptr 
  estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
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
  rgrl_transformation_sptr 
  estimate( rgrl_match_set_sptr matches,
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

// ===================================================================
//
//: Interface for linear transform estimators
//
// An estimator creates a transform object from a set of matches.
//
class rgrl_linear_estimator
  : public rgrl_estimator
{
public:
  //: Default constructor
  //
  //  Does nothing. 
  rgrl_linear_estimator()
   : rgrl_estimator()
  {  }

  //: Constructor.
  //
  // See the comments for param_dof(). The parameter is required by
  // some algorithms such as random sampling and DBICP.
  rgrl_linear_estimator( unsigned int param_dof )
   : rgrl_estimator( param_dof )
  {   }

  virtual
  ~rgrl_linear_estimator()
  {   }

  //: Linear estimator is non-iterative
  //
  virtual 
  bool 
  is_iterative_method() const
  { return false; }
  
};

// ===================================================================
//
//: Interface for non-linear transform estimators
//
// An estimator creates a transform object from a set of matches.
//
class rgrl_nonlinear_estimator
  : public rgrl_estimator
{
public:
  //: Default constructor
  //
  //  Does nothing. 
  rgrl_nonlinear_estimator()
   : max_num_iterations_(0),
     relative_threshold_( 1e-8 ),
     rgrl_estimator()
  {  }

  //: Constructor.
  //
  // See the comments for param_dof(). The parameter is required by
  // some algorithms such as random sampling and DBICP.
  rgrl_nonlinear_estimator( unsigned int param_dof )
   : max_num_iterations_(0),
     relative_threshold_( 1e-8 ),
     rgrl_estimator()
  {   }

  virtual
  ~rgrl_nonlinear_estimator()
  {   }

  //: Linear estimator is non-iterative
  //
  virtual 
  bool 
  is_iterative_method() const
  { return true; }

  //: set max number of iterations
  void set_max_num_iter( int max ) 
  { max_num_iterations_ = max; }
  
  //: return max number of iterations
  int  max_num_iter() const
  { return max_num_iterations_; }
  
  //: set relative threshold for parameters change
  void set_rel_thres( double thres )
  { relative_threshold_ = thres; }
  
  //: relative threshold   
  double rel_thres() const
  { return relative_threshold_; }

protected:
  //: specify the maximum number of iterations for this estimator
  int max_num_iterations_;
  
  //: The threshold for relative parameter change before termination
  double relative_threshold_; 
};

#endif // rgrl_estimator_h_
