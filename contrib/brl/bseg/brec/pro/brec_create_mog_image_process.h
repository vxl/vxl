// This is brl/bseg/brec/pro/brec_create_mog_image_process.h
#ifndef brec_create_mog_image_process_h_
#define brec_create_mog_image_process_h_
//:
// \file
// \brief A class to create a mixture of gaussian image for a given voxel world from a given view
//        Summarizes the appearance model of the voxel world and constitutes a view-based background model
//        Outputs the mixture in the bbgm format for binary io with that libraries methods
//
// \author Ozge Can Ozcanli
// \date 12/15/2008
// \verbatim
//  Modifications
//
// 
// \endverbatim

#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <vcl_cmath.h>
#ifdef DEBUG
#include <vcl_iostream.h>
#endif
#include <bprb/bprb_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view.h>
#include <bvxm/bvxm_voxel_world.h>

#include <bbgm/bbgm_image_sptr.h>

class brec_create_mog_image_process : public bprb_process
{
public:

  brec_create_mog_image_process();

  //: Copy Constructor (no local data)
  brec_create_mog_image_process(const brec_create_mog_image_process& other): bprb_process(*static_cast<const bprb_process*>(&other)) {}

  ~brec_create_mog_image_process() {}

  //: Clone the process
  virtual brec_create_mog_image_process* clone() const { return new brec_create_mog_image_process(*this); }

  vcl_string name() { return "brecCreateMOGImageProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }

protected:

  //:This local function calculates and retrieves optimal normalization parameters
  template <bvxm_voxel_type APM_T> 
  bool create_mog(vpgl_camera_double_sptr const &camera,
    bvxm_voxel_world_sptr const &world,
    unsigned const bin_index,
    unsigned const scale_index,
    bool verbose);

  //:This local function calculates and retrieves optimal normalization parameters
  template <bvxm_voxel_type APM_T> 
  bool create_bbgm_image(bool verbose);

  unsigned ni_;
  unsigned nj_;
  unsigned nplanes_;

  bvxm_voxel_slab_base_sptr mog_image_;
  vil_image_view_base_sptr out_img_;
  bbgm_image_sptr out_model_img_;
};

template <bvxm_voxel_type APM_T>
bool brec_create_mog_image_process::create_mog(vpgl_camera_double_sptr const &camera,
                                               bvxm_voxel_world_sptr const &world,
                                               unsigned const bin_index,
                                               unsigned const scale_index,
                                               bool verbose)
{
  //1)Set up a dummy image for the observation
  vil_image_view_base_sptr input_img = new vil_image_view<vxl_byte>(ni_, nj_, 1);
  
  // create metadata:
  bvxm_image_metadata observation(input_img,camera);

  // get parameters
  unsigned mog_creation_method = (unsigned)bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE; // default is most_probable_mode
  parameters()->get_value("mog_method", mog_creation_method);

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
      { done = world->mog_most_probable_image<APM_T>(observation, mog_image_, bin_index,scale_index); } break;
    case bvxm_mog_image_creation_methods::EXPECTED_VALUE:
      { done = world->mixture_of_gaussians_image<APM_T>(observation, mog_image_, bin_index,scale_index); } break;
    case bvxm_mog_image_creation_methods::SAMPLING:
      { unsigned n_samples;
        parameters()->get_value("n_samples", n_samples);  
        done = world->mog_image_with_random_order_sampling<APM_T>(observation, n_samples, mog_image_, bin_index, scale_index); 
      } break;
    default:
      { vcl_cout << "In brec_create_mog_image_process::create_mog() - unrecognized option: " << mog_creation_method << " to create mog image\n"; return false; }
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
  //vil_save(*temp_img, "./mixture_expected_img.png");

  return true;
}

template <bvxm_voxel_type APM_T> 
bool brec_create_mog_image_process::create_bbgm_image(bool verbose)
{
  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype mog_type;
  out_model_img_ = new bbgm_image_of<mog_type>();
  bbgm_image_of<mog_type>* out_model_img_ptr = dynamic_cast<bbgm_image_of<mog_type>* >(out_model_img_.ptr());
  out_model_img_ptr->set_size(ni_, nj_);
  bbgm_image_of<mog_type>::iterator model_it = out_model_img_ptr->begin();

  bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>* >(mog_image_.ptr());
  bvxm_voxel_slab<mog_type>::const_iterator iter = mog_image_ptr->begin();

  for ( ; iter != mog_image_ptr->end(); iter++, ++model_it) 
    (*model_it) = (*iter);
  
  return true;
}


#endif // brec_create_mog_image_process_h_
