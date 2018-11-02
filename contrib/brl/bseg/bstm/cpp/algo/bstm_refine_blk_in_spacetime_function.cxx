#include <iostream>
#include <algorithm>
#include "bstm_refine_blk_in_spacetime_function.h"
#include <bstm/io/bstm_lru_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


bool bstm_refine_blk_in_spacetime_function::init_data(bstm_time_block* blk_t, bstm_block* blk, std::vector<bstm_data_base*> & datas, float prob_thresh)
{
  //store block and pointer to uchar16 3d block
   blk_   = blk;
   blk_t_ = blk_t;

   //store data buffers
   int i=0;
   alpha_   = (float*)   datas[i++]->data_buffer();
   mog_     = (bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype*)   datas[i++]->data_buffer();
   num_obs_     = (bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datatype*)   datas[i++]->data_buffer();

   //block max level
   max_level_t_ = blk_t->max_level();
   max_level_ = blk->max_level();

   sub_block_num_t_ = blk_t_->sub_block_num();


   //length of one side of a sub block
   block_len_ = blk->sub_block_dim().x();

   //USE rootlevel to determine MAX_INNER and MAX_CELLS
   if (max_level_t_ == 1) {
     std::cout<<"Trying to refine scene with max level 1"<<std::endl;
     return true;
   }
   else if (max_level_t_ == 2) {
     MAX_INNER_CELLS_T_=1, MAX_CELLS_T_=3;
   }
   else if (max_level_t_ == 3) {
     MAX_INNER_CELLS_T_=3, MAX_CELLS_T_=7;
   }
   else if (max_level_t_ == 4) {
     MAX_INNER_CELLS_T_=7, MAX_CELLS_T_=15;
   }
   else if (max_level_t_ == 5) {
     MAX_INNER_CELLS_T_=15, MAX_CELLS_T_=31;
   }
   else if (max_level_t_ == 6) {
     MAX_INNER_CELLS_T_=31, MAX_CELLS_T_=63;
   }

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

   //for debugging
   num_split_ = 0;
   num_split_t_ = 0;

   prob_t_ = prob_thresh;

   return true;
}

bool bstm_refine_blk_in_spacetime_function::refine(std::vector<bstm_data_base*>&  /*datas*/)
{

  //1. loop over each tree, refine it in place
  boxm2_array_3d<uchar16>&  trees = blk_->trees();  //trees to refine
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
      boct_bit_tree refined_tree = this->refine_bit_tree(curr_tree);
      int newSize = refined_tree.num_cells();

      //save refined tree to trees_copy
      std::memcpy (trees_copy[currIndex].data_block(), refined_tree.get_bits(), sizeof(uchar16));
      dataSize += newSize;
  }
  std::cout << "Num space cells split: " << num_split_ << std::endl;




  //2. allocate new time blk of the appropriate size
  bstm_block_id id = blk_->block_id();
  bstm_block_metadata m_data; m_data.init_level_t_ = blk_t_->init_level(); m_data.max_level_t_ = blk_t_->max_level(); m_data.sub_block_num_t_ = blk_t_->sub_block_num();
  bstm_time_block* newTimeBlk = new bstm_time_block(id, m_data, dataSize); //create empty time block
  bstm_time_block* newRefinedTimeBlk = new bstm_time_block(id, m_data, dataSize); //create empty time block
  char* depths = new char[dataSize];


  //3. loop through trees again, putting the time trees in the right place as well as refining the time trees
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
      newInitCount += this->move_time_trees(old_tree, refined_tree, newTimeBlk, newRefinedTimeBlk, depths);

      //4. store old tree in new tree, swap data out
      std::memcpy(blk_iter, refined_tree.get_bits(), sizeof(uchar16));
  }

  std::cout << "Num time cells split: " << num_split_t_ << std::endl;


  delete[] trees_copy;
  delete[] dataIndex;



  //4. figure out new data size
  boxm2_array_1d<uchar8>&  new_refined_time_trees = newRefinedTimeBlk->time_trees();    //refined new time trees
  boxm2_array_1d<uchar8>&  new_unrefined_time_trees = newTimeBlk->time_trees();         //unrefined new time treesunrefined_time_tree
  boxm2_array_1d<uchar8>::iterator refined_time_trees_iter, unrefined_time_trees_iter;
  dataIndex = new int[new_refined_time_trees.size()];                  //data index for each new tree
  currIndex = 0;                                                        //curr tree being looked at
  dataSize = 0;
  for (refined_time_trees_iter = new_refined_time_trees.begin(); refined_time_trees_iter != new_refined_time_trees.end(); ++refined_time_trees_iter, ++currIndex)
  {
      //0. store data index for each tree.
      dataIndex[currIndex] = dataSize;
      //1. get refined tree
      bstm_time_tree new_time_tree((unsigned char*) (*refined_time_trees_iter).data_block(), max_level_t_);
      int newSize = new_time_tree.num_leaves(); //number of leaves, not all cells.
      dataSize += newSize;
  }
  std::cout << "New data size: " << dataSize << std::endl;

  //alloc new buffers
  bstm_data_base* newA = new bstm_data_base(new char[dataSize * bstm_data_traits<BSTM_ALPHA>::datasize() ],
                                                      dataSize * bstm_data_traits<BSTM_ALPHA>::datasize(), id);
  bstm_data_base* newM = new bstm_data_base(new char[dataSize * bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datasize() ],
                                                      dataSize * bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datasize() , id);
  bstm_data_base* newN = new bstm_data_base(new char[dataSize * bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datasize() ],
                                                      dataSize * bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datasize(), id);
  auto *   alpha_cpy = (bstm_data_traits<BSTM_ALPHA>::datatype *) newA->data_buffer();
  auto *  mog_cpy = ( bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype *) newM->data_buffer();
  auto *  numobs_cpy = (bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datatype *) newN->data_buffer();


  //5. move data from old data buffers to new data buffers
  currIndex = 0;
  for (refined_time_trees_iter = new_refined_time_trees.begin(), unrefined_time_trees_iter = new_unrefined_time_trees.begin();
        refined_time_trees_iter != new_refined_time_trees.end(); ++refined_time_trees_iter, ++currIndex, ++unrefined_time_trees_iter)
  {
      //1. get refined and unrefined tree
      bstm_time_tree refined_time_tree((unsigned char*) (*refined_time_trees_iter).data_block(), max_level_t_);
      bstm_time_tree unrefined_time_tree((unsigned char*) (*unrefined_time_trees_iter).data_block(), max_level_t_);
      //2. correct data ptr
      refined_time_tree.set_data_ptr(dataIndex[currIndex]);
      //3. save it back to newRefinedTimeBlk
      std::memcpy(refined_time_trees_iter, refined_time_tree.get_bits(), TT_NUM_BYTES);
      //4. move the data
      this->move_data(unrefined_time_tree, refined_time_tree, alpha_cpy, mog_cpy, numobs_cpy, (int)( depths[currIndex / sub_block_num_t_]) );
  }



  //6. update cache, replace time trees
  bstm_cache_sptr cache = bstm_cache::instance();
  cache->replace_time_block(id, newRefinedTimeBlk);
  cache->replace_data_base(id, bstm_data_traits<BSTM_ALPHA>::prefix(), newA);
  cache->replace_data_base(id, bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix(), newM);
  cache->replace_data_base(id, bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix(), newN);

  delete[] dataIndex;
  delete newTimeBlk;
  delete[] depths;

  return true;
}

void bstm_refine_blk_in_spacetime_function::move_data(bstm_time_tree& unrefined_time_tree, bstm_time_tree& refined_time_tree,
                                                                                 bstm_data_traits<BSTM_ALPHA>::datatype* alpha_cpy,
                                                                                 bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype * mog_cpy,
                                                                                 bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datatype * numobs_cpy,
                                                                                 int depth)
{
  std::vector<int> new_leaves = refined_time_tree.get_leaf_bits();
  std::vector<int> old_leaves = unrefined_time_tree.get_leaf_bits();

  for (int & new_leave : new_leaves)
  {
    //get new data ptr
    int newDataPtr = refined_time_tree.get_data_index(new_leave);

    //find out if this leaf exists in the unrefined tree as well
    int pj = unrefined_time_tree.parent_index(new_leave);           //Bit_index of parent bit
    bool validCellOld = (new_leave==0) || unrefined_time_tree.bit_at(pj);

    int oldDataPtr;
    if (validCellOld) { //if they both exist
      oldDataPtr = unrefined_time_tree.get_data_index(new_leave);

      //copy data
      alpha_cpy[newDataPtr]= alpha_[oldDataPtr];
      mog_cpy[newDataPtr]  = mog_[oldDataPtr];
      numobs_cpy[newDataPtr] = num_obs_[oldDataPtr];
    }
    else
    {
      //find parent in old tree
      oldDataPtr = unrefined_time_tree.get_data_index(pj);

      float max_alpha_int = -std::log(1.0f - prob_t_);

      float side_len = block_len_ / (float) (1 << depth );
      float newAlpha = (max_alpha_int / side_len);
      vnl_vector_fixed<unsigned short, 8> zeros( (unsigned short)0 );
      bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype empty_mog( (unsigned char) 0);

      alpha_cpy[newDataPtr]  = newAlpha;
      mog_cpy[newDataPtr]    = mog_[oldDataPtr];
      numobs_cpy[newDataPtr] = zeros;
    }
  }
}

int bstm_refine_blk_in_spacetime_function::move_time_trees(boct_bit_tree& unrefined_tree, boct_bit_tree& refined_tree,
                                                                  bstm_time_block* newTimeBlk, bstm_time_block* newRefinedTimeBlk, char* depths )
{
  int newSize = refined_tree.num_cells();

  //zip through each leaf cell and
  int oldDataPtr = unrefined_tree.get_data_ptr(false);
  int newDataPtr = refined_tree.get_data_ptr(false);
  int newInitCount = 0;
  int cellsMoved = 0;

  for (int j=0; j<MAX_CELLS_ && cellsMoved<newSize; ++j)
  {
    //if parent bit is 1, then you're a valid cell
    int pj = unrefined_tree.parent_index(j);                //Bit_index of parent bit
    bool validCellOld = (j==0) || unrefined_tree.bit_at(pj);
    bool validCellNew = (j==0) || refined_tree.bit_at(pj);
    if (validCellOld && validCellNew) { //both old and new cell exists
      depths[newDataPtr] = unrefined_tree.depth_at(j);

      //move data to new location
      boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > old_time_trees = blk_t_->get_cell_all_tt(oldDataPtr); //get all tt from prev. loc

      if (!refined_tree.is_leaf(j)) //if the time trees being copied belong to an inner cell, erase their time trees
      {
        auto * erased_old_time_trees = new vnl_vector_fixed<unsigned char, 8>[sub_block_num_t_];
        for (unsigned int t_idx = 0;t_idx < sub_block_num_t_; ++t_idx) {
          bstm_time_tree tmp_tree(old_time_trees[t_idx].data_block() );   //create tree by copying the tree data
          tmp_tree.erase_cells();                                         //erase all cells except for root.
          erased_old_time_trees[t_idx].set( tmp_tree.get_bits() );        //copy to erased_old_time_trees
        }
        newTimeBlk->set_cell_all_tt(newDataPtr, boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > (sub_block_num_t_, erased_old_time_trees) ); //set all tt to new loc
        newRefinedTimeBlk->set_cell_all_tt(newDataPtr, boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > (sub_block_num_t_, erased_old_time_trees) ); //set all tt to new loc
        delete[] erased_old_time_trees;
      }
      else
      {
        //if the time trees being copied belong to a leaf, then just copy them.
        newTimeBlk->set_cell_all_tt(newDataPtr, old_time_trees); //set all tt to new loc
        newRefinedTimeBlk->set_cell_all_tt(newDataPtr, old_time_trees); //set all tt to new loc

        int currDepth = unrefined_tree.depth_at(j);
        double side_len = block_len_ / double(1<<currDepth);
        boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > refined_time_trees = newRefinedTimeBlk->get_cell_all_tt(newDataPtr);
        for (unsigned int t_idx = 0;t_idx < sub_block_num_t_; ++t_idx) {
          bstm_time_tree tmp_tree(refined_time_trees[t_idx].data_block() );       //create tree by copying the tree data
          bstm_time_tree refined_tree = refine_time_tree(tmp_tree, side_len );    //refine time tree
          refined_time_trees[t_idx].set( refined_tree.get_bits() );               //copy to refined_time_trees.
        }
      }

      //increment
      ++oldDataPtr;
      ++newDataPtr;
      ++cellsMoved;
    }
    //case where it's a new leaf...
    else if (validCellNew)
    {
      depths[newDataPtr] = refined_tree.depth_at(pj); //save depth


      int parent_dataPtr = unrefined_tree.get_data_index(pj, false);
      //move root data to new location
      boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > old_time_trees = blk_t_->get_cell_all_tt(parent_dataPtr); //get all tt from root loc
      newTimeBlk->set_cell_all_tt(newDataPtr,old_time_trees); //set all tt to new loc(child)

      //save and then refine
      newRefinedTimeBlk->set_cell_all_tt(newDataPtr, old_time_trees); //set all tt to new loc

      int currDepth = unrefined_tree.depth_at(pj);
      double side_len = block_len_ / double(1<<currDepth);
      boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > refined_time_trees = newRefinedTimeBlk->get_cell_all_tt(newDataPtr);
      for (unsigned int t_idx = 0;t_idx < sub_block_num_t_; ++t_idx) {
        bstm_time_tree tmp_tree(refined_time_trees[t_idx].data_block() );       //create tree by copying the tree data
        bstm_time_tree refined_tree = refine_time_tree(tmp_tree, side_len );    //refine time tree
        refined_time_trees[t_idx].set( refined_tree.get_bits() );               //copy to refined_time_trees.
      }


      //update new data pointer
      ++newDataPtr;
      ++newInitCount;
      ++cellsMoved;
    }
  }

  return newInitCount;
}


bool bstm_refine_blk_in_spacetime_function::decide_refinement_in_space(int dataIndex, double side_len)
{
  //fetch time tree(s) and see if they contain any cell with prob > prob_t.
  boxm2_array_1d<uchar8>  all_time_trees =  blk_t_->get_cell_all_tt(dataIndex);
  for (auto & all_time_tree : all_time_trees)
  {
    bstm_time_tree time_tree( all_time_tree.data_block(), max_level_t_);
    //loop over its leaves to see if any cell has prob > prob_t
    std::vector<int> leaves = time_tree.get_leaf_bits();
    for(std::vector<int>::const_iterator iter = leaves.begin(); iter != leaves.end(); iter++)
    {
      int data_ptr = time_tree.get_data_index(*iter);
      //fetch prob
      float p = 1 - std::exp(- alpha_[data_ptr] * side_len);
      if(p > prob_t_)
        return true;
    }
  }
  return false;
}

bstm_time_tree bstm_refine_blk_in_spacetime_function::refine_time_tree(bstm_time_tree& unrefined_time_tree, double side_len )
{
  //initialize tree to return
  bstm_time_tree refined_tree(unrefined_time_tree.get_bits(), max_level_t_);
  std::vector<int> leaves = unrefined_time_tree.get_leaf_bits();

  for(std::vector<int>::const_iterator iter = leaves.begin(); iter != leaves.end(); iter++)
  {
    //////////////////////////////////////////////////
    //LEAF CODE HERE
    //////////////////////////////////////////////////
    int data_ptr = unrefined_time_tree.get_data_index(*iter);
    //fetch prob
    float p = 1 - std::exp(- alpha_[data_ptr] * side_len);
    bool should_refine = (p > prob_t_);
    if (should_refine && unrefined_time_tree.depth_at(*iter) < max_level_t_ -1) {
      refined_tree.set_bit_at(*iter, true);
      num_split_t_++;
    }
  }
  return refined_tree;
}


boct_bit_tree bstm_refine_blk_in_spacetime_function::refine_bit_tree(const boct_bit_tree& unrefined_tree)
{
  //initialize tree to return
  boct_bit_tree refined_tree(unrefined_tree.get_bits(), max_level_);

  //no need to do depth first search, just iterate and check each node along the way
  //(iterate through the max number of inner cells)
  for (int i=0; i<MAX_CELLS_; ++i)
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
      double side_len = block_len_ / double(1<<currDepth);

      //get dataIndex for this cell;

      int dataIndex = unrefined_tree.get_data_index(i, false);
      bool should_refine = decide_refinement_in_space(dataIndex, side_len);
      if (should_refine && currDepth < max_level_-1)
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

////////////////////////////////////////////////////////////////////////////////
//MAIN REFINE FUNCTION
////////////////////////////////////////////////////////////////////////////////
void bstm_refine_block_spacetime(bstm_time_block* t_blk, bstm_block* blk,
                        std::vector<bstm_data_base*> & datas,
                        float prob_thresh)
{
  bstm_refine_blk_in_spacetime_function refine_block;
  refine_block.init_data(t_blk, blk, datas, prob_thresh);

  refine_block.refine(datas);
}
