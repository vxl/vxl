// This is brl/bseg/bstm/pro/processes/bstm_scene_statistics_process.cxx
//:
// \file
// \brief  A process for printing out various statistics of a given scene.
//
// \author Ali Osman Ulusoy
// \date Nov 27, 2012

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <bstm/bstm_scene.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_block_metadata.h>

#include <boxm2/basic/boxm2_array_3d.h>
#include <boct/boct_bit_tree.h>

namespace bstm_scene_statistics_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}
bool bstm_scene_statistics_process_cons(bprb_func_process& pro)
{
  using namespace bstm_scene_statistics_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";

  return pro.set_input_types(input_types_) ;
}

bool bstm_scene_statistics_process(bprb_func_process& pro)
{
  using namespace bstm_scene_statistics_process_globals;

  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene = pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache = pro.get_input<bstm_cache_sptr>(i++);

  unsigned total_num_cells = 0;
  unsigned average_time_tree_depths;
  unsigned num_time_trees = 0;

  //get blocks
  vcl_map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  vcl_map<bstm_block_id, bstm_block_metadata>::const_iterator bstm_iter = blocks.begin();
  for(; bstm_iter != blocks.end(); bstm_iter++)
  {
    bstm_block_id bstm_id = bstm_iter->first;
    bstm_block_metadata bstm_metadata = bstm_iter->second;

    bstm_block* blk = cache->get_block(bstm_id);
    bstm_time_block* blk_t = cache->get_time_block(bstm_id);


    boxm2_array_3d<uchar16>&  trees = blk->trees();
    boxm2_array_3d<uchar16>::iterator blk_iter;
    for (blk_iter = trees.begin(); blk_iter != trees.end(); ++blk_iter)
    {
      uchar16 tree  = (*blk_iter);
      boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), bstm_metadata.max_level_);
      total_num_cells += curr_tree.num_cells();
    }

    boxm2_array_1d<uchar8>&  time_trees = blk_t->time_trees();
    boxm2_array_1d<uchar8>::iterator time_trees_iter;
    for (time_trees_iter = time_trees.begin(); time_trees_iter != time_trees.end(); ++time_trees_iter)
    {
        bstm_time_tree new_time_tree((unsigned char*) (*time_trees_iter).data_block(), bstm_metadata.max_level_t_);
        average_time_tree_depths += new_time_tree.num_cells();
        num_time_trees++;
    }
  }

  vcl_cout << "Total number of cells: " << total_num_cells << vcl_endl;
  vcl_cout << "Average time tree number of cells: " << ((float)average_time_tree_depths)/num_time_trees << vcl_endl;




  return true;
}
