#ifndef rgrl_invariant_set_h_
#define rgrl_invariant_set_h_
//:
// \file
// \brief  Represent a feature associated with a set of invariant properties.
// \author Gehua Yang
// \date   Feb 2006

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <rgrl/rgrl_invariant_sptr.h>
#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_mask_sptr.h>

//: Represent a feature associated with a set of invariant properties.
//
class rgrl_invariant_set
  : public rgrl_object
{
 public:
  rgrl_invariant_set() = default;

  //:
  ~rgrl_invariant_set() override = default;

  virtual void
  as_invariants( std::vector<rgrl_invariant_sptr>& invariant_key_points, rgrl_mask_sptr const& mask=nullptr ) = 0;

  //: return a bounding box
  virtual rgrl_mask_sptr bounding_box() const = 0;
};

#endif // rgrl_invariant_set_h_
