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
#include <vgl/algo/vgl_rotation_3d.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/vecf/ocl/boxm2_vecf_ocl_vector_field.h>
//: Map a scene with Euclidean and anisotropic scale transforms.
// the input transform is the inverse so that the target scene voxels
// are mapped backwards to extract the data from the source
// thus each target voxel is defined if the inverse transform lands on a
// valid source voxel.

class boxm2_vecf_ocl_transform_scene;
typedef vbl_smart_ptr<boxm2_vecf_ocl_transform_scene> boxm2_vecf_ocl_transform_scene_sptr;

class boxm2_vecf_ocl_transform_scene : public vbl_ref_count
{
 public:
  //: Constructor.
  boxm2_vecf_ocl_transform_scene(const boxm2_scene_sptr& source_scene,
                                 const boxm2_scene_sptr& target_scene,
                                 const boxm2_opencl_cache_sptr& ocl_cache,
                                 std::string gray_app_id="",
                                 std::string color_app_id="",
                                 bool do_alpha = true,
                                 bool do_interp = true);

  //constructor if target scene is not known at the time of creation
  boxm2_vecf_ocl_transform_scene(const boxm2_scene_sptr& source_scene,
                                 const boxm2_opencl_cache_sptr& ocl_cache,
                                 std::string gray_app_id="",
                                 std::string color_app_id="",
                                 bool do_alpha = true,
                                 bool do_interp = true);

  ~boxm2_vecf_ocl_transform_scene() override;

  //: transform a scene of arbitray size, block by block
  // no interpolation
  bool transform(const vgl_rotation_3d<double>&  rot,
                 vgl_vector_3d<double> trans,
                 vgl_vector_3d<double> scale);

  //: transform a scene with one block so that
  //  both source and target blocks fit in GPU memory
  //  if "finish==true" then GPU state is cleared
  //  and kernel args are released after target
  //  is transformed. The target data is written to disk.
  //  no interpolation
  bool transform_1_blk(const vgl_rotation_3d<double>&  rot,
                       vgl_vector_3d<double> trans,
                       vgl_vector_3d<double> scale,
                       bool finish = true);
  //: transform a scene with one block and interpolate appearance and alpha
  // leave GPU buffers in place unless finish == true
  bool transform_1_blk_interp(const vgl_rotation_3d<double>&  rot,
                              vgl_vector_3d<double> trans,
                              vgl_vector_3d<double> scale,
                              bool finish = true);


  //: warps using similarty transform
  bool transform_1_blk_interp_trilin(const boxm2_scene_sptr& target_scene,
                                     const vgl_rotation_3d<double>&  rot,
                                     vgl_vector_3d<double> trans,
                                     vgl_vector_3d<double> scale,
                                     bool finish = true);

  //: warps using general vector field
  bool transform_1_blk_interp_trilin(boxm2_vecf_ocl_vector_field &vec_field, bool finish);
  boxm2_scene_sptr target_scene(){return target_scene_;}
  boxm2_scene_sptr source_scene(){return source_scene_;}
  boxm2_opencl_cache_sptr opencl_cache(){return opencl_cache_;}
  bool get_scene_appearance(const boxm2_scene_sptr& scene,
                            std::string&      options);

private:
  bool init_target_scene_buffers(boxm2_scene_sptr target_scene);


 protected:
  //bool compile_trans_kernel();
  bool compile_trans_interp_kernel();
  bool compile_trans_interp_trilin_kernel();
  bool compile_trans_interp_vecf_trilin_kernel();
  bool compile_compute_cell_centers_kernel();
  bool init_ocl_trans();

  boxm2_opencl_cache_sptr  opencl_cache_;
  boxm2_scene_sptr target_scene_;
  boxm2_scene_sptr source_scene_;
  bocl_device_sptr device_;
  int apptypesize_;//size of the appearance model in bytes
  boxm2_data_type app_type_; //type of appearance
  std::string color_app_type_id_; //type of appearance

  //transform kernels and args
  //bocl_kernel * trans_kern;
  bocl_kernel * trans_interp_kern;
  bocl_kernel * trans_interp_trilin_kern;
  bocl_kernel * trans_interp_vecf_trilin_kern;
  bocl_kernel * compute_cell_centers_kern;
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

  bool target_initialized_;
  boxm2_scene_info* info_buffer_;
  int octree_depth_buff_;
  float output_buff_[1000];
  cl_uchar lookup_arr_[256];
  bocl_mem_sptr output_;
  bocl_mem_sptr blk_info_target_;
  bocl_mem_sptr octree_depth_;
  bocl_mem_sptr lookup_;
  int data_size;
  float * long_output;
  bocl_mem_sptr output_f;
  std::string grey_app_id_,color_app_id_;
  bool do_alpha_,do_interp_;

  bocl_mem* alpha_target_;
  bocl_mem* mog_target_;
  bocl_mem* blk_target_;
  bocl_mem* nobs_target_;
  bocl_mem* rgb_target_;
  cl_command_queue queue_;}
  ;

#endif
