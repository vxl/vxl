#ifndef bstm_scene_info_h
#define  bstm_scene_info_h
//:
// \file
#include <bocl/bocl_cl.h>
#include <bstm/bstm_block_metadata.h>

//: block info that can be easily made into a buffer and sent to gpu
struct bstm_scene_info
{
  //world information
  cl_float    scene_origin[4];          // scene origin (4d point)
  cl_int      scene_dims[4];            // number of blocks in each dimension
  cl_float    block_len;                // size of each block (can only be 1 number now that we've established blocks are cubes)
  cl_float    time_block_len;           // dimension of the time axis
  cl_int      tree_buffer_length;       // length of tree buffer (number of cells/trees)
  cl_int      data_buffer_length;       // length of data buffer (number of cells)
};

//: return a heap pointer to a scene info
static bstm_scene_info* populate_scene_info(bstm_block_metadata data)
{
  bstm_scene_info* info = new bstm_scene_info();

  info->scene_origin[0] = (float) data.local_origin_.x();
  info->scene_origin[1] = (float) data.local_origin_.y();
  info->scene_origin[2] = (float) data.local_origin_.z();
  info->scene_origin[3] = (float) data.local_origin_t_;

  info->scene_dims[0] = (int) data.sub_block_num_.x();  // number of blocks in each dimension
  info->scene_dims[1] = (int) data.sub_block_num_.y();
  info->scene_dims[2] = (int) data.sub_block_num_.z();
  info->scene_dims[3] = (int) data.sub_block_num_t_;

  info->block_len = (float) data.sub_block_dim_.x();
  info->time_block_len = (float) data.sub_block_dim_t_;

  info->tree_buffer_length = 0;
  info->data_buffer_length = 0;

  return info;
};

class bstm_scene_info_wrapper : public vbl_ref_count
{
 public:
  bstm_scene_info * info;
};


//Smart_Pointer typedef for bstm_scene
typedef vbl_smart_ptr<bstm_scene_info_wrapper> bstm_scene_info_wrapper_sptr;


#endif //bstm_scene_info_h
