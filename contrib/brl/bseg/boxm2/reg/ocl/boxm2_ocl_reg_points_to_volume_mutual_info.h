//PUBLIC RELEASE APPROVAL FROM AFRL
//Case Number: RY-14-0126
//PA Approval Number: 88ABW-2014-1143
#ifndef boxm2_ocl_reg_points_to_volume_mutual_info_h
#define boxm2_ocl_reg_points_to_volume_mutual_info_h
//:
// \file
#include <boxm2/io/boxm2_cache.h>
#include <vgl/vgl_vector_3d.h>

#include <bocl/bocl_device.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vgl/algo/vgl_rotation_3d.h>

//: A cost function for registering video frames by minimizing square difference in intensities.
class boxm2_ocl_reg_points_to_volume_mutual_info
{
 public:
  //: Constructor. The source image is mapped to the destination frame by dt. nbins is the number of histogram bins used to compute entropies.
  boxm2_ocl_reg_points_to_volume_mutual_info(boxm2_opencl_cache_sptr& opencl_cache,
                                             float * pts,
                                             boxm2_scene_sptr& sceneB,
                                             const bocl_device_sptr& device,
                                             int npts,
                                             bool do_vary_scale);

  ~boxm2_ocl_reg_points_to_volume_mutual_info();

  //: Mutual information between the destination and mapped source image
  double cost(vgl_rotation_3d<double> rot, vgl_vector_3d<double> trans, double scale,  int depth  =3 );
  double cost(vnl_vector<double> const& x,  int depth =3 );
 protected:
  bool compile_kernel();
  bool boxm2_ocl_register_world(const vgl_rotation_3d<double>&  rot,
                                vgl_vector_3d<double> tx,
                                double s,
                                int depth,
                                float & mi);

  bool init_ocl_minfo();
  boxm2_opencl_cache_sptr  opencl_cache_;
  float * ptsA_;
  int nptsA_;
  boxm2_scene_sptr sceneB_;
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
  bocl_mem_sptr ptsA_ocl;
  bocl_mem_sptr nptsA_ocl;
  bocl_mem_sptr output;

  unsigned int * blk_offsets_array ;
  unsigned int * alpha_offsets_array;
  int bbox_buff[9];
  float sceneB_origin_buff[4];
  float block_dims[4];
  int subblk_num_buff[4];
  float output_buff[1000];
  cl_command_queue queue;
  bool do_vary_scale_ ;
};


#endif // boxm2_ocl_reg_mutual_info_h
