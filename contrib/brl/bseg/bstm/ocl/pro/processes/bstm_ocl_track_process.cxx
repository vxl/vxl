// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_track_process.cxx
//:
// \file
// \brief
//
// \author Ali Osman Ulusoy
// \date Feb 20, 2013

//directory utility
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vcl_where_root_dir.h>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/bstm_util.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <brdb/brdb_value.h> //brdb stuff
#include <bstm/ocl/algo/bstm_ocl_annealed_particle_filter.h>
#include <bstm/ocl/algo/bstm_ocl_particle_filter.h>



namespace bstm_ocl_track_process_globals
{
  constexpr unsigned n_inputs_ = 12;
  constexpr unsigned n_outputs_ = 0;
}

bool bstm_ocl_track_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_track_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_cache_sptr";
  input_types_[3] = "bstm_opencl_cache_sptr";
  input_types_[4] = "float"; //center x
  input_types_[5] = "float"; //center y
  input_types_[6] = "float"; //center z
  input_types_[7] = "float"; //len x
  input_types_[8] = "float"; //len y
  input_types_[9] = "float"; //len z
  input_types_[10] = "int"; //label to track
  input_types_[11] = "vcl_string"; //path to save particles

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool bstm_ocl_track_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_track_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  bstm_opencl_cache_sptr opencl_cache= pro.get_input<bstm_opencl_cache_sptr>(i++);
  auto center_x = pro.get_input<float>(i++);
  auto center_y = pro.get_input<float>(i++);
  auto center_z = pro.get_input<float>(i++);
  auto len_x = pro.get_input<float>(i++);
  auto len_y = pro.get_input<float>(i++);
  auto len_z = pro.get_input<float>(i++);
  int label = pro.get_input<int>(i++);
  std::string pf_output_path = pro.get_input<std::string>(i++);

  //get scene data type and appTypeSize
  std::string data_type;
  int apptypesize;
  std::vector<std::string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  if ( !bstm_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
    std::cout<<"bstm_ocl_track_process ERROR: scene doesn't have BSTM_MOG6_VIEW_COMPACT data type"<<std::endl;
    return false;
  }
  std::string options = bstm_ocl_util::mog_options(data_type);

  //construct bounding box
  vgl_point_3d<double> center(center_x,center_y,center_z);
  vgl_box_3d<double> bb(center,len_x,len_y,len_z, vgl_box_3d<double>::centre);

  //get scene time span
  double min_t,max_t;
  scene->bounding_box_t(min_t, max_t);

  unsigned num_particles = 1*1024/2;
  double w_sigma = 0.2;
  double t_sigma = 0.1;
  double w_kappa = 25;

  bstm_ocl_annealed_particle_filter filter(device,scene,cache, opencl_cache,(unsigned int)  0, (unsigned int) 520, bb, num_particles,
                                            t_sigma, w_sigma, w_kappa,options, 16, label, pf_output_path);
  filter.set_num_annealing_layers(5);
  filter.track();
  return true;
}
