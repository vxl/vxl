
#ifndef octree_test_driver_h_
#define octree_test_driver_h_

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <CL/cl.h>
#include <boct/boct_tree.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_vector.h>
#include <boxm/opt/open_cl/boxm_ray_trace_manager.h>
#define SDK_SUCCESS 0
#define SDK_FAILURE 1
#define VECTOR_SIZE 4

class octree_test_driver
{
  boxm_ray_trace_manager* cl_manager_;
  cl_ulong used_local_memory_;      
  cl_ulong kernel_work_group_size_; 
  cl_command_queue command_queue_;  
  cl_mem   input_cell_buf_;
  cl_mem   input_data_buf_;
  cl_mem   input_ray_origin_buf_;
  cl_mem   input_ray_dir_buf_;

  //output results
  cl_mem   tree_test_output_;

 public:
  /**
   * Constructor
   * Initialize member variables
   * @param name name of sample (string)
   */
  octree_test_driver()
    {cl_manager_ = boxm_ray_trace_manager::instance();}

  ~octree_test_driver();

  bool init()
  {
    if (setup_cl()!=SDK_SUCCESS) {
      vcl_cout << "In octree_test_driver::constructor - setup_cl failed" << vcl_endl;
      return false;
    }
    else
      return true;
  }

  int check_val(cl_int status, cl_int result, std::string message) {
    if (status != result) {
      vcl_cout << message << vcl_endl;
      return 0;
    }
    else
      return 1;
  }
  cl_int* tree_results() {return cl_manager_->tree_results();}
  vcl_size_t tree_result_size_bytes() const {return 4*cl_manager_->tree_result_size();}

  void set_buffers();

  int create_kernel(vcl_string const& name);

  int release_kernel();

  int run_tree_test_kernels();

  int run_ray_trace_test_kernels();

  int build_program();

  int cleanup_tree_test();

  int cleanup_ray_test();

  vcl_size_t n_rays() const {return cl_manager_->n_rays();}

  void print_kernel_usage_info();
 private:
  int set_tree_args();
  int set_ray_trace_args();
  int setup_cl();
};

#endif // octree_test_driver_h_
