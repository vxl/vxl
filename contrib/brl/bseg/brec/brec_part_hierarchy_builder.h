// This is brl/bseg/brec/brec_part_hierarchy_builder.h
#ifndef brec_part_hierarchy_builder_h_
#define brec_part_hierarchy_builder_h_
//:
// \file
// \brief class to build a hierarchy of composable parts for recognition
//
// Includes tools to learn co-occurrence statistics or manual construction of hierarchies for specific purposes
//
// Note: higher layer parts are positioned wrt to a coordinate system origined at the center of the central part
//       so during training, create samples by subtracting other parts' centers from central part's center after measuring from the image
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date October 16, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vector>
#include "brec_part_hierarchy_sptr.h"
#include "brec_part_base_sptr.h"
#include <vil/vil_image_resource_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class brec_part_hierarchy_builder
{
 public:

  //: uses 4 gaussians as primitives
  static brec_part_hierarchy_sptr construct_candidates_from_one_image(vil_image_resource_sptr img, float min_strength);

  //: construct layer_n from all pairwise combinations of detected parts of layer_n-1
  static bool construct_layer_candidates(unsigned layer_n, brec_part_hierarchy_sptr& h, std::vector<brec_part_instance_sptr>& parts);

  //: construct a hierarchy manually
  static brec_part_hierarchy_sptr construct_vehicle_detector();
  static brec_part_hierarchy_sptr construct_detector_roi1_0();
  static brec_part_hierarchy_sptr construct_detector_roi1_1();
  static brec_part_hierarchy_sptr construct_detector_roi1_2(); // recognize sides of roads, good to remove artefacts from change map

  static brec_part_hierarchy_sptr construct_detector_roi1_3(); // building 1
  static brec_part_hierarchy_sptr construct_detector_roi1_4(); // building 2

  //: for debugging purposes
  static brec_part_hierarchy_sptr construct_eight_detector(); // recognize digit 8
  static brec_part_hierarchy_sptr construct_test_detector();  // a detector with one primitive part only
};

#endif // brec_part_hierarchy_builder_h_
