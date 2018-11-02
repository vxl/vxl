#ifndef bstm_refine_blk_in_space_function_hxx_
#define bstm_refine_blk_in_space_function_hxx_

#include <iostream>
#include <algorithm>
#include "bstm_refine_blk_in_space_function.h"
#include <bstm/io/bstm_lru_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
bstm_refine_blk_in_space_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::bstm_refine_blk_in_space_function(bstm_time_block* t_blk, bstm_block* blk, std::vector<bstm_data_base*> & datas, float change_prob_t)
{
  init_data(t_blk, blk, datas, change_prob_t);
  refine(datas);
}

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
bool bstm_refine_blk_in_space_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::init_data(bstm_time_block* blk_t, bstm_block* blk, std::vector<bstm_data_base*> & datas, float change_prob_t)
{
  //store block and pointer to uchar16 3d block
   blk_   = blk;
   blk_t_ = blk_t;

   //store data buffers
   int i=0;
   alpha_   = (float*)   datas[i++]->data_buffer();
   mog_     = (typename bstm_data_traits<APM_DATA_TYPE>::datatype*)   datas[i++]->data_buffer();
   num_obs_     = (typename bstm_data_traits<NOBS_DATA_TYPE>::datatype*)   datas[i++]->data_buffer();
   change_     = (bstm_data_traits<BSTM_CHANGE>::datatype*)   datas[i++]->data_buffer();

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

   change_prob_t_ = change_prob_t;
   return true;
}

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
bool bstm_refine_blk_in_space_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::refine(std::vector<bstm_data_base*>& datas)
{

  //1. loop over each tree, refine it in place
  boxm2_array_3d<uchar16>&  trees = blk_->trees();  //trees to refine
  uchar16* trees_copy = new uchar16[trees.size()];  //copy of those trees
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

  //create new label database
  bstm_data_base* newC = new bstm_data_base(new char[dataSize * bstm_data_traits<BSTM_CHANGE>::datasize() ], dataSize * bstm_data_traits<BSTM_CHANGE>::datasize(), id);
  bstm_data_traits<BSTM_CHANGE>::datatype *   newC_cpy = (bstm_data_traits<BSTM_CHANGE>::datatype *) newC->data_buffer();

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
      newInitCount += this->move_time_trees(old_tree, refined_tree, newTimeBlk,newC_cpy);

      //4. store old tree in new tree, swap data out
      std::memcpy(blk_iter, refined_tree.get_bits(), sizeof(uchar16));
  }

  delete[] trees_copy;
  delete[] dataIndex;



  //4. figure out new data size
  boxm2_array_1d<uchar8>&  time_trees = newTimeBlk->time_trees();         //unrefined new time treesunrefined_time_tree
  boxm2_array_1d<uchar8>::iterator time_trees_iter;
  dataIndex = new int[time_trees.size()];                                //data index for each new tree
  currIndex = 0;                                                          //curr tree being looked at
  dataSize = 0;
  for (time_trees_iter = time_trees.begin(); time_trees_iter != time_trees.end(); ++time_trees_iter, ++currIndex)
  {
      //0. store data index for each tree.
      dataIndex[currIndex] = dataSize;
      //1. get refined tree
      bstm_time_tree new_time_tree((unsigned char*) (*time_trees_iter).data_block(), max_level_t_);
      int newSize = new_time_tree.num_leaves(); //number of leaves, not all cells.
      dataSize += newSize;
  }
  std::cout << "New data size: " << dataSize << std::endl;

  //alloc new buffers
  bstm_data_base* newA = new bstm_data_base(new char[dataSize * bstm_data_traits<BSTM_ALPHA>::datasize() ],
                                                      dataSize * bstm_data_traits<BSTM_ALPHA>::datasize(), id);
  bstm_data_base* newM = new bstm_data_base(new char[dataSize * bstm_data_traits<APM_DATA_TYPE>::datasize()],
                                                      dataSize * bstm_data_traits<APM_DATA_TYPE>::datasize() , id);
  bstm_data_base* newN = new bstm_data_base(new char[dataSize * bstm_data_traits<NOBS_DATA_TYPE>::datasize()],
                                                      dataSize * bstm_data_traits<NOBS_DATA_TYPE>::datasize(), id);
  bstm_data_traits<BSTM_ALPHA>::datatype *   alpha_cpy = (bstm_data_traits<BSTM_ALPHA>::datatype *) newA->data_buffer();
  typename bstm_data_traits<APM_DATA_TYPE>::datatype *  mog_cpy = (typename bstm_data_traits<APM_DATA_TYPE>::datatype *) newM->data_buffer();
  typename bstm_data_traits<NOBS_DATA_TYPE>::datatype *  numobs_cpy = (typename bstm_data_traits<NOBS_DATA_TYPE>::datatype *) newN->data_buffer();


  //5. move data from old data buffers to new data buffers
  currIndex = 0;
  for (time_trees_iter = time_trees.begin(); time_trees_iter != time_trees.end(); ++time_trees_iter, ++currIndex )
  {
      //1. get old time tree and new one with correct data ptr
      bstm_time_tree old_time_tree((unsigned char*) (*time_trees_iter).data_block(), max_level_t_);
      bstm_time_tree time_tree((unsigned char*) (*time_trees_iter).data_block(), max_level_t_);
      //2. correct data ptr
      time_tree.set_data_ptr(dataIndex[currIndex]);
      //3. save it back to newRefinedTimeBlk
      std::memcpy(time_trees_iter, time_tree.get_bits(), TT_NUM_BYTES);
      //4. move the data
      this->move_data(old_time_tree, time_tree, alpha_cpy, mog_cpy, numobs_cpy);
  }



  //6. update cache, replace time trees
  bstm_cache_sptr cache = bstm_cache::instance();
  cache->replace_time_block(id, newTimeBlk);
  cache->replace_data_base(id, bstm_data_traits<BSTM_ALPHA>::prefix(), newA);
  cache->replace_data_base(id, bstm_data_traits<APM_DATA_TYPE>::prefix(), newM);
  cache->replace_data_base(id, bstm_data_traits<NOBS_DATA_TYPE>::prefix(), newN);
  cache->replace_data_base(id, bstm_data_traits<BSTM_CHANGE>::prefix(), newC);

  delete[] dataIndex;

  return true;
}

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
void bstm_refine_blk_in_space_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::move_data(bstm_time_tree& old_time_tree, bstm_time_tree& time_tree,
                                                         bstm_data_traits<BSTM_ALPHA>::datatype* alpha_cpy,
                                                         typename bstm_data_traits<APM_DATA_TYPE>::datatype * mog_cpy,
                                                         typename bstm_data_traits<NOBS_DATA_TYPE>::datatype * numobs_cpy)
{
  std::vector<int> new_leaves = time_tree.get_leaf_bits();

  for (int & new_leave : new_leaves)
  {
    //get new data ptr
    int newDataPtr = time_tree.get_data_index(new_leave);
    int oldDataPtr = old_time_tree.get_data_index(new_leave);

    //copy data
    alpha_cpy[newDataPtr]= alpha_[oldDataPtr];
    mog_cpy[newDataPtr]  = mog_[oldDataPtr];
    numobs_cpy[newDataPtr] = num_obs_[oldDataPtr];
  }
}

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
int bstm_refine_blk_in_space_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::move_time_trees(boct_bit_tree& unrefined_tree, boct_bit_tree& refined_tree, bstm_time_block* newTimeBlk, bstm_data_traits<BSTM_CHANGE>::datatype *  newC )
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


      //move data to new location
      boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > old_time_trees = blk_t_->get_cell_all_tt(oldDataPtr); //get all tt from prev. loc

      if (!refined_tree.is_leaf(j)) //if the time trees being copied belong to an inner cell, erase their time trees
      {
        vnl_vector_fixed<unsigned char, 8> * erased_old_time_trees = new vnl_vector_fixed<unsigned char, 8>[sub_block_num_t_];
        for (unsigned int t_idx = 0;t_idx < sub_block_num_t_; ++t_idx) {
          bstm_time_tree tmp_tree(old_time_trees[t_idx].data_block() );   //create tree by copying the tree data
          tmp_tree.erase_cells();                                         //erase all cells except for root.
          erased_old_time_trees[t_idx].set( tmp_tree.get_bits() );        //copy to erased_old_time_trees
        }
        newTimeBlk->set_cell_all_tt(newDataPtr, boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > (sub_block_num_t_, erased_old_time_trees) ); //set all tt to new loc
        delete[] erased_old_time_trees;
      }
      else
      {
        //if the time trees being copied belong to a leaf, then just copy them.
        newTimeBlk->set_cell_all_tt(newDataPtr, old_time_trees); //set all tt to new loc
      }

      //now copy the change prob.
      newC[newDataPtr] = change_[oldDataPtr];

      //increment
      ++oldDataPtr;
      ++newDataPtr;
      ++cellsMoved;
    }
    //case where it's a new leaf...
    else if (validCellNew)
    {
      int parent_dataPtr = unrefined_tree.get_data_index(pj, false);
      //move root data to new location
      boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > old_time_trees = blk_t_->get_cell_all_tt(parent_dataPtr); //get all tt from root loc
      newTimeBlk->set_cell_all_tt(newDataPtr,old_time_trees); //set all tt to new loc(child)

      //now copy the change prob.
      newC[newDataPtr] = change_[parent_dataPtr];

      //update new data pointer
      ++newDataPtr;
      ++newInitCount;
      ++cellsMoved;
    }
  }

  return newInitCount;
}


template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
boct_bit_tree bstm_refine_blk_in_space_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::refine_bit_tree(const boct_bit_tree& unrefined_tree)
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
      bool should_refine = (change_[dataIndex] >= change_prob_t_);

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


#endif //bstm_refine_blk_in_space_function_hxx_
