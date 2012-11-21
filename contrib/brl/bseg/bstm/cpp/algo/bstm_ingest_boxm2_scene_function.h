#ifndef bstm_ingest_boxm2_scene_function_h
#define bstm_ingest_boxm2_scene_function_h
//:
// \file

#include <bstm/bstm_time_block.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>
#include <bstm/io/bstm_cache.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <vcl_map.h>

#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>


#define SIMILARITY_T 0.2

class bstm_ingest_boxm2_scene_function
{
  public:

    typedef unsigned char uchar;
    typedef unsigned short ushort;
    typedef vnl_vector_fixed<uchar, 16> uchar16;
    typedef vnl_vector_fixed<uchar, 8> uchar8;
    typedef vnl_vector_fixed<ushort, 4> ushort4;

     //: "default" constructor
     bstm_ingest_boxm2_scene_function() {}

     //: initialize generic data base pointers as their data type
     bool init_data(bstm_block* blk,bstm_time_block* blk_t, vcl_vector<bstm_data_base*> & datas,
                       boxm2_block* boxm2_blk, vcl_vector<boxm2_data_base*> & boxm2_datas, double time);

     bool ingest();

     bool conform();

  private:

   boct_bit_tree conform_tree(boct_bit_tree curr_tree, boct_bit_tree boxm2_curr_tree);

   int move_time_trees(boct_bit_tree& unrefined_tree, boct_bit_tree& refined_tree, bstm_time_block* newTimeBlk,char* depth_diff );

   //: refines all the time trees of a given cell, based on the boxm2 data input.
   void refine_all_time_trees(int bstm_data_offset,int boxm2_data_offset, int* dataIndex, int& currIndex, int& dataSize,
                                 int currDepth, int currDepth_boxm2, bool is_leaf);

   //: moves the data of all time trees of a given cell. Copies data from parents and places the boxm2 data to current time cell.
   int move_all_time_trees_data( boxm2_array_1d<uchar8>& time_trees_blk_copy,
                                         int bstm_data_offset,int boxm2_data_offset, int* dataIndex, int& currIndex,
                                         float*  alpha_cpy, uchar8* mog_cpy, ushort4* num_obs_cpy, int depth_diff);

   int move_data(bstm_time_tree& unrefined_tree, bstm_time_tree& refined_tree, float*  alpha_cpy, uchar8* mog_cpy, ushort4* num_obs_cpy );

   void place_curr_data( bstm_time_tree& refined_tree, int boxm2_data_offset, float*  alpha_cpy, uchar8* mog_cpy, ushort4* num_obs_cpy, int depth_diff );

   //: refine input tree and return refined tree
   bstm_time_tree refine_time_tree(const bstm_time_tree& input_tree, int boxm2_data_offset, int currDepth, int currDepth_boxm2);

   //: function to define similarity measure between bstm data and current boxm2 data.
   bool is_similar(float p, uchar8 mog, float boxm2_p, uchar8 boxm2_mog);

   bstm_block* blk_;
   bstm_time_block* blk_t_;
   boxm2_block* boxm2_blk_;

   //bstm datas
   float*       alpha_;
   uchar8*      mog_;
   ushort4*     num_obs_;

   //boxm2 datas
   float*     boxm2_alpha_;
   uchar8*     boxm2_mog_;
   ushort4*    boxm2_num_obs_;

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
};



////////////////////////////////////////////////////////////////////////////////
//MAIN REFINE FUNCTION
////////////////////////////////////////////////////////////////////////////////
bool ingest_boxm2_blk(bstm_block* blk,bstm_time_block* blk_t, vcl_vector<bstm_data_base*> & datas,
    boxm2_block* boxm2_blk, vcl_vector<boxm2_data_base*> & boxm2_datas, double time);


#endif //bstm_ingest_boxm2_scene_function
