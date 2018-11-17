#ifndef bstm_ocl_minfo_function_h_
#define bstm_ocl_minfo_function_h_


#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/bstm_util.h>
#include <bstm/ocl/bstm_ocl_util.h>

#include <vnl/vnl_matrix_fixed.h>

class bstm_ocl_minfo_function
{
 public:
  bstm_ocl_minfo_function(const bocl_device_sptr& device, const bstm_scene_sptr& scene, const bstm_opencl_cache_sptr& opencl_cache,
                              vgl_box_3d<double> bb, vgl_rotation_3d<double> rot, vgl_vector_3d<double> T,
                              float time1, float time2, std::string kernel_opt, int nbins );

  float evaluate();

  ~bstm_ocl_minfo_function() { delete[] app_joint_histogram_buff; delete[] surf_joint_histogram_buff; };

 private:

  void init_ocl_minfo();

  void populate_blk_mapping();

  bool compile_kernel();

  //members
  bocl_device_sptr device_;
  bstm_scene_sptr scene_;
  bstm_opencl_cache_sptr opencl_cache_;
  float time1_;
  float time2_;

  std::vector<bstm_block_id> relevant_blocks_;
  std::map<bstm_block_id, std::vector<bstm_block_id> > relevant_block_mapping_;

  bocl_mem_sptr centerX_;
  bocl_mem_sptr centerY_;
  bocl_mem_sptr centerZ_;
  bocl_mem_sptr target_blk_origin_;
  bocl_mem_sptr target_subblk_num_;
  bocl_mem_sptr scene_sub_block_len_;
  bocl_mem_sptr output_;
  bocl_mem_sptr lookup_;
  bocl_mem_sptr T_mem_;
  bocl_mem_sptr R_mem_;
  bocl_mem_sptr times_mem_;
  bocl_mem_sptr bb_mem_;
  bocl_mem_sptr app_joint_hist_mem_;
  bocl_mem_sptr surf_joint_hist_mem_;
  bocl_mem_sptr num_bins_mem_;

  cl_float translation_buff[4];
  cl_float rotation_buff[16];
  cl_float target_blk_origin_buff[4];
  cl_int target_subblk_num_buff[4];
  cl_float output_buff[1000];
  cl_float block_len_buff;
  cl_uchar lookup_arr[256];
  cl_float times[4];
  cl_float bb_buf_[8];
  cl_uint app_nbins_;
  cl_uint surf_nbins_;
  cl_float *app_joint_histogram_buff;
  cl_float *surf_joint_histogram_buff;
  cl_uint num_bins_buff_[4];
  cl_uint app_view_dir_num_;

  vgl_box_3d<double> bb_;
  vgl_rotation_3d<double> R_;
  vgl_vector_3d<double> T_;


  std::string kernel_opt_;
  bocl_kernel * kern_;
};


#endif //bstm_ocl_minfo_function_h_
