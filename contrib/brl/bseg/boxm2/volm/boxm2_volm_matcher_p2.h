// This is brl/bseg/boxm2/volm/boxm2_volm_matcher_p2.h
#ifndef boxm2_volm_matcher_p2_h_
#define boxm2_volm_matcher_p2_h_
//:
// \file
// \brief  A class to match a voxelized query volume to an indexed reference volume
//         based on the orientation and land classification of voxels
//
// \author Yi Dong
// \date January 15, 2012
// \verbatim
//   Modifications
// \endverbatim
//

#include <volm/volm_query_sptr.h>
#include <volm/volm_query.h>
#include "boxm2_volm_wr3db_index_sptr.h"
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>
#include <volm/volm_io.h>


class boxm2_volm_matcher_p2
{
 public:
  //: default constructor
  boxm2_volm_matcher_p2() {}
  //: constructor
  boxm2_volm_matcher_p2(volm_query_sptr query,
                        boxm2_volm_wr3db_index_sptr ind,
                        unsigned long ei,
                        bocl_device_sptr gpu)
  : query_(query), ind_(ind), gpu_(gpu), ei_(ei) {}
  //: destructor
  ~boxm2_volm_matcher_p2();

  //: matcher function
  bool volm_matcher_p2();

 private:
  //: query, indices and devices
  volm_query_sptr                 query_;
  boxm2_volm_wr3db_index_sptr       ind_;
  bocl_device_sptr                  gpu_;

  //: kernel related
  vcl_size_t                    matcher_local_threads_[2];
  vcl_size_t                   matcher_global_threads_[2];
  cl_uint                               matcher_work_dim_;
  cl_ulong                              query_global_mem_;
  cl_ulong                               query_local_mem_;
  cl_ulong                              avail_global_mem_;
  cl_ulong                               avail_local_mem_;
  cl_command_queue                                 queue_;
  vcl_map<vcl_string, vcl_vector<bocl_kernel*> > kernels_;

  //: total number of indices
  unsigned long        ei_;
  //: total number of cameras, number of objects
  unsigned*         n_cam_;
  bocl_mem*  n_cam_cl_mem_;
  unsigned*         n_obj_;
  bocl_mem*  n_obj_cl_mem_;

  //: query buffer
  unsigned*           grd_id_buff_;
  bocl_mem*         grd_id_cl_mem_;
  unsigned*       grd_offset_buff_;
  bocl_mem*     grd_offset_cl_mem_;
  unsigned char*    grd_nlcd_buff_;
  bocl_mem*       grd_nlcd_cl_mem_;
  unsigned char*  grd_orient_buff_;
  bocl_mem*     grd_orient_cl_mem_;
  float*          grd_weight_buff_;
  bocl_mem*     grd_weight_cl_mem_;

  unsigned*           sky_id_buff_;
  bocl_mem*         sky_id_cl_mem_;
  unsigned*       sky_offset_buff_;
  bocl_mem*     sky_offset_cl_mem_;
  unsigned char*  sky_orient_buff_;
  bocl_mem*     sky_orient_cl_mem_;
  float*          sky_weight_buff_;
  bocl_mem*     sky_weight_cl_mem_;

  unsigned*           obj_id_buff_;
  bocl_mem*         obj_id_cl_mem_;
  unsigned*       obj_offset_buff_;
  bocl_mem*     obj_offset_cl_mem_;
  unsigned char*  obj_orient_buff_;
  bocl_mem*     obj_orient_cl_mem_;
  unsigned char*    obj_nlcd_buff_;
  bocl_mem*       obj_nlcd_cl_mem_;
  float*          obj_weight_buff_;
  bocl_mem*     obj_weight_cl_mem_;

  //: score output
  vcl_vector<volm_score> index_score_all;

  //: transfer volm_query to 1D array for kernel calculation
  bool transfer_query();
  //: read given numbers of indices from total ei_ indices
  bool fill_index();
  //: create opencl queue
  bool create_queue();
  //: compile kernel
  bool compile_kernel(vcl_vector<bocl_kernel*>& vec_kernels);
  //: kernel execution
  bool execute_matcher_kernel();
  //: clean all query cl_mem pointer
  bool clean_query_cl_mem();
};

#endif // boxm2_volm_matcher_p2_h_
