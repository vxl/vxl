//This is brl/bseg/bvxm/pro/processes/bvxm_normalize_image_process.cxx
// :
// \file
// \brief A class for contrast normalization of images using a voxel world.
//  CAUTION: Input image is assumed to have type vxl_byte
//
// \author Ozge Can Ozcanli
// \date Feb. 13, 2008
// \verbatim
//  Modifications
//   Ozge C Ozcanli - 03/25/08 - fixed a compiler warning as suggested by Daniel Lyddy
//   Isabel Restrepo- 08/22/08 - Moved most of calculations to template function
//                    norm_parameters<bvxm_voxel_type APM_T>.
//                  - Added support for multichannel appereance model processor,
//                  - Removed support for rgb_mog_processor
//
//   Ozge C Ozcanli - 12/12/08 - added a third option to create mixture of gaussians (mog) image by sampling from the mixtures along the ray
//   Isabel Restrepo - 1/27/09 - converted process-class to functions which is the new design for processes.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <vcl_cmath.h>
#ifdef DEBUG
#include <vcl_iostream.h>
#endif

#include <brdb/brdb_value.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>

//global variables/functions for bvxm_normalize_image_process_globals
namespace bvxm_normalize_image_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 3;
  
  //Set the strings that identify parameters
  //specify whether to use most probable mode or expected value
  const vcl_string param_mog_method_ = "mog_method";
  //number of samples if using random sampling as mog_method
  const vcl_string param_nsamples_ = "n_samples";
  //normalized image = a*(original_image) +b;
  const vcl_string param_a_start_= "a_start";
  const vcl_string param_a_inc_ = "a_inc";
  const vcl_string param_a_end_ = "a_end";
  const vcl_string param_b_start_ = "b_start";
  const vcl_string param_b_end_ = "b_end";
  const vcl_string param_b_ratio_ = "b_ration";
  const vcl_string param_verbose_ = "verbose";
  
  //initialize variables that hold paramerters.  
  unsigned mog_creation_method_ = bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE;
  unsigned n_samples_ = 10;
  float a_start_ = 0.6f;
  float a_inc_ = 0.05f;
  float a_end_ = 1.8f;
  float b_start_ = 100.0f/255.0f;
  float b_end_ = 5.0f/255.0f;
  float b_ratio_ = 0.5f;
  bool verbose_ = false;

  //other gloabal variables
  unsigned ni_= 0;
  unsigned nj_= 0;
  unsigned nplanes_= 0;
  
  //this processes functions
  
  //: Float specialized function to normalize and image given a,b where new_I = a*I +b;
  bool normalize_image(const vil_image_view<float>& in_view,
                                      vil_image_view<float>& out_img,
                                      float a, float b, float max_value);
  
  
  //: Byte specialized function to normalize and image given a,b where new_I = a*I +b;
  bool normalize_image(const vil_image_view<vxl_byte>& in_view,
                       vil_image_view<vxl_byte>& out_img,
                        float a, float b, vxl_byte max_value = 255);
  
  
  template <bvxm_voxel_type APM_T>
  bool norm_parameters(vil_image_view_base_sptr const& input_img,vil_image_view<float>*& input_img_float_stretched,
                       vpgl_camera_double_sptr const& camera, bvxm_voxel_world_sptr const& world,
                       unsigned bin_index, unsigned scale_index, float& a, float& b);

  
  
}

//:sets input and output types for bvxm_create_normalized_image_process
bool bvxm_normalize_image_process_init(bprb_func_process& pro)
{
  using namespace bvxm_normalize_image_process_globals;
  //inputs
  //0: The unnormalized image
  //1: The camera
  //2: The voxel world
  //3: The appereance model type
  //4: The illumination bin index
  //5: The scale
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "bvxm_voxel_world_sptr";
  input_types_[3] = "vcl_string"; 
  input_types_[4] = "unsigned";  
  input_types_[5] = "unsigned"; 
  if(!pro.set_input_types(input_types_))
    return false;
  
  //output
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "float";  // output a
  output_types_[2]= "float";  // output b
  if(!pro.set_output_types(output_types_))
    return false;
  
  return true;
}

bool bvxm_normalize_image_process(bprb_func_process& pro)
{

  using namespace bvxm_normalize_image_process_globals;
  
 //check number of inputs
  if(pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false; 
  }
  
  //get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr input_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string voxel_type = pro.get_input<vcl_string>(i++);
  unsigned bin_index = pro.get_input<unsigned>(i++);;
  unsigned scale_index = pro.get_input<unsigned>(i++);
  
  //check inputs validity
  if (!input_img) {
    vcl_cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }
  if (!camera) {
    vcl_cout << pro.name() <<" :--  Input 1  is not valid!\n";
    return false;
  }
  if (!world) {
    vcl_cout << pro.name() <<" :--  Input 2  is not valid!\n";
    return false;
  }
  
  //get parameters and overwrite global values
  pro.parameters()->get_value(param_mog_method_, mog_creation_method_);
  pro.parameters()->get_value(param_nsamples_, n_samples_);
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

  // if the world is not updated yet, we just return the input image
  unsigned num_observations = 0;
  if (voxel_type == "apm_mog_grey") 
    num_observations = world->num_observations<APM_MOG_GREY>(bin_index,scale_index);
  else if (voxel_type == "apm_mog_mc_3_3")
    num_observations = world->num_observations<APM_MOG_MC_3_3>(bin_index,scale_index);
  else if (voxel_type == "apm_mog_mc_4_3")
    num_observations = world->num_observations<APM_MOG_MC_4_3>(bin_index,scale_index);
  else{
    vcl_cout << "In bvxm_normalize_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  if(num_observations == 0)
  {
    //return the input img.Thus, a=1, b=0
    pro.set_output_val<vil_image_view_base_sptr>(0,input_img);
    pro.set_output_val<float>(1,1.0f); // a
    pro.set_output_val<float>(2, 0.0f); // b
    if (verbose_)
    {
      vcl_ofstream file;
      file.open("./normalization_parameters.txt", vcl_ofstream::app);
      file << 1.0 << ' ' << 0.0 <<'\n';
      file.close();
    }
    return true;
  }

  vil_image_view<float>*  input_img_float_stretched = new vil_image_view<float>( ni_, nj_, nplanes_ );
  //calculate a, b parameters
  float a = 1.0;
  float b = 0.0;  
  if (voxel_type == "apm_mog_grey") 
    norm_parameters<APM_MOG_GREY>(input_img,input_img_float_stretched,camera,world,bin_index,scale_index,a,b);
  else if (voxel_type == "apm_mog_mc_3_3")
    norm_parameters<APM_MOG_MC_3_3>(input_img,input_img_float_stretched,camera,world,bin_index,scale_index,a,b);
  else if (voxel_type == "apm_mog_mc_4_3")
    norm_parameters<APM_MOG_MC_4_3>(input_img,input_img_float_stretched,camera,world,bin_index,scale_index,a,b);
  else{
    vcl_cout << "In bvxm_normalize_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }
  
  // Normalize the image with the best a and b.
  //vil_image_view<vxl_byte> output_img(ni, nj, nplanes);
  //normalize_image<vxl_byte>(*input_image_sptr, output_img, a, b, 255);

  vil_image_view<float> output_img_float(ni_, nj_, nplanes_);
  normalize_image(*input_img_float_stretched, output_img_float, a, b, 1.0f);

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

//: Float specialized function to normalize and image given a,b where new_I = a*I +b;
bool bvxm_normalize_image_process_globals::normalize_image(const vil_image_view<float>& in_view,
                                                           vil_image_view<float>& out_img,
                                                           float a, float b, float max_value)
{
  unsigned ni = in_view.ni();
  unsigned nj = in_view.nj();
  unsigned np = in_view.nplanes();

  if (ni != out_img.ni() || nj != out_img.nj() || np != out_img.nplanes())
    return false;

  for (unsigned k=0;k<np;++k)
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
  {
    float p = a*in_view(i,j,k) + b;
        // Proposed fix
    out_img(i, j, k) = vcl_min(vcl_max(0.f, p), max_value);
  }

  return true;
}

//: Byte specialized function to normalize and image given a,b where new_I = a*I +b;
bool bvxm_normalize_image_process_globals::normalize_image(const vil_image_view<vxl_byte>& in_view,
                                                           vil_image_view<vxl_byte>& out_img,
                                                           float a, float b, vxl_byte max_value)
{
  unsigned ni = in_view.ni();
  unsigned nj = in_view.nj();
  unsigned np = in_view.nplanes();

  if (ni != out_img.ni() || nj != out_img.nj() || np != out_img.nplanes())
    return false;

  for (unsigned k=0;k<np;++k)
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
  {
    int p = (int)vcl_floor(a*in_view(i,j,k) + b);
    out_img(i, j, k) = (vxl_byte)vcl_min(vcl_max(0, p), (int)max_value);
  }
#ifdef DEBUG
  vcl_cerr << "entered byte case..................\n";
#endif
  return true;
}

template <bvxm_voxel_type APM_T>
bool bvxm_normalize_image_process_globals::norm_parameters(vil_image_view_base_sptr const& input_img,
                                                           vil_image_view<float>*& input_img_float_stretched,
                                                           vpgl_camera_double_sptr const& camera,
                                                           bvxm_voxel_world_sptr const& world,
                                                           unsigned bin_index,
                                                           unsigned scale_index,
                                                           float& a, float& b)
{
  //1)Set up the data

  // create metadata:
  bvxm_image_metadata observation(input_img,camera);

  if (verbose_) {
    switch (mog_creation_method_) {
      case bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE:
      { vcl_cout << "using most probable modes' colors to create mog image "; } break;
      case bvxm_mog_image_creation_methods::EXPECTED_VALUE:
      { vcl_cout << "using expected colors to create mog image "; } break;
      case bvxm_mog_image_creation_methods::SAMPLING:
      { vcl_cout << "using random sampling to create mog image "; } break;
      default:
      { vcl_cout << "In bvxm_normalize_image_process::norm_parameters() - unrecognized option: " << mog_creation_method_ << " to create mog image\n"; return false; }
    }

    vcl_cout << "normalization parameters to be used in this run:\n"
        << "a_start: " << a_start_ << " a_end: " << a_end_ << " a_inc: " << a_inc_ << vcl_endl
        << "b_start: " << b_start_ << " b_end: " << b_end_ << " b_ratio: " << b_ratio_ << vcl_endl;
  }

  // CAUTION: Assumption: Input image is of type vxl_byte
  if (input_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
    vcl_cout << "Input image pixel format is not VIL_PIXEL_FORMAT_BYTE!\n";
    return false;
  }

  vil_image_view<vxl_byte>* input_img_ptr = new vil_image_view<vxl_byte>(input_img);
  vil_convert_stretch_range_limited<vxl_byte>(*input_img_ptr, *input_img_float_stretched, 0, 255, 0.f, 1.f);

  // use the weight slab below to calculate total probability
  bvxm_voxel_slab<float> weights(ni_, nj_, 1);
  weights.fill(1.f/(ni_ * nj_));

  //2) get probability mixtures of all pixels in image
  bvxm_voxel_slab_base_sptr mog_image;

  a = 1.f;
  b = 0.f;
  float best_prob = 0.f;

  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype mog_type;
  typedef typename bvxm_voxel_traits<APM_T>::obs_datatype obs_datatype;

  bool done = false;
  switch (mog_creation_method_) {
    case bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE:
    { done = world->mog_most_probable_image<APM_T>(observation, mog_image, bin_index,scale_index); } break;
    case bvxm_mog_image_creation_methods::EXPECTED_VALUE:
    { done = world->mixture_of_gaussians_image<APM_T>(observation, mog_image, bin_index,scale_index); } break;
    case bvxm_mog_image_creation_methods::SAMPLING:
    { 
     done = world->mog_image_with_random_order_sampling<APM_T>(observation, n_samples_, mog_image, bin_index, scale_index);
    } break;
    default:
    { vcl_cout << "In bvxm_normalize_image_process::norm_parameters() - unrecognized option: " << mog_creation_method_ << " to create mog image\n"; return false; }
  }

  if (!done) {
    vcl_cout << "In bvxm_normalize_image_process::norm_parameters() - problems in creating mixture of gaussian image!\n";
    return false;
  }

  bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>* >(mog_image.ptr());

  typename bvxm_voxel_traits<APM_T>::appearance_processor apm_processor;
  if (verbose_) {
    bvxm_voxel_slab<obs_datatype> exp_img = apm_processor.expected_color(*mog_image_ptr);
    vil_image_view_base_sptr temp_img = new vil_image_view<vxl_byte>(ni_, nj_, nplanes_);
    bvxm_util::slab_to_img(exp_img, temp_img);
    vil_save(*temp_img, "./mixture_expected_img.png");
  }

  //3) optimize two parameters for the input image so that it is the maximally probable image seen wrt mog_image
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

        vil_image_view<float>* nimg = new vil_image_view<float>( ni_, nj_, nplanes_ );
        normalize_image(*input_img_float_stretched, *nimg, sa, sb, 1.f);
        vil_image_view_base_sptr nimg_sptr = nimg;

        // convert image to a voxel_slab
        bvxm_voxel_slab<obs_datatype> image_slab(ni_, nj_, 1);
        bvxm_util::img_to_slab(nimg_sptr,image_slab);
        nimg_sptr = 0;  // to clear up space

        bvxm_voxel_slab<float> prob = apm_processor.prob_density(*mog_image_ptr,image_slab); //prob( nimg );

        // find the total prob
        bvxm_voxel_slab<float> product(ni_, nj_, 1);
        bvxm_util::multiply_slabs(prob, weights, product);
        float this_prob = bvxm_util::sum_slab(product);

        //vcl_cerr << this_prob << ' ';
        if ( this_prob < 0 ) {
          vcl_cout << "In bvxm_normalize_image_process::execute() -- prob is negative, Exiting!\n";
          return false;
        }

        if ( this_prob > sb_best_prob ){ sb_best_prob = this_prob; sb_best = sb; }
        if ( this_prob > best_prob ){ best_prob = this_prob; a = sa; b = sb; }
      }
      //vcl_cerr << '\n';
    }
    //vcl_cerr << '\n';
  }
  if (verbose_)
  {
    vcl_ofstream file;
    file.open("./normalization_parameters.txt", vcl_ofstream::app);
    file << a << ' ' << b <<'\n';
  }

  return true;
}



