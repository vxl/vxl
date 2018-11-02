#ifndef bstm_refine_blk_in_space_function_h
#define bstm_refine_blk_in_space_function_h
//:


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

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
class bstm_refine_blk_in_space_function
{
 public:
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  //: "default" constructor
  bstm_refine_blk_in_space_function(bstm_time_block* t_blk, bstm_block* blk, std::vector<bstm_data_base*> & datas, float change_prob_t);


  //: initialize generic data base pointers as their data type
  bool init_data(bstm_time_block* t_blk, bstm_block* blk, std::vector<bstm_data_base*> & datas, float change_prob_t);

  bool refine(std::vector<bstm_data_base*>& datas);

 private:

  //: refine input tree and return refined tree
  boct_bit_tree refine_bit_tree(const boct_bit_tree& input_tree);

  //: moves time trees to their new locations after space cells are subdivded
  int move_time_trees(boct_bit_tree& unrefined_tree, boct_bit_tree& refined_tree, bstm_time_block* newTimeBlk, bstm_data_traits<BSTM_CHANGE>::datatype *  newC);

  //: moves data from old time trees to new tree
  void move_data(bstm_time_tree& old_time_tree, bstm_time_tree& time_tree, bstm_data_traits<BSTM_ALPHA>::datatype* alpha_cpy,
                  typename bstm_data_traits<APM_DATA_TYPE>::datatype * mog_cpy,
                  typename bstm_data_traits<NOBS_DATA_TYPE>::datatype * numobs_cpy);

  bstm_time_block* blk_t_;
  bstm_block* blk_;

  float*       alpha_;
  typename bstm_data_traits<APM_DATA_TYPE>::datatype*    mog_;
  typename bstm_data_traits<NOBS_DATA_TYPE>::datatype*     num_obs_;
  bstm_data_traits<BSTM_CHANGE>::datatype*     change_;

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

  float change_prob_t_;
};


#endif //bstm_refine_blk_in_space_function_h
