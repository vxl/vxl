#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bvxm/pro/processes/bvxm_normalization_util.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>

#include <brdb/brdb_value.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>

bool bvxm_create_mog_image_process_cons(bprb_func_process& pro)
{
  
  //inputs
  //0: The unnormalized image
  //1: The camera
  //2: The voxel world
  //3: The appereance model type
  //4: The illumination bin index nplanes_
  //5: The scale
  vcl_vector<vcl_string> input_types_;
  input_types_.push_back("vil_image_view_base_sptr");
  input_types_.push_back("vpgl_camera_double_sptr");
  input_types_.push_back("bvxm_voxel_world_sptr");
  input_types_.push_back("vcl_string");
  input_types_.push_back("unsigned");
  input_types_.push_back("unsigned");
  if (!pro.set_input_types(input_types_))
    return false;

  //output
  vcl_vector<vcl_string> output_types_;
  output_types_.push_back("bvxm_voxel_slab_base_sptr");
  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

template <bvxm_voxel_type APM_T>
bool mix_gaussian(bvxm_voxel_world_sptr world, 
                  unsigned mog_creation_method_, 
                  unsigned bin_index, unsigned scale_index,unsigned n_samples,
                  bvxm_image_metadata observation,
                  bvxm_voxel_slab_base_sptr& mog_image)
{
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
      done = world->mog_image_with_random_order_sampling<APM_T>(observation, n_samples, mog_image, bin_index, scale_index);
    } break;
    default:
    { vcl_cout << "In bvxm_normalize_image_process::norm_parameters() - unrecognized option: " << mog_creation_method_ << " to create mog image\n"; return false; }
  }

  if (!done) {
    vcl_cout << "In bvxm_normalize_image_process::norm_parameters() - problems in creating mixture of gaussian image!\n";
    return false;
  }
  return true;
}

bool bvxm_create_mog_image_process(bprb_func_process& pro)
{
   //get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr input_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string voxel_type = pro.get_input<vcl_string>(i++);
  unsigned bin_index = pro.get_input<unsigned>(i++);;
  unsigned scale_index = pro.get_input<unsigned>(i++);

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
  vcl_string param_mog_method_ = "mog_method";
  const vcl_string param_nsamples_ = "n_samples";

  unsigned n_samples = 10;
  unsigned mog_creation_method_ = bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE;

  pro.parameters()->get_value(param_mog_method_, mog_creation_method_);
  pro.parameters()->get_value(param_nsamples_, n_samples);

  bvxm_image_metadata observation(input_img,camera);

  bvxm_voxel_slab_base_sptr mog_image;
  bool good = false;
  if (voxel_type == "apm_mog_grey")
    good = mix_gaussian<APM_MOG_GREY>(world,mog_creation_method_,bin_index,scale_index,n_samples,observation,mog_image);
  else if (voxel_type == "apm_mog_mc_3_3")
    good = mix_gaussian<APM_MOG_MC_3_3>(world,mog_creation_method_,bin_index,scale_index,n_samples,observation,mog_image);
  else if (voxel_type == "apm_mog_mc_4_3")
    good = mix_gaussian<APM_MOG_MC_4_3>(world,mog_creation_method_, bin_index,scale_index,n_samples,observation,mog_image);
  else{
    vcl_cout << "In bvxm_normalize_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  if (!good)
    return false;
  
  unsigned j=0;
  pro.set_output_val<bvxm_voxel_slab_base_sptr>(j++,mog_image);
  return true;
}
