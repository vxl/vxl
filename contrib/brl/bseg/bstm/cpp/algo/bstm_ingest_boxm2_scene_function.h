#ifndef bstm_ingest_boxm2_scene_function_h_
#define bstm_ingest_boxm2_scene_function_h_
//:
// \file

#include <iostream>
#include <map>
#include <bstm/bstm_time_block.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/io/bstm_cache.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

#include <bstm/cpp/algo/bstm_data_similarity_traits.h>


template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
class bstm_ingest_boxm2_scene_function
{
 public:

   typedef unsigned char uchar;
   typedef unsigned short ushort;
   typedef vnl_vector_fixed<uchar, 16> uchar16;
   typedef vnl_vector_fixed<uchar, 8> uchar8;
   typedef vnl_vector_fixed<ushort, 4> ushort4;

   //: "default" constructor does all the work
   bstm_ingest_boxm2_scene_function(bstm_block* blk,bstm_time_block* blk_t, std::map<std::string, bstm_data_base*> & datas,
                                    boxm2_block* boxm2_blk, std::map<std::string, boxm2_data_base*> & boxm2_datas, double local_time, double p_threshold, double app_threshold);

 private:

   //: initialize generic data base pointers as their data type
   bool init_data(bstm_block* blk,bstm_time_block* blk_t,  std::map<std::string, bstm_data_base*> & datas,
                  boxm2_block* boxm2_blk, std::map<std::string, boxm2_data_base*> & boxm2_datas, double time);

   //main functions
   bool ingest();  //inserts the boxm2 data into bstm blk
   bool conform(); //make sure bstm blk conforms to the tree structure of boxm2 blk

   //helpers

   //: makes sure all the leaves of curr_tree is at least as deep as boxm2_curr_tree
   boct_bit_tree conform_tree(boct_bit_tree curr_tree, boct_bit_tree boxm2_curr_tree);

   //: moves the time trees of new refined bstm blk
   int move_time_trees(boct_bit_tree& unrefined_tree, boct_bit_tree& refined_tree, bstm_time_block* newTimeBlk,char* depth_diff );

   //: refines all the time trees of a given cell, based on the boxm2 data input.
   bool refine_all_time_trees(int bstm_data_offset,int boxm2_data_offset, int* dataIndex, int& currIndex, int& dataSize,
                              int currDepth, int currDepth_boxm2, bool is_leaf);

   //: moves the data of all time trees of a given cell. Copies data from parents and places the boxm2 data to current time cell.
   int move_all_time_trees_data( boxm2_array_1d<uchar8>& time_trees_blk_copy,
                                 int bstm_data_offset,int boxm2_data_offset, int* dataIndex, int& currIndex,
                                 bstm_data_traits<BSTM_ALPHA>::datatype*  alpha_cpy,
                                 typename bstm_data_traits<APM_TYPE>::datatype * apm_cpy, int depth_diff);

   //: moves the data from the old time tree to the new time tree
   int move_data(bstm_time_tree& unrefined_tree, bstm_time_tree& refined_tree, bstm_data_traits<BSTM_ALPHA>::datatype*  alpha_cpy, typename bstm_data_traits<APM_TYPE>::datatype * apm_cpy);

   //: inserts the current boxm2 data into the bstm scene
   void place_curr_data( bstm_time_tree& refined_tree, int boxm2_data_offset, bstm_data_traits<BSTM_ALPHA>::datatype*  alpha_cpy, typename bstm_data_traits<APM_TYPE>::datatype * apm_cpy, int depth_diff );

   //: refine input tree and return refined tree
   bstm_time_tree refine_time_tree(const bstm_time_tree& input_tree, int bstm_data_offset, int boxm2_data_offset, int currDepth, int currDepth_boxm2);

   //: function to define similarity measure between bstm data and current boxm2 data.
   bool is_similar(float p, typename bstm_data_traits<APM_TYPE>::datatype mog, float boxm2_p, typename boxm2_data_traits<BOXM2_APM_TYPE>::datatype boxm2_mog);

   bstm_block* blk_;
   bstm_time_block* blk_t_;
   boxm2_block* boxm2_blk_;

   //bstm datas
   bstm_data_traits<BSTM_ALPHA>::datatype *       alpha_;
   typename bstm_data_traits<APM_TYPE>::datatype* apm_model_;
   bstm_data_traits<BSTM_CHANGE>::datatype *   change_array_;

   //boxm2 datas
   boxm2_data_traits<BOXM2_ALPHA>::datatype *     boxm2_alpha_;
   typename boxm2_data_traits<BOXM2_APM_TYPE>::datatype*  boxm2_apm_model_;

   double local_time_;

   unsigned sub_block_num_t_;

   //block max level
   int max_level_t_;
   int max_level_;
   int MAX_INNER_CELLS_;
   int MAX_INNER_CELLS_T_;
   int MAX_CELLS_;
   int MAX_CELLS_T_;

   //length of one side of a sub block
   double block_len_;

   int num_split_;
   int num_split_t_;

   double p_threshold_;
   double app_threshold_;
};


#endif // bstm_ingest_boxm2_scene_function_h_
