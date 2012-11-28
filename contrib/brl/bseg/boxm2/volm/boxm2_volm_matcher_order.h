//This is brl/bseg/boxm2/volm/boxm2_volm_matcher.h
#ifndef boxm2_volm_matcher_order_h_
#define boxm2_volm_matcher_order_h_
//:
// \file
// \brief  A class to match a voxelized query volume to an indexed reference volume
//
// \author Yi Dong
// \date October 07, 2012
// \verbatim
//   Modifications
//       created by Ozge C. Ozcanli - Oct 07, 2012
// \endverbatim
//

#include <volm/volm_query_sptr.h>
#include <volm/volm_query.h>
#include "boxm2_volm_wr3db_index_sptr.h"
#include "boxm2_volm_wr3db_index.h"
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>


class boxm2_volm_score;

class boxm2_volm_matcher_order
{
public:
  //: default constructor
  boxm2_volm_matcher_order(){}
  //: constructor
  boxm2_volm_matcher_order(volm_query_sptr query,
                           boxm2_volm_wr3db_index_sptr ind,
                           unsigned long ei, bocl_device_sptr gpu);
  //: destructor
  ~boxm2_volm_matcher_order();
  //: create 1D array for object order
  bool create_order_array();
  //: compile kernel for each device
  bool compile_kernel_order(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels);
  //: create queue, bocl_mem for queries
  bool setup_queue_order();
  //: execute kernel for the min_dist matching
  bool execute_match_kernel_min_dist_order(bocl_device_sptr device, 
                                           cl_command_queue& queue, 
                                           bocl_mem* query,
                                           bocl_mem* score,
                                           bocl_mem* index,
                                           bocl_kernel* kern);
  //: execute kernel for the order matching
  bool execute_match_kernel_order_order(bocl_device_sptr device,
                                        cl_command_queue& queue,
                                        bocl_mem* index,
                                        bocl_mem* order,
                                        bocl_mem* order_offset,
                                        bocl_mem* order_size,
                                        bocl_mem* score_order,
                                        bocl_kernel* kern);
  //: execute kernel of summerizing scores for each camera weighted by their order
  bool execute_weight_sum_kernel_order(bocl_device_sptr device,
                                       cl_command_queue& queue, 
                                       bocl_mem* score,
                                       bocl_mem* score_order,
                                       bocl_mem* weight_all,
                                       bocl_mem* voxel_size,
                                       bocl_mem* weight,
                                       bocl_mem* score_cam,
                                       bocl_kernel* kern);
  //: main process to streaming indices into kernel
  bool matching_cost_layer_order();
  //: Binary score to stream.
  //: write the top30 score among all location and all cameras
  bool write_score_order(vcl_string const& out_file);
  //: write score for all indices and all cameras, stored associated with the top field of view
  bool write_all_score(vcl_string const& out_prefix);


 private:
  //: members
  volm_query_sptr query_;
  boxm2_volm_wr3db_index_sptr ind_;
  unsigned long ei_;
  vcl_vector<float> score_all_;
  vcl_vector<unsigned> cam_all_id_;
  vcl_vector<boxm2_volm_score> index_score;
  //: ocl instance
  bocl_device_sptr gpu_;
  vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
  //: GPU work item and work group (1D case)
  unsigned int n_cam;
  unsigned int n_vox;
  unsigned int n_obj;
  unsigned int cl_ni;
  unsigned int cl_nj;
  cl_uint work_dim;
  
  vcl_size_t local_threads[2];
  //: work item size for min_dist kernel row -- camera; col -- voxels
  vcl_size_t global_threads[2];

  //: buffer definition for cost kernel
  cl_command_queue queue;
  
  bocl_mem* query_cl;
  unsigned char* queries_buff;
  
  bocl_mem* order_cl;
  unsigned* order_buff;

  bocl_mem* order_offset_cl;
  unsigned* order_offset_buff;

  bocl_mem* order_size_cl;
  unsigned* order_size_buff;

  bocl_mem* weight;
  float* weight_buff;

  bocl_mem* weight_all_cl;
  float* weight_all_buff;

  bocl_mem* voxel_size;
  unsigned* v_size_buff;
};


class boxm2_volm_score
{
public:
  boxm2_volm_score() {}
  boxm2_volm_score(unsigned const& index_id, unsigned const& camera_id, float const& current_score)
    : ind_id(index_id), cam_id(camera_id), score(current_score) {}
  ~boxm2_volm_score() {}
  unsigned ind_id; // also index in score.bin and cam.bin files
  unsigned cam_id;   // index for camera in volm_query
  float score;       // score for current index and current camera
};

#endif  // boxm2_volm_matcher_h_
