// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_convert_float_image_to_rgba_process.cxx
//:
// \file
// \brief  A process for rendering the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>


namespace boxm2_ocl_convert_float_image_to_rgba_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 0;
  std::size_t lthreads[2]={8,8};

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels, const std::string&  /*opts*/)
  {
    //create normalize image kernel
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/pixel_conversion_kernels.cl");
    auto * convert_float_to_rgba=new bocl_kernel();

    convert_float_to_rgba->create_kernel( &device->context(),
                                          device->device_id(),
                                          norm_src_paths,
                                          "convert_float_to_rgba","",   //kernel name
                                          "convert float to rgba"); //kernel identifier (for error checking)

    vec_kernels.push_back(convert_float_to_rgba);
    auto * convert_float4_to_rgba=new bocl_kernel();

    convert_float4_to_rgba->create_kernel( &device->context(),
                                           device->device_id(),
                                           norm_src_paths,
                                           "convert_float4_to_rgba","",   //kernel name
                                           "convert float4 to rgba"); //kernel identifier (for error checking)

    vec_kernels.push_back(convert_float4_to_rgba);
  }
}

bool boxm2_ocl_convert_float_image_to_rgba_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_convert_float_image_to_rgba_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "bocl_mem_sptr"; // float image
  input_types_[3] = "bocl_mem_sptr"; // float image dims
  input_types_[4] = "bocl_mem_sptr"; // gl image (rgba)
  input_types_[5] = "unsigned"; // gl image (rgba)
  input_types_[6] = "unsigned"; // gl image (rgba)

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_convert_float_image_to_rgba_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_convert_float_image_to_rgba_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  bocl_mem_sptr in_image =pro.get_input<bocl_mem_sptr>(i++);
  bocl_mem_sptr in_img_dim =pro.get_input<bocl_mem_sptr>(i++);
  bocl_mem_sptr gl_img =pro.get_input<bocl_mem_sptr>(i++);
  auto ni =pro.get_input<unsigned>(i++);
  auto nj =pro.get_input<unsigned>(i++);

  bool foundDataType = false;
  std::string data_type="";
  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
      if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
      {
          data_type = app;
          foundDataType = true;
      }
      else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
      {
          data_type = app;
          foundDataType = true;
      }
  }
  if (!foundDataType) {
      std::cout<<"BOXM2_OCL_CONVERT_FLOAT_IMAGE_TO_RGBA_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
      return false;
  }

//: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  std::string identifier=device->device_identifier();

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,"");
    kernels[identifier]=ks;
  }

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);

  float time =0.0f;

  if (data_type=="boxm2_gauss_rgb")
  {
    std::size_t gThreads[] = {cl_ni,cl_nj};
    bocl_kernel* convert_to_rgba_kern= kernels[identifier][1];
    convert_to_rgba_kern->set_arg( in_image.ptr() );
    convert_to_rgba_kern->set_arg( in_img_dim.ptr());
    convert_to_rgba_kern->set_arg( gl_img.ptr());
    convert_to_rgba_kern->execute( queue, 2, lthreads, gThreads);
    clFinish(queue);

    //clear render kernel args so it can reset em on next execution
    convert_to_rgba_kern->clear_args();
    time += convert_to_rgba_kern->exec_time();
  }
  else  if (data_type=="boxm2_mog3_grey" || data_type=="boxm2_mog3_grey_16")
  {
    std::size_t gThreads[] = {cl_ni,cl_nj};
    bocl_kernel* convert_to_rgba_kern= kernels[identifier][0];
    convert_to_rgba_kern->set_arg( in_image.ptr() );
    convert_to_rgba_kern->set_arg( in_img_dim.ptr());
    convert_to_rgba_kern->set_arg( gl_img.ptr());
    convert_to_rgba_kern->execute( queue, 2, lthreads, gThreads);
    clFinish(queue);

    //clear render kernel args so it can reset em on next execution
    convert_to_rgba_kern->clear_args();
    time += convert_to_rgba_kern->exec_time();
  }
  else
  {
    std::cout<<" Unable to know data type of the image"<<std::endl;
    return false;
  }

  // read out expected image
  clReleaseCommandQueue(queue);

 return true;
}
