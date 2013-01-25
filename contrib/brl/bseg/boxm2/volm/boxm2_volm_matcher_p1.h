// This is brl/bseg/boxm2/volm/boxm2_volm_matcher_p1.h
#ifndef boxm2_volm_matcher_p1_h_
#define boxm2_volm_matcher_p1_h_
//:
// \file
// \brief  A class to match a voxelized query volume to an indexed reference volume
//         based on the depth value and relative order of voxels.  The index and hypotheses
//         are now referred by volm_geo_index
//
// \author Yi Dong
// \date January 21, 2012
// \verbatim
//   Modifications
// \endverbatim
//

#include <volm/volm_query_sptr.h>
#include <volm/volm_query.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include "boxm2_volm_wr3db_index_sptr.h"
#include "boxm2_volm_wr3db_index.h"
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>

class boxm2_volm_score_out;

class boxm2_volm_matcher_p1
{
public:
  //: default constructor
  boxm2_volm_matcher_p1() {}
  //: constructor
  boxm2_volm_matcher_p1(volm_query_sptr const& query,
                        vcl_vector<volm_geo_index_node_sptr> const& leaves,
                        vcl_vector<boxm2_volm_wr3db_index_sptr> const& ind_vec,
                        vcl_vector<float> const& depth_interval,
                        vgl_polygon<double> const& cand_poly,
                        bocl_device_sptr gpu,
                        bool const& is_candidate,
                        bool const& is_last_pass,
                        vcl_string const& out_folder,
                        float const& score_thres = 0.5,
                        float const& cam_thres = 0.95) :
  query_(query), leaves_(leaves), ind_vec_(ind_vec), depth_interval_(depth_interval),
  cand_poly_(cand_poly), gpu_(gpu), is_candidate_(is_candidate), is_last_pass_(is_last_pass),
  score_thres_(score_thres), cam_thres_(cam_thres), out_folder_(out_folder)
  { layer_size_ = query_->get_query_size(); }
  
  //: destructor
  ~boxm2_volm_matcher_p1();
  //: matcher function
  bool volm_matcher_p1();
  //: generate output -- probability map, binary score file, etc
  bool write_matcher_result();
    
private:
  //: query, indices, device
  volm_query_sptr                                   query_;
  vcl_vector<volm_geo_index_node_sptr>             leaves_;
  vcl_vector<boxm2_volm_wr3db_index_sptr>         ind_vec_;
  //: shell container size
  unsigned                                     layer_size_;
  unsigned*                               layer_size_buff_;
  bocl_mem*                             layer_size_cl_mem_;
  //: candidate list
  bool                                       is_candidate_;
  vgl_polygon<double>                           cand_poly_;
  //: score profile from last matcher
  bool                                       is_last_pass_;
  vcl_string                                   out_folder_;
  //: depth interval table
  vcl_vector<float>                        depth_interval_;
  //: kernel related
  bocl_device_sptr                                    gpu_;
  vcl_size_t                             local_threads_[2];
  vcl_size_t                            global_threads_[2];
  cl_uint                                        work_dim_;
  cl_command_queue                                  queue_;
  cl_ulong                               query_global_mem_;
  cl_ulong                                query_local_mem_;
  cl_ulong                              device_global_mem_;
  cl_ulong                               device_local_mem_;
  vcl_map<vcl_string, vcl_vector<bocl_kernel*> >  kernels_;

  //: query related
  unsigned*                       n_cam_;
  bocl_mem*                n_cam_cl_mem_;
  unsigned*                       n_obj_;
  bocl_mem*                n_obj_cl_mem_;

  unsigned*                 grd_id_buff_;
  bocl_mem*               grd_id_cl_mem_;
  unsigned char*          grd_dist_buff_;
  bocl_mem*             grd_dist_cl_mem_;
  unsigned*          grd_id_offset_buff_;
  bocl_mem*        grd_id_offset_cl_mem_;
  float*                grd_weight_buff_;
  bocl_mem*           grd_weight_cl_mem_;
  
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
  
  //: depth interval
  float*            depth_interval_buff_;
  bocl_mem*       depth_interval_cl_mem_;
  unsigned*           depth_length_buff_;
  bocl_mem*         depth_length_cl_mem_;
  
  //: indices related
  unsigned*                       n_ind_;
  
  //: output related
  // threshold value for current matcher to exclude indice from last matcher whosh score is lower than given threshold here
  float                               score_thres_;
  // threshold value for current matcher to exclude the camera whose score is lower than given threshold
  float                                 cam_thres_;
  vcl_vector<boxm2_volm_score_out>      score_all_;
  
  
  //: transfer volm_query to 1D array for kernel calculation
  bool transfer_query();
  //: read given number of indeices from volo_geo_index
  bool fill_index(unsigned const& n_ind,
                  unsigned const& layer_size,
                  unsigned& leaf_id,
                  unsigned char* index_buff,
                  vcl_vector<unsigned>& l_id,
                  vcl_vector<unsigned>& h_id,
                  unsigned& actual_n_ind);
  //: clare all query cl_mem pointer
  bool clean_query_cl_mem();
  //: compile kernel
  bool compile_kernel(vcl_vector<bocl_kernel*>& vec_kernels);
  //: create queue
  bool create_queue();
   //: kernel execution function
  bool execute_matcher_kernel(bocl_device_sptr             device,
                              cl_command_queue&             queue,
                              bocl_kernel*                   kern,
                              bocl_mem*             n_ind_cl_mem_,
                              bocl_mem*              index_cl_mem,
                              bocl_mem*             score_cl_mem_,
                              bocl_mem*                mu_cl_mem_);

  //: a test function to check the kernel implementation
  bool volm_matcher_p1_test(unsigned n_ind, unsigned char* index, float* score_buff, float* mu_buff);

};



// \brief  A class to store the highest score for each location
// leaf_id     ----> id of the leaf in leaves vector
// hypo_id     ----> local id of the hypothese in the leaf
// max_score_  ----> highest score for current location
// max_cam_id_ ----> the camera id associated with the highest score
// cam_id      ----> vector of camera ids whose score is higher than defined threshold

class boxm2_volm_score_out{
public:
  boxm2_volm_score_out () {}
  boxm2_volm_score_out(unsigned const& leaf_id, unsigned const& hypo_id, float const& max_score, unsigned const& max_cam_id, vcl_vector<unsigned> const& cam_id)
    : leaf_id_(leaf_id), hypo_id_(hypo_id), max_score_(max_score), max_cam_id_(max_cam_id), cam_id_(cam_id) {}
  ~boxm2_volm_score_out() {}
  unsigned leaf_id_;
  unsigned hypo_id_;
  float    max_score_;
  unsigned max_cam_id_;
  vcl_vector<unsigned> cam_id_;
};

#endif // boxm2_volm_matcher_p1_h_
