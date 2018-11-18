// This is brl/bseg/brec/brec_glitch.h
#ifndef brec_glitch_h_
#define brec_glitch_h_
//:
// \file
// \brief class to represent a center-surround geometry to be detected as a foreground pattern
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date December 09, 2008
//
// \verbatim
//  Modifications
//    Ozge C. Ozcanli  Dec 09, 2008 : support for only square regions for now
// \endverbatim

#include <iostream>
#include <vector>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>

class brec_glitch
{
 public:

   //: given a size, generate a square center mask together with a surround neighborhood mask with the same number of pixels with the center
   static void square_glitch(int square_size, std::vector<std::pair<int, int> >& neighborhood_center, std::vector<std::pair<int, int> >& neighborhood_surround);

   static vil_image_view<vxl_byte> square_glitch_mask_img(int square_size);

   static void extend_prob_to_square_region(int square_size, vil_image_view<float>& input_map, vil_image_view<float>& output_map);
   //: output_img needs to have 3 planes
   //static void generate_output_img(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<vxl_byte>& input_img, vil_image_view<vxl_byte>& output_img);
};

#endif  //brec_glitch_h_
