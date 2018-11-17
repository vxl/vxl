#include <iostream>
#include <list>
#include "boxm2_merge_block_function.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:
// \file

//: initialize generic data base pointers as their data type
bool boxm2_merge_block_function::init_data(boxm2_block* blk, std::vector<boxm2_data_base*>& datas, float prob_thresh)
{
    //store block and pointer to uchar16 3d block
    blk_   = blk;
    trees_ = blk_->trees().data_block();

    //store data buffers
    if (datas.size() < 3) {
      std::cout<<"boxm2_merge_block_function:: too few data buffers passed in"<<std::endl;
      return false;
    }
    int i=0;
    boxm2_data_base* alph = datas[i++];
    boxm2_data_base* mog = datas[i++];
    boxm2_data_base* nobs = datas[i++];

    //Data length now is constant
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    // check for invalid parameters
    if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
    {
      std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
      return false;
    }

    data_len_ = (int) (alph->buffer_length()/alphaTypeSize);
    alpha_   = (float*)   alph->data_buffer();
    mog_     = (uchar8*)  mog->data_buffer();
    num_obs_ = (ushort4*) nobs->data_buffer();

    //block max level
    max_level_ = blk_->max_level();

    //max alpha integrated
    //max_alpha_int_ = -std::log(1.f - prob_thresh);
    prob_thresh_ = prob_thresh;

    //length of one side of a sub block
    block_len_ = blk_->sub_block_dim().x();

    //USE rootlevel to determine MAX_INNER and MAX_CELLS
    if (max_level_ == 1) {
      std::cout<<"Trying to refine scene with max level 1"<<std::endl;
      return true;
    }
    else if (max_level_ == 2) {
      MAX_INNER_CELLS_=1, MAX_CELLS_=9;
    }
    else if (max_level_ == 3) {
      MAX_INNER_CELLS_=9, MAX_CELLS_=73;
    }
    else if (max_level_ == 4) {
      MAX_INNER_CELLS_=73, MAX_CELLS_=585;
    }
    std::cout<<"Merge Info: [blk "<<blk->block_id()
            <<"] [blk_len "<<block_len_
            <<"] [data_len "<<data_len_
            <<"] [prob_thresh "<<prob_thresh_
            <<"] [max level "<<max_level_
            <<']'<<std::endl;

    //for debugging
    merge_count_ = 0;

    return true;
}

//----- IF THE BLOCK IS NOT RANDOMLY DISTRIBUTED, USE DETERMINISTIC METHOD -----
// New Method Summary:
//  - NEED TO CLEAR OUT THE GPU CACHE BEFORE YOU START.. so you don't overwrite stuff accidentally...
//  - Create Block Copy, refine trees into that copy, maintaining old copy and array of new tree sizes
//  - Do scan on size vector (cum sum)
//  - Swap data into new buffers: For each data type
//    - get BOCL_MEM* data independent of cpu pointer (from cache)
//    - remove the BOCL_MEM* from the gpu cache (don't delete it)
//    - do a deep delete (delete CPU buffer from CPU cache)
//    - get a new data pointer (with newSize), will create CPU buffer and GPU buffer
//    - Run refine_data_kernel with the two buffers
//    - delete the old BOCL_MEM*, and that's it...
bool boxm2_merge_block_function::merge(std::vector<boxm2_data_base*>& datas)
{
  std::cout<<"CPU merge:"<<std::endl;

  //1. loop over each tree, refine it in place (keep a vector of locations for
  boxm2_array_3d<uchar16>  trees = blk_->trees_copy();  //trees to refine
  auto* trees_copy = new uchar16[trees.size()];  //copy of those trees
  int* dataIndex = new int[trees.size()];           //data index for each new tree
  int currIndex = 0;                                //curr tree being looked at
  int dataSize = 0;                                 //running sum of data size
  boxm2_array_3d<uchar16>::iterator blk_iter;
  for (blk_iter = trees.begin(); blk_iter != trees.end(); ++blk_iter, ++currIndex)
  {
      //0. store data index for eahc tree.
      dataIndex[currIndex] = dataSize;

      //1. get current tree information
      uchar16 tree  = (*blk_iter);
      boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), max_level_);

      //3. merge tree locally (only updates refined_tree and returns new tree size)
      boct_bit_tree refined_tree = this->merge_bit_tree(curr_tree, alpha_, prob_thresh_);
      int newSize = refined_tree.num_cells();

      //cache refined tree
      std::memcpy (trees_copy[currIndex].data_block(), refined_tree.get_bits(), 16);
      dataSize += newSize;
  }

  //2. allocate new data arrays of the appropriate size
  std::cout<<"Allocating new data blocks of length "<<dataSize<<std::endl;
  boxm2_block_id id = datas[0]->block_id();
  boxm2_data_base* newA = new boxm2_data_base(new char[dataSize * sizeof(float) ], dataSize * sizeof(float), id);
  boxm2_data_base* newM = new boxm2_data_base(new char[dataSize * sizeof(uchar8)], dataSize * sizeof(uchar8), id);
  boxm2_data_base* newN = new boxm2_data_base(new char[dataSize * sizeof(ushort4)], dataSize * sizeof(ushort4), id);
  auto*   alpha_cpy = (float*) newA->data_buffer();
  auto*  mog_cpy   = (uchar8*) newM->data_buffer();
  auto* num_obs_cpy = (ushort4*) newN->data_buffer();

  //3. loop through tree again, putting the data in the right place
  std::cout<<"Swapping data into new blocks..."<<std::endl;
  int newInitCount = 0;
  currIndex = 0;
  for (blk_iter = trees.begin(); blk_iter != trees.end(); ++blk_iter, ++currIndex)
  {
      //1. get current tree information
      uchar16 tree  = (*blk_iter);
      boct_bit_tree old_tree( (unsigned char*) tree.data_block(), max_level_);

      //2. refine tree locally (only updates refined_tree and returns new tree size)
      boct_bit_tree refined_tree( (unsigned char*) trees_copy[currIndex].data_block(), max_level_);

      //2.5 pack data bits into refined tree
      //store data index in bits [10, 11, 12, 13] ;
      int root_index = dataIndex[currIndex];
      refined_tree.set_data_ptr(root_index, false); //is not random

      int old_root_index = old_tree.get_data_ptr();

      //3. swap data from old location to new location, pass in shifted buffers
      newInitCount += this->move_data(old_tree,
                                      refined_tree,
                                      alpha_ + old_root_index,
                                      mog_ + old_root_index,
                                      num_obs_ + old_root_index,
                                      alpha_cpy+root_index,
                                      mog_cpy+root_index,
                                      num_obs_cpy+root_index);

      //4. store old tree in new tree, swap data out
      std::memcpy(blk_iter, refined_tree.get_bits(), 16);
  }
  blk_->set_trees(trees);
  std::cout<<"Number of merged cells: "<<merge_count_ << '\n'
          <<"  New Alpha Size: "<<newA->buffer_length() / 1024.0/1024.0<<" mb" << '\n'
          <<"  New MOG   Size: "<<newM->buffer_length() / 1024.0/1024.0<<" mb" << '\n'
          <<"  New NOBS  Size: "<<newN->buffer_length() / 1024.0/1024.0<<" mb" << std::endl;

  //3. Replace data in the cache
  boxm2_cache_sptr cache = boxm2_cache::instance();
  cache->replace_data_base(scene_, id, boxm2_data_traits<BOXM2_ALPHA>::prefix(), newA);
  cache->replace_data_base(scene_, id, boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(), newM);
  cache->replace_data_base(scene_, id, boxm2_data_traits<BOXM2_NUM_OBS>::prefix(), newN);

  delete [] dataIndex;
  return true;
}

/////////////////////////////////////////////////////////////////
////Refine Tree (refines local tree)
////Depth first search iteration of the tree (keeping track of node level)
////1) parent pointer, 2) child pointer 3) data pointer 4) nothing right now
// Kind of a weird mix of functions - the tree structure is modified locally,
// so no tree_buffer information is needed, whereas the data is modified
// on the global level, so buffers, offsets are used
/////////////////////////////////////////////////////////////////
boct_bit_tree boxm2_merge_block_function::merge_bit_tree(boct_bit_tree& unrefined_tree, const float* alphas, float prob_thresh)
{
  //initialize tree to return
  boct_bit_tree merged_tree(unrefined_tree.get_bits(), max_level_);

  //it can't be merged if it's a root
  if (merged_tree.bit_at(0) == 0)
    return merged_tree;

  //create float array to keep track of probs
  float probs[8];

  //push back first generation
  std::list<int> toVisit;
  for (int i=1; i<9; ++i)
    toVisit.push_back(i);

  //iterate through tree if there are children to get to
  int genCounter = 0;    //when this hits 8, a full generation should have been reached
  bool allLeaves = true; //true until a non-leaf gets hit
  while ( !toVisit.empty() )
  {
    //get front node off the top of the list, do an intersection for all 8 children
    int currBit = toVisit.front();
    toVisit.pop_front();

    //get alpha value for this cell;
    int dataIndex = unrefined_tree.get_data_index(currBit);
    float alpha   = alphas[dataIndex];

    //calculate the theoretical radius of this cell
    int curr_depth = unrefined_tree.depth_at(currBit);
    float side_len = (float)block_len_ / float(1<<curr_depth);
    probs[genCounter++] = 1.0f - (float)std::exp(-alpha * side_len);

    //track current generation's leaf status, and push back children if not
    if (! unrefined_tree.is_leaf(currBit)) {
      allLeaves = false;
      for (int i=0; i<8; ++i)
        toVisit.push_back( currBit*8+1+i );
    }

    //if we've finished up a set of siblings, check to see if we can merge em
    if (genCounter >= 8) {
      //calculate if all cells fall below threshold
      bool allBelow = true;
      for (float prob : probs) {
        allBelow = allBelow && (prob < prob_thresh);
      }

      //if all are leaves and all below, then reset the parent index to 0 (merge)
      if (allLeaves && allBelow)
      {
        int pi = (currBit-1)>>3; //Bit_index of parent bit
        merged_tree.set_bit_at(pi, false);
        merge_count_++;
      }
      //reset gen and allLeaves to default
      genCounter = 0;
      allLeaves = true;
    }
  } //end BFS while
  return merged_tree;
}


//Deterministic move data
//moves data from src to destination
//returns the number of split nodes for this tree (for assertions)
// Algo:
// dataIndex = 0
// For each bit
//   if merged_tree(bit) == 0 && unrefined_tree(bit)==1
//     mean = average(8 children)
//     data[dataIndex++] = mean
//   else if merged_tree(bit)==1 && unrefined_tree(bit)==1
//     for i=1:8
//       data[dataIndex++] = old_data[unrefined_tree.data_ptr(bit)];
int boxm2_merge_block_function::move_data( boct_bit_tree& old_tree,
                                           boct_bit_tree& merged_tree,
                                           const float*  alpha,
                                           uchar8* mog,
                                           ushort4* num_obs,
                                           float*  alpha_cpy,
                                           uchar8*  mog_cpy,
                                           ushort4* num_obs_cpy )
{
  //do a traversal over all the old_tree nodes,
  //compressing the refined bits

  //place to get data and place to put it in new buffer
  int oldDataIndex=0, newDataIndex=0;
  float max_alpha = -std::log(1.0f - init_prob_);

  //push back root
  std::list<int> toVisit;
  toVisit.push_back(0);
  while ( !toVisit.empty() )
  {
    //get front node off the top of the list, do an intersection for all 8 children
    int currBit = toVisit.front();
    toVisit.pop_front();

    //we're traversing merged and old at the same time, but don't branch on old tree
    if (old_tree.bit_at(currBit)==1) {
      for (int i=0; i<8; ++i)
        toVisit.push_back( currBit*8+1+i );
    }

    //first case: gone to leave that doesn't exist in merged, incrememnt old, not new
    if (old_tree.valid_cell(currBit) && !merged_tree.valid_cell(currBit))
    {
      oldDataIndex++;
    }
    //second case: found a merged cell in new tree, copy init vals
    else if (merged_tree.is_leaf(currBit) && !old_tree.is_leaf(currBit))
    {
      int currLevel = merged_tree.depth_at(currBit);
      float side_len = (float)block_len_ / float(1<<currLevel);
      float newAlpha = max_alpha / side_len;
      alpha_cpy[newDataIndex]  = newAlpha;
      mog_cpy[newDataIndex]    = uchar8((uchar) 0);
      num_obs_cpy[newDataIndex]= ushort4((ushort) 0);
      newDataIndex++;
      oldDataIndex++;
    }
    //last case: they are both valid and unchanged, just copy over old
    else
    {
      alpha_cpy[newDataIndex]   = alpha[oldDataIndex];
      mog_cpy[newDataIndex]     = mog[oldDataIndex];
      num_obs_cpy[newDataIndex] = num_obs[oldDataIndex];
      newDataIndex++;
      oldDataIndex++;
    }
  } //end BFS while
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//MAIN REFINE FUNCTION
////////////////////////////////////////////////////////////////////////////////
void boxm2_merge_block( const boxm2_scene_sptr& scene,
                        boxm2_block* blk,
                        std::vector<boxm2_data_base*> & datas,
                        float prob_thresh,
                        bool  /*is_random*/)
{
  boxm2_merge_block_function merge_block(scene);
  merge_block.init_data(blk, datas, prob_thresh);
  merge_block.merge(datas);
}
