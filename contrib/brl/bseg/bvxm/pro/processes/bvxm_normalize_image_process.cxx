//This is brl/bseg/bvxm/pro/processes/bvxm_normalize_image_process.cxx
#include <string>
#include <iostream>
#include "bvxm_normalize_image_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bvxm/pro/processes/bvxm_normalization_util.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_world.h>

#ifdef DEBUG
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

#include <brdb/brdb_value.h>
#include <brip/brip_vil_float_ops.h>

//:sets input and output types for bvxm_create_normalized_nplanes_image_process
bool bvxm_normalize_image_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_normalize_image_process_globals;
  //inputs
  //0: The unnormalized image
  //1: The mog_image which should be created from the voxel world from the point of view of this input image (meaning using its camera) via bvxm_create_mog_image_process
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "bvxm_voxel_slab_base_sptr";
  input_types_[2] = "vcl_string";  // voxel type of the mog_image (same as voxel type of the voxel world that it is created from)
  if (!pro.set_input_types(input_types_))
    return false;

  //output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "float";  // output a
  output_types_[2]= "float";  // output b
  return pro.set_output_types(output_types_);
}

bool bvxm_normalize_image_process(bprb_func_process& pro)
{
  using namespace bvxm_normalize_image_process_globals;

 //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr input_img = pro.get_input<vil_image_view_base_sptr>(i++);
  bvxm_voxel_slab_base_sptr mog_image = pro.get_input<bvxm_voxel_slab_base_sptr>(i++);
  std::string voxel_type = pro.get_input<std::string>(i++);

  //check inputs validity
  if (!input_img) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  //get parameters and overwrite global values
  pro.parameters()->get_value(param_a_start_,a_start_);
  pro.parameters()->get_value(param_a_inc_, a_inc_);
  pro.parameters()->get_value(param_a_end_, a_end_);
  pro.parameters()->get_value(param_b_start_, b_start_);
  pro.parameters()->get_value(param_b_end_,b_end_);
  pro.parameters()->get_value(param_b_ratio_, b_ratio_);

  //set other global variables
  ni_= input_img->ni();
  nj_= input_img->nj();
  nplanes_= input_img->nplanes();

  if (!mog_image) {
    std::cout << pro.name() <<" bvxm_normalize_image_process(): Warning: -- Input 1 is an empty pointer! Either the world was empty, or a problem occurred during MOG creation, will return input image back.\n";
    pro.set_output_val<vil_image_view_base_sptr>(0,input_img);
    pro.set_output_val<float>(1, 1.0f);
    pro.set_output_val<float>(2, 0.0f); // switch back to byte
    return true;
  }

  auto*  input_img_float_stretched = new vil_image_view<float>( ni_, nj_, nplanes_ );
  //calculate a, b parameters
  float a = 1.0;
  float b = 0.0;
  if (voxel_type == "apm_mog_grey")
    norm_parameters<APM_MOG_GREY>(input_img,input_img_float_stretched,mog_image,a,b);
  else if (voxel_type == "apm_mog_mc_3_3")
    norm_parameters<APM_MOG_MC_3_3>(input_img,input_img_float_stretched,mog_image,a,b);
  else if (voxel_type == "apm_mog_mc_4_3")
    norm_parameters<APM_MOG_MC_4_3>(input_img,input_img_float_stretched,mog_image,a,b);
  else{
    std::cout << "In bvxm_normalize_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  // Normalize the image with the best a and b.
  //vil_image_view<vxl_byte> output_img(ni, nj, nplanes);
  //normalize_image<vxl_byte>(*input_image_sptr, output_img, a, b, 255);

  vil_image_view<float> output_img_float(ni_, nj_, nplanes_);
  bvxm_normalization_util::normalize_image(*input_img_float_stretched, output_img_float, a, b, 1.0f);

  vil_image_view<float> output_img_stretched(ni_, nj_, nplanes_);
  vil_convert_stretch_range_limited<float>(output_img_float, output_img_stretched, 0.0f, 1.0f, 0.0f, 255.0f);
  vil_image_view<vxl_byte> output_img;
  vil_convert_cast(output_img_stretched, output_img);

  // return the output img, a,b
  unsigned j=0;
  pro.set_output_val<vil_image_view_base_sptr>(j++,new vil_image_view<vxl_byte>(output_img));
  pro.set_output_val<float>(j++ ,a);
  pro.set_output_val<float>(j++, b*255.0f); // switch back to byte

  return true;
}


template <bvxm_voxel_type APM_T>
    bool bvxm_normalize_image_process_globals::norm_parameters(vil_image_view_base_sptr const& input_img,
                                                               vil_image_view<float>*& input_img_float_stretched,
                                                               bvxm_voxel_slab_base_sptr const& mog_image,
                                                               float& a, float& b)
{
  if (verbose_) {
    std::cout << "normalization parameters to be used in this run:\n"
             << "a_start: " << a_start_ << " a_end: " << a_end_ << " a_inc: " << a_inc_ << std::endl
             << "b_start: " << b_start_ << " b_end: " << b_end_ << " b_ratio: " << b_ratio_ << std::endl;
  }

  // CAUTION: Assumption: Input image is of type vxl_byte
  if (input_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
    std::cout << "Input image pixel format is not VIL_PIXEL_FORMAT_BYTE!\n";
    return false;
  }

  auto* input_img_ptr = new vil_image_view<vxl_byte>(input_img);
  vil_convert_stretch_range_limited<vxl_byte>(*input_img_ptr, *input_img_float_stretched, 0, 255, 0.f, 1.f);

  // use the weight slab below to calculate total probability
  bvxm_voxel_slab<float> weights(ni_, nj_, 1);
  weights.fill(1.f/float(ni_ * nj_));

  a = 1.f;
  b = 0.f;
  float best_prob = 0.f;

  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype mog_type;
  typedef typename bvxm_voxel_traits<APM_T>::obs_datatype obs_datatype;

  if (!mog_image) {
    std::cout << "In bvxm_normalize_image_process::norm_parameters() - problems in creating mixture of gaussian image!\n";
    return false;
  }

  auto* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>* >(mog_image.ptr());

  typename bvxm_voxel_traits<APM_T>::appearance_processor apm_processor;
  if (verbose_) {
    bvxm_voxel_slab<obs_datatype> exp_img = apm_processor.expected_color(*mog_image_ptr);
    vil_image_view_base_sptr temp_img = new vil_image_view<vxl_byte>(ni_, nj_, nplanes_);
    bvxm_util::slab_to_img(exp_img, temp_img);
    vil_save(*temp_img, "./mixture_expected_img.png");
  }

  //optimize two parameters for the input image so that it is the maximally probable image seen wrt mog_image
  for ( float sa = a_start_; sa <= a_end_; sa+=a_inc_ )
  {
    float sb_best = 0.f;
    bool tried_zero = false;
    for ( float sb_inc = b_start_; sb_inc > b_end_; sb_inc *= b_ratio_ )
    {
      float sb_best_prob = 0.f;
      for ( float sb = sb_best-2.f*sb_inc; sb <= sb_best+2.01f*sb_inc; sb+= sb_inc )
      {
        if (sb == 0.f) {
          if (tried_zero)
            continue;
          else
            tried_zero = true;
        }

        auto* nimg = new vil_image_view<float>( ni_, nj_, nplanes_ );
        bvxm_normalization_util::normalize_image(*input_img_float_stretched, *nimg, sa, sb, 1.f);
        vil_image_view_base_sptr nimg_sptr = nimg;

        // convert image to a voxel_slab
        bvxm_voxel_slab<obs_datatype> image_slab(ni_, nj_, 1);
        bvxm_util::img_to_slab(nimg_sptr,image_slab);
        nimg_sptr = nullptr;  // to clear up space

        bvxm_voxel_slab<float> prob = apm_processor.prob_density(*mog_image_ptr,image_slab); //prob( nimg );

        // find the total prob
        bvxm_voxel_slab<float> product(ni_, nj_, 1);
        bvxm_util::multiply_slabs(prob, weights, product);
        float this_prob = bvxm_util::sum_slab(product);

        //std::cerr << this_prob << ' ';
        if ( this_prob < 0 ) {
          std::cout << "In bvxm_normalize_image_process::execute() -- prob is negative, Exiting!\n";
          return false;
        }

        if ( this_prob > sb_best_prob ){ sb_best_prob = this_prob; sb_best = sb; }
        if ( this_prob > best_prob ){ best_prob = this_prob; a = sa; b = sb; }
      }
      //std::cerr << '\n';
    }
    //std::cerr << '\n';
  }
  if (verbose_)
  {
    std::ofstream file;
    file.open("./normalization_parameters.txt", std::ofstream::app);
    file << a << ' ' << b <<'\n';
  }

  return true;
}
