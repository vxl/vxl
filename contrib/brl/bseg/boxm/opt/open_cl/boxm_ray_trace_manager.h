#ifndef boxm_ray_trace_manager_h_
#define boxm_ray_trace_manager_h_

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <CL/cl.h>
#include <boct/boct_tree.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_array_2d.h>
#include <vcl_vector.h>
#include "boxm_opencl_manager.h"

class boxm_ray_trace_manager : public boxm_opencl_manager<boxm_ray_trace_manager>
{
  vcl_string prog_;
  boct_tree<short, vnl_vector_fixed<float, 2> >* tree_;

  cl_command_queue command_queue_;
  cl_program program_;
  cl_kernel kernel_;
  vcl_vector<vnl_vector_fixed<int, 4> > cell_input_;
  vcl_vector<vnl_vector_fixed<float, 2> > data_input_;
  vbl_array_2d<vnl_vector_fixed<float, 3> > ray_origin_input_;
  vbl_array_2d<vnl_vector_fixed<float, 3> > ray_dir_input_;
  cl_int* cells_;
  cl_float* cell_data_;
  cl_int* tree_results_;
  cl_float* ray_origin_;
  cl_float* ray_dir_;
  cl_float* ray_results_;
  cl_mem   input_cell_buf_;
  cl_mem   input_data_buf_;
  cl_mem   tree_test_output_buf_;
  cl_mem   ray_origin_buf_;
  cl_mem   ray_dir_buf_;

  //private methods
  void delete_memory();
  void free_ray_memory();
 public:

  // static boxm_ray_trace_manager*  instance();

  boxm_ray_trace_manager():
    prog_(""),
    program_(0),
    cells_(0),
    cell_data_(0),
    tree_results_(0),
    ray_origin_(0),
    ray_dir_(0),
    ray_results_(0),
    tree_(0)
    {}

  ~boxm_ray_trace_manager();

  void set_tree(boct_tree<short, vnl_vector_fixed<float, 2> >* tree) {tree_ = tree;}
  boct_tree<short, vnl_vector_fixed<float, 2> >* tree() {return tree_;}

  bool setup_tree();

  void set_rays(vbl_array_2d<vnl_vector_fixed<float, 3> > const& ray_origin,
                vbl_array_2d<vnl_vector_fixed<float, 3> > const& ray_dir)
    {ray_origin_input_ = ray_origin; ray_dir_input_ = ray_dir;}

  bool setup_rays();


  cl_mem cell_buf() {return input_cell_buf_;}
  cl_mem data_buf() {return input_data_buf_;}
  cl_mem ray_origin_buf() {return ray_origin_buf_;}
  cl_mem ray_dir_buf() {return ray_dir_buf_;}

  vcl_size_t cell_array_size() const {return cell_input_.size();}
  vcl_size_t cell_data_array_size() const {return data_input_.size();}

  vcl_size_t tree_result_size() const {return 2*cell_input_.size();}
  cl_int* tree_results() {return tree_results_;}
  void clear_tree_results();

  vcl_size_t n_rays() const {return ray_dir_input_.rows()*ray_dir_input_.cols();}
  vcl_size_t ray_rows() const {return ray_dir_input_.rows();}
  vcl_size_t ray_cols() const {return ray_dir_input_.cols();}
  cl_float* ray_results() {return ray_results_;}
  void clear_ray_results();
  vcl_size_t n_ray_groups();

  vcl_string program_source() const {return prog_;}
  cl_program program() {return program_;}

  void print_tree_input();
  void print_ray_input();
  void print_tree_results() {} //TODO: later
  void print_ray_results();

  int setup_tree_input_buffers();

  int setup_ray_input_buffers();

  int build_kernel_program();

  int create_kernel(vcl_string const& name);

  cl_kernel kernel() {return kernel_;}

  int release_kernel();

  int setup_tree_processing();

  int setup_ray_processing();

  //creates new ray cl_memory buffers with input ray data
  bool update_ray_input(vbl_array_2d<vnl_vector_fixed<float, 3> > const& ray_origin,
                        vbl_array_2d<vnl_vector_fixed<float, 3> > const& ray_dir);

  int cleanup_tree_processing();

  int cleanup_ray_processing();

  bool load_kernel_source(vcl_string const& path);
  bool append_process_kernels(vcl_string const& path);
  bool write_program(vcl_string const& path);
  bool load_tree(vcl_string const& path);
  bool write_tree(vcl_string const& path);
  bool load_rays(vcl_string const& path);
  bool write_rays(vcl_string const& path);
  };

#endif // boxm_ray_trace_manager_h_
