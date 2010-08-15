#ifndef bit_tree_test_manager_h_
#define bit_tree_test_manager_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_array_2d.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_utils.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_apm_traits.h>

#include <vil/vil_image_view.h>

class bit_tree_test_manager : public bocl_manager<bit_tree_test_manager>
{
 public:

  //(data_type and tree_type)
  typedef vnl_vector_fixed<float,16> float16;

  //: constructor
  bit_tree_test_manager() :
    program_() {}

  //: destructor
  ~bit_tree_test_manager() {
    if (program_)
      clReleaseProgram(program_);
  }

  //: init bit_tree_manager
  bool init_manager() {
    return    this->build_test_program()
           && this->set_test_kernels()
           && this->set_buffers();
  }

  //: run tests
  bool run_test_loc_code();
  bool run_test_traverse();
  bool run_test_traverse_to_level();
  bool run_test_traverse_force();
  bool run_test_traverse_force_local();

  //: get output from OpenCL
  cl_int* get_output();

  //: set the tree
  bool set_tree(unsigned char* bit_tree) {
    for (int i=0; i<16; ++i)
      bit_tree_[i] = bit_tree[i];
    return true;
  }

  //: profiling information...
  double gpu_time_;

 protected:

  //-------------------------------------------------------------------
  // GPU side buffers
  //-------------------------------------------------------------------
  cl_uchar bit_tree_[16];
  cl_float data_[100];
  cl_int   output_[16];

  //cl_mem buffers
  cl_mem bit_tree_buf_;
  cl_mem data_buf_;
  cl_mem output_buf_;

  //-------------------------------------------------------------------
  //  Buffer setters
  //-------------------------------------------------------------------
  bool set_buffers();
  bool release_buffers();

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

#endif // bit_tree_test_manager_h_
