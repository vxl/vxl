#ifndef rgrl_initializer_reader_h_
#define rgrl_initializer_reader_h_
//:
// \file
// \brief Generate initial estimates based on prior transformations in input stream.
// \author Gehua Yang
// \date 16 Feb 2005

#include <vcl_vector.h>

#include <rgrl/rgrl_initializer.h>
#include <rgrl/rgrl_scale_sptr.h>
#include <rgrl/rgrl_scale_sptr.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_transformation_sptr.h>

//: Generate initial estimates based on a single prior transformation.
class rgrl_initializer_reader
  : public rgrl_initializer
{
 public:
  //: Initialize with a view
  rgrl_initializer_reader( vcl_istream& istr,
                           rgrl_mask_box              const& from_image_roi,
                           rgrl_mask_box              const& to_image_roi,
                           rgrl_scale_sptr            const& prior_scale=0,
                           rgrl_estimator_sptr        const& estimator = 0,
                           unsigned int                      resolution = 0 );

  //: Add more potential prior transformations
  void add_prior_xform( rgrl_transformation_sptr   xform_estimate );

  //: Get next initial estimate when first called, but return false thereafter.
  bool next_initial( rgrl_view_sptr           & view,
                     rgrl_scale_sptr          & prior_scale );

  void reset_xform_index( ) { xform_index_ = 0; }

  // Defines type-related functions
  rgrl_type_macro( rgrl_initializer_reader, rgrl_initializer );

 protected:
 
  struct init_record {
    rgrl_view_sptr            view_;
    rgrl_scale_sptr           scale_;
    rgrl_transformation_sptr  xform_;
  };   
 
  vcl_vector< init_record >           init_records_;
  unsigned int xform_index_;
  
  rgrl_mask_box       from_image_roi_;
  rgrl_mask_box       to_image_roi_;
  rgrl_scale_sptr     prior_scale_;
  rgrl_estimator_sptr estimator_;
  unsigned int        res_;
};

#endif
