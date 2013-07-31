// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_label_tt_depth_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process labeling a box in the bstm_scene.
//
// \author Ali Osman Ulusoy
// \date Jan 28, 2013

#include <vcl_fstream.h>
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
#include <boxm2/basic/boxm2_array_1d.h>

namespace bstm_cpp_label_tt_depth_process_globals
{
  const unsigned n_inputs_ =  2;
  const unsigned n_outputs_ = 0;
}

bool bstm_cpp_label_tt_depth_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_label_tt_depth_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_cpp_label_tt_depth_process(bprb_func_process& pro)
{
  using namespace bstm_cpp_label_tt_depth_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);

  //iterate over each block/metadata to check if bbox intersects the input bbox
  vcl_map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  vcl_map<bstm_block_id, bstm_block_metadata> ::const_iterator bstm_iter = blocks.begin();
  int max_depth = -1;
  int min_depth = 10;
  for(; bstm_iter != blocks.end() ; bstm_iter++)
  {
    bstm_block_id bstm_id = bstm_iter->first;
    bstm_block_metadata bstm_metadata = bstm_iter->second;
    bstm_block* blk = cache->get_block(bstm_metadata.id_);
    bstm_time_block* blk_t = cache->get_time_block(bstm_metadata.id_);
    bstm_data_base * alph    = cache->get_data_base(bstm_metadata.id_, bstm_data_traits<BSTM_ALPHA>::prefix());
    bstm_data_base * label_data_base = cache->get_data_base(bstm_metadata.id_, bstm_data_traits<BSTM_LABEL>::prefix(),
                                    alph->buffer_length() / bstm_data_traits<BSTM_ALPHA>::datasize() * bstm_data_traits<BSTM_LABEL>::datasize() );

    bstm_data_traits<BSTM_LABEL>::datatype * label_data = (bstm_data_traits<BSTM_LABEL>::datatype*) label_data_base->data_buffer();

    boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > &  time_trees = blk_t->time_trees();

    boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> >::iterator time_trees_iter;
    for (time_trees_iter = time_trees.begin(); time_trees_iter != time_trees.end(); ++time_trees_iter)
    {
      bstm_time_tree tt ((unsigned char*) (*time_trees_iter).data_block(), bstm_metadata.max_level_t_);
      vcl_vector<int> leaves = tt.get_leaf_bits();
      for(int i = 0; i < leaves.size(); i++) {
        label_data[tt.get_data_index( leaves[i] ) ] = (bstm_data_traits<BSTM_LABEL>::datatype) tt.depth_at( leaves[i] ) + 1;
        if( tt.depth_at( leaves[i] ) > max_depth)
          max_depth = tt.depth_at( leaves[i] ) ;
        if (tt.depth_at( leaves[i] ) < min_depth)
          min_depth = tt.depth_at( leaves[i] ) ;
      }
    }
  }
  vcl_cout << "Min depth = " << min_depth << " max: " << max_depth << vcl_endl;
  return true;

}
