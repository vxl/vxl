#ifndef rgrl_cast_h_
#define rgrl_cast_h_
//:
// \file
// \author Amitha Perera
// \brief Cast operations to simplify downcasting in the hierarchy.

#include "rgrl_transformation_sptr.h"
#include "rgrl_converge_status_sptr.h"
#include "rgrl_invariant_sptr.h"
#include "rgrl_feature_sptr.h"
#include "rgrl_feature_set_sptr.h"
#include <vcl_cassert.h>

//: Cast down the hierarchy.
//
//  This does a dynamic_cast and then asserts that the result is not
//  null. Therefore, you are guaranteed that the result is a valid
//  pointer, or else the program will halt.
//
// Example of usage:
// \example
//    rgrl_transformation_sptr trans = new rgrl_trans_affine(...);
//    rgrl_trans_affine* aff = rgrl_cast<rgrl_trans_affine*>(trans);
// \endexample
//
template<typename ToType>
class rgrl_cast
{
 public:
  //: Downcast a feature object
  rgrl_cast( rgrl_feature_sptr feature )
  {
    ptr = dynamic_cast<ToType>( feature.as_pointer() );
    assert ( ptr );
  }

  //: Downcast a feature object
  rgrl_cast( rgrl_feature_set_sptr feature_set )
  {
    ptr = dynamic_cast<ToType>( feature_set.as_pointer() );
    assert ( ptr );
  }

  //: Downcast a transformation object.
  rgrl_cast( rgrl_transformation_sptr trans )
  {
    ptr = dynamic_cast<ToType>( trans.as_pointer() );
    assert ( ptr );
  }

  //: Downcast a convergence status object.
  rgrl_cast( rgrl_converge_status_sptr status )
  {
    ptr = dynamic_cast<ToType>( status.as_pointer() );
    assert ( ptr );
  }

  //: Downcast a invariant feature object.
  rgrl_cast( rgrl_invariant_sptr invariant )
  {
    ptr = dynamic_cast<ToType>( invariant.as_pointer() );
    assert ( ptr );
  }

  //:
  operator ToType() const
  {
    return reinterpret_cast<ToType>(ptr);
  }

  //:
  ToType operator->() const
  {
    return reinterpret_cast<ToType>(ptr);
  }

 private:
  void *ptr;
};


//: Cast down the hierarchy.
//
//  This does a dynamic_cast and then asserts that the result is not
//  null. Therefore, you are guaranteed that the result is a valid
//  pointer, or else the program will halt.
//
// Example of usage:
// \example
//    rgrl_transformation_sptr trans = new rgrl_trans_affine(...);
//    const rgrl_trans_affine* aff = rgrl_const_cast<rgrl_trans_affine*>(trans);
// \endexample
//
template<typename ToType>
class rgrl_const_cast
{
 public:
  //: Downcast a feature object
  rgrl_const_cast( rgrl_feature_sptr const& feature )
  {
    ptr = dynamic_cast<const ToType>( feature.as_pointer() );
    assert ( ptr );
  }

  //: Downcast a feature object
  rgrl_const_cast( rgrl_feature_set_sptr const& feature_set )
  {
    ptr = dynamic_cast<const ToType>( feature_set.as_pointer() );
    assert ( ptr );
  }

  //: Downcast a transformation object.
  rgrl_const_cast( rgrl_transformation_sptr const& trans )
  {
    ptr = dynamic_cast<const ToType>( trans.as_pointer() );
    assert ( ptr );
  }

  //: Downcast a convergence status object.
  rgrl_const_cast( rgrl_converge_status_sptr const& status )
  {
    ptr = dynamic_cast<const ToType>( status.as_pointer() );
    assert ( ptr );
  }

  //: Downcast a invariant feature object.
  rgrl_const_cast( rgrl_invariant_sptr const& invariant )
  {
    ptr = dynamic_cast<const ToType>( invariant.as_pointer() );
    assert ( ptr );
  }

  //:
  operator const ToType() const
  {
    return reinterpret_cast<const ToType>(const_cast<void*>(ptr));
  }

  //:
  const ToType operator->() const
  {
    return reinterpret_cast<const ToType>(const_cast<void*>(ptr));
  }

 private:
  void *ptr;
};

#endif // rgrl_cast_h_
