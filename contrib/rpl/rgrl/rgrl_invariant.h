#ifndef rgrl_invariant_h_
#define rgrl_invariant_h_

// \file
// \brief  Represent a feature associated with a set of invariant properties.
// \author Charlene Tsai
// \date   March 2004

#include <vnl/vnl_vector.h>

#include "rgrl_transformation_sptr.h"
#include "rgrl_scale_sptr.h"
#include "rgrl_mask.h"
#include "rgrl_invariant_sptr.h"
#include "rgrl_object.h"

//: Represent a feature associated with a set of invariant properties. 
//
class rgrl_invariant
  : public rgrl_object
{
public:

  rgrl_invariant();

  //: 
  virtual ~rgrl_invariant(){ }

  //: Estimate the xform mapping \a from to the current feature
  virtual bool estimate(rgrl_invariant_sptr         from,
                        rgrl_transformation_sptr&   xform,
                        rgrl_scale_sptr&            scale ) = 0;
  
  //: Returns the vector of invariants normalized by scale
  virtual const vnl_vector<double>& cartesian_invariants() const;

  //: Returns the vector of invariants normalized by scale
  virtual const vnl_vector<double>& angular_invariants() const;
  

  //: Return the ROI associated with the feature
  virtual rgrl_mask_box region() const;

  //: Return true if the feature has an initial ROI
  virtual bool has_region() const;
  
  // Defines type-related functions
  rgrl_type_macro( rgrl_invariant, rgrl_object );

private:  
  // disabled
  rgrl_invariant& operator=( rgrl_invariant const& );

private:
  // a 0-length dummy vector
  vnl_vector<double> zero_vec;
};

#endif // rgrl_invariant_h_
