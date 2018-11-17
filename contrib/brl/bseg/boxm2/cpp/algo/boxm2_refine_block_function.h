#ifndef boxm2_refine_block_function_h
#define boxm2_refine_block_function_h
//:
// \file

#include <iostream>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>

class boxm2_refine_block_function
{
 public:
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  //: "default" constructor
  boxm2_refine_block_function() = default;

  //: initialize generic data base pointers as their data type
  bool init_data(const boxm2_scene_sptr& scene, boxm2_block* blk, std::vector<boxm2_data_base*> & datas, float prob_thresh);

  //: refine function;
  bool refine();
  bool refine_deterministic(std::vector<boxm2_data_base*>& datas);

  //: refine bit tree
  boct_bit_tree refine_bit_tree(boct_bit_tree& curr_tree,
                                 int buff_offset,
                                 bool is_random=true);

  //: move data into new location
  int move_data(boct_bit_tree& unrefined_tree,
                boct_bit_tree& refined_tree,
                float*  alpha_cpy,
                uchar8*  mog_cpy,
                ushort4* num_obs_cpy );

  //: helper method
  int free_space(int startPtr, int endPtr);

 private:
  boxm2_scene_sptr scene_;
  boxm2_block* blk_;

  float*       alpha_;
  uchar8*      mog_;
  ushort4*     num_obs_;

  //block max level
  int max_level_;
  int MAX_INNER_CELLS_;
  int MAX_CELLS_;

  //max alpha integrated
  float max_alpha_int_;

  //length of data buffer
  int data_len_;

  //length of one side of a sub block
  double block_len_;

  int num_split_;
};

////////////////////////////////////////////////////////////////////////////////
//MAIN REFINE FUNCTION
////////////////////////////////////////////////////////////////////////////////
void boxm2_refine_block( const boxm2_scene_sptr& scene,
                         boxm2_block* blk,
                         std::vector<boxm2_data_base*> & datas,
                         float prob_thresh,
                         bool is_random = true);

#endif
