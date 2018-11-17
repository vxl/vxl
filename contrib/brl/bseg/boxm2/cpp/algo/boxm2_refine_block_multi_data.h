#ifndef boxm2_refine_block_multi_data_h
#define boxm2_refine_block_multi_data_h
//:
// \file
// \brief  Refine a block with a set of databases specified by prefix string - assumes the block is in the cache
// \author J.L. Mundy
// \date   26 November 2015
//
#include <iostream>
#include <boxm2/boxm2_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <vbl/vbl_array_3d.h>
class boxm2_refine_block_multi_data
{
 public:
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  //: "default" constructor
 boxm2_refine_block_multi_data():alpha_(nullptr),alpha_index_(0), old_bufs_(0){}

  //: initialize generic data base pointers as their data type
  bool init_data(boxm2_scene_sptr scene, const boxm2_block_sptr& blk, std::vector<std::string> const& prefixes, float prob_thresh);

  //: refine function (based on alpha);
  bool refine_deterministic(std::vector<std::string> const& prefixes);

  //: fully refine trees to match specified depths
  bool match_refine(std::vector<std::string> const& prefixes,vbl_array_3d<int> const& depths_to_match);

  //: refine bit tree
  boct_bit_tree refine_bit_tree(boct_bit_tree& curr_tree,
                                int buff_offset);

  //: move data into new location
  int move_data(boct_bit_tree& unrefined_tree,
                boct_bit_tree& refined_tree,
                std::vector<std::size_t> const& type_sizes,
                std::vector<char*>& new_bufs);
  //: helper method
  int free_space(int startPtr, int endPtr);

 private:
  uchar16 fully_refined(int depth, int& data_size);
  boxm2_scene_sptr scene_;
  boxm2_block_sptr blk_;
  float* alpha_;
  int alpha_index_;
  std::vector<char*> old_bufs_;
  std::vector<boxm2_data_base*> dbs_;
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
// REFINE FUNCTION BASED ON ALPHA
////////////////////////////////////////////////////////////////////////////////
void boxm2_refine_block_multi_data_function( const boxm2_scene_sptr& scene,
                                             const boxm2_block_sptr& blk,
                                             std::vector<std::string> const& prefixes,
                                             float prob_thresh);

////////////////////////////////////////////////////////////////////////////////
// REFINE FUNCTION TO MATCH SPECIFIED DEPTHS
////////////////////////////////////////////////////////////////////////////////
void boxm2_refine_block_multi_data_function( const boxm2_scene_sptr& scene,
                                             const boxm2_block_sptr& blk,
                                             std::vector<std::string> const& prefixes,
                                             vbl_array_3d<int> const& depths_to_match);
#endif
