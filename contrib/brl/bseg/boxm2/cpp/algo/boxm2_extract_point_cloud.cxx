#include <iostream>
#include <map>
#include "boxm2_extract_point_cloud.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <boxm2/boxm2_util.h>
#include <boct/boct_bit_tree.h>


bool boxm2_extract_point_cloud::extract_point_cloud(boxm2_scene_sptr scene, const boxm2_cache_sptr& cache,
                                                    float prob_thresh, unsigned int depth)
{
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  //zip through each block
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    boxm2_block_metadata data = blk_iter->second;

    //get data from cache
    boxm2_data_base * alpha = cache->get_data_base(scene, id,boxm2_data_traits<BOXM2_ALPHA>::prefix());

    std::size_t alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    std::size_t pointTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());

    if( alphaTypeSize == 0 ) //this should never happen, it results in division by 0 in later calculations
    {
      std::cout << "ERROR alphaTypeSize == 0 " << __FILE__ << __LINE__ << std::endl;
      return false;
    }

    boxm2_data_base * points= cache->get_data_base(scene, id,boxm2_data_traits<BOXM2_POINT>::prefix(), (alpha->buffer_length() /alphaTypeSize) * pointTypeSize, false);

    //3d array of trees
    const boxm2_array_3d<uchar16>& trees = cache->get_block(scene,id)->trees();

    auto * alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha->data_buffer();
    auto * points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();

    for ( unsigned i = 0 ; i < (alpha->buffer_length() /alphaTypeSize) ; ++i)
    {
      points_data[i].fill(-1.0);
    }
    //iterate through each tree
    for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
      //std::cout << '[' << x << '/' << trees.get_row1_count() << ']' << std::flush;
      for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
       for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
         //load current block/tree
         uchar16 tree = trees(x, y, z);
         boct_bit_tree bit_tree((unsigned char*) tree.data_block(), data.max_level_);

         //iterate through leaves of the tree
         std::vector<int> leafBits = bit_tree.get_leaf_bits(0,depth);
         std::vector<int>::iterator iter;
         for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
           int currBitIndex = (*iter);
           int currIdx = bit_tree.get_data_index(currBitIndex); //data index

           //compute probability
           int curr_depth = bit_tree.depth_at(currBitIndex);
           double side_len = 1.0 / (double) (1<<curr_depth);
           float prob = 1.0f - (float)std::exp(-alpha_data[currIdx] * side_len * data.sub_block_dim_.x());
           if (prob < prob_thresh)
           {
             points_data[currIdx][3] = -1.0f;
             continue;
           }
           vgl_point_3d<double> localCenter = bit_tree.cell_center(currBitIndex);
           vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y()+ y, localCenter.z() + z);
           points_data[currIdx][0] = float(cellCenter.x()*data.sub_block_dim_.x() + data.local_origin_.x());
           points_data[currIdx][1] = float(cellCenter.y()*data.sub_block_dim_.y() + data.local_origin_.y());
           points_data[currIdx][2] = float(cellCenter.z()*data.sub_block_dim_.z() + data.local_origin_.z());
           //points_data[currIdx][3] = 0.0f;
           // saving the prob here to avoid very costly prob calculation during exportPointCloudPLY, invalid points are marked with -1.0 in this field
           points_data[currIdx][3] = prob;
         }

       }
      }
    }
  }
  return true;
}
