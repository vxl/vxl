#ifndef boxm_ray_trace_manager_h_
#define boxm_ray_trace_manager_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <bcl/bcl_cl.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_array_2d.h>
#include "boxm_opencl_manager.h"
#include <boxm/boxm_scene.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_apm_traits.h>
#include <boct/boct_tree.h>
#include <vpgl/vpgl_perspective_camera.h>
#include "boxm_opt_opencl_utils.h"


template <class T_data>
class boxm_ray_trace_manager : public boxm_opencl_manager<boxm_ray_trace_manager<T_data> >
{
 public:
  typedef boct_tree<short,T_data> tree_type;
  //typedef boct_tree<short,T_aux> aux_tree_type;
  typedef boct_tree_cell<short,T_data> cell_type;

  boxm_ray_trace_manager()
    : prog_(""),
    scene_(0),
    program_(0),
    cells_(0),
    cell_data_(0),
    tree_results_(0),
    ray_origin_(0),
    ray_dir_(0),
    ray_results_(0),
    //tree_(0),
    cam_(0),
    svd_UtWV_(0),imgdims_(0),roidims_(0),global_bbox_(0),
    ni_(0),nj_(0)
  {}

  ~boxm_ray_trace_manager();

  //: setup the parameters needed to run the raytrace
  bool init_raytrace(boxm_scene<tree_type> *scene,
    vpgl_camera_double_sptr cam,
    unsigned int ni, unsigned int nj,
    vcl_vector<vcl_string> functor_source_filenames,
    unsigned int i0 = 0, unsigned int j0 = 0);

  //: run the raytrace
  bool run();

  //: clean up allocated memory
  // (call after run has completed and results have been retrieved)
  bool clean_raytrace();

  unsigned ni(){return ni_;}
  unsigned nj(){return nj_;}

  cl_float* ray_results() {return ray_results_;}

  vcl_size_t tree_result_size() const {return 2*cell_input_.size();}
  cl_int* tree_results() {return tree_results_;}

  vcl_size_t n_rays() const {return ni_*nj_;}
  vcl_size_t ray_rows() const {return ni_;}
  vcl_size_t ray_cols() const {return nj_;}

  cl_mem cell_buf() {return input_cell_buf_;}
  cl_mem data_buf() {return input_data_buf_;}
  cl_mem ray_origin_buf() {return ray_origin_buf_;}
  cl_mem camera_buf() {return camera_buf_;}
  cl_mem imgdims_buf() {return imgdims_buf_;}
  vcl_size_t cell_array_size() const {return cell_input_.size();}
  vcl_size_t cell_data_array_size() const {return data_input_.size();}

  void set_camera(vpgl_camera_double_sptr cam){cam_ = cam;}

  void set_tree(boct_tree<short, T_data >* tree) {tree_ = tree;}
  boct_tree<short, T_data >* tree() {return tree_;}

  bool setup_tree();
  bool clean_tree();

  bool setup_tree_results();
  bool clean_tree_results();

  bool setup_camera();
  bool clean_camera();

  // does not do any allocation
  bool setup_img_bb(vpgl_camera_double_sptr cam, vgl_box_3d<double> const& block_bb, vgl_box_2d<double> &img_bb, unsigned ni,unsigned nj);

  void clear_tree_results();
  void set_dims(int ni,int nj){ni_=ni;nj_=nj;imgdims_[0]=ni_;imgdims_[1]=nj_;imgdims_[2]=0;imgdims_[3]=0;}

  void clear_ray_results();
  vcl_size_t n_ray_groups();

  vcl_string program_source() const {return prog_;}
  cl_program program() {return program_;}

  void print_tree_input();
  void print_ray_input();
  void print_tree_results() {} //TODO: later
  void print_ray_results();

  bool setup_img_dims(unsigned ni,unsigned nj);
  bool clean_img_dims();

  bool setup_roi_dims(unsigned min_i,unsigned max_i,unsigned min_j,unsigned max_j);
  bool clean_roi_dims();

  bool setup_work_image(unsigned ni,unsigned nj);
  bool clean_work_image();

  bool setup_ray_origin();
  bool clean_ray_origin();

  int setup_tree_input_buffers();
  int clean_tree_input_buffers();

  int setup_camera_input_buffer();
  int clean_camera_input_buffer();

  int setup_roidims_input_buffer();
  int clean_roidims_input_buffer();

  int setup_ray_origin_buffer();
  int clean_ray_origin_buffer();

  int setup_work_img_buffer();
  int clean_work_img_buffer();

  int setup_tree_global_bbox_buffer();
  int clean_tree_global_bbox_buffer();

  int setup_imgdims_buffer();
  int clean_imgdims_buffer();

  int build_kernel_program();

  int create_kernel(vcl_string const& name);
  int release_kernel();

  cl_kernel kernel() {return kernel_;}

  bool load_kernel_source(vcl_string const& path);
  bool append_process_kernels(vcl_string const& path);
  bool write_program(vcl_string const& path);
  bool load_tree(vcl_string const& path);
  bool write_tree(vcl_string const& path);

 protected:

  bool run_block();

  boxm_scene<tree_type> *scene_;
  vpgl_camera_double_sptr cam_;
  boct_tree<short, T_data > *tree_;

  vcl_string prog_;
  cl_command_queue command_queue_;
  cl_program program_;
  cl_kernel kernel_;
  vcl_vector<vnl_vector_fixed<int, 4> > cell_input_;
  vcl_vector<vnl_vector_fixed<float, 16>  > data_input_;
  vnl_vector_fixed<float, 4>  ray_origin_input_;
  vbl_array_2d<vnl_vector_fixed<float, 3> > ray_dir_input_;
  unsigned int i0_;
  unsigned int j0_;
  unsigned ni_;
  unsigned nj_;
  cl_int* cells_;
  cl_float* cell_data_;
  cl_int* tree_results_;
  cl_float* ray_origin_;
  cl_float* ray_dir_;
  cl_float* ray_results_;
  cl_float* svd_UtWV_;
  cl_uint * imgdims_;
  cl_uint * roidims_;
  cl_float * global_bbox_;
  cl_mem   input_cell_buf_;
  cl_mem   input_data_buf_;
  cl_mem   work_image_buf_;
  cl_mem   out_work_image_buf_;
  cl_mem   ray_origin_buf_;
  cl_mem   camera_buf_;
  cl_mem   imgdims_buf_;
  cl_mem   roidims_buf_;
  cl_mem   global_bbox_buf_;
};

#endif // boxm_ray_trace_manager_h_
