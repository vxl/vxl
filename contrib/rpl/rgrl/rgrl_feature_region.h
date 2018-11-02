#ifndef rgrl_feature_region_h_
#define rgrl_feature_region_h_
//:
// \file
// \brief Base class for feature associated with a region
// \author Gehua yang
// \date 21 Oct. 2004

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>

class rgrl_transformation;

//: Represents a feature ("data point") used by the registration algorithms.
//
class rgrl_feature_region
{
 public:
  // default ctor
  rgrl_feature_region()  : pixel_coordinates_cached_( false ) {}

  // destructor
  virtual ~rgrl_feature_region() = default;

#if 0
  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_region, rgrl_feature );
#endif // 0

  virtual const std::vector< vnl_vector<int> >&
  pixel_coordinates();

  //: get pixels coordinates within the region
  virtual const std::vector< vnl_vector<int> >&
  pixel_coordinates_ratio( vnl_vector< double > const& spacing_ratio );

  //: generate pixels coordinates within the region
  virtual void
  generate_pixel_coordinates( vnl_vector< double > const& spacing_ratio ) = 0;

 protected:
  //  The pixel coordinates in the image are computed the first time
  //  they are needed and then cached.  This is safe the location of
  //  the feature does not change.
  bool pixel_coordinates_cached_;
  std::vector< vnl_vector<int> > pixel_coordinates_;
  vnl_vector< double >          spacing_ratio_;
};

#endif
