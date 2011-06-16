#ifndef boxm_update_bit_scene_manager_h_
#define boxm_update_bit_scene_manager_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_ref_count.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_utils.h>
#include <boxm/ocl/boxm_ocl_bit_scene.h>

#include <vil/vil_image_view.h>
#include <vpgl/vpgl_perspective_camera.h>

//PASS ENUM
enum {
  UPDATE_SEGLEN = 0,
  UPDATE_PREINF = 1,
  UPDATE_DIVIDE = 2,
  UPDATE_PROC   = 3,
  UPDATE_BAYES  = 4,
  UPDATE_CELL   = 5,
  RENDER_PASS   = 6,
  REFINE_PASS   = 7,
  QUERY_POINT   = 8,
  RAY_PROBE     = 9,
  CHANGE_DETECT = 10,
  CLEAN_AUX     = 11,
  MERGE_PASS    = 12,
  CHANGE_DETECT_OLD = 13
};

class boxm_update_bit_scene_manager : public bocl_manager<boxm_update_bit_scene_manager>, public vbl_ref_count
{
  typedef vnl_vector_fixed<int, 4> int4;
  typedef vnl_vector_fixed<float, 16> float16;
  typedef vnl_vector_fixed<int, 2> int2;

 public:
  typedef float obs_type;

  boxm_update_bit_scene_manager() :
    program_(0),
    render_kernel_(0),
    refine_kernel_(0),
    query_point_kernel_(0),
    ray_probe_kernel_(0),
    clean_aux_data_kernel_(0),
    raydepth_(1000),
    use_gl_(true),
    use_atomic_(true) {}
  ~boxm_update_bit_scene_manager() {
    if (program_)
      clReleaseProgram(program_);
  }

  //----------------------------------------------------------------------------
  // Setup/destruct methods
  //----------------------------------------------------------------------------
  //allocates/deallocates host and gpu side scene buffers
  //(calls set and clean scene buffers)
  bool init_scene(boxm_ocl_bit_scene * scene,
                  vpgl_camera_double_sptr cam,
                  vil_image_view<float> &obs,
                  float prob_thresh=0.3,
                  bool use_gl=true,
                  bool use_atomic=true);
  bool init_scene(boxm_ocl_bit_scene *scene,
                  unsigned ni,
                  unsigned nj,
                  float prob_thresh=0.3);

  bool init_scene_buffers(boxm_ocl_bit_scene *scene);

  bool uninit_scene();
  bool setup_online_processing();
  bool finish_online_processing();

  //----------------------------------------------------------------------------
  // Execute methods
  //----------------------------------------------------------------------------
  //update, render, refine publicly callable methods
  bool rendering();
  bool refine();
  bool merge();
  bool update();
  bool change_detection();
  bool change_detection_old();
  bool clean_aux_data();
  bool query_point(vgl_point_3d<float> p);
  bool ray_probe(unsigned i,unsigned j, float intensity);

  //: before calling render and update, make sure persp cam is set (maybe combine w/ above methods for simplicity)
  bool set_persp_camera(vpgl_proj_camera<double> * pcam);
  bool write_persp_camera_buffers();

  //: before calling update, make sure input image, image dimensions and camera are set
  bool set_input_image(vil_image_view<float>  obs);
  bool set_input_image(vil_image_view<float>  obs1,
                       vil_image_view<float>  obs2);

  bool write_image_buffer();

  //: set use gl (defaults to true)
  void set_use_gl(bool use) { use_gl_ = use; }

  //: set 2d workgroup size
  void set_bundle_ni(unsigned bundle_x) { bni_=bundle_x; }
  void set_bundle_nj(unsigned bundle_y) { bnj_=bundle_y; }

  //Enqueues a read command for output image and scene.  saves image/scene
  bool read_output_image();
  vil_image_view_base_sptr get_output_image(int plane_num=0);

  bool read_scene();
  void save_image();
  bool save_scene();

  //----------------------------------------------------------------------------
  // Get methods (unsure if when these are used)
  //----------------------------------------------------------------------------
  unsigned wni() const { return wni_; }
  unsigned wnj() const { return wnj_; }
  cl_float * output_image() { return image_; }

  //---------------------------------------------------------------------------
  // Appearance density setup - this can be factored out of public and done
  // during setup
  //---------------------------------------------------------------------------
  //set up appearence density
  bool setup_app_density(bool use_uniform=true, float mean = 0.0f, float sigma = 0.0f);
  int setup_app_density_buffer();
  bool clean_app_density();
  int clean_app_density_buffer();
  cl_mem& app_density() { return app_density_buf_; }
  bool setup_norm_data(bool use_uniform=true,float mean = 0.0f, float sigma = 0.0f);
  bool clean_norm_data();

  //set offset buffers (offset for update, making sure you don't run into the same
  //block twice
  bool set_offset_buffers(int off_x, int off_y, int factor);
  bool release_offset_buffers();

  //----------------------------------------------------------------------------
  //necessary CL items (program, command queue and kernels
  //three separate groups of kernels for 3 separate functions
  //----------------------------------------------------------------------------
  // for pass0 to compute seg len
  cl_program program_;
  cl_command_queue command_queue_;
  cl_kernel render_kernel_;
  cl_kernel refine_kernel_;
  cl_kernel merge_kernel_;
  cl_kernel query_point_kernel_;
  cl_kernel ray_probe_kernel_;
  cl_kernel change_detection_kernel_;
  cl_kernel change_detection_old_kernel_;
  cl_kernel clean_aux_data_kernel_;
  vcl_vector<cl_kernel> update_kernels_;

  //----------------------------------------------------------------------------
  // input image (can make this private too, have a setter)
  // MIGHT NEED A THIRD, one for writing out...
  //----------------------------------------------------------------------------
  cl_float * image_;              //input image
  cl_mem     image_buf_;          //input image buf
  cl_uint  * image_gl_;           //gl image (expected image?)
  cl_mem     image_gl_buf_;       //gl image buf
  bool read_output_debug();
  bool get_output_debug_array(vcl_vector<float> & data);

  bool set_rayoutput();
  bool set_rayoutput_buffers();
  bool release_rayoutput_buffers();
  bool clean_rayoutput();
  bool read_output_array();
  void getoutputarray(vcl_vector< vcl_vector<float> >& out);
  vpgl_camera_double_sptr get_camera() { return cam_; }
 protected:
  //----------------------------------------------------------------------------
  // PROTECTED helper methods (no need for these to be public, makes api simpler)
  //----------------------------------------------------------------------------

  //called by init/uninit: sets/cleans scene buffers
  bool set_scene_buffers();
  bool clean_scene_buffers();

  //set kernels/build programs (set kernels calls build programs)
  bool set_kernels();
  bool release_kernels();
  bool build_update_program(vcl_string const& functor, bool use_cell_data);
  bool build_rendering_program();
  bool build_refining_program();
  bool build_merging_program();
  bool build_query_point_program();
  bool build_ray_probe_program();
  bool build_change_detection_program();
  bool build_change_detection_old_program();
  bool build_clean_aux_data_program();
  //: executes specified kernel
  bool run(cl_kernel, unsigned pass);

  //: sets args for each kernel (should only be called once)
  bool set_render_args();
  bool set_refine_args();
  bool set_merge_args();
  bool set_update_args(unsigned pass);
  bool set_query_point_args();
  bool set_ray_probe_args(int i, int j, float intensity);
  bool set_change_detection_args();
  bool set_change_detection_old_args();
  bool set_clean_aux_data_args();
  //: set/release command_queue
  bool set_commandqueue();
  bool release_commandqueue();
  bool set_workspace(cl_kernel kernel, unsigned pass);

  //persp camera setup
  bool set_persp_camera();
  bool set_persp_camera_buffers();
  bool release_persp_camera_buffers();
  bool clean_persp_camera();

  //set_input_view
  bool set_input_image();
  bool set_input_image_buffers();
  bool set_image_dims_buffers();
  bool release_input_image_buffers();
  bool clean_input_image();

 /*****************************************
  * scene information
  *****************************************/
  RenderSceneInfo * scene_info_;      //world origin, dimensions, block len, buffer size all packed in here
  cl_ushort       * block_ptrs_;      //(int4) 3d array, points to (buff,offset) for each block
  cl_ushort       * mem_ptrs_;        //(int2) points to tree_cells_ free mem
  cl_ushort       * blocks_in_buffers_; //(ushort) number of blocks in each buffer
  cl_uchar        * cells_;           //(int2) packed tree cell data

  /* optimized data format in 4 buffers */
  cl_float        * cell_alpha_;      //array of floats  (prob density)
  cl_uchar        * cell_mixture_;    //array of uchar8s (mixutre model)
  //cl_uchar        * cell_weight_;     //last weight in the mixture model
  cl_ushort       * cell_num_obs_;    //array of short4s (num obs counts)
  //cl_int          * cell_lock_;
  //cl_uchar        * cell_mean_vis_;   //mean obs and visibility aux data
  //cl_float        * cell_cum_beta_;   //cum_length, beta aux data

  //integer auxiliary data...
  cl_int          * cell_seg_len_;
  cl_int          * cell_mean_obs_;
  cl_int          * cell_beta_;
  cl_int          * cell_vis_;

  /* other */
  cl_uchar        * bit_lookup_;
  cl_float          prob_thresh_;     //update prob thresh
  cl_float          merge_thresh_;
  cl_float        * output_debug_;    //output for debugging

 /*****************************************
  * update information
  *****************************************/
  cl_uint   * img_dims_;              //input image dimensions

  /* offset for non-overlapping sections */
  cl_uint     offset_x_;
  cl_uint     offset_y_;
  cl_uint     factor_;

  /* bounding box for each tree */
  cl_float * tree_bbox_;
  cl_float * point_3d_;

  cl_float* app_density_;
  cl_mem    app_density_buf_;

  // camera
  cl_float * persp_cam_;
  cl_float * persp_mat_;

  cl_uint bni_;
  cl_uint bnj_;

  cl_int scene_x_;
  cl_int scene_y_;
  cl_int scene_z_;

  // workspace dimensions which will be
  // greater than or equal to image dimensions
  cl_uint wni_;
  cl_uint wnj_;

 /*****************************************
  *pointers to cl memory on GPU
  *****************************************/
  /* scene buffers */
  cl_mem    scene_info_buf_;
  cl_mem    block_ptrs_buf_;
  cl_mem    mem_ptrs_buf_;
  cl_mem    blocks_in_buffers_buf_;
  cl_mem    cells_buf_;
  cl_mem    cell_alpha_buf_;
  cl_mem    cell_mixture_buf_;
  //cl_mem    cell_weight_buf_;
  cl_mem    cell_num_obs_buf_;
  //cl_mem    cell_cum_beta_buf_;
  //cl_mem    cell_mean_vis_buf_;
  //cl_mem    cell_lock_buf_;
  cl_mem    bit_lookup_buf_;

  //new integer aux data
  cl_mem    cell_seg_len_buf_;
  cl_mem    cell_mean_obs_buf_;
  cl_mem    cell_beta_buf_;
  cl_mem    cell_vis_buf_;

  /* update buffers */
  cl_mem    tree_bbox_buf_;
  cl_mem    persp_cam_buf_;
  cl_mem    persp_mat_buf_;
  cl_mem    img_dims_buf_;
  cl_mem    offset_x_buf_;
  cl_mem    offset_y_buf_;
  cl_mem    factor_buf_;

  cl_mem   point_3d_buf_;

  /*debugger ... */
  cl_mem    output_debug_buf_;
  cl_mem rayoutput_buf_[10]  ;
  float *  rayoutput_[10]  ;
  unsigned raydepth_;

 /*****************************************
  *helper member variables
  *****************************************/
  boxm_ocl_bit_scene* scene_;
  vpgl_camera_double_sptr cam_;
  vil_image_view<float>  input_img_;
  vil_image_view<float>  expected_img_;

  float gpu_time_;
  vcl_size_t globalThreads[2];
  vcl_size_t localThreads[2];
  bool use_gl_;
  bool use_atomic_;
};

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream & os, boxm_update_bit_scene_manager const& mgr);

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream & is, boxm_update_bit_scene_manager &mgr);

void vsl_b_read(vsl_b_istream& is, boxm_update_bit_scene_manager* p);

//: Binary write  scene pointer to stream
void vsl_b_write(vsl_b_ostream& os, const boxm_update_bit_scene_manager* &p);

#endif // boxm_update_bit_scene_manager_h_
