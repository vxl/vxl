// This is brl/bseg/boxm/algo/rt/pro/processes/boxm_ray_statistics_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for describing the attributes of a scene
//
// \author J.L. Mundy
// \date January 24, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boct/boct_tree.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_compute_scene_statistics.h>
#include <boxm/boxm_apm_traits.h>
#include <bsta/bsta_histogram_sptr.h>
#include <boxm/algo/rt/boxm_compute_num_rays_statistics.h>

namespace boxm_ray_statistics_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm_ray_statistics_process_cons(bprb_func_process& pro)
{
  using namespace boxm_ray_statistics_process_globals;

  //process takes 2 input
  //input[0]: scene binary file
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "unsigned";  //ni
  input_types_[3] = "unsigned";  //nj

  // process has 1 output:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "bsta_histogram_sptr"; // No of rays per cell Histogram
  //output_types_[1] = "bsta_histogram_sptr"; // Sigma Histogram
  //output_types_[2] = "bsta_histogram_sptr"; // Level Histogram
  //output_types_[3] = "unsigned";            // Number of leaves

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: This process produces one output:
//  * The distribution of "Omega" values where Omega = 1-e^( alpha x length )
//  The following three outputs are no longer present:
//  * The distribution of "Sigma" values where Sigma is the std_dev of
//    the appearance model. (currently implemented only for simple grey)
//  * The histogram of levels for leaves in the octree
//  * The number of leaves in the tree
//
bool boxm_ray_statistics_process(bprb_func_process& pro)
{
  using namespace boxm_ray_statistics_process_globals;

  if ( !pro.verify_inputs() ) {
    std::cerr << pro.name() << ": invalid inputs\n";
    return false;
  }
  // assign tree type
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(0);
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(1);
  auto ni = pro.get_input<unsigned>(2);
  auto nj = pro.get_input<unsigned>(3);

  if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      auto* scene = dynamic_cast<boxm_scene<tree_type> *> (scene_ptr.as_pointer());

      bsta_histogram<float> num_rays_hist;
      if (!compute_ray_statistics<short, boxm_sample<BOXM_APM_MOG_GREY> >(*scene, cam_ptr,num_rays_hist,ni,nj))
        return false;
      pro.set_output_val<bsta_histogram_sptr>(0, new bsta_histogram<float>(num_rays_hist));    }
    else
    {
      std::cerr<<"boxm_ray_statistics_process not yet implemented for multi-bin/Gaussian mixture\n";
      return false;
    }
  }
  else if (scene_ptr->appearence_model() == BOXM_APM_SIMPLE_GREY)
  {
    if (!scene_ptr->multi_bin()) {
      typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
      auto* scene = dynamic_cast<boxm_scene<tree_type> *> (scene_ptr.as_pointer());

      bsta_histogram<float> num_rays_hist;
      if (!compute_ray_statistics<short,boxm_sample<BOXM_APM_SIMPLE_GREY> >(*scene, cam_ptr,num_rays_hist,ni,nj))
        return false;
      pro.set_output_val<bsta_histogram_sptr>(0, new bsta_histogram<float>(num_rays_hist));
    }
    else {
      std::cerr<<"boxm_ray_statistics_process not yet implemented for multi-bin/simple_grey\n";
      return false;
    }
  }

  return true;
}
