#ifndef boxm2_ocl_reg_depth_map_to_vol2_h_
#define boxm2_ocl_reg_depth_map_to_vol2_h_
//:
// \file
// \brief A cost function for registering volumes using mutual information
// \author Ozge C. Ozcanli - Modified boxm2_ocl_reg_depth_map_to_vol to work with large scenes
//
// \date August 22, 2012
//
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cost_function.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera.h>
//: A cost function for registering video frames by minimizing square difference in intensities.
class boxm2_ocl_reg_depth_map_to_vol2 : public vnl_cost_function
{
 public:
  //: Constructor. The source image is mapped to the destination frame by dt. nbins is the number of histogram bins used to compute entropies.
  boxm2_ocl_reg_depth_map_to_vol2( vpgl_camera_double_sptr & cam,
                                   vil_image_view<float> * img,
                                   //vil_image_view<float> * skyimg,
                                   vcl_vector<boxm2_stream_scene_cache_sptr>& caches,
                                   vgl_box_3d<double>& global_bbox,
                                   bocl_device_sptr device,
                                   int nbins);

  ~boxm2_ocl_reg_depth_map_to_vol2();

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  virtual double f(vnl_vector<double> const& x);

  // === debug purposes ===

  //: error based on absolute difference
  double error(vgl_rotation_3d<double> rot, vgl_vector_3d<double> trans);

  //: Mutual information between the destination and mapped source image
  double mutual_info(vgl_rotation_3d<double> rot,vgl_vector_3d<double> trans);
 protected:
  bool compile_kernel();
  bool boxm2_ocl_register_world(vgl_rotation_3d<double>  rot,
                                vgl_vector_3d<double> tx,
                                int nbins,
                                float & mi);
  bool estimate_xyz();
  bool init_ocl_minfo();
  unsigned nbins_;
  vcl_vector<boxm2_stream_scene_cache_sptr> & caches_;
  vgl_box_3d<double> global_bbox_;
  bocl_device_sptr device_;
  bocl_kernel * kern;
  bocl_mem_sptr centerX;
  bocl_mem_sptr centerY;
  bocl_mem_sptr centerZ;

  bocl_mem_sptr lookup;

  vcl_vector<bocl_mem_sptr> sceneB_origin;
  //bocl_mem_sptr sceneB_origin;
  vcl_vector<bocl_mem_sptr> sceneB_bbox_ids;
  vcl_vector<bocl_mem_sptr> sceneB_block_dims;
  vcl_vector<bocl_mem_sptr> sceneB_sub_block_len;
  vcl_vector<bocl_mem_sptr> sceneB_sub_block_num;
  vcl_vector<bocl_mem_sptr> blks_ocl_B;
  vcl_vector<bocl_mem_sptr> alpha_ocl_B;
  vcl_vector<bocl_mem_sptr> blks_ocl_B_offsets;
  vcl_vector<bocl_mem_sptr> alpha_ocl_B_offsets;

  vcl_vector<unsigned int *> blk_offsets_array ;
  vcl_vector<unsigned int *> alpha_offsets_array;

  int bbox_buff[9];
  float sceneB_origin_buff[4];
  float block_dims[4];
  int subblk_num_buff[4];

  bocl_mem_sptr output;
  vpgl_camera_double_sptr cam_;

  float output_buff[1000];

  vcl_size_t local_threads[2];
  vcl_size_t global_threads[2];

  unsigned cl_ni;
  unsigned cl_nj;

  vil_image_view<float> * Xdepth;
  vil_image_view<float> * Ydepth;
  vil_image_view<float> * Zdepth;
  vil_image_view<float> * depth_img_;
  //vil_image_view<float> * sky_img_;
  bocl_mem_sptr x_image;
  bocl_mem_sptr y_image;
  bocl_mem_sptr z_image;
  bocl_mem_sptr sky_image;
  float* xdepth_buff;
  float* ydepth_buff;
  float* zdepth_buff;
  float* skyimg_buff;
  bocl_mem_sptr global_bbox_mem;
  float* global_bbox_buff;
};


#endif // boxm2_ocl_reg_depth_map_to_vol2_h_
