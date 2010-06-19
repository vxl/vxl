#ifndef stat_test_driver_h_
#define stat_test_driver_h_

#include <boxm/ocl/boxm_stat_manager.h>
#include <bocl/bocl_cl.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>

#define SDK_SUCCESS 0
#define SDK_FAILURE 1
#define VECTOR_SIZE 4

template <class T>
class stat_test_driver
{
  boxm_stat_manager<T> * cl_manager_;
  cl_ulong used_local_memory_;
  cl_ulong kernel_work_group_size_;
  cl_command_queue command_queue_;

 public:
  stat_test_driver() { cl_manager_ = boxm_stat_manager<T>::instance(); }

  ~stat_test_driver();

  bool init()
  {
    if (setup_cl()!=SDK_SUCCESS) {
      vcl_cout << "In stat_test_driver::constructor - setup_cl failed" << vcl_endl;
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

  int create_kernel(vcl_string const& name);

  int release_kernel();

  int run_stat_kernels();

  int build_program();

  int cleanup_queue();
  bool cleanup_stat_test();

  void print_kernel_usage_info();

  bool setup_result_data(unsigned rsize);

  int set_stat_args(vcl_string arg_setup_spec="basic");

  bool setup_data(vcl_vector<float> const& data);

  bool clean_io_data();

  bool read_stat_results();

 private:
  int setup_cl();
};

#endif // stat_test_driver_h_
