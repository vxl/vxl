// This is brl/bpro/core/brip_pro/processes/brip_blobwise_kl_div_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  process to calculate blob wise mutual information between expected and input image

#include <brip/brip_vil_float_ops.h>
#include <brip/brip_blobwise_mutual_info.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>

//: global variables
namespace brip_blobwise_kl_div_process_globals
{
  const unsigned n_inputs_  = 4;
  const unsigned n_outputs_ = 2;

  // private helper method prepares an input image to be processed by update
  vil_image_view<float>* prepare_input_image(vil_image_view_base_sptr loaded_image) {
    //then it's an RGB image (assumes byte image...)
    if (loaded_image->nplanes() == 3 || loaded_image->nplanes() == 4)
    {
      vcl_cout<<"preparing rgb as input to grey scale float image"<<vcl_endl;

      //load image from file and format it into grey
      vil_image_view<vxl_byte>* inimg    = dynamic_cast<vil_image_view<vxl_byte>* >(loaded_image.ptr());
      vil_image_view<float>     gimg(loaded_image->ni(), loaded_image->nj());
      vil_convert_planes_to_grey<vxl_byte, float>(*inimg, gimg);

      //stretch it into 0-1 range
      vil_image_view<float>*    floatimg = new vil_image_view<float>(loaded_image->ni(), loaded_image->nj());
      vil_convert_stretch_range_limited(gimg, *floatimg, 0.0f, 255.0f, 0.0f, 1.0f);
      return floatimg;
    }

    //else if loaded planes is just one...
    if (loaded_image->nplanes() == 1)
    {
      vcl_cout<<"Preparing grey scale image"<<vcl_endl;

      //preapre floatimg for stretched img
      vil_image_view<float>* floatimg;
      if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
      {
        floatimg = new vil_image_view<float>(loaded_image->ni(), loaded_image->nj(), 1);
        vil_convert_stretch_range_limited(*img_byte, *floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
      }
      else if (vil_image_view<unsigned short> *img_byte = dynamic_cast<vil_image_view<unsigned short>*>(loaded_image.ptr()))
      {
        floatimg = new vil_image_view<float>(loaded_image->ni(), loaded_image->nj(), 1);
        vil_convert_stretch_range_limited(*img_byte, *floatimg,(unsigned short)30500,(unsigned short)32500,  0.0f, 1.0f); // hardcoded to be fixed.
      }
      else if (vil_image_view<float> *img_float = dynamic_cast<vil_image_view<float>*>(loaded_image.ptr()))
      {
        return img_float;
      }
      else {
        vcl_cerr << "Failed to load image\n";
        return 0;
      }
      return floatimg;
    }
    //otherwise it's messed up, return a null pointer
    vcl_cerr<<"Failed to recognize input image type\n";
    return 0;
  }
};

//: Constructor
bool brip_blobwise_kl_div_process_cons(bprb_func_process& pro)
{
  using namespace brip_blobwise_kl_div_process_globals;

  //input
  vcl_vector<vcl_string> input_types(n_inputs_);
  input_types[0] = "vil_image_view_base_sptr"; // img1
  input_types[1] = "vil_image_view_base_sptr"; // img2
  input_types[2] = "vil_image_view_base_sptr"; // blob/mask image
  input_types[3] = "float";
  bool good = pro.set_input_types(input_types);
  if (!good) return good;

  //input default args
  brdb_value_sptr thresh = new brdb_value_t<float>(.1f);
  pro.set_input(3, thresh);

  //output
  vcl_vector<vcl_string> output_types(n_outputs_);
  output_types[0] = "vil_image_view_base_sptr";  // KL Div image (kl div in each blobs place)
  output_types[1] = "vil_image_view_base_sptr";  // new blob image (thresholded by percentage of KL)
  return pro.set_output_types(output_types);
}


//: Execute the process
bool brip_blobwise_kl_div_process(bprb_func_process& pro)
{
  using namespace brip_blobwise_kl_div_process_globals;

  // Sanity check
  if (pro.n_inputs() < n_inputs_) {
    vcl_cout << "brip_blobwise_kl_div_process: The input number should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr in_img_ptr   = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr exp_img_ptr  = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr blob_img_ptr = pro.get_input<vil_image_view_base_sptr>(i++);
  float                    thresh       = pro.get_input<float>(i++);

  //prepare input images
  vil_image_view<float>*  in_img  = prepare_input_image(in_img_ptr);
  vil_image_view<float>*  exp_img = prepare_input_image(exp_img_ptr);
  unsigned ni=blob_img_ptr->ni(),
           nj=blob_img_ptr->nj();

  //----------------------------------
  //pre process mask image
  vil_image_view<vxl_byte>* blob_img = static_cast<vil_image_view<vxl_byte>* >(blob_img_ptr.ptr());
  vil_image_view<bool> mask_img(ni, nj), mask_dest(ni,nj), mask_dest1(ni,nj);
  for (unsigned int i=0; i<ni; ++i)
    for (unsigned int j=0; j<nj; ++j)
      mask_img(i,j) = ( (*blob_img)(i,j) > 0 ) ? true : false;

  //dilate twice
  vil_structuring_element selem;
  selem.set_to_disk(1.05);
  vil_binary_dilate(mask_img, mask_dest, selem);
  vil_binary_dilate(mask_dest, mask_dest1, selem);
  vil_binary_dilate(mask_dest1, mask_dest, selem);

  //-----------------------------------------
  //calculate per blob KL Divergence
  vil_image_view<float>* kl_img = new vil_image_view<float>(ni, nj);
  kl_img->fill(0.0f);
  brip_blobwise_kl_div(*in_img, *exp_img, mask_dest, *kl_img);

  //-----------------------------------------
  //create thresholded KL Img
  //find min max on Mutual Info
  float min_value, max_value;
  vil_math_value_range(*kl_img, min_value, max_value);

  //create new blob info (threshold the KL image by some value)
  // USE old, unchanged mask image to keep blobs small
  thresh *= max_value;
  vil_image_view<vxl_byte>* new_blobs = new vil_image_view<vxl_byte>(ni,nj);
  for (unsigned int i=0; i<ni; ++i)
    for (unsigned int j=0; j<nj; ++j) {
      if ( mask_img(i,j) && (*kl_img)(i,j) > thresh)
        (*new_blobs)(i,j) = (vxl_byte) 255;
      else
        (*new_blobs)(i,j) = (vxl_byte) 0;
    }

  //make sure KL div image isn't overstepping old blobs
  for (unsigned i=0; i<ni; ++i)
    for (unsigned int j=0; j<nj; ++j)
      if ( !mask_img(i,j) )
        (*kl_img)(i,j) = 0.0f;

  //------------------------------------------------
  // set outputs
  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++, kl_img);
  pro.set_output_val<vil_image_view_base_sptr>(i++, new_blobs);
  return true;
}

