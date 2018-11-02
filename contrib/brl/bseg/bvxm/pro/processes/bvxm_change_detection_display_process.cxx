//This is brl/bseg/bvxm/pro/processes/bvxm_change_detection_display_process.cxx
#include <iostream>
#include <cmath>
#include "bvxm_change_detection_display_process.h"
//:
// \file
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bvxm/bvxm_image_metadata.h>

#if 0
#include <vgl/vgl_point_2d.h>
#endif

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:sets input and output types for  bvxm_change_detection_display_process
bool bvxm_change_detection_display_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_change_detection_display_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  //: original image
  input_types_[i++] = "vil_image_view_base_sptr";
  //: probability image
  input_types_[i++] = "vil_image_view_base_sptr";
  //: mask image
  input_types_[i++] = "vil_image_view_base_sptr";

  if (!pro.set_input_types(input_types_))
    return false;

  unsigned j = 0;
  //output
  std::vector<std::string> output_types_(n_outputs_);
  // red changes image
  output_types_[j++]= "vil_image_view_base_sptr";
  // probability image in range 0-255
  output_types_[j++]= "vil_image_view_base_sptr";

  return pro.set_output_types(output_types_);
}

//: process to threshold the changes for foreground detection.
bool bvxm_change_detection_display_process(bprb_func_process& pro)
{
  using namespace bvxm_change_detection_display_process_globals;

  // Check number of inputs
  if (pro.n_inputs()<3)
  {
    std::cout << "bvxm_change_detection_display_process: The input number should be 3" << std::endl;
    return false;
  }

  // get inputs:
  unsigned i=0;
  vil_image_view_base_sptr img0 = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img1 = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img2 = pro.get_input<vil_image_view_base_sptr>(i++);

  // check input's validity
  i = 0;
  if (!img0) {
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if (!img1) {
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if (!img2) {
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if (img0->pixel_format()!=7) {
    std::cout << pro.name() <<" :--  Input " << 0 << " wrong pixel-format!\n";
    return false;
  }
  if (img1->pixel_format()!=9) {
    std::cout << pro.name() <<" :--  Input " << 1 << " wrong pixel-format!\n";
    return false;
  }

  // define locals
  unsigned image_height = img0->nj();
  unsigned image_width = img0->ni();

  vil_image_view<unsigned char> input_image(img0);
  vil_image_view<float> prob_image(img1);
  vil_image_view<bool> mask_image(img2);
  vil_image_view<unsigned char> prob_img_byte( image_width, image_height, 1 );
  vil_image_view<unsigned char> output_image0( image_width, image_height, 3 );
  vil_image_view<unsigned char> output_image1( image_width, image_height, 1 );

  // read the parameters
  float prob_thresh = .5f;
  float prob_image_scale = .7f;
  pro.parameters()->get_value(param_prob_thresh_,prob_thresh);
  pro.parameters()->get_value(param_prob_image_scale_,prob_image_scale);

  // obtain max probability
  float max_prob = 0.0f;
  for ( unsigned int i = 0; i < image_width; i++ ) {
    for ( unsigned int j = 0; j < image_height; j++ ) {
      if (prob_image(i , j) > max_prob)
        max_prob = prob_image(i,j);
    }
  }

  //map to 0-255 range
    for ( unsigned int i = 0; i < image_width; i++ ) {
    for ( unsigned int j = 0; j < image_height; j++ ) {
      prob_img_byte(i,j) = static_cast<unsigned char>(std::floor((prob_image(i,j)/max_prob) * 255.0f));
    }
  }

  for ( unsigned int i = 0; i < image_width; i++ ) {
    for ( unsigned int j = 0; j < image_height; j++ ) {
#if 0
      vgl_point_2d<double> prob_img_pixel((int)std::floor(prob_image_scale*i),
                                          (int)std::floor(prob_image_scale*j) );
#endif
      float this_prob = 1.0f;
      unsigned original_prob_byte = 0;

      if ( prob_image(i,j) < prob_thresh && mask_image(i,j)) {
        this_prob = prob_image( i,j )/(prob_thresh);
      }

      output_image0(i,j,0) = static_cast<unsigned char>(std::floor( float(input_image(i,j))*this_prob + 255.f*(1.f-this_prob) ));
      output_image0(i,j,1) = static_cast<unsigned char>(std::floor( float(input_image(i,j))*this_prob ));
      output_image0(i,j,2) = static_cast<unsigned char>(std::floor( float(input_image(i,j))*this_prob ));

      if (mask_image(i,j)) {
        original_prob_byte = prob_img_byte(i,j);
        if ( original_prob_byte > 255)
          original_prob_byte = 255;
      }
      output_image1(i,j) = static_cast<unsigned char>(original_prob_byte);
    }
  }

  std::cout << "Max prob: " << max_prob;

  // Set and Store outputs
  int j =0;
  pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<unsigned char>(output_image0));
  pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<unsigned char>(output_image1));
  return true;
}
