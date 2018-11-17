// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_adaptive_cone_update_process.cxx
#include <bprb/bprb_func_process.h>
#include <fstream>
#include <iostream>
#include <utility>
//:
// \file
// \brief  OpenCL accelerated Cone Update process
//
// \author Andrew Miller
// \date June 1, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/ocl/algo/boxm2_ocl_cone_update_function.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_adaptive_cone_update_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 0;

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels,std::string opts)
  {
    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "basic/linked_list.cl");
    src_paths.push_back(source_dir + "basic/ray_pyramid.cl");
    src_paths.push_back(source_dir + "basic/image_pyramid.cl");
    src_paths.push_back(source_dir + "ogl/intersect.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "cone/update_adaptive_cone_kernels.cl");
    std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
    src_paths.push_back(source_dir + "cone/cone_util.cl");
    src_paths.push_back(source_dir + "cone/cast_adaptive_cone_ray.cl");

    //compilation options
    const std::string& options = std::move(opts);

    //proc norm pass computes the proc_norm image, mean_obs for each cell
    auto* pass_one = new bocl_kernel();
    std::string one_opts = options + " -D PASSONE ";
    one_opts += " -D IMG_TYPE=float ";
    one_opts += " -D STEP_CELL=step_cell(aux_args,data_ptr,intersect_volume) ";
    one_opts += " -D COMPUTE_BALL_PROPERTIES=compute_ball_properties(aux_args)  ";
    pass_one->create_kernel(&device->context(),device->device_id(), src_paths, "pass_one", one_opts, "cone_update::pass_one");
    vec_kernels.push_back(pass_one);

    //computes bayes ratio for each cell
    auto* bayes_main = new bocl_kernel();
    std::string bayes_opt = options + " -D BAYES ";
    bayes_opt += " -D IMG_TYPE=float ";
    bayes_opt += " -D STEP_CELL=step_cell(aux_args,data_ptr,intersect_volume)  ";
    bayes_opt += " -D COMPUTE_BALL_PROPERTIES=compute_ball_properties(aux_args)  ";
    bayes_opt += " -D REDISTRIBUTE=redistribute(aux_args,data_ptr,intersect_volume)  ";
    bayes_main->create_kernel(&device->context(),device->device_id(), src_paths, "bayes_main", bayes_opt, "cone_update::bayes_main");
    vec_kernels.push_back(bayes_main);

    //may need
    auto* update = new bocl_kernel();
    update->create_kernel(&device->context(),device->device_id(), non_ray_src, "update_cone_data", options, "cone_update::update_data");
    vec_kernels.push_back(update);
    return ;
  }

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_adaptive_cone_update_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_adaptive_cone_update_process_globals;

  //process takes 6 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vcl_string";
  bool good = pro.set_input_types(input_types_);

  // process has no outputs:
  // output[0]: scene sptr

  // in case the 6th input is not set
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  pro.set_input(5, idx);
  return good;
}

bool boxm2_ocl_adaptive_cone_update_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_adaptive_cone_update_process_globals;

  if ( pro.n_inputs() < n_inputs_-1 ) {
    std::cout << pro.name() << ": The number of inputsshould be 5 or " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img =pro.get_input<vil_image_view_base_sptr>(i++);
  std::string ident = pro.get_input<std::string>(i++);

  long binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  //find scene types
  bool foundDataType = false, foundNumObsType = false;
  std::string data_type,num_obs_type,options;
  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
      // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_16 ";
      // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
    {
      num_obs_type = app;
      foundNumObsType = true;
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_OPENCL_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }
  if (!foundNumObsType) {
    std::cout<<"BOXM2_OPENCL_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_NUM_OBS type"<<std::endl;
    return false;
  }
  if (ident.size() > 0) {
    data_type += "_" + ident;
    num_obs_type += "_" + ident;
  }

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;

  //: compile the kernel if not already compiled
  std::string identifier=device->device_identifier()+options;
  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }

  //run ocl cone update function
  float gpu_time = boxm2_ocl_adaptive_cone_update(scene,
                                                  device,
                                                  opencl_cache,
                                                  kernels[identifier],
                                                  queue,
                                                  data_type,
                                                  num_obs_type,
                                                  cam ,
                                                  img );

  std::cout<<"Gpu time "<<gpu_time<<std::endl; //<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}
