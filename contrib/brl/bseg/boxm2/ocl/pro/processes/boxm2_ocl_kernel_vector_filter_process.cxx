//:
// \file
// \brief A process to filter a boxm2 scene with a vectors of bvpl_kernels
// \author Isabel Restrepo
// \date April 12, 2012

#include "boxm2_ocl_kernel_vector_filter_process.h"

//:sets input and output types
bool boxm2_ocl_kernel_vector_filter_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_kernel_vector_filter_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++] = "bocl_device_sptr";
  input_types_[i++] = "boxm2_scene_sptr";
  input_types_[i++] = "boxm2_opencl_cache_sptr";
  input_types_[i++] = "bvpl_kernel_vector_sptr";

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool boxm2_ocl_kernel_vector_filter_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_kernel_vector_filter_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  bvpl_kernel_vector_sptr filter_vector = pro.get_input<bvpl_kernel_vector_sptr>(i++);

  std::cout<<"Using the following gpu device:\n" << *(device.ptr());

  // instantiate the vector filter object if need be
  std::string identifier = device->device_identifier();
  if (engines.find(identifier)==engines.end())
  {
    engines[identifier] = new boxm2_ocl_kernel_vector_filter(device);
  }

  bool status = engines[identifier]->run(scene, opencl_cache, filter_vector);

  return status;
}
