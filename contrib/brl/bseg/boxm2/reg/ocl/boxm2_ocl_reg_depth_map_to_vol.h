#ifndef boxm2_ocl_reg_depth_map_to_vol_h_
#define boxm2_ocl_reg_depth_map_to_vol_h_
//:
// \file
// \brief A cost function for registering volumes using mutual information
// \author Vishal Jain
// \date March 9, 2012
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
class boxm2_ocl_reg_depth_map_to_vol : public vnl_cost_function
{
 public:
  //: Constructor. The source image is mapped to the destination frame by dt. nbins is the number of histogram bins used to compute entropies.
  boxm2_ocl_reg_depth_map_to_vol( vpgl_camera_double_sptr & cam,
                                  vil_image_view<float> * img,
                                  boxm2_stream_scene_cache& cacheB,
                                  bocl_device_sptr device,
                                  int nbins);

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
  boxm2_stream_scene_cache & cache_;
  bocl_device_sptr device_;
  bocl_kernel * kern;
  bocl_mem_sptr centerX;
  bocl_mem_sptr centerY;
  bocl_mem_sptr centerZ;

  bocl_mem_sptr lookup;
  bocl_mem_sptr sceneB_origin;
  bocl_mem_sptr sceneB_bbox_ids;
  bocl_mem_sptr sceneB_block_dims;
  bocl_mem_sptr sceneB_sub_block_len;
  bocl_mem_sptr sceneB_sub_block_num;
  bocl_mem_sptr blks_ocl_B;
  bocl_mem_sptr alpha_ocl_B;
  //bocl_mem_sptr aux2_ocl_B;
  bocl_mem_sptr blks_ocl_B_offsets;
  bocl_mem_sptr alpha_ocl_B_offsets;
  bocl_mem_sptr output;

  vil_image_view<float> * Xdepth;
  vil_image_view<float> * Ydepth;
  vil_image_view<float> * Zdepth;
  vil_image_view<float> * depth_img_;
  unsigned int * blk_offsets_array ;
  unsigned int * alpha_offsets_array;
  //unsigned int * aux2_offsets_array;
  vpgl_camera_double_sptr cam_;
  int bbox_buff[9];
  float sceneB_origin_buff[4];
  float block_dims[4];
  int subblk_num_buff[4];
  float output_buff[1000];
};


#endif // boxm2_ocl_reg_depth_map_to_vol_h_
