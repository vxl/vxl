#ifndef boxm_render_bit_scene_manager_h_
#define boxm_render_bit_scene_manager_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_array_2d.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_utils.h>
#include <boxm/ocl/boxm_ocl_bit_scene.h>

#include <vil/vil_image_view.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vul/vul_file_iterator.h>

class boxm_render_bit_scene_manager : public bocl_manager<boxm_render_bit_scene_manager >
{
  typedef vnl_vector_fixed<int, 4> int4;
  typedef vnl_vector_fixed<float, 16> float16;
  typedef vnl_vector_fixed<int, 2> int2;

 public:
  typedef float obs_type;

  boxm_render_bit_scene_manager() :
    program_(0),        //cl program created from source
    //model info
    block_ptrs_(0),     //3D array of ushort2 pointers
    trees_(0),          //2D array of uchar
    trees_size_(0),
    data_alpha_(0),
    data_mixture_(0),
    data_size_(0),
    //shape of buffers
    numbuffer_(0),
    tree_buffer_length_(0),
    data_buffer_length_(0),
    //other scene info
    scene_dims_(0),
    scene_origin_(0),
    block_dims_(0),     //block_dim
    root_level_(0),
    // image dimensions
    img_dims_(0),
    offset_x_(0),
    offset_y_(0),
    tree_bbox_(0),
    // camera information
    persp_cam_(0),
    ext_cam_(0),
    bni_(1), bnj_(1),
    scene_x_(0),scene_y_(0),scene_z_(0),
    wni_(1), wnj_(1),
    bit_lookup_(0),
    output_(0),
    output_img_() {}

  ~boxm_render_bit_scene_manager() {
    if (program_)
      clReleaseProgram(program_);
  }

  // read the scene, cam and image
  bool init_ray_trace(boxm_ocl_bit_scene *scene,
                      vpgl_camera_double_sptr cam,
                      vil_image_view<obs_type> &obs,
                      bool render_depth=false);
  //: 2d workgroup
  void set_bundle_ni(unsigned bundle_x) {bni_=bundle_x;}
  void set_bundle_nj(unsigned bundle_y) {bnj_=bundle_y;}

  //: run update
  bool run_scene();
  bool set_args(unsigned kernel_index);
  bool set_kernel();
  bool release_kernel();
  bool set_commandqueue();
  bool release_commandqueue();
  bool set_workspace();

  //: set  root level
  bool set_scene_data();
  bool set_scene_data_buffers();
  bool release_scene_data_buffers();
  bool clean_scene_data();

  //: set input image and image dimensions and camera
  bool set_input_view();
  bool set_input_view_buffers();
  bool release_input_view_buffers();
  bool clean_input_view();

  //: set the tree, data , aux_data and bbox
  bool set_tree_buffers();
  bool release_tree_buffers();
  bool clean_tree();

  //: load all blocks in an array and store the tree pointers in block_ptrs;
  bool set_all_blocks();

  unsigned wni() {return wni_;}
  unsigned wnj() {return wnj_;}

  float gpu_time() {return gpu_time_; }

  bool read_output_image();
  bool read_trees();
  void print_tree();
  void print_image();

  //: cleanup
  bool clean_update();

  // cl/gl image
  cl_float * output_image() {return image_;}
  cl_float * image_;
  cl_uint  * image_gl_;
  cl_mem     image_buf_;
  cl_mem     image_gl_buf_;

  bool start(bool set_gl_buffer=false);
  //: helper functions
  bool run(bool rerender=false);
  bool set_gl_buffer();
  bool release_gl_buffer();
  bool finish();

  void save_image(vcl_string img_filename);

  // Set up Scene
  bool set_scene_dims();
  bool set_scene_dims_buffers();
  bool set_scene_origin();
  bool set_scene_origin_buffers();
  bool set_block_dims();
  bool set_block_dims_buffers();
  bool set_block_ptrs();
  bool set_block_ptrs_buffers();
  bool set_scene_info();
  bool set_scene_info_buffer();

  //clean up scene
  bool clean_scene_dims();
  bool release_scene_dims_buffers();
  bool clean_scene_origin();
  bool release_scene_origin_buffers();
  bool clean_block_dims();
  bool release_block_dims_buffers();
  bool clean_block_ptrs();
  bool release_block_ptrs_buffers();

  // Set up Camera
  bool set_persp_camera();
  bool set_persp_camera(vpgl_perspective_camera<double> * pcam);

  bool set_persp_camera_buffers();
  bool write_persp_camera_buffers();
  bool release_persp_camera_buffers();
  bool clean_persp_camera();

  // Set up input image
  bool set_input_image();
  bool set_input_image_buffers();
  bool set_image_dims_buffers();
  bool release_input_image_buffers();
  bool clean_input_image();
  // bool set_root_level
  bool set_root_level();
  bool clean_root_level();

  bool set_offset_buffers(int off_x, int off_y);
  bool release_offset_buffers();
  //open cl side helper functions
  int build_kernel_program(cl_program & program, bool render_depth=false);
  cl_kernel kernel(int kernelindex) {return kernels_[kernelindex];}

  //necessary CL items
  // for pass0 to compute seg len
  cl_program program_;

  cl_command_queue command_queue_;
  vcl_vector<cl_kernel> kernels_;

 protected:

  //-----------------------------------------------------------------
  // host side scene buffers
  //-----------------------------------------------------------------
  //model info
  cl_ushort * block_ptrs_;
  cl_uchar  * trees_;
  cl_uint     trees_size_;      //numbuffers_*tree_buffer_length_

  cl_float  * data_alpha_;
  cl_uchar  * data_mixture_;
  cl_uint     data_size_;       //numbuffers_*data_buffer_length_;

  //shape of buffers
  cl_int numbuffer_;
  cl_int tree_buffer_length_;
  cl_int data_buffer_length_;

  //other scene info
  RenderSceneInfo * scene_info_;
  cl_int   * scene_dims_;
  cl_float * scene_origin_;
  cl_float * block_dims_;
  cl_uint    root_level_;

  // image dimensions
  cl_uint  * img_dims_;

  //offset for non-overlapping sections
  cl_uint    offset_x_;
  cl_uint    offset_y_;

  // bounding box for each tree
  cl_float * tree_bbox_;

  // camera information
  cl_float * persp_cam_;
  cl_float * ext_cam_;
  cl_uint    bni_;
  cl_uint    bnj_;
  cl_int     scene_x_;
  cl_int     scene_y_;
  cl_int     scene_z_;

  // workspace dimensions which will be
  // greater than or equal to image dimensions
  cl_uint    wni_;
  cl_uint    wnj_;

  //bit lookup table
  cl_uchar*  bit_lookup_;
  //debugger
  cl_float*  output_;

  //------------------------------------------------------------------
  // GPU side pointers to cl memory
  //------------------------------------------------------------------
  cl_mem   scene_info_buf_;
  cl_mem   trees_buf_;
  cl_mem   data_alpha_buf_;
  cl_mem   data_mixture_buf_;
  cl_mem   tree_bbox_buf_;

  cl_mem   persp_cam_buf_;
  cl_mem   img_dims_buf_;

  cl_mem   scene_dims_buf_;
  cl_mem   scene_origin_buf_;
  cl_mem   block_ptrs_buf_;
  cl_mem   block_dims_buf_;

  cl_mem   bit_lookup_buf_;
  cl_mem   output_buf_;
  //------------------------------------------------------------------
  // scene and cam information in c++ format
  //------------------------------------------------------------------
  boxm_ocl_bit_scene * scene_;
  vpgl_camera_double_sptr cam_;
  vil_image_view<float>  output_img_;

  vcl_size_t globalThreads[2];
  vcl_size_t localThreads[2] ;

  //gpu time
  float gpu_time_;
};

#endif // boxm_render_bit_scene_manager_h_
