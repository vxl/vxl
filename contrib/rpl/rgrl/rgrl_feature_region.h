#ifndef rgrl_feature_region_h_
#define rgrl_feature_region_h_
//:
// \file
// \brief Base class for feature associated with a region
// \author Gehua yang
// \date 21 Oct. 2004

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
//#include <rgrl/rgrl_feature.h>

class rgrl_transformation;

//: Represents a feature ("data point") used by the registration algorithms.
//
class rgrl_feature_region
{
 public:

  //: ctor
  rgrl_feature_region()  : pixel_coordinates_cached_( false )
  {   }

  // Defines type-related functions
  // rgrl_type_macro( rgrl_feature_region, rgrl_feature );

  virtual const vcl_vector< vnl_vector<int> >&
  pixel_coordinates();
  
  //: get pixels coordinates within the region
  virtual const vcl_vector< vnl_vector<int> >&
  pixel_coordinates_ratio( vnl_vector< double > const& spacing_ratio );

  //: generate pixels coordinates within the region
  virtual void 
  generate_pixel_coordinates( vnl_vector< double > const& spacing_ratio ) = 0;

 protected:
  //  The pixel coordinates in the image are computed the first time
  //  they are needed and then cached.  This is safe the location of
  //  the feature does not change.
  bool pixel_coordinates_cached_;
  vcl_vector< vnl_vector<int> > pixel_coordinates_;
  vnl_vector< double >          spacing_ratio_;
};


#endif
