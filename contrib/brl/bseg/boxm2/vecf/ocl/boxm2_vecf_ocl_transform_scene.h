#ifndef boxm2_vecf_ocl_transform_scene_h
#define boxm2_vecf_ocl_transform_scene_h
//:
// \file
// \brief A function to filter a block using the six neigborhood
// \author J.L. Mundy
// \date October 5, 2014
//
#include <vbl/vbl_ref_count.h>
#include <boxm2/io/boxm2_cache.h>
#include <bocl/bocl_mem.h>
#include <vgl/vgl_vector_3d.h>
#include <vil/vil_image_view.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/ocl/algo/boxm2_ocl_expected_image_renderer.h>
//: Map a scene with Euclidean and anisotropic scale transforms.
// the input transform is the inverse so that the target scene voxels
// are mapped backwards to extract the data from the source
// thus each target voxel is defined if the inverse transform lands on a
// valid source voxel.
class boxm2_vecf_ocl_transform_scene : public vbl_ref_count
{
 public:
  //: Constructor. 
  boxm2_vecf_ocl_transform_scene(boxm2_scene_sptr& source_scene,
                                 boxm2_scene_sptr& target_scene,
                                 boxm2_opencl_cache_sptr ocl_cache,
                                 unsigned ni, unsigned nj);

  ~boxm2_vecf_ocl_transform_scene();

  unsigned ni() const{return  cl_ni_;}
  unsigned nj() const{return  cl_nj_;}

  //: transform a scene of arbitray size, block by block
  // no interpolation
  bool transform(vgl_rotation_3d<double>  rot,
                 vgl_vector_3d<double> trans,
                 vgl_vector_3d<double> scale);

  //: transform a scene with one block so that
  //  both source and target blocks fit in GPU memory
  //  if "finish==true" then GPU state is cleared
  //  and kernel args are released after target
  //  is transformed. The target data is written to disk.
  //  no interpolation
  bool transform_1_blk(vgl_rotation_3d<double>  rot,
                       vgl_vector_3d<double> trans,
                       vgl_vector_3d<double> scale,
                       bool finish = true);
  //: transform a scene with one block and interpolate appearance and alpha
  // leave GPU buffers in place unless finish == true
  bool transform_1_blk_interp(vgl_rotation_3d<double>  rot,
                              vgl_vector_3d<double> trans,
                              vgl_vector_3d<double> scale,
                              bool finish = true);

  //:render the current state of the target scene leaving scene GPU buffers in place
  // thus rendering can be faster since block buffer transfers are not needed
  bool render_scene_appearance(vpgl_camera_double_sptr const & cam, vil_image_view<float>& expected_img,
                               vil_image_view<float>& vis_img);

  //:render the depth of the current state of the target scene leaving scene GPU buffers in place
  bool render_scene_depth(vpgl_camera_double_sptr const & cam, vil_image_view<float>& expected_depth,
                          vil_image_view<float>& vis_img);
 private:
  bool init_render_args();

  boxm2_ocl_expected_image_renderer renderer_;

 protected:
  //bool compile_trans_kernel();
  bool compile_trans_interp_kernel();
  bool compile_depth_kernel();
  bool compile_depth_norm_kernel();
  bool init_ocl_trans();
  bool get_scene_appearance(boxm2_scene_sptr scene,
                            vcl_string&      options);

  boxm2_opencl_cache_sptr  opencl_cache_;
  boxm2_scene_sptr target_scene_;
  boxm2_scene_sptr source_scene_;
  bocl_device_sptr device_;
  int apptypesize_;//size of the appearance model in bytes
  boxm2_data_type app_type_; //type of appearance

  //transform kernels and args 
  //bocl_kernel * trans_kern;
  bocl_kernel * trans_interp_kern;
  float* translation_buff;
  float* rotation_buff;
  float* scale_buff;
  bocl_mem* translation;
  bocl_mem* rotation;
  bocl_mem* scalem;
  bocl_mem* blk_info_source;
  boxm2_scene_info* info_buffer_source;
  bocl_mem* blk_source;
  bocl_mem* alpha_source;
  bocl_mem* mog_source;
  bocl_mem* nobs_source;
  bocl_mem* nbr_exint;   // neighborhood info
  bocl_mem* nbr_exists;  // to support
  bocl_mem* nbr_prob;    //interpolation

  bocl_mem_sptr centerX_;
  bocl_mem_sptr centerY_;
  bocl_mem_sptr centerZ_;

  // common stuff
  boxm2_scene_info* info_buffer_;
  int octree_depth_buff_;
  float output_buff_[1000];
  cl_uchar lookup_arr_[256];
  bocl_mem_sptr output_;
  bocl_mem_sptr blk_info_target_;
  bocl_mem_sptr octree_depth_;
  bocl_mem_sptr lookup_;

  bocl_mem* alpha_target_;
  bocl_mem* mog_target_;
  bocl_mem* blk_target_;

  // expected image kernels and args
  vcl_size_t lthreads_[2];
  float* img_buff_; // exp. img
  float* vis_buff_; // exp. img and depth
  float* max_omega_buff_; // for exp. img

  float* depth_buff_; // for depth
  float* var_buff_; // for depth
  float* prob_image_buff_; // for depth
  float* t_infinity_buff_; // for depth
  float subblk_dim_buff_;

  cl_float* ray_origins_;
  cl_float* ray_directions_;
  bocl_mem_sptr ray_o_buff_;
  bocl_mem_sptr ray_d_buff_;
  bocl_kernel * depth_kern_;
  bocl_kernel * depth_norm_kern_;

  // for exp image
  bocl_mem_sptr exp_image_;
  bocl_mem_sptr vis_image_; // used by both
  bocl_mem_sptr max_omega_image_;
  bocl_mem_sptr exp_img_dim_;

  // for depth
  bocl_mem_sptr depth_image_;
  bocl_mem_sptr var_image_;
  bocl_mem_sptr prob_image_;
  bocl_mem_sptr t_infinity_;
  bocl_mem_sptr subblk_dim_;

  const unsigned ni_;
  const unsigned nj_;
  unsigned cl_ni_;
  unsigned cl_nj_;
  float tnearfar_buff_[2];
  int img_dim_buff_[4];
  bocl_mem_sptr tnearfar_mem_ptr_;
  cl_command_queue queue_;
};

#endif


