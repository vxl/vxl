// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_change_btw_frames_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for analyzing the temporal coherency of a region (bounding box) over a specified time interval.
//
// \author Ali Osman Ulusoy
// \date Mar 18, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/io/bstm_cache.h>
#include <bstm/io/bstm_lru_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_intersection.h>

#include <bstm/bstm_util.h>
#include <bstm/cpp/algo/bstm_analyze_coherency_function.h>

namespace bstm_cpp_change_btw_frames_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;

  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;


}

bool bstm_cpp_change_btw_frames_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_change_btw_frames_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "float"; //time 0
  input_types_[3] = "float"; //time 1

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_cpp_change_btw_frames_process(bprb_func_process& pro)
{
  using namespace bstm_cpp_change_btw_frames_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  auto time_0 = pro.get_input<float>(i++);
  auto time_1 = pro.get_input<float>(i++);



  //iterate over each block/metadata to check if bbox intersects the input bbox
  std::map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  std::map<bstm_block_id, bstm_block_metadata> ::const_iterator bstm_iter = blocks.begin();
  for(; bstm_iter != blocks.end() ; bstm_iter++)
  {
    bstm_block_id bstm_id = bstm_iter->first;
    bstm_block_metadata bstm_metadata = bstm_iter->second;

    double local_time_0, local_time_1;
    if(bstm_metadata.contains_t (time_0, local_time_0) && bstm_metadata.contains_t (time_1, local_time_1)) //if the block box contains the given times
    {
      std::cout << "Found intersecting bbox at block " << bstm_id << "..." << std::endl;

      bstm_block* blk = cache->get_block(bstm_metadata.id_);
      bstm_time_block* blk_t = cache->get_time_block(bstm_metadata.id_);
      bstm_data_base * alph = cache->get_data_base(bstm_metadata.id_, bstm_data_traits<BSTM_ALPHA>::prefix());
      auto * alpha_data = (bstm_data_traits<BSTM_ALPHA>::datatype*) alph->data_buffer();
      bstm_data_base *change_buffer = cache->get_data_base_new(bstm_metadata.id_,
                                                        bstm_data_traits<BSTM_CHANGE>::prefix(),
                                                        blk_t->tree_buff_length() * bstm_data_traits<BSTM_CHANGE>::datasize()  );
      auto* change_array = (bstm_data_traits<BSTM_CHANGE>::datatype*) change_buffer->data_buffer();

      //iterate through each tree
      boxm2_array_3d<uchar16>&  trees = blk->trees();
      for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
        for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
         for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
           //load current block/tree
           uchar16 tree = trees(x, y, z);
           boct_bit_tree bit_tree((unsigned char*) tree.data_block());

           //iterate through leaves of the tree
           std::vector<int> leafBits = bit_tree.get_leaf_bits();
           std::vector<int>::iterator iter;
           for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
             int curr_depth = bit_tree.depth_at((*iter));
             double side_len = 1.0 / (double) (1<<curr_depth);
             int data_offset = bit_tree.get_data_index(*iter);

             float prob_0, prob_1;
             {
               vnl_vector_fixed<unsigned char,8> time_treebits = blk_t->get_cell_tt(data_offset,local_time_0);
               bstm_time_tree time_tree(time_treebits.data_block());
               int bit_index_t = time_tree.traverse(local_time_0 - blk_t->tree_index(local_time_0) );
               int data_offset_t = time_tree.get_data_index(bit_index_t);
               prob_0 = 1.0f - (float)std::exp(-alpha_data[data_offset_t] * side_len * bstm_metadata.sub_block_dim_.x());
             }
             {
               vnl_vector_fixed<unsigned char,8> time_treebits = blk_t->get_cell_tt(data_offset,local_time_1);
               bstm_time_tree time_tree(time_treebits.data_block());
               int bit_index_t = time_tree.traverse(local_time_1 - blk_t->tree_index(local_time_1) );
               int data_offset_t = time_tree.get_data_index(bit_index_t);
               prob_1 = 1.0f - (float)std::exp(-alpha_data[data_offset_t] * side_len * bstm_metadata.sub_block_dim_.x());
             }
             change_array[data_offset] = std::fabs(prob_0 - prob_1);
           }

         }
        }
      }



    }
  }

  return true;

}
