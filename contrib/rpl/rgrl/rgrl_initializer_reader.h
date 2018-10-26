#ifndef rgrl_initializer_reader_h_
#define rgrl_initializer_reader_h_
//:
// \file
// \brief Generate initial estimates based on prior transformations in input stream.
// \author Gehua Yang
// \date 16 Feb 2005

#include <vector>
#include <iostream>
#include <iosfwd>
#include <vcl_compiler.h>

#include <rgrl/rgrl_initializer.h>
#include <rgrl/rgrl_scale_sptr.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_mask_sptr.h>
#include <rgrl/rgrl_transformation_sptr.h>

//: Generate initial estimates based on a single prior transformation.
class rgrl_initializer_reader
  : public rgrl_initializer
{
 public:
  //: Initialize with a view
  rgrl_initializer_reader( std::istream& istr,
                           rgrl_mask_sptr             const& from_image_roi,
                           rgrl_mask_sptr             const& to_image_roi,
                           rgrl_scale_sptr            const& prior_scale=nullptr,
                           rgrl_estimator_sptr        const& estimator = nullptr,
                           unsigned int                      resolution = 0 );

  //: Add more potential prior transformations
  void add_prior_xform( rgrl_transformation_sptr   xform_estimate );

  //: Get next initial estimate when first called, but return false thereafter.
  bool next_initial( rgrl_view_sptr           & view,
                     rgrl_scale_sptr          & prior_scale ) override;

  //: Set index pointing to vector of initializations to zero.
  void reset_xform_index( ) { xform_index_ = 0; }

  //: Set index pointing to the vector of initializations to a given value.
  void set_xform_index( unsigned int index ) { xform_index_ = index; }

  //: Return number of initializations
  //  -1 stands for unknown
  int size() const override;

  //: Defines type-related functions
  rgrl_type_macro( rgrl_initializer_reader, rgrl_initializer );

 protected:

  struct init_record {
    rgrl_view_sptr            view_;
    rgrl_scale_sptr           scale_;
    rgrl_transformation_sptr  xform_;
  };

  //: Initializations read from a stream
  std::vector< init_record >           init_records_;

  //: Index pointing into initialization in init_records_ vector that will be used
  unsigned int xform_index_;

  rgrl_mask_sptr       from_image_roi_;
  rgrl_mask_sptr       to_image_roi_;
  rgrl_scale_sptr      prior_scale_;
  rgrl_estimator_sptr  estimator_;
  unsigned int         res_;
};

#endif
