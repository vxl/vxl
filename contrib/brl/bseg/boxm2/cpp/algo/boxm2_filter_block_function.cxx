#include "boxm2_filter_block_function.h"


//:
// \file

//: "default" constructor
boxm2_filter_block_function::boxm2_filter_block_function(const boxm2_scene_sptr& scene, boxm2_block_metadata data, boxm2_block* blk, boxm2_data_base* alphas)
  : scene_(scene)
{
  //1. allocate new alpha data array (stays the same size)
  std::cout<<"Allocating new data blocks"<<std::endl;
  boxm2_block_id id = blk->block_id();
  std::size_t dataSize = alphas->buffer_length();
  boxm2_data_base* newA = new boxm2_data_base(new char[dataSize], dataSize, id);
  auto*   alpha_cpy = (float*) newA->data_buffer();

  //3d array of trees
  const boxm2_array_3d<uchar16>& trees = blk->trees();
  auto*   alpha_data = (float*) alphas->data_buffer();

  //iterate through each block, filtering the root level first
  std::cout<<"Filtering scene: "<<std::flush;
  for (unsigned int x=0; x<trees.get_row1_count(); ++x)
  {
    std::cout<<'['<<x<<'/'<<trees.get_row1_count()<<']'<<std::flush;
    for (unsigned int y=0; y<trees.get_row2_count(); ++y)
    {
      for (unsigned int z=0; z<trees.get_row3_count(); ++z)
      {
        //load current block/tree
        uchar16 tree = trees(x,y,z);
        boct_bit_tree bit_tree( (unsigned char*) tree.data_block(), data.max_level_);

        //FOR ALL LEAVES IN CURRENT TREE
        std::vector<int> leafBits = bit_tree.get_leaf_bits();
        std::vector<int>::iterator iter;
        for (iter = leafBits.begin(); iter != leafBits.end(); ++iter)
        {
          int currBitIndex = (*iter);

          //side length of the cell
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
#if 0 // unused
              int neighborDepth = neighborTree.depth_at(neighborBitIdx);
#endif
              //grab alpha, calculate probability
              float alpha = alpha_data[idx];
              float prob = 1.0f - (float)std::exp(-alpha * side_len * data.sub_block_dim_.x());
              probs.push_back(prob);
            }
            else //neighbor is smaller, must combine neighborhood
            {
              //get cell, combine neighborhood to one probability
#ifdef USE_AVGPROB
              float totalProb = 0.0f;
              float totalLen = 0.0f;
#else // USE_ALPHALEN
              float totalAlphaL = 0.0f;
#endif
              std::vector<int> subLeafBits = neighborTree.get_leaf_bits(neighborBitIdx);
              std::vector<int>::iterator leafIter;
              for (leafIter = subLeafBits.begin(); leafIter != subLeafBits.end(); ++leafIter) {
                 //side length of the cell
                int ndepth = bit_tree.depth_at( *leafIter );
                double nlen = 1.0 / (double) (1<<ndepth);
                int dataIndex = neighborTree.get_data_index(*leafIter);
#ifdef USE_AVGPROB
                totalProb += (1.0f - std::exp(-alpha_data[dataIndex] * nlen * data.sub_block_dim_.x()) );
                totalLen += nlen*data.sub_block_dim_.x();
#else
                totalAlphaL += (float)(alpha_data[dataIndex] * nlen * data.sub_block_dim_.x());
#endif
              }
#ifdef USE_AVGPROB
              float prob = totalProb / totalLen;
#else
              float prob = 1.0f - std::exp( -totalAlphaL );
#endif
              probs.push_back(prob);
            }
          }

          //if you've collected a nonzero amount of probs, update it
          int currIdx = bit_tree.get_data_index(currBitIndex);
          float prob = 1.0f - (float)std::exp( -alpha_data[currIdx] * side_len * data.sub_block_dim_.x() );
          probs.push_back(prob);
          if (probs.size() > 0) {
            std::sort( probs.begin(), probs.end() );
            double median = probs[ (int) (probs.size()/2) ];
            double medAlpha = - std::log(1.0-median) / ( side_len * data.sub_block_dim_.x() );

            //store the median value in the new alpha (copy)
            alpha_cpy[currIdx] = float(medAlpha);
          }
        } //end leaf for
      } //end z for
    } //end y for
  } // end x for

  //3. Replace data in the cache
  boxm2_cache_sptr cache = boxm2_cache::instance();
  cache->replace_data_base(scene_, id, boxm2_data_traits<BOXM2_ALPHA>::prefix(), newA);
}


//: returns a list of 3d points (int locations) of neighboring blocks
std::vector<vgl_point_3d<int> >
boxm2_filter_block_function::neighbors( vgl_point_3d<int>& center, boxm2_array_3d<uchar16>& trees )
{
  std::vector<vgl_point_3d<int> > toReturn;

  //neighbors along X
  if ( center.x() + 1 < (int)trees.get_row1_count() )
    toReturn.emplace_back(center.x()+1, center.y(), center.z() );
  if ( center.x() - 1 >= 0 )
    toReturn.emplace_back(center.x()-1, center.y(), center.z() );

  //neighbors along Y
  if ( center.y() + 1 < (int)trees.get_row2_count() )
    toReturn.emplace_back(center.x(), center.y()+1, center.z() );
  if ( center.y() - 1 >= 0 )
    toReturn.emplace_back(center.x(), center.y()-1, center.z() );

  //neighbors along Z
  if ( center.z() + 1 < (int)trees.get_row3_count() )
    toReturn.emplace_back(center.x(), center.y(), center.z()+1 );
  if ( center.z() - 1 >= 0 )
    toReturn.emplace_back(center.x(), center.y(), center.z()-1 );

  return toReturn;
}


//: returns a list of 3d points of neighboring blocks
std::vector<vgl_point_3d<double> >
boxm2_filter_block_function::neighbor_points( vgl_point_3d<double>& cellCenter, double side_len, const boxm2_array_3d<uchar16>& trees )
{
  std::vector<vgl_point_3d<double> > toReturn;

  //neighbors along X
  if ( cellCenter.x() + side_len < trees.get_row1_count() )
    toReturn.emplace_back(cellCenter.x()+side_len, cellCenter.y(), cellCenter.z() );
  if ( cellCenter.x() - side_len >= 0 )
    toReturn.emplace_back(cellCenter.x()-side_len, cellCenter.y(), cellCenter.z() );

  //neighbors along Y
  if ( cellCenter.y() + side_len < trees.get_row2_count() )
    toReturn.emplace_back(cellCenter.x(), cellCenter.y()+side_len, cellCenter.z() );
  if ( cellCenter.y() - side_len >= 0 )
    toReturn.emplace_back(cellCenter.x(), cellCenter.y()-side_len, cellCenter.z() );

  //neighbors along Z
  if ( cellCenter.z() + side_len < trees.get_row3_count() )
    toReturn.emplace_back(cellCenter.x(), cellCenter.y(), cellCenter.z()+side_len );
  if ( cellCenter.z() - side_len >= 0 )
    toReturn.emplace_back(cellCenter.x(), cellCenter.y(), cellCenter.z()-side_len );

  return toReturn;
}
