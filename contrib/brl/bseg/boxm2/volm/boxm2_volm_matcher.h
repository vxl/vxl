//This is brl/bseg/boxm2/volm/boxm2_volm_matcher.h
#ifndef boxm2_volm_matcher_h_
#define boxm2_volm_matcher_h_
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

class boxm2_volm_matcher
{
 public:
  //: default constructor
  boxm2_volm_matcher(){}
  //: constructor
  boxm2_volm_matcher(volm_query_sptr query, boxm2_volm_wr3db_index_sptr ind, unsigned long ei, bocl_device_sptr gpu);
  //: destructor
  ~boxm2_volm_matcher();

  //: compile kernel for each device
  bool compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels);
  //: create queue, bocl_mem for queries
  bool setup_queue();
  //: execute_kernel for given chunk of indices
  bool execute_kernel(bocl_device_sptr device, cl_command_queue& queue, bocl_mem* query, bocl_mem* score, bocl_mem* index, bocl_kernel* kern);
  //: main process to streaming indices into kernel
  bool matching_cost_layer();
  //: Binary score to stream.
  bool write_score(vcl_string const& out_file);

 private:
  //: members
  volm_query_sptr query_;
  boxm2_volm_wr3db_index_sptr ind_;
  unsigned long ei_;
  vcl_vector<float> score_all_;
  vcl_vector<unsigned> cam_all_id_;
  //: ocl instance
  bocl_device_sptr gpu_;
  vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
  //: GPU work item and work group (1D case)
  unsigned int cl_ni;
  unsigned int cl_nj;
  cl_uint work_dim;
  vcl_size_t local_threads[2];
  vcl_size_t global_threads[2];

  //: buffer definition for cost kernel
  cl_command_queue queue;
  bocl_mem* query_cl;
  unsigned char* queries_buff;
};

#endif  // boxm2_volm_matcher_h_
