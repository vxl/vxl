#ifndef rgrl_cast_h_
#define rgrl_cast_h_

//:
// \file
// \author Amitha Perera
// Cast operations to simplify downcasting in the heirarchy.

#include <vcl_cassert.h>

#include "rgrl_transformation_sptr.h"
#include "rgrl_converge_status_sptr.h"
#include "rgrl_invariant_sptr.h"
#include "rgrl_feature_sptr.h"

//: Cast down the heirarchy.
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

#endif // rgrl_cast_h_
