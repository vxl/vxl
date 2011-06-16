#ifndef boxm_render_probe_manager_h_
#define boxm_render_probe_manager_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_ref_count.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_utils.h>
#include <boxm/ocl/boxm_ocl_scene.h>

#include <vpgl/vpgl_perspective_camera.h>

class boxm_render_probe_manager : public bocl_manager<boxm_render_probe_manager >, public vbl_ref_count
{
  typedef vnl_vector_fixed<int, 4> int4;
  typedef vnl_vector_fixed<float, 16> float16;
  typedef vnl_vector_fixed<int, 2> int2;

 public:

  boxm_render_probe_manager() :
    program_(0),
    block_ptrs_(0),
    scene_dims_(0),
    block_dims_(0),
    cells_(0),
    cells_size_(0),
    cell_alpha_(0),
    cell_data_size_(0),
    cell_mixture_(0),
    numbuffer_(0),
    lenbuffer_(0),
    root_level_(0),
    bni_(1),bnj_(1),
    scene_x_(0), scene_y_(0), scene_z_(0),
    wni_(1), wnj_(1)
     {}

  ~boxm_render_probe_manager() {
    if (program_)
      clReleaseProgram(program_);
  }

  // read the scene, cam and image
  bool init_ray_trace(boxm_ocl_scene *scene,
                      vpgl_camera_double_sptr cam,
                      unsigned i,
                      unsigned j,
                      float intensity);
  bool setup_online_processing();
  bool online_processing(vpgl_camera_double_sptr & camera);
  bool finish_online_processing();
  //: 2d workgroup
  void set_bundle_ni(unsigned bundle_x) { bni_=bundle_x; }
  void set_bundle_nj(unsigned bundle_y) { bnj_=bundle_y; }

  //: run update
  bool run_scene();
  bool set_args();
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
  bool set_rayoutput();
  bool set_rayoutput_buffers();
  bool release_rayoutput_buffers();
  bool clean_rayoutput();

  //: set the tree, data , aux_data and bbox
  bool set_tree_buffers();
  bool release_tree_buffers();
  bool clean_tree();

  //: load all blocks in an array and store the tree pointers in block_ptrs;
  bool set_all_blocks();

  unsigned wni() const { return wni_; }
  unsigned wnj() const { return wnj_; }

  float gpu_time() const { return gpu_time_; }

  bool read_output_array();
  bool read_trees();
  void print_tree();

  bool run();

  // Set up Scene
  bool set_scene_dims();
  bool set_scene_dims_buffers();
  bool set_scene_origin();
  bool set_scene_origin_buffers();
  bool set_block_dims();
  bool set_block_dims_buffers();
  bool set_block_ptrs();
  bool set_block_ptrs_buffers();

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
  // bool set_root_level
  bool set_root_level();
  bool set_root_level_buffers();
  bool release_root_level_buffers();
  bool clean_root_level();

  void set_ij(int i,int j) { i_=i;j_=j; }
  void set_intensity(float intensity) { intensity_=intensity; }
  float get_output() const { return *output_; }
  //open cl side helper functions
  int build_kernel_program(cl_program & program);
  cl_kernel kernel() { return kernel_; }
  void getoutputarray(vcl_vector< vcl_vector<float> > & out);

  //necessary CL items
  cl_program program_;

  cl_command_queue command_queue_;
  cl_kernel kernel_;

 protected:

  // scene information
  cl_int * block_ptrs_;
  // (x,y,z,0)
  cl_int * scene_dims_;
  cl_float * scene_origin_;
  // (x,y,z,0)
  cl_float * block_dims_;

  //array of tree cells,
  cl_int* cells_;
  cl_uint  cells_size_;

  //array of data pointed to by tree
  cl_float* cell_alpha_;
  cl_uint  cell_data_size_;

  //OPTIMIZED DATA to replace float data
  cl_uchar* cell_mixture_;

  cl_int numbuffer_;
  cl_int lenbuffer_;
  //root level
  cl_uint root_level_;

  // image dimensions
  cl_uint* img_dims_;

  //offset for non-overlapping sections
  cl_uint  offset_x_;
  cl_uint  offset_y_;
  // bounding box for each tree
  cl_float * tree_bbox_;

  // camera
  cl_float * persp_cam_;
  cl_float * ext_cam_;

  cl_uint bni_;
  cl_uint bnj_;

  cl_int scene_x_;
  cl_int scene_y_;
  cl_int scene_z_;

  // workspace dimensions which will be
  // greater than or equal to image dimensions
  cl_uint wni_;
  cl_uint wnj_;

  // pointer to cl memory on GPU
  cl_mem   cells_buf_;
  cl_mem   cell_alpha_buf_;
  cl_mem   cell_mixture_buf_;
  cl_mem   tree_bbox_buf_;

  cl_mem   persp_cam_buf_;
  cl_mem   img_dims_buf_;

  cl_mem   scene_orig_buf_;

  cl_mem   root_level_buf_;
  cl_mem   numbuffer_buf_;
  cl_mem   lenbuffer_buf_;

  cl_mem   scene_dims_buf_;
  cl_mem   scene_origin_buf_;
  cl_mem   block_ptrs_buf_;
  cl_mem   block_dims_buf_;

  cl_mem  output_buf_;

  cl_mem rayoutput_buf_[10]  ;
  boxm_ocl_scene * scene_;
  vpgl_camera_double_sptr cam_;
  float *  rayoutput_[10]  ;
  cl_int i_;
  cl_int j_;
  unsigned raydepth_;
  cl_float intensity_;
  cl_float * output_;

  vcl_size_t globalThreads[2];
  vcl_size_t localThreads[2] ;

  //gpu time
  float gpu_time_;
};

//: Binary write boxm_render_probe_manager scene to stream
void vsl_b_write(vsl_b_ostream & os, boxm_render_probe_manager const& multi_tracker);

//: Binary load boxm_render_probe_manager scene from stream.
void vsl_b_read(vsl_b_istream & is, boxm_render_probe_manager &multi_tracker);

void vsl_b_read(vsl_b_istream& is, boxm_render_probe_manager* p);

//: Binary write  scene pointer to stream
void vsl_b_write(vsl_b_ostream& os, const boxm_render_probe_manager* &p);

#endif // boxm_render_probe_manager_h_
