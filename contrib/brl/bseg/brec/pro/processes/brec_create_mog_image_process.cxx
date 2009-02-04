// This is brl/bseg/brec/pro/processes/brec_create_mog_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to create a mixture of gaussian image of type bbgm_image_of from a bvxm_voxel_slab which 
//        should have been created for a voxel world from a given view using the corresponding bvxm process
//       
//
// Summarizes the appearance model of the voxel world and constitutes a view-based background model
// Outputs the mixture in the bbgm format for binary io with that libraries methods
//
// \author Ozge Can Ozcanli
// \date December 15, 2008
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - 02/03/09 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bbgm/bbgm_image_sptr.h>

//global variables/functions for bvxm_normalize_image_process_globals
namespace brec_create_mog_image_process_globals
{
  unsigned ni_= 0;
  unsigned nj_= 0;
  unsigned nplanes_= 0;

  bvxm_voxel_slab_base_sptr mog_image_;
  vil_image_view_base_sptr out_img_;
  bbgm_image_sptr out_model_img_;

  //this processes functions
  template <bvxm_voxel_type APM_T>
  bool create_mog(vpgl_camera_double_sptr const &camera,
                  bvxm_voxel_world_sptr const &world,
                  unsigned const bin_index,
                  unsigned const scale_index,
                  unsigned mog_creation_method,
                  unsigned n_samples,
                  bool verbose);

  template <bvxm_voxel_type APM_T>
  bool create_bbgm_image(bool verbose);
}

template <bvxm_voxel_type APM_T>
bool brec_create_mog_image_process_globals::create_mog(vpgl_camera_double_sptr const &camera,
                                               bvxm_voxel_world_sptr const &world,
                                               unsigned const bin_index,
                                               unsigned const scale_index,
                                               unsigned mog_creation_method,
                                               unsigned n_samples,
                                               bool verbose)
{
  //1)Set up a dummy image for the observation
  vil_image_view_base_sptr input_img = new vil_image_view<vxl_byte>(ni_, nj_, 1);

  // create metadata:
  bvxm_image_metadata observation(input_img,camera);

  if (verbose) {
    switch (mog_creation_method) {
      case bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE:
      { vcl_cout << "using most probable modes' colors to create mog image "; } break;
      case bvxm_mog_image_creation_methods::EXPECTED_VALUE:
      { vcl_cout << "using expected colors to create mog image "; } break;
      case bvxm_mog_image_creation_methods::SAMPLING:
      { vcl_cout << "using random sampling to create mog image "; } break;
      default:
      { vcl_cout << "In brec_create_mog_image_process::create_mog() - unrecognized option: " << mog_creation_method << " to create mog image\n"; return false; }
    }
  }

  //2) get probability mixtures of all pixels in image

  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype mog_type;
  typedef typename bvxm_voxel_traits<APM_T>::obs_datatype obs_datatype;

  bool done = false;
  switch (mog_creation_method) {
    case bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE:
      done = world->mog_most_probable_image<APM_T>(observation, mog_image_, bin_index,scale_index); break;
    case bvxm_mog_image_creation_methods::EXPECTED_VALUE:
      done = world->mixture_of_gaussians_image<APM_T>(observation, mog_image_, bin_index,scale_index); break;
    case bvxm_mog_image_creation_methods::SAMPLING:
    { done = world->mog_image_with_random_order_sampling<APM_T>(observation, n_samples, mog_image_, bin_index, scale_index);
    } break;
    default:
      vcl_cout << "In brec_create_mog_image_process::create_mog() - unrecognized option: " << mog_creation_method << " to create mog image\n";
      return false;
  }

  if (!done) {
    vcl_cout << "In brec_create_mog_image_process::create_mog() - problems in creating mixture of gaussian image!\n";
    return false;
  }

  bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>* >(mog_image_.ptr());

  typename bvxm_voxel_traits<APM_T>::appearance_processor apm_processor;

  bvxm_voxel_slab<obs_datatype> exp_img = apm_processor.expected_color(*mog_image_ptr);
  out_img_ = new vil_image_view<vxl_byte>(ni_, nj_, nplanes_);
  bvxm_util::slab_to_img(exp_img, out_img_);
#ifdef DEBUG
  vil_save(*temp_img, "./mixture_expected_img.png");
#endif

  return true;
}

template <bvxm_voxel_type APM_T>
bool brec_create_mog_image_process_globals::create_bbgm_image(bool verbose)
{
  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype mog_type;
  out_model_img_ = new bbgm_image_of<mog_type>();
  bbgm_image_of<mog_type>* out_model_img_ptr = dynamic_cast<bbgm_image_of<mog_type>* >(out_model_img_.ptr());
  out_model_img_ptr->set_size(ni_, nj_);
  typename bbgm_image_of<mog_type>::iterator model_it = out_model_img_ptr->begin();

  bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>* >(mog_image_.ptr());
  typename bvxm_voxel_slab<mog_type>::const_iterator iter = mog_image_ptr->begin();

  for ( ; iter != mog_image_ptr->end(); iter++, ++model_it)
    *model_it = *iter;

  return true;
}

//:Constructor
bool brec_create_mog_image_process_const(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");
  input_types.push_back("bvxm_voxel_world_sptr");
  input_types.push_back("vcl_string");
  input_types.push_back("unsigned");
  input_types.push_back("unsigned");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");
  output_types.push_back("bbgm_image_sptr");  // output mog image as a bbgm distribution image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  pro.parameters()->add("method to use to create a mog image", "mog_method", (unsigned)bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE);   // otherwise uses expected values of the mixtures at voxels along the path of the rays
  pro.parameters()->add("number of samples if using random sampling as mog_method", "n_samples", (unsigned)10);
  pro.parameters()->add("ni for output mog", "ni", (unsigned)1000);  // should be set according to each test image
  pro.parameters()->add("nj for output mog", "nj", (unsigned)1000);
  pro.parameters()->add("nplanes for output expected view of output mog", "nplanes", (unsigned)1);
  pro.parameters()->add("verbose", "verbose", false);

  return true;
}

bool brec_create_mog_image_process(bprb_func_process& pro)
{
  using namespace brec_create_mog_image_process_globals;

  // Sanity check
  if (pro.n_inputs() < 5){
    vcl_cerr << "brec_create_hierarchy_process - invalid inputs\n";
    return false;
  }

  //: get input
  unsigned i = 0;
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string voxel_type = pro.get_input<vcl_string>(i++);
  unsigned bin_index = pro.get_input<unsigned>(i++);
  unsigned scale_index = pro.get_input<unsigned>(i++);

  bool verbose = false;
  pro.parameters()->get_value("verbose", verbose);

  //set class variables
  pro.parameters()->get_value("ni", ni_);
  pro.parameters()->get_value("nj", nj_);
  pro.parameters()->get_value("nplanes", nplanes_);

  // if the world is not updated yet, we just return the input image
  unsigned num_observations = 0;
  if (voxel_type == "apm_mog_grey")
    num_observations = world->num_observations<APM_MOG_GREY>(bin_index,scale_index);
  else if (voxel_type == "apm_mog_mc_3_3")
    num_observations = world->num_observations<APM_MOG_MC_3_3>(bin_index,scale_index);
  else if (voxel_type == "apm_mog_mc_4_3")
    num_observations = world->num_observations<APM_MOG_MC_4_3>(bin_index,scale_index);
  else {
    vcl_cout << "In brec_create_mog_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  if (num_observations == 0)
  {
    vcl_cout << "In brec_create_mog_image_process::execute() -- input voxel world has 0 observations, not possible to create MOG image!\n";
    return false;
  }

  // get parameters
  unsigned mog_creation_method = (unsigned)bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE; // default is most_probable_mode
  pro.parameters()->get_value("mog_method", mog_creation_method);
  unsigned n_samples = 0;
  pro.parameters()->get_value("n_samples", n_samples);

  bool done = false;
  if (voxel_type == "apm_mog_grey")
    done = create_mog<APM_MOG_GREY>(camera,world,bin_index,scale_index,mog_creation_method,n_samples,verbose);
  else if (voxel_type == "apm_mog_mc_3_3")
    done = create_mog<APM_MOG_MC_3_3>(camera,world,bin_index,scale_index,mog_creation_method,n_samples,verbose);
  else if (voxel_type == "apm_mog_mc_4_3")
    done = create_mog<APM_MOG_MC_4_3>(camera,world,bin_index,scale_index,mog_creation_method,n_samples,verbose);
  else {
    vcl_cout << "In brec_create_mog_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  if (!done) {
    vcl_cout << "In brec_create_mog_image_process::execute() - problems in creating mixture of gaussian image!\n";
    return false;
  }

  // turn the bvxm_slab into a bbgm_image_of instance for binary io (i.e. use processes in bbgm_pro)
  done = false;
  if (voxel_type == "apm_mog_grey")
    done = create_bbgm_image<APM_MOG_GREY>(verbose);
  else if (voxel_type == "apm_mog_mc_3_3")
    done = create_bbgm_image<APM_MOG_MC_3_3>(verbose);
  else if (voxel_type == "apm_mog_mc_4_3")
    done = create_bbgm_image<APM_MOG_MC_4_3>(verbose);
  else {
    vcl_cout << "In brec_create_mog_image_process::execute() -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  if (!done) {
    vcl_cout << "In brec_create_mog_image_process::execute() - problems in creating mixture of gaussian image!\n";
    return false;
  }

  // return the output img
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_);
  pro.set_output_val<bbgm_image_sptr>(1, out_model_img_);

  return true;
}

