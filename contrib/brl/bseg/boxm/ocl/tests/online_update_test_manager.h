#ifndef online_update_test_manager_h_
#define online_update_test_manager_h_
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

#include <vil/vil_image_view.h>

template <class T_data>
class online_update_test_manager : public bocl_manager<online_update_test_manager<T_data> >
{
 public:

  typedef boct_tree<short,T_data> tree_type;
  typedef boct_tree_cell<short,T_data> cell_type;

  typedef float obs_type;

  online_update_test_manager() :
    program_(),
    cells_(0),
    cells_size_(0),
    cell_data_(0),
    cell_data_size_(0),
    cell_aux_data_(0),
    root_level_(0),
    img_dims_(0),
    offset_x_(0),
    offset_y_(0),
    factor_(0),
    bni_(1),bnj_(1),
    wni_(1),wnj_(1),
    app_density_(0),
    block_(0),
    input_img_()
  {}

  ~online_update_test_manager() {
    if (program_)
      clReleaseProgram(program_);
    if (block_)
      delete block_;
  }

  // read the scene, cam and image
  void set_block_items(boxm_block<tree_type> *block,
                       vpgl_camera_double_sptr cam,
                       vil_image_view<obs_type> &obs);
  //: 2d workgroup
  void set_bundle_ni(unsigned bundle_x) { bni_=bundle_x; }
  void set_bundle_nj(unsigned bundle_y) { bnj_=bundle_y; }
  bool process_block(int numpass=5);
  //: set input image and image dimensions and camera
  bool set_input_view();
  bool set_input_view_buffers();
  bool release_input_view_buffers();
  bool clean_input_view();

  //: set the tree, data , aux_data and bbox
  bool set_tree(tree_type* tree);
  bool set_tree_buffers();
  bool release_tree_buffers();
  bool clean_tree();
  bool setup_norm_data(bool use_uniform=true,
                       float mean = 0.0f, float sigma = 0.0f);
  unsigned wni() const { return wni_; }
  unsigned wnj() const { return wnj_; }

  bool read_output_image();
  bool read_trees();
  vcl_vector<vnl_vector_fixed<float, 16> > tree_data() const { return tree_data_; }
  vcl_vector<vnl_vector_fixed<float, 4> > aux_data() const { return tree_aux_data_; }
  void print_tree();
  void print_leaves();
  void print_image();
  void save_image();
  cl_float * output_image() { return image_; }

 protected:
  void clear_tree_data();
  void archive_tree_data();
  bool create_command_queue();
  bool build_program(vcl_string const& functor, bool use_cell_data);
  bool set_kernels();
  bool clean_kernels();
  bool set_kernel_args(unsigned pass);
  bool set_block_data();
  bool set_block_data_buffers();
  bool release_block_data_buffers();
  bool clean_block_data();

  //: helper functions
  bool run_block(unsigned pass);

  // Set up Camera
  bool set_persp_camera();
  bool set_persp_camera_buffers();
  bool release_persp_camera_buffers();
  bool clean_persp_camera();

  // Set up input image
  bool set_input_image();
  bool set_input_image_buffers();
  bool release_input_image_buffers();
  bool clean_input_image();
  // bool set_root_level
  bool set_root_level();
  bool set_root_level_buffers();
  bool release_root_level_buffers();
  bool clean_root_level();

  bool set_offset_buffers(int off_x, int off_y,int factor);
  bool release_offset_buffers();
  bool release_command_queue();

  bool setup_app_density(bool use_uniform=true, float mean = 0.0f, float sigma = 0.0f);
  int setup_app_density_buffer();
  bool clean_app_density();
  int clean_app_density_buffer();
  cl_mem& app_density() { return app_density_buf_; }

  bool clean_norm_data();

  int build_kernel_program(cl_program & program);
  cl_kernel kernel(unsigned i)
  { if (i<kernels_.size()) return kernels_[i]; return 0; }

  cl_program program_;

  cl_command_queue command_queue_;

  vcl_vector<cl_kernel> kernels_;

  //array of tree cells,
  cl_int* cells_;
  cl_uint  cells_size_;

  //array of data pointed to by tree
  cl_float* cell_data_;
  cl_uint  cell_data_size_;

  // array of aux data point to by tree
  cl_float* cell_aux_data_;

  //root level
  cl_uint root_level_;

  // image dimensions
  cl_uint* img_dims_;

  cl_uint* data_array_size_;
  //offset for non-overlapping sections
  cl_uint  offset_x_;
  cl_uint  offset_y_;
  cl_uint  factor_;
  // bounding box for each tree
  cl_float * tree_bbox_;

  // image
  cl_float * image_;

  // camera

  cl_float * persp_cam_;

  cl_uint bni_;
  cl_uint bnj_;

  // workspace dimensions which will be
  // greater than or equal to image dimensions

  cl_uint wni_;
  cl_uint wnj_;
  // pointer to cl memory on GPU
  cl_mem   cells_buf_;
  cl_mem   cell_data_buf_;
  cl_mem   cell_aux_data_buf_;
  cl_mem   tree_bbox_buf_;
  cl_mem   data_array_size_buf_;

  cl_mem   persp_cam_buf_;
  cl_mem   image_buf_;
  cl_mem   img_dims_buf_;

  cl_mem   offset_x_buf_;
  cl_mem   offset_y_buf_;
  cl_mem   factor_buf_;

  cl_mem   root_level_buf_;

  cl_float* app_density_;
  cl_mem    app_density_buf_;

  boxm_block<tree_type >* block_;
  vpgl_camera_double_sptr cam_;
  vil_image_view<obs_type>  input_img_;
  vcl_vector<vnl_vector_fixed<float, 16> > tree_data_;
  vcl_vector<vnl_vector_fixed<float, 4> > tree_aux_data_;
};

#endif // online_update_test_manager_h_
