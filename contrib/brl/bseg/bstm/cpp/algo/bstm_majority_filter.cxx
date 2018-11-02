#include <iostream>
#include <algorithm>
#include "bstm_majority_filter.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


bstm_majority_filter::bstm_majority_filter(bstm_block_metadata data, bstm_block* blk, bstm_time_block*  /*blk_t*/, bstm_data_base* changes)
{

  boxm2_array_3d<uchar16>& trees = blk->trees();

  std::size_t data_size = changes->buffer_length();
  bstm_data_base* new_change = new bstm_data_base(new char[data_size], data_size, data.id_);
  auto*  new_change_data = (bstm_data_traits<BSTM_CHANGE>::datatype*) new_change->data_buffer();
  auto * change_data = (bstm_data_traits<BSTM_CHANGE>::datatype*) changes->data_buffer();

  //iterate through each tree
  for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
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
         int curr_depth = bit_tree.depth_at(currBitIndex);
         double side_len = 1.0 / double(1<<curr_depth);

         //get cell center, and get six neighbor points
          vgl_point_3d<double> localCenter = bit_tree.cell_center(currBitIndex);
          vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y() + y, localCenter.z() + z);
          std::vector<vgl_point_3d<double> > neighborPoints = this->neighbor_points(cellCenter, side_len, trees);

          //get each prob and sort it
          std::vector<float> probs;
          for (auto abCenter : neighborPoints)
          {
            //load neighbor block/tree
            vgl_point_3d<int>    blkIdx((int) abCenter.x(),
                                        (int) abCenter.y(),
                                        (int) abCenter.z() );
            uchar16 ntree = trees(blkIdx.x(), blkIdx.y(), blkIdx.z());
            boct_bit_tree neighborTree( (unsigned char*) ntree.data_block(), data.max_level_);

            //traverse to local center
            vgl_point_3d<double> locCenter((double) abCenter.x() - blkIdx.x(),
                                            (double) abCenter.y() - blkIdx.y(),
                                            (double) abCenter.z() - blkIdx.z() );
            int neighborBitIdx = neighborTree.traverse(locCenter, curr_depth);

            //if the cells are the same size, or the neighbor is larger
            if ( neighborTree.is_leaf(neighborBitIdx) ) {
              //get data index
              int idx = neighborTree.get_data_index(neighborBitIdx);
              probs.push_back(change_data[idx]);
            }
            else //neighbor is smaller, must combine neighborhood
            {
              //get cell, combine neighborhood to one probability

              float totalChange = 0.0f;
              float totalLen = 0.0f;

              std::vector<int> subLeafBits = neighborTree.get_leaf_bits(neighborBitIdx);
              std::vector<int>::iterator leafIter;
              for (leafIter = subLeafBits.begin(); leafIter != subLeafBits.end(); ++leafIter) {
                 //side length of the cell
                int ndepth = bit_tree.depth_at( *leafIter );
                double nlen = 1.0 / (double) (1<<ndepth);
                int dataIndex = neighborTree.get_data_index(*leafIter);

                totalChange += change_data[dataIndex] * nlen;
                totalLen += nlen;
              }
              float change = totalChange/ totalLen;

              probs.push_back(change);
            }
          }

          //if you've collected a nonzero amount of probs, update it
          probs.push_back(change_data[currIdx] );
          if (probs.size() > 0) {
            std::sort( probs.begin(), probs.end() );
            double median = probs[ (int) (3*probs.size()/4) ];
            new_change_data[currIdx] = float(median);
          }

       }

     }
    }
  }

  //replace the data
  bstm_cache_sptr cache = bstm_cache::instance();
  cache->replace_data_base(data.id_, bstm_data_traits<BSTM_CHANGE>::prefix(), new_change);
}



//: returns a list of 3d points of neighboring blocks
std::vector<vgl_point_3d<double> >
bstm_majority_filter::neighbor_points( vgl_point_3d<double>& cellCenter, double side_len, boxm2_array_3d<uchar16>& trees )
{
  std::vector<vgl_point_3d<double> > toReturn;

  for(int i = 1; i < 3; i++) {
    //neighbors along X
    if ( cellCenter.x() + i*side_len < trees.get_row1_count() )
      toReturn.emplace_back(cellCenter.x()+i*side_len, cellCenter.y(), cellCenter.z() );
    if ( cellCenter.x() - i*side_len >= 0 )
      toReturn.emplace_back(cellCenter.x()-i*side_len, cellCenter.y(), cellCenter.z() );

    //neighbors along Y
    if ( cellCenter.y() + i*side_len < trees.get_row2_count() )
      toReturn.emplace_back(cellCenter.x(), cellCenter.y()+i*side_len, cellCenter.z() );
    if ( cellCenter.y() - i*side_len >= 0 )
      toReturn.emplace_back(cellCenter.x(), cellCenter.y()-i*side_len, cellCenter.z() );

    //neighbors along Z
    if ( cellCenter.z() + i*side_len < trees.get_row3_count() )
      toReturn.emplace_back(cellCenter.x(), cellCenter.y(), cellCenter.z()+i*side_len );
    if ( cellCenter.z() - i*side_len >= 0 )
      toReturn.emplace_back(cellCenter.x(), cellCenter.y(), cellCenter.z()-i*side_len );
  }
  return toReturn;
}
