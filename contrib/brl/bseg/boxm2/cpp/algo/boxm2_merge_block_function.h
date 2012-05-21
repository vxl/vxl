#ifndef boxm2_merge_block_function_h
#define boxm2_merge_block_function_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>
#include <boxm2/io/boxm2_cache.h>

class boxm2_merge_block_function
{
 public:
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  //: "default" constructor
  boxm2_merge_block_function() : merge_count_(0),
                                 prob_thresh_(.05f),
                                 init_prob_(.001f),
                                 block_len_(1.0) {}

  //: initialize generic data base pointers as their data type
  bool init_data(boxm2_block* blk, vcl_vector<boxm2_data_base*> & datas, float prob_thresh);

  //: refine function;
  bool merge(vcl_vector<boxm2_data_base*>& datas);

  //: refine bit tree
  boct_bit_tree merge_bit_tree(boct_bit_tree& curr_tree, float* alphas, float prob_thresh);

  //: move data into new location
  int move_data( boct_bit_tree& unrefined_tree,
                 boct_bit_tree& merged_tree,
                 float*   alpha,
                 uchar8*  mog,
                 ushort4* num_obs,
                 float*   alpha_cpy,
                 uchar8*  mog_cpy,
                 ushort4* num_obs_cpy);

 private:
  int          merge_count_;
  boxm2_block* blk_;
  uchar16*     trees_;

  float*       alpha_;
  uchar8*      mog_;
  ushort4*     num_obs_;

  //block max level
  int max_level_;
  int MAX_INNER_CELLS_;
  int MAX_CELLS_;

  //probability threshold
  float prob_thresh_;
  float init_prob_;

  //length of data buffer
  int data_len_;

  //length of one side of a sub block
  double block_len_;
};

////////////////////////////////////////////////////////////////////////////////
//MAIN REFINE FUNCTION
////////////////////////////////////////////////////////////////////////////////
void boxm2_merge_block( boxm2_block* blk,
                        vcl_vector<boxm2_data_base*> & datas,
                        float prob_thresh,
                        bool is_random = true);

#endif // boxm2_merge_block_function_h
