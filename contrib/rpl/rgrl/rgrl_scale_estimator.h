#ifndef rgrl_scale_estimator_h_
#define rgrl_scale_estimator_h_

//:
// \file
// \author Amitha Perera
// \date   Feb 2003
//

#include "rgrl_scale_sptr.h"
#include "rgrl_object.h"

class rgrl_scale;
class rgrl_match_set;

#include "rgrl_scale_estimator_sptr.h"

// We want to have smart pointers to the scale the weighted and
// unweighted scale estimators, and so they should be reference
// counted. However, dervied classes may implement both interfaces, so
// we must make sure that the having a smart pointer via either type
// of base doesn't mess up the reference count. The solution: a the
// reference counter is a virtual base class. Since it doesn't require
// any special construction, the derived classes are not any more
// complicated.

//: Interface for unweighted scale estimator.
//
// Actual estimators will probably inherit from (provide the interface
// for) both this and for rgrl_scale_estimator_wgted
//
class rgrl_scale_estimator_unwgted
  : virtual public rgrl_object
{
public:
  rgrl_scale_estimator_unwgted();

  virtual
  ~rgrl_scale_estimator_unwgted();

  //: Estimated an unweighted scale.
  //
  // \a match_set stores the current matches, and, if not null, \a
  // current_scales has the current scale estimates. (Perhaps from the
  // previous iteration.)
  //
  virtual
  rgrl_scale_sptr
  estimate_unweighted( rgrl_match_set const& match_set,
                       rgrl_scale_sptr const& current_scales,
                       bool penalize_scaling = false) const = 0;

  // Defines type-related functions
  rgrl_type_macro( rgrl_scale_estimator_unwgted, rgrl_object );
};


//: Interface for weighted scale estimator.
//
// Actual estimators will probably inherit from (provide the interface
// for) both this and for rgrl_scale_estimator_wgted
//
class rgrl_scale_estimator_wgted
  : virtual public rgrl_object
{
public:
  rgrl_scale_estimator_wgted();

  virtual
  ~rgrl_scale_estimator_wgted();

  //: Estimated a weighted scale.
  //
  // \a match_set stores the current matches, and, if not null, \a
  // current_scales has the current scale estimates. (Perhaps from the
  // previous iteration.)
  //
  virtual
  rgrl_scale_sptr
  estimate_weighted( rgrl_match_set const& match_set,
                     rgrl_scale_sptr const& current_scales,
                     bool use_signature_only = false,
                     bool penalize_scaling = false) const = 0;

  // Defines type-related functions
  rgrl_type_macro( rgrl_scale_estimator_wgted, rgrl_object );
};


//: Interface for scale estimator that can estimate with and without weights.
//
class rgrl_scale_estimator
  : public rgrl_scale_estimator_unwgted,
    public rgrl_scale_estimator_wgted
{
public:
  static const vcl_type_info& type_id()
  { return typeid(rgrl_scale_estimator); }
  
  virtual bool is_type( const vcl_type_info& type ) const
  { return (typeid(rgrl_scale_estimator) == type)!=0 || 
      this->rgrl_scale_estimator_unwgted::is_type(type) ||
      this->rgrl_scale_estimator_wgted::is_type(type); }

};

#endif // rgrl_scale_estimator_h_
