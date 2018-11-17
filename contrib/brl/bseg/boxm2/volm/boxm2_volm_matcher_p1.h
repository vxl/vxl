// This is brl/bseg/boxm2/volm/boxm2_volm_matcher_p1.h
#ifndef boxm2_volm_matcher_p1_h_
#define boxm2_volm_matcher_p1_h_
//:
// \file
// \brief Match a voxelized query volume to an indexed reference volume.
//  A class to match a voxelized query volume to an indexed reference volume
//  based on the depth value and relative order of voxels.  The index and hypotheses
//  are now referred by volm_geo_index
//
// \author Yi Dong
// \date January 21, 2012
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <utility>
#include "boxm2_volm_wr3db_index_sptr.h"
#include "boxm2_volm_wr3db_index.h"
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_query_sptr.h>
#include <volm/volm_query.h>
#include <volm/volm_io.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>


class boxm2_volm_score_out;

class boxm2_volm_matcher_p1
{
 public:
  //: default constructor
  boxm2_volm_matcher_p1() = default;
  //: constructor
  boxm2_volm_matcher_p1(volm_camera_space_sptr const& cam_space,
                        volm_query_sptr const& query,
                        std::vector<volm_geo_index_node_sptr>  leaves,
                        float const& buffer_capacity,
                        std::string const& geo_index_folder,
                        unsigned const& tile_id,
                        std::vector<float>  depth_interval,
                        vgl_polygon<double> const& cand_poly,
                        const bocl_device_sptr& gpu,
                        bool const& is_candidate,
                        bool const& is_last_pass,
                        std::string  out_folder,
                        float const& threshold,
                        unsigned const& max_cam_per_loc,
                        std::vector<volm_weight> weights);

  //: destructor
  ~boxm2_volm_matcher_p1();
  //: matcher function
  bool volm_matcher_p1(int const& num_locs_to_kernel = -1);
  //: generate output -- probability map, binary score file, etc
  bool write_matcher_result(std::string const& tile_fname_bin, std::string const& tile_fname_txt);
  bool write_matcher_result(std::string const& tile_fname_bin);
  //: for testing purpose -- output score for all camera (should only be used for ground truth location)
  bool write_gt_cam_score(unsigned const& leaf_id, unsigned const& hypo_id, std::string const& out_fname);


 private:
  //: query, indices, device
  volm_camera_space_sptr                        cam_space_;
  std::vector<unsigned>                  valid_cam_indices_;
  volm_query_sptr                                   query_;
  std::vector<volm_geo_index_node_sptr>             leaves_;
  boxm2_volm_wr3db_index_sptr                         ind_;
  boxm2_volm_wr3db_index_sptr                  ind_orient_;
  boxm2_volm_wr3db_index_sptr                   ind_label_;
  float                                        ind_buffer_;
  std::stringstream                          file_name_pre_;
  std::vector<volm_weight>                         weights_;

  //: land fallback category table size
  unsigned char                             fallback_size_;
  unsigned char*                       fallback_size_buff_;
  bocl_mem*                          fallback_size_cl_mem_;
  //: shell container size
  unsigned                                     layer_size_;
  unsigned*                               layer_size_buff_;
  bocl_mem*                             layer_size_cl_mem_;
  //: candidate list
  bool                                       is_candidate_;
  vgl_polygon<double>                           cand_poly_;
  //: score profile from last matcher
  bool                                       is_last_pass_;
  std::string                                   out_folder_;
  //: depth interval table
  std::vector<float>                        depth_interval_;
  //: kernel related
  bocl_device_sptr                                    gpu_;
  std::size_t                             local_threads_[2];
  std::size_t                            global_threads_[2];
  cl_uint                                        work_dim_;
  cl_command_queue                                  queue_;
  cl_ulong                               query_global_mem_;
  cl_ulong                                query_local_mem_;
  cl_ulong                              device_global_mem_;
  cl_ulong                               device_local_mem_;
  std::map<std::string, std::vector<bocl_kernel*> >  kernels_;

  //: query related
  bool                       is_grd_reg_;
  bool                       is_sky_reg_;
  bool                       is_obj_reg_;
  unsigned*                       n_cam_;
  bocl_mem*                n_cam_cl_mem_;
  unsigned*                       n_obj_;
  bocl_mem*                n_obj_cl_mem_;

  unsigned*                 grd_id_buff_;
  bocl_mem*               grd_id_cl_mem_;
  unsigned char*          grd_dist_buff_;
  bocl_mem*             grd_dist_cl_mem_;
  unsigned char*          grd_land_buff_;
  bocl_mem*             grd_land_cl_mem_;
  float*              grd_land_wgt_buff_;
  bocl_mem*         grd_land_wgt_cl_mem_;
  unsigned*          grd_id_offset_buff_;
  bocl_mem*        grd_id_offset_cl_mem_;
  float*                grd_weight_buff_;
  bocl_mem*           grd_weight_cl_mem_;
  float*             grd_wgt_attri_buff_;
  bocl_mem*        grd_wgt_attri_cl_mem_;

  unsigned*                 sky_id_buff_;
  bocl_mem*               sky_id_cl_mem_;
  unsigned*          sky_id_offset_buff_;
  bocl_mem*        sky_id_offset_cl_mem_;
  float*                sky_weight_buff_;
  bocl_mem*           sky_weight_cl_mem_;

  unsigned*                 obj_id_buff_;
  bocl_mem*               obj_id_cl_mem_;
  unsigned*          obj_id_offset_buff_;
  bocl_mem*        obj_id_offset_cl_mem_;

  unsigned char*      obj_min_dist_buff_;
  bocl_mem*         obj_min_dist_cl_mem_;
  unsigned char*         obj_order_buff_;
  bocl_mem*            obj_order_cl_mem_;
  float*                obj_weight_buff_;
  bocl_mem*           obj_weight_cl_mem_;
  float*             obj_wgt_attri_buff_;
  bocl_mem*        obj_wgt_attri_cl_mem_;
  unsigned char*        obj_orient_buff_;
  bocl_mem*           obj_orient_cl_mem_;
  unsigned char*          obj_land_buff_;
  bocl_mem*             obj_land_cl_mem_;
  float*              obj_land_wgt_buff_;
  bocl_mem*         obj_land_wgt_cl_mem_;

  //: depth interval
  float*            depth_interval_buff_;
  bocl_mem*       depth_interval_cl_mem_;
  unsigned*           depth_length_buff_;
  bocl_mem*         depth_length_cl_mem_;

  //: indices related
  unsigned*                       n_ind_;

  //: output related
  // threshold that only the camera with score higher than threshold will be considered to put into output
  float                            threshold_;
  // maximum number of cameras for each location
  unsigned                   max_cam_per_loc_;
  std::vector<volm_score_sptr>      score_all_;
  std::vector<boxm2_volm_score_out> score_cam_;

  //: transfer volm_query to 1D array for kernel
  bool transfer_query();
  //: transfer volm_query orientation information to 1D array for kernel
  bool transfer_orient();
  //: transfer volm_weight parameters to 1D array for kernel
  bool transfer_weight();
  //: read given number of indices from volm_geo_index, with two index files, index depth and index orientation
  bool fill_index(unsigned const& n_ind,
                  unsigned const& layer_size,
                  unsigned& leaf_id,
                  unsigned char* index_buff,
                  unsigned char* index_orient_buff,
                  unsigned char* index_land_buff,
                  std::vector<unsigned>& l_id,
                  std::vector<unsigned>& h_id,
                  unsigned& actual_n_ind);
  //: check the given leaf has un-read hypothesis or not
  bool is_leaf_finish(unsigned const& leaf_id);
  //: clear all query cl_mem pointer
  bool clean_query_cl_mem();
  //: clear all weight cl_mem pointer
  bool clean_weight_cl_mem();
  //: compile kernel
  bool compile_kernel(std::vector<bocl_kernel*>& vec_kernels);
  //: create queue
  bool create_queue();
  //: check whether a given point is inside the candidate polygon
  bool inside_candidate(vgl_polygon<double> const& poly, double const& x, double const& y);

#if 0
  //: kernel execution function
  bool execute_matcher_kernel(bocl_device_sptr                         device,
                              cl_command_queue&                         queue,
                              std::vector<bocl_kernel*>                   kern,
                              bocl_mem*                         n_ind_cl_mem_,
                              bocl_mem*                         index_cl_mem_,
                              bocl_mem*                         score_cl_mem_,
                              bocl_mem*                            mu_cl_mem_);
#endif

  // kernel execution function with orientation
  bool execute_matcher_kernel_orient(const bocl_device_sptr&                  device,
                                     cl_command_queue&                  queue,
                                     std::vector<bocl_kernel*>        kern_vec,
                                     bocl_mem*                  n_ind_cl_mem_,
                                     bocl_mem*                  index_cl_mem_,
                                     bocl_mem*           index_orient_cl_mem_,
                                     bocl_mem*             index_land_cl_mem_,
                                     bocl_mem*                  score_cl_mem_,
                                     bocl_mem*                     mu_cl_mem_);

  //: a test function to check the kernel implementation
  bool volm_matcher_p1_test_ori(unsigned n_ind,
                                unsigned char* index,
                                const unsigned char* index_ori,
                                const unsigned char* index_lnd,
                                float* score_buff,
                                float* mu_buff);
};

class boxm2_volm_score_out
{
 public:
  boxm2_volm_score_out() = default;
  boxm2_volm_score_out(unsigned const& leaf_id, unsigned const& hypo_id,
                       std::vector<unsigned>  cam_id,
                       std::vector<float>  cam_score)
  : l_id_(leaf_id), h_id_(hypo_id), cam_id_(std::move(cam_id)), cam_score_(std::move(cam_score)) {}
  ~boxm2_volm_score_out() = default;

  unsigned l_id_;
  unsigned h_id_;
  std::vector<unsigned> cam_id_;
  std::vector<float> cam_score_;
};


#endif // boxm2_volm_matcher_p1_h_
