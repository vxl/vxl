#ifndef rgrl_invariant_match_h_
#define rgrl_invariant_match_h_

// \file
// \brief A match based on the similarity of the invariant features
// \author Charlene Tsai
// \date March 2004

#include "rgrl_invariant_sptr.h"
#include "rgrl_transformation_sptr.h"
#include "rgrl_scale_sptr.h"
#include "rgrl_mask.h"

#include "rgrl_invariant.h"
#include "rgrl_transformation.h"
#include "rgrl_scale.h"
#include "rgrl_object.h"

class rgrl_scale;

class rgrl_invariant_match
  : public rgrl_object
{
public:
  //: Constructor
  rgrl_invariant_match(rgrl_invariant_sptr from,
                       rgrl_invariant_sptr to);

  //: Destructor
  ~rgrl_invariant_match(){}

  //: Returns a smart pointer to the "from" invariant
  rgrl_invariant_sptr from() const {return from_;}
  
  //: Returns a smart pointer to the "to" invariant
  rgrl_invariant_sptr to() const {return to_;}
  
  //: Returns the distance between the constellations in the match
  //  \note distance is calculated as the Euclidean distance in the
  //  "invariant space"
  double distance() const {return distance_;}

  //: Returns a transform smart pointer to the estimated transform
  rgrl_transformation_sptr transform() const; 

  //: Return a scale smart pointer to the estimated similarity transform
  rgrl_scale_sptr scale() const;

  //: Computes the initial ROI where the initial transformation is valid
  rgrl_mask_box initial_region() const;

  //: Return true if the match is associated with an initial ROI
  //  The initial ROI is where the initial transformation is valid
  bool has_initial_region() const;

  //: Trigger the \a estimate() function of the \a to_ feature
  bool estimate();

  // Defines type-related functions
  rgrl_type_macro( rgrl_invariant_match, rgrl_object );

private:
  rgrl_invariant_sptr from_;
  rgrl_invariant_sptr to_;
  double distance_;
  rgrl_transformation_sptr xform_;
  rgrl_scale_sptr          scale_;
  bool is_estimate_set_;
};

//: Compares two matches on the basis of invariance distance
bool operator < (const rgrl_invariant_match& left,
                 const rgrl_invariant_match& right);

//: Compares two matches on the basis of invariance distance
bool operator > (const rgrl_invariant_match& left,
                 const rgrl_invariant_match& right);

//: Same as operator <, but operates on smart pointers to constellation matches
//  \note This function is used to in vcl_sort to sort vectors of smart 
//        pointers to pregl_constellation_match objects
bool dist_less (const vbl_smart_ptr<rgrl_invariant_match>& left,
                const vbl_smart_ptr<rgrl_invariant_match>& right);

//: Same as operator >, but operates on smart pointers to constellation matches
//  \note This function is used to in vcl_sort to sort vectors of smart 
//        pointers to pregl_constellation_match objects
bool dist_greater (const vbl_smart_ptr<rgrl_invariant_match>& left,
                   const vbl_smart_ptr<rgrl_invariant_match>& right);

#endif
