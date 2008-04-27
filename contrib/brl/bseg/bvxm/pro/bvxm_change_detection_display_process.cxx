#include "bvxm_change_detection_display_process.h"
//:
// \file

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_grid_base.h>
#include <bvxm/bvxm_image_metadata.h>

#if 0
#include <vgl/vgl_point_2d.h>
#endif

#include <vcl_cmath.h>

bvxm_change_detection_display_process::bvxm_change_detection_display_process()
{
 //inputs
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  //: original image
  input_types_[0] = "vil_image_view_base_sptr";
  //: probability image
  input_types_[1] = "vil_image_view_base_sptr";
  //: mask image
  input_types_[2] = "vil_image_view_base_sptr";

  //output
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  //: red changes image
  output_types_[0]= "vil_image_view_base_sptr";
  //: probability image in ragen 0-255
  output_types_[1]= "vil_image_view_base_sptr";


  parameters()->add("Probability Threshold for detection", "prob_thresh", 0.5f);
  parameters()->add("Probability Image Scale", "prob_image_scale", 0.5f);
}


bool bvxm_change_detection_display_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the input image
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());

  //get the probability image
  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());

    //get the mask image
  brdb_value_t<vil_image_view_base_sptr>* input2 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[2].ptr());

  vil_image_view_base_sptr img0 = input0->value();
  vil_image_view_base_sptr img1 = input1->value();
  vil_image_view_base_sptr img2 = input2->value();

  if (img0->pixel_format()!=7)
    return false;
  if (img1->pixel_format()!=9)
    return false;
  unsigned image_height = img0->nj();
  unsigned image_width = img0->ni();

  vil_image_view<unsigned char> input_image(img0);
  vil_image_view<float> prob_image(img1);
  vil_image_view<bool> mask_image(img2);
  vil_image_view<unsigned char> output_image0( image_width, image_height, 3 );
  vil_image_view<unsigned char> output_image1( image_width, image_height, 1 );

  float prob_thresh = .50f;
  float prob_image_scale = .7f;
  parameters()->get_value("prob_thresh",prob_thresh);
  parameters()->get_value("prob_image_scale",prob_image_scale);

  for ( unsigned int i = 0; i < image_width; i++ ) {
    for ( unsigned int j = 0; j < image_height; j++ ) {
#if 0
      vgl_point_2d<double> prob_img_pixel((int)vcl_floor(prob_image_scale*i),
                                          (int)vcl_floor(prob_image_scale*j) );
#endif
      float this_prob = 1.0f;
      float original_prob = 0.0f;

      if ( prob_image(i,j) < prob_thresh && mask_image(i,j)) {
        original_prob = prob_image (i, j);
        this_prob = prob_image( i,j )/(prob_thresh);
      }

      output_image0(i,j,0) = (int)vcl_floor( input_image(i,j)*this_prob + 255*(1-this_prob) );
      output_image0(i,j,1) = (int)vcl_floor( input_image(i,j)*this_prob );
      output_image0(i,j,2) = (int)vcl_floor( input_image(i,j)*this_prob );

      if (mask_image(i,j)) {
        original_prob = prob_image(i,j) * 128.0f;
        if ( original_prob > 255.0f)
          original_prob = 255.0f;
      }
      output_image1(i,j) = (int)(original_prob);
    }
  }

  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(output_image0));
  output_data_[0] = output0;

    brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(output_image1));
  output_data_[1] = output1;

  return true;
}

