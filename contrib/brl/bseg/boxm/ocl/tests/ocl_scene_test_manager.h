#ifndef ocl_scene_test_manager_h_
#define ocl_scene_test_manager_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_utils.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_apm_traits.h>

class ocl_scene_test_manager : public bocl_manager<ocl_scene_test_manager>
{
 public:

  //(data_type and tree_type)
  typedef vnl_vector_fixed<int,2> int2;
  typedef vnl_vector_fixed<int,4> int4;
  typedef vnl_vector_fixed<float,16> float16;

  //: constructor
  ocl_scene_test_manager() :
    program_(0) {}

  //: destructor
  ~ocl_scene_test_manager() {
    if (program_)
      clReleaseProgram(program_);
  }

  //: init buffers for ocl scene
  bool init_arrays();

  //: init bit_tree_manager
  bool init_manager();


  //: run tests
  bool run_test(unsigned pass);

  //: get output from OpenCL
  cl_int* get_output();

  //: set the tree
  bool set_tree(vcl_vector<int4> ocl_tree);

  //: profiling information...
  double gpu_time_;

  //-------------------------------------------------------------------
  //  Buffer setters
  //-------------------------------------------------------------------
  bool set_buffers();
  bool release_buffers();
 protected:

  //-------------------------------------------------------------------
  // CPU/GPU side buffers
  //-------------------------------------------------------------------
  cl_int*   ocl_tree_;
  cl_int*   output_;

  //cl_mem buffers
  cl_mem ocl_tree_buf_;
  cl_mem output_buf_;

  //-------------------------------------------------------------------
  // Kernel and program helpers
  //-------------------------------------------------------------------
  bool build_test_program();
  bool set_test_kernels();
  bool set_kernel_args(unsigned int kernel_index);
  vcl_vector<cl_kernel> kernels_;

  //------------------------------------------------------------------
  //necessary CL stuff
  //------------------------------------------------------------------
  bool create_command_queue();
  bool release_command_queue();
  int build_kernel_program(cl_program & program);
  bool release_kernels();
  cl_program program_;
  cl_command_queue command_queue_;
};

#endif // ocl_scene_test_manager_h_
