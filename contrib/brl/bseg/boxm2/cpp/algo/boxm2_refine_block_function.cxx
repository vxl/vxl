#include "boxm2_refine_block_function.h"
//:
// \file

#define copy_parent_data_ 1


//: initialize generic data base pointers as their data type
bool boxm2_refine_block_function::init_data(const boxm2_scene_sptr& scene, boxm2_block* blk, std::vector<boxm2_data_base*> & datas, float prob_thresh)
{
    //store block and pointer to uchar16 3d block
    scene_ = scene;
    blk_   = blk;

    //store data buffers
    int i=0;
    alpha_   = (float*)   datas[i++]->data_buffer();
    mog_     = (uchar8*)  datas[i++]->data_buffer();
    num_obs_ = (ushort4*) datas[i++]->data_buffer();

    //block max level
    max_level_ = blk_->max_level();

    //max alpha integrated
    max_alpha_int_ = -std::log(1.f - prob_thresh);

    //Data length now is constant
    data_len_ = 65536;

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

    std::cout<<"Refine Info: [blk "<<blk->block_id()
            <<"] [blk_len "<<block_len_
            <<"] [data_len "<<data_len_
            <<"] [max_alpha_int "<<max_alpha_int_
            <<"] [max level "<<max_level_
            <<']'<<std::endl;

    //for debugging
    num_split_ = 0;

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
bool boxm2_refine_block_function::refine_deterministic(std::vector<boxm2_data_base*>& datas)
{
  std::cout<<"CPU deterministic refine:"<<std::endl;

  //loop over each tree, refine it in place (keep a vector of locations for
  // posterities sake
  boxm2_array_3d<uchar16> trees = blk_->trees_copy();  //trees to refine
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

      //3. refine tree locally (only updates refined_tree and returns new tree size)
      boct_bit_tree refined_tree = this->refine_bit_tree(curr_tree, 0, false);  //i.e. is not random
      int newSize = refined_tree.num_cells();

      //cache refined tree
      std::memcpy (trees_copy[currIndex].data_block(), refined_tree.get_bits(), 16);
      dataSize += newSize;
  }


  //2. allocate new data arrays of the appropriate size
  std::cout<<"Allocating new data blocks"<<std::endl;
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

      //3. swap data from old location to new location
      newInitCount += this->move_data(old_tree, refined_tree, alpha_cpy, mog_cpy, num_obs_cpy);

      //4. store old tree in new tree, swap data out
      std::memcpy(blk_iter, refined_tree.get_bits(), 16);
  }
  blk_->set_trees(trees);
  std::cout<<"Number of new cells: "<<newInitCount<<std::endl;

  //3. Replace data in the cache
  boxm2_cache_sptr cache = boxm2_cache::instance();
  cache->replace_data_base(scene_, id, boxm2_data_traits<BOXM2_ALPHA>::prefix(), newA);
  cache->replace_data_base(scene_, id, boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(), newM);
  cache->replace_data_base(scene_, id, boxm2_data_traits<BOXM2_NUM_OBS>::prefix(), newN);

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
boct_bit_tree boxm2_refine_block_function::refine_bit_tree(boct_bit_tree& unrefined_tree,
                                                           int buff_offset,
                                                           bool is_random)
{
  //initialize tree to return
  boct_bit_tree refined_tree(unrefined_tree.get_bits(), max_level_);

  //no need to do depth first search, just iterate and check each node along the way
  //(iterate through the max number of inner cells)
  for (int i=0; i<MAX_INNER_CELLS_; ++i)
  {
    //if current bit is 0 and parent bit is 1, you're at a leaf
    int pi = (i-1)>>3;           //Bit_index of parent bit
    bool validParent = unrefined_tree.bit_at(pi) || (i==0); // special case for root
    if (validParent && unrefined_tree.bit_at(i)==0)
    {
      //////////////////////////////////////////////////
      //LEAF CODE HERE
      //////////////////////////////////////////////////
      //find side length for cell of this level = block_len/2^currDepth
      int currDepth = unrefined_tree.depth_at(i);
      double side_len = block_len_/ double(1<<currDepth);

      //get alpha value for this cell;
      int dataIndex = unrefined_tree.get_data_index(i, is_random);
      if (is_random) {
        dataIndex %= data_len_;             //gets offset within buffer
        dataIndex += buff_offset;
      }
      float alpha = alpha_[dataIndex];

      //integrate alpha value
      float alpha_int = alpha * float(side_len);

      //IF alpha value triggers split, tack on 8 children to end of tree array
      if (alpha_int > max_alpha_int_ && currDepth < max_level_-1)
      {
        //change value of bit_at(i) to 1;
        refined_tree.set_bit_at(i, true);

        //keep track of number of nodes that split
        ++num_split_;
      }
      ////////////////////////////////////////////
      //END LEAF SPECIFIC CODE
      ////////////////////////////////////////////
    }
  }
  return refined_tree;
}


//Deterministic move data
//moves data from src to destination
//returns the number of split nodes for this tree (for assertions)
int boxm2_refine_block_function::move_data(boct_bit_tree& unrefined_tree,
                                           boct_bit_tree& refined_tree,
                                           float*  alpha_cpy,
                                           uchar8*  mog_cpy,
                                           ushort4* num_obs_cpy )
{
  int newSize = refined_tree.num_cells();

  //zip through each leaf cell and
  int oldDataPtr = unrefined_tree.get_data_ptr(false);
  int newDataPtr = refined_tree.get_data_ptr(false);
  int newInitCount = 0;
  int cellsMoved = 0;
  for (int j=0; j<MAX_CELLS_ && cellsMoved<newSize; ++j)
  {
    //--------------------------------------------------------------------
    //4 Cases:
    // - Old cell and new cell exist - transfer data over
    // - new cell exists, old cell doesn't - create new occupancy based on depth
    // - old cell exists, new cell doesn't - uh oh this is bad news
    // - neither cell exists - do nothing and carry on
    //--------------------------------------------------------------------
    //if parent bit is 1, then you're a valid cell
    int pj = (j-1)>>3;           //Bit_index of parent bit
    bool validCellOld = (j==0) || unrefined_tree.bit_at(pj);
    bool validCellNew = (j==0) || refined_tree.bit_at(pj);
    if (validCellOld && validCellNew) {
      //move root data to new location
      alpha_cpy[newDataPtr]  = alpha_[oldDataPtr];
      mog_cpy[newDataPtr]    = mog_[oldDataPtr];
      num_obs_cpy[newDataPtr]= num_obs_[oldDataPtr];

      //increment
      ++oldDataPtr;
      ++newDataPtr;
      ++cellsMoved;
    }
    //case where it's a new leaf...
    else if (validCellNew) {
      //move root data to new location
      int parentLevel = unrefined_tree.depth_at(pj);
      double side_len = block_len_ / double(1<<parentLevel);
      int dataIndex = unrefined_tree.get_data_index(pj, false);
      alpha_cpy[newDataPtr]  = float(max_alpha_int_ / side_len); // (float(-std::log(1.0f - p_init_) / side_len));
#if copy_parent_data_
      mog_cpy[newDataPtr]    = mog_[dataIndex];
#else
      mog_cpy[newDataPtr]    = uchar8((uchar) 0);
#endif
      num_obs_cpy[newDataPtr]= ushort4((ushort) 0);

      //update new data pointer
      ++newDataPtr;
      ++newInitCount;
      ++cellsMoved;
    }
  }
  return newInitCount;
}


int boxm2_refine_block_function::free_space(int startPtr, int endPtr)
{
  int freeSpace = (startPtr >= endPtr)? startPtr-endPtr : data_len_ - (endPtr-startPtr);
  return freeSpace;
}


////////////////////////////////////////////////////////////////////////////////
//MAIN REFINE FUNCTION
////////////////////////////////////////////////////////////////////////////////
void boxm2_refine_block( const boxm2_scene_sptr& scene,
                         boxm2_block* blk,
                         std::vector<boxm2_data_base*> & datas,
                         float prob_thresh,
                         bool  /*is_random*/)
{
  boxm2_refine_block_function refine_block;
  refine_block.init_data(scene, blk, datas, prob_thresh);

  refine_block.refine_deterministic(datas);
}
