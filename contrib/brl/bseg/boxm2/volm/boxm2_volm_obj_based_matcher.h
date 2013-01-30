//This is brl/bseg/boxm2/volm/boxm2_volm_obj_based_matcher.h
#ifndef boxm2_volm_obj_based_matcher_h_
#define boxm2_volm_obj_based_matcher_h_
//:
// \file
// \brief  A class to match a voxelized query volume to an indexed reference volume based on objects defined in query
//
// \author Yi Dong
// \date December 20, 2012
// \verbatim
//   Modifications
// \endverbatim
//

#include <volm/volm_query_sptr.h>
#include <volm/volm_query.h>
#include "boxm2_volm_wr3db_index_sptr.h"
#include "boxm2_volm_wr3db_index.h"
#include "boxm2_volm_matcher_order.h"
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>


class boxm2_volm_obj_based_matcher
{
public:
  //: default constructor
  boxm2_volm_obj_based_matcher() {}
  //: constructor
  boxm2_volm_obj_based_matcher(volm_query_sptr query,
                               boxm2_volm_wr3db_index_sptr ind,
                               unsigned long ei,
                               bocl_device_sptr gpu) : query_(query), ind_(ind), ei_(ei), gpu_(gpu) {}
  
  //: destructor
  ~boxm2_volm_obj_based_matcher();
  
  //: object based matcher
  bool obj_based_matcher();
  bool obj_based_matcher_test();

private:
  //: members
  volm_query_sptr                 query_;
  boxm2_volm_wr3db_index_sptr       ind_;
  //: score output
  vcl_vector<float>            index_max_score;
  vcl_vector<unsigned>        index_max_camera;
  vcl_vector<boxm2_volm_score> index_score_all;
  //: total number of indices
  unsigned long                      ei_;
  //: total number of 
  unsigned*                       n_cam_;
  bocl_mem*                n_cam_cl_mem_;
  //: number of non_ground, non_sky objects
  unsigned*                       n_obj_;
  bocl_mem*                n_obj_cl_mem_;       

  //: device -- single GPU
  bocl_device_sptr gpu_;

  //: NDRangetable parameters
  vcl_size_t   matcher_local_threads_[2];
  vcl_size_t  matcher_global_threads_[2];
  cl_uint              matcher_work_dim_;
  
  //: required memory size (byte) for query 
  cl_ulong             query_global_mem_;
  cl_ulong              query_local_mem_;
  //: available device memory
  cl_ulong             avail_global_mem_;
  cl_ulong              avail_local_mem_;

  //: buffer objects and bocl_mem for query_
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
//unsigned char*  obj_max_dist_buff_;
//bocl_mem*          obj_max_cl_mem_;
  unsigned char*         obj_order_buff_;
  bocl_mem*            obj_order_cl_mem_;
  float*                obj_weight_buff_;
  bocl_mem*           obj_weight_cl_mem_;
  //: depth interval
  unsigned*         depth_interval_buff_;
  bocl_mem*       depth_interval_cl_mem_;

  //: kernels
  vcl_map<vcl_string, vcl_vector<bocl_kernel*> > kernels_;
  //: queues
  cl_command_queue queue_;
  //: transfer query into 1D arrays and construct associated cl_mem for OpenCL matcher
  bool transfer_query();
  //: read numbers of indices from overall ei_ indices
  bool fill_index(cl_ulong start, cl_ulong end, unsigned char* index_buff, unsigned* index_offset_buff);
  //: create queue for OpenCL matcher
  bool create_queue();
  //: compile the matcher kernel
  bool compile_kernel(vcl_vector<bocl_kernel*>& vec_kernels);
  //: kernel execution function
  bool execute_matcher_kernel(bocl_device_sptr             device,
                              cl_command_queue&             queue,
                              bocl_kernel*                   kern,
                              bocl_mem*             n_ind_cl_mem_,
                              bocl_mem*              index_cl_mem,
                              bocl_mem*       index_offset_cl_mem,
                              bocl_mem*                 mu_cl_mem,
                              bocl_mem*             score_cl_mem);
};

#endif  // boxm2_volm_obj_based_matcher_h_
