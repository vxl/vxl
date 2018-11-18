// This is brl/bseg/bstm/pro/processes/bstm_cpp_extract_point_cloud_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for extracting a point cloud from a bstm scene. The points correspond to the cell centroids of leaf cells in the scene.
//         There is a probability threshold on cells to save for convenience. The points are saved in the BSTM_POINT datatype.
//
// \author Ali Osman Ulusoy
// \date June 25, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/bstm_scene.h>
#include <bstm/bstm_util.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/bstm_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>

namespace bstm_cpp_extract_point_cloud_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;

  typedef unsigned char uchar;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
}

bool bstm_cpp_extract_point_cloud_process_cons (bprb_func_process& pro)
{
  using namespace bstm_cpp_extract_point_cloud_process_globals;

  //process takes 3 inputs, no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "float"; //prob. threshold
  input_types_[3] = "float"; //time


  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(2, prob_t);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool bstm_cpp_extract_point_cloud_process (bprb_func_process& pro)
{
  using namespace bstm_cpp_extract_point_cloud_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene = pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache = pro.get_input<bstm_cache_sptr>(i++);
  auto prob_t = pro.get_input<float>(i++);
  auto time = pro.get_input<float>(i++);


  //zip through each block
  std::map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  std::map<bstm_block_id, bstm_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    bstm_block_id id = blk_iter->first;
    bstm_block_metadata data = blk_iter->second;
    double local_time;
    if(!data.contains_t(time,local_time))
      continue;
    int time_tree_index = std::floor(local_time);

    //get data from cache
    bstm_data_base * alpha = cache->get_data_base(id,bstm_data_traits<BSTM_ALPHA>::prefix());
    int num_time_trees = cache->get_time_block(id)->tree_buff_length();
    bstm_data_base * points= cache->get_data_base(id,bstm_data_traits<BSTM_POINT>::prefix(), num_time_trees * bstm_data_traits<BSTM_POINT>::datasize(), false);

    //3d array of trees
    boxm2_array_3d<uchar16>& trees = cache->get_block(id)->trees();
    auto * alpha_data = (bstm_data_traits<BSTM_ALPHA>::datatype*) alpha->data_buffer();
    auto * points_data = (bstm_data_traits<BSTM_POINT>::datatype*) points->data_buffer();

    for ( unsigned i = 0 ; i < num_time_trees; ++i)
    {
      points_data[i].fill(-1.0);
    }

    int num_points = 0;
    //iterate through each tree
    for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
      //std::cout << '[' << x << '/' << trees.get_row1_count() << ']' << std::flush;
      for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
       for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
         //load current block/tree
         uchar16 tree = trees(x, y, z);
         boct_bit_tree bit_tree((unsigned char*) tree.data_block(), data.max_level_);

         //iterate through leaves of the tree
         std::vector<int> leafBits = bit_tree.get_leaf_bits(0);
         std::vector<int>::iterator iter;
         for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
           int currBitIndex = (*iter);
           int currIdx = bit_tree.get_data_index(currBitIndex); //data index
           //time tree data ptr

           bstm_time_tree tt ( cache->get_time_block(id)->get_cell_tt(currIdx,local_time).data_block(), data.max_level_t_ );
           int data_ptr = tt.get_data_index(   tt. traverse(local_time - time_tree_index) );
           //compute probability
           int curr_depth = bit_tree.depth_at(currBitIndex);
           double side_len = 1.0 / (double) (1<<curr_depth);
           float prob = 1.0f - (float)std::exp(-alpha_data[data_ptr] * side_len * data.sub_block_dim_.x());

           //std::cout << prob << " " << curr_depth << " " << side_len << " " <<  local_time << " " << time_tree_index << " " << tt. traverse(local_time - time_tree_index) << std::endl;

           if (prob < prob_t)
           {
             points_data[currIdx][3] = -1.0f;
             continue;
           }
           vgl_point_3d<double> localCenter = bit_tree.cell_center(currBitIndex);
           vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y()+ y, localCenter.z() + z);
           points_data[currIdx][0] = float(cellCenter.x()*data.sub_block_dim_.x() + data.local_origin_.x());
           points_data[currIdx][1] = float(cellCenter.y()*data.sub_block_dim_.y() + data.local_origin_.y());
           points_data[currIdx][2] = float(cellCenter.z()*data.sub_block_dim_.z() + data.local_origin_.z());
           points_data[currIdx][3] = prob;
           num_points++;
         }

       }
      }
    }
  }
  std::cout << "Done extracting." << std::endl;
  return true;
}
