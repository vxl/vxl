#ifndef boxm2_vecf_ocl_store_nbrs_h
#define boxm2_vecf_ocl_store_nbrs_h
//:
// \file
// \brief A function to cache neigbor information in the scene
// \author J.L. Mundy
// \date October 18, 2014
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
//
// input a scene block and output an augmented block with neigborhood information
// current information is:
// 1) the existence of each of the six neighbors
//     (x0 +- side_len, y0 +- side_len, z0 +-side_len)                   (uchar8)
// 2) the expected intensity of each of the neighbors                    (uchar8)
// 3) the probability derived from alpha stored in each of the neighbors (float8)
// This information is written back into the scene model directory
// after the function completes
//
class boxm2_vecf_ocl_store_nbrs : public vbl_ref_count
{
 public:
  //: Constructor.
  boxm2_vecf_ocl_store_nbrs(boxm2_scene_sptr& source_scene,
                            const boxm2_opencl_cache_sptr& ocl_cache);

  ~boxm2_vecf_ocl_store_nbrs() override;

  unsigned ni() const{return  cl_ni;}
  unsigned nj() const{return  cl_nj;}

  //: augment a scene with one block with six-neighbor info
  bool augment_1_blk();

 protected:
  bool compile_kernel();
  bool init_ocl_store();
  bool get_scene_appearance(const boxm2_scene_sptr& scene,
                            std::string&      options);

  boxm2_opencl_cache_sptr  opencl_cache_;
  boxm2_scene_sptr source_scene_;
  bocl_device_sptr device_;
  int status;
  int apptypesize_;//size of the appearance model
  std::string app_type_;
  bocl_kernel * kern;
  bocl_mem_sptr centerX;
  bocl_mem_sptr centerY;
  bocl_mem_sptr centerZ;
  bocl_mem_sptr lookup;
  bocl_mem_sptr output;
  float output_buff[1000];
  bocl_mem* ocl_depth;
  bocl_mem* blk_info_source;
  boxm2_scene_info* info_buffer;
  boxm2_scene_info* info_buffer_source;
  bocl_mem* blk_source;
  bocl_mem* mog_source;
  bocl_mem* alpha_source;
  bocl_mem* nbr_exint;
  bocl_mem* nbr_prob;
  bocl_mem* nbr_exists;
  unsigned cl_ni;
  unsigned cl_nj;
  cl_command_queue queue;
};

#endif
