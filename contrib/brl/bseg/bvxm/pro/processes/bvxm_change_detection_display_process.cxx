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

//DEFINE PARAMETERS
#define PARAM_PROB_THRESH "prob_thresh"
#define PARAM_PROB_IMAGE_SCALE "prob_image_scale"

bool bvxm_change_detection_display_process(bprb_func_process& pro)
{
 //inputs
  //:input 0: original image
  //:input 1: probability image
  //:input 2: mask image
  if(pro.n_inputs()<3)
    {
    vcl_cout << "bvxm_change_detection_display_process: The input number should be 3" << vcl_endl;
    return false; 
  }

  //get inputs:
  unsigned i=0;
  vil_image_view_base_sptr img0 = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img1 = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img2 = pro.get_input<vil_image_view_base_sptr>(i++);

  //check imput's validity
  i = 0;
   if (!img0) {
     vcl_cout << pro.name <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if (!img1) {
    vcl_cout << pro.name <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if (!img2) {
    vcl_cout << pro.name <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if (img0->pixel_format()!=7){
    vcl_cout << pro.name <<" :--  Input " << 0 << " wrong pixel-format!\n";
    return false;
  }
  if (img1->pixel_format()!=9){
    vcl_cout << pro.name <<" :--  Input " << 1 << " wrong pixel-format!\n";
    return false;
  }

  //define locals
  unsigned image_height = img0->nj();
  unsigned image_width = img0->ni();

  vil_image_view<unsigned char> input_image(img0);
  vil_image_view<float> prob_image(img1);
  vil_image_view<bool> mask_image(img2);
  vil_image_view<unsigned char> prob_img_byte( image_width, image_height, 1 );
  vil_image_view<unsigned char> output_image0( image_width, image_height, 3 );
  vil_image_view<unsigned char> output_image1( image_width, image_height, 1 );

  //read the parameters
  float prob_thresh = .50f;
  float prob_image_scale = .7f;
  parameters()->get_value(PARAM_PROB_THRESH,prob_thresh);
  parameters()->get_value(PARAM_PROB_IMAGE_SCALE,prob_image_scale);

  //obtain max probability
  float max_prob = 0.0f;
  for ( unsigned int i = 0; i < image_width; i++ ) {
    for ( unsigned int j = 0; j < image_height; j++ ) {
       if(prob_image(i , j) > max_prob)
          max_prob = prob_image(i,j);
    }
  }

  //map to 0-255 range
    for ( unsigned int i = 0; i < image_width; i++ ) {
    for ( unsigned int j = 0; j < image_height; j++ ) {
       prob_img_byte(i,j) = (unsigned)vcl_floor((prob_image(i,j)/max_prob) * 255.0f);
    }
  }

  for ( unsigned int i = 0; i < image_width; i++ ) {
    for ( unsigned int j = 0; j < image_height; j++ ) {
#if 0
      vgl_point_2d<double> prob_img_pixel((int)vcl_floor(prob_image_scale*i),
                                          (int)vcl_floor(prob_image_scale*j) );
#endif
      float this_prob = 1.0f;
      float original_prob = 0.0f;
      unsigned original_prob_byte = 0;

      if ( prob_image(i,j) < prob_thresh && mask_image(i,j)) {
        original_prob = prob_image (i, j);
        this_prob = prob_image( i,j )/(prob_thresh);
      }

      output_image0(i,j,0) = (int)vcl_floor( input_image(i,j)*this_prob + 255*(1-this_prob) );
      output_image0(i,j,1) = (int)vcl_floor( input_image(i,j)*this_prob );
      output_image0(i,j,2) = (int)vcl_floor( input_image(i,j)*this_prob );

      if (mask_image(i,j)) {
       original_prob_byte = prob_img_byte(i,j);
        if ( original_prob_byte > 255)
          original_prob_byte = 255;
      }
      output_image1(i,j) = original_prob_byte;
    }
  }

  vcl_cout << "Max prob: " << max_prob;

  //Set and Store outputs
  int j =0;
  vcl_vector<vcl_string> output_types_(2);
  //: red changes image
  output_types_[j++]= "vil_image_view_base_sptr";
  //: probability image in ragen 0-255
  output_types_[j++]= "vil_image_view_base_sptr";
  pro.set_output_types(output_types_);

  j = 0;
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(output_image0));
  pro.set_(j++, output0);

    brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(output_image1));
  pro.set_(j++, output1);
  return true;



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


