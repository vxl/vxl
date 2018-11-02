//This is brl/bseg/bvxm/pro/processes/bvxm_create_mog_image_process.h
#ifndef bvxm_create_mog_image_process_h_
#define bvxm_create_mog_image_process_h_
//:
// \file
// \brief A process to create a Mixture of Gaussian (MOG) image of a voxel world from a given view point
//        MOG is the 2D appearance model (background model) of the 3D scene
//        the view point is given by the input camera
//        the size of the output MOG is given by the input ni and nj
//
// \author Gamze Tunali
// \date Feb. 10, 2009
// \verbatim
//  Modifications
//   Ozge C Ozcanli - Feb 10, 2009 - change input/output signature
//   Peter Vanroose - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <iostream>
#include <bprb/bprb_func_process.h>

#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/grid/bvxm_voxel_slab_base.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool bvxm_create_mog_image_process_cons(bprb_func_process& pro);

bool bvxm_create_mog_image_process(bprb_func_process& pro);

template <bvxm_voxel_type APM_T>
bool mix_gaussian(bvxm_voxel_world_sptr world,
                  unsigned mog_creation_method_,
                  unsigned bin_index, unsigned scale_index,unsigned n_samples,
                  bvxm_image_metadata observation,
                  bvxm_voxel_slab_base_sptr& mog_image)
{
  typedef typename bvxm_voxel_traits<APM_T>::obs_datatype obs_datatype;

  bool done = false;
  switch (mog_creation_method_)
  {
    case bvxm_mog_image_creation_methods::MOST_PROBABLE_MODE:
      done = world->mog_most_probable_image<APM_T>(observation, mog_image, bin_index,scale_index); break;
    case bvxm_mog_image_creation_methods::EXPECTED_VALUE:
      done = world->mixture_of_gaussians_image<APM_T>(observation, mog_image, bin_index,scale_index); break;
    case bvxm_mog_image_creation_methods::SAMPLING:
      done = world->mog_image_with_random_order_sampling<APM_T>(observation, n_samples, mog_image, bin_index, scale_index);
      break;
    default:
      std::cout << "In bvxm_create_mog_image_process::mix_gaussian() - unrecognized option: " << mog_creation_method_ << " to create mog image\n";
      return false;
  }
  if (!done)
    std::cout << "In bvxm_create_mog_image_process::mix_gaussian() - problems in creating mixture of gaussian image!\n";

  return done;
}

#endif // bvxm_create_mog_image_process_h_
