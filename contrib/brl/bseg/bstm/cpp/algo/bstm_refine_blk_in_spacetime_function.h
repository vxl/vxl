#ifndef bstm_refine_blk_in_spacetime_function_h
#define bstm_refine_blk_in_spacetime_function_h
//:
// \file    This function refines the space-time data structures. A space cell is subdivided if it contains a cell (at any time) with prob. larger than provided threshold.
//          A time cell is subdivided if it contains such a cell.

#include <iostream>
#include <bstm/bstm_data_traits.h>
#include <bstm/bstm_time_tree.h>
#include <bstm/bstm_data_base.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/io/bstm_cache.h>

class bstm_refine_blk_in_spacetime_function
{
 public:
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  //: "default" constructor
  bstm_refine_blk_in_spacetime_function() = default;

  //: initialize generic data base pointers as their data type
  bool init_data(bstm_time_block* t_blk, bstm_block* blk, std::vector<bstm_data_base*> & datas, float prob_thresh);

  bool refine(std::vector<bstm_data_base*>& datas);

 private:

  //: refine input tree and return refined tree
  boct_bit_tree refine_bit_tree(const boct_bit_tree& input_tree);

  //: decides if the provided space cell should refine
  bool decide_refinement_in_space(int dataIndex, double side_len);

  //: moves time trees to their new locations after space cells are subdivded
  int move_time_trees(boct_bit_tree& unrefined_tree, boct_bit_tree& refined_tree,
                        bstm_time_block* newTimeBlk, bstm_time_block* newRefinedTimeBlk, char* depths);

  //: refines a time tree
  bstm_time_tree refine_time_tree(bstm_time_tree& unrefined_time_tree, double sidelen );

  //: moves data from old time trees to new tree
  void move_data(bstm_time_tree& unrefined_time_tree, bstm_time_tree& refined_time_tree, bstm_data_traits<BSTM_ALPHA>::datatype* alpha_cpy,
                                                                                           bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype * mog_cpy,
                                                                                           bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datatype * numobs_cpy,
                                                                                           int depth);

  bstm_time_block* blk_t_;
  bstm_block* blk_;

  //TODO: data types are hardcoded for now
  float*       alpha_;
  bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype*    mog_;
  bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datatype*     num_obs_;

  //block max level
  int max_level_t_;
  int max_level_;
  int MAX_INNER_CELLS_;
  int MAX_INNER_CELLS_T_;
  int MAX_CELLS_;
  int MAX_CELLS_T_;

  //length of one side of a sub block
  double block_len_;

  //number of time trees in blk
  unsigned sub_block_num_t_;


  int num_split_;
  int num_split_t_;

  float prob_t_;
};

////////////////////////////////////////////////////////////////////////////////
//MAIN REFINE FUNCTION
////////////////////////////////////////////////////////////////////////////////
void bstm_refine_block_spacetime( bstm_time_block* t_blk, bstm_block* blk,
                         std::vector<bstm_data_base*> & datas,
                         float prob_thresh);

#endif //bstm_refine_blk_in_spacetime_function_h
