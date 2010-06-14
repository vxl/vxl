#ifndef ray_bundle_test_driver_h_
#define ray_bundle_test_driver_h_

#include <boxm/ocl/boxm_ray_trace_manager.h>
#include <boct/boct_tree.h>
#include <bocl/bocl_cl.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>

#define SDK_SUCCESS 0
#define SDK_FAILURE 1
#define VECTOR_SIZE 4

template <class T>
class ray_bundle_test_driver
{
  boxm_ray_trace_manager<T> * cl_manager_;
  cl_ulong used_local_memory_;
  cl_ulong kernel_work_group_size_;
  cl_command_queue command_queue_;
  cl_mem   input_cell_buf_;
  cl_mem   input_data_buf_;
  //output results
  cl_mem   tree_test_output_;
  unsigned bni_;
  unsigned bnj_;
  unsigned wni_;
  unsigned wnj_;
 public:
  ray_bundle_test_driver() { cl_manager_ = boxm_ray_trace_manager<T>::instance(); bni_=0; bnj_=0;wni_=0; wnj_=0;}

  ~ray_bundle_test_driver();

  bool init()
  {
    if (setup_cl()!=SDK_SUCCESS) {
      vcl_cout << "In ray_bundle_test_driver::constructor - setup_cl failed" << vcl_endl;
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
  cl_int* tree_results() { return cl_manager_->tree_results(); }
  vcl_size_t tree_result_size_bytes() const { return 4*cl_manager_->tree_result_size(); }

  int create_kernel(vcl_string const& name);
  cl_float * ray_results() { return cl_manager_->ray_results(); }
  int release_kernel();

  int run_bundle_test_kernels();

  int build_program();

  int cleanup_bundle_test();

  void set_bundle_ni(unsigned ni){bni_=ni;}
  void set_bundle_nj(unsigned nj){bnj_=nj;}

  void set_work_space_ni(unsigned ni){wni_=ni;}
  void set_work_space_nj(unsigned nj){wnj_=nj;}

  vcl_size_t bundle_ni(){return bni_;}
  vcl_size_t bundle_nj(){return bnj_;}

  vcl_size_t work_space_ni(){return wni_;}
  vcl_size_t work_space_nj(){return wnj_;}

  vcl_size_t n_rays_in_bundle() const {return bni_*bnj_;}

  void print_kernel_usage_info();
  int set_tree_args();
  int set_basic_test_args(vcl_string arg_setup_spec="basic");
  bool setup_image_cam_data();
  int set_image_cam_args();
  bool clean_image_cam_data();
  bool setup_norm_data(vcl_string mode,bool use_uniform=true,
                       float mean = 0.0f, float sigma = 0.0f);
  int set_norm_args();
  int run_norm_kernel();
  bool clean_norm_data();
  bool init_work_image(vcl_string mode);
  void print_work_image(){cl_manager_->print_ray_results();}
  cl_float* work_image(){return cl_manager_->ray_results();}
 private:
  int setup_cl();
};

#endif // ray_bundle_test_driver_h_
