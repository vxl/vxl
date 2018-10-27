#ifndef bstm_refine_blk_in_time_function_hxx_
#define bstm_refine_blk_in_time_function_hxx_

#include "bstm_refine_blk_in_time_function.h"
//:
// \file

#define INIT_PROB 0.3f

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
bstm_refine_blk_in_time_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::bstm_refine_blk_in_time_function(
                                  bstm_time_block* t_blk, bstm_block* blk, std::vector<bstm_data_base*> & datas, float change_prob_t, float time)
{
  init_data(t_blk, blk, datas, change_prob_t,time);
  refine(datas);
}

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
bool bstm_refine_blk_in_time_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::init_data(bstm_time_block* blk_t, bstm_block* blk, std::vector<bstm_data_base*> & datas, float change_prob_t, float local_time)
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

  //length of one side of a sub block
  block_len_ = blk->sub_block_dim().x();

  sub_block_num_t_ = blk_t_->sub_block_num();

  change_prob_t_ = change_prob_t;
  local_time_ = local_time;

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

   return true;
}

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
bool bstm_refine_blk_in_time_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::refine(std::vector<bstm_data_base*>& datas)
{

  //0. allocate new time trees and copy old time trees to here.
  bstm_block_id id = blk_->block_id();
  bstm_block_metadata m_data; m_data.init_level_t_ = blk_t_->init_level(); m_data.max_level_t_ = blk_t_->max_level(); m_data.sub_block_num_t_ = blk_t_->sub_block_num();
  bstm_time_block* newTimeBlk = new bstm_time_block(id, m_data, blk_t_->tree_buff_length() / blk_t_->sub_block_num()); //create empty time block
  std::memcpy(newTimeBlk->buffer(), blk_t_->buffer(), blk_t_->byte_count() ); //copy the time trees to new loc
  char* depths = new char[blk_t_->tree_buff_length() / blk_t_->sub_block_num() ];

  //1. loop over each tree, decide at each leaf whether the time tree should be refined or not.
  boxm2_array_3d<uchar16>&  trees = blk_->trees();  //trees to refine
  boxm2_array_3d<uchar16>::iterator blk_iter;
  for (blk_iter = trees.begin(); blk_iter != trees.end(); ++blk_iter)
  {
      //1. get current tree information
      uchar16 tree  = (*blk_iter);
      boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), max_level_);

      std::vector<int> leaves = curr_tree.get_leaf_bits(0);
      for(std::vector<int>::const_iterator iter = leaves.begin(); iter != leaves.end(); iter++)
      {
        //2. decide whether to refine its time trees or not.
        int dataPtr = curr_tree.get_data_index(*iter);
        depths[dataPtr] = curr_tree.depth_at(*iter);
        boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > time_trees = newTimeBlk->get_cell_all_tt(dataPtr); //get all tt

        if(this->should_refine_tt(dataPtr))
        {
          boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > refined_time_trees = newTimeBlk->get_cell_all_tt(dataPtr); //get all tt
          for (unsigned int t_idx = 0;t_idx < sub_block_num_t_; ++t_idx) {
            if (t_idx == newTimeBlk->tree_index(local_time_)) {                       //process only if the time tree contains the
              bstm_time_tree tmp_tree(refined_time_trees[t_idx].data_block() );       //create tree by copying the tree data
              refine_tt(tmp_tree);                                                    //refine the time tree to accomodate new
              refined_time_trees[t_idx].set( tmp_tree.get_bits() );                   //copy back to time blk
            }
          }
        }
      }
  }
  std::cout << "Num time cells split: " << num_split_ << std::endl;

  //2. figure out new sizes for the time blk
  boxm2_array_1d<uchar8>&  old_time_trees = blk_t_->time_trees();           //refined trees

  boxm2_array_1d<uchar8>&  new_time_trees = newTimeBlk->time_trees();       //refined trees
  int* dataIndex = new int[new_time_trees.size()];                         //data index for each new tree
  int currIndex = 0;                                                        //curr tree being looked at
  int dataSize = 0;                                                         //running sum of data size
  boxm2_array_1d<uchar8>::iterator time_trees_iter;
  for (time_trees_iter = new_time_trees.begin(); time_trees_iter != new_time_trees.end(); ++time_trees_iter, ++currIndex)
  {
      //0. store data index for each tree.
      dataIndex[currIndex] = dataSize;
      //1. get refined tree
      bstm_time_tree new_time_tree((unsigned char*) (*time_trees_iter).data_block(), max_level_t_);
      //2. save its new size
      int newSize = new_time_tree.num_leaves(); //number of leaves, not all cells.
      dataSize += newSize;
  }

  //3. alloc new data
  bstm_data_base* newA = new bstm_data_base(new char[dataSize * bstm_data_traits<BSTM_ALPHA>::datasize() ],
                                                      dataSize * bstm_data_traits<BSTM_ALPHA>::datasize(), id);
  bstm_data_base* newM = new bstm_data_base(new char[dataSize * bstm_data_traits<APM_DATA_TYPE>::datasize()],
                                                      dataSize * bstm_data_traits<APM_DATA_TYPE>::datasize() , id);
  bstm_data_base* newN = new bstm_data_base(new char[dataSize * bstm_data_traits<NOBS_DATA_TYPE>::datasize()],
                                                      dataSize * bstm_data_traits<NOBS_DATA_TYPE>::datasize(), id);
  bstm_data_traits<BSTM_ALPHA>::datatype *   alpha_cpy = (bstm_data_traits<BSTM_ALPHA>::datatype *) newA->data_buffer();
  typename bstm_data_traits<APM_DATA_TYPE>::datatype *  mog_cpy = (typename bstm_data_traits<APM_DATA_TYPE>::datatype *) newM->data_buffer();
  typename bstm_data_traits<NOBS_DATA_TYPE>::datatype *  numobs_cpy = (typename bstm_data_traits<NOBS_DATA_TYPE>::datatype *) newN->data_buffer();


  //4. move data from old data buffers to new data buffers
  currIndex = 0;
  boxm2_array_1d<uchar8>::iterator refined_time_trees_iter, unrefined_time_trees_iter;
  for (refined_time_trees_iter = new_time_trees.begin(), unrefined_time_trees_iter = old_time_trees.begin();
        refined_time_trees_iter != new_time_trees.end(); ++refined_time_trees_iter, ++currIndex, ++unrefined_time_trees_iter)
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

  //
  bstm_cache_sptr cache = bstm_cache::instance();
  cache->replace_time_block(id, newTimeBlk);
  cache->replace_data_base(id, bstm_data_traits<BSTM_ALPHA>::prefix(), newA);
  cache->replace_data_base(id, bstm_data_traits<APM_DATA_TYPE>::prefix(), newM);
  cache->replace_data_base(id, bstm_data_traits<NOBS_DATA_TYPE>::prefix(), newN);

  delete[] dataIndex;

  return true;
}

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
void bstm_refine_blk_in_time_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::refine_tt(bstm_time_tree& tt)
{
  float trees_local_time = local_time_ - blk_t_->tree_index(local_time_);
  bool split_complete = false;
  while (!split_complete)
  {
    int curr_cell = tt.traverse(trees_local_time);
    int currDepth = tt.depth_at(curr_cell);

    float cell_min,cell_max;
    tt.cell_range(curr_cell, cell_min,cell_max);
    if (cell_min == trees_local_time)           //found cell starting at queried time.
      split_complete = true;                   //we're done here.
    else if (currDepth < TT_NUM_LVLS-1) {
      tt.set_bit_at(curr_cell,true); //split curr_cell
      ++num_split_;
    }
    else                                       //reached end of tree...
      split_complete = true;
  }

}

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
void bstm_refine_blk_in_time_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::move_data(bstm_time_tree& unrefined_time_tree, bstm_time_tree& refined_time_tree,
                                                                                 bstm_data_traits<BSTM_ALPHA>::datatype* alpha_cpy,
                                                                                 typename bstm_data_traits<APM_DATA_TYPE>::datatype * mog_cpy,
                                                                                 typename bstm_data_traits<NOBS_DATA_TYPE>::datatype * numobs_cpy,
                                                                                 int depth)
{
  std::vector<int> new_leaves = refined_time_tree.get_leaf_bits();
  std::vector<int> old_leaves = unrefined_time_tree.get_leaf_bits();

  int curr_time_tree_leaf = refined_time_tree.traverse(local_time_ - blk_t_->tree_index(local_time_));
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
      //if the cell contains the current time, initialize with 0. Otherwise, copy from parents
      if (  curr_time_tree_leaf == new_leave )  { //if the current time is the start of a cell in which new data will be placed
        float max_alpha_int = -std::log(1.0f - INIT_PROB);

        float side_len = block_len_ / (float) (1 << depth );
        float newAlpha = (max_alpha_int / side_len);
        typename bstm_data_traits<NOBS_DATA_TYPE>::datatype zeros( (unsigned short)0 );
        typename bstm_data_traits<APM_DATA_TYPE>::datatype empty_mog( (unsigned char) 0);

        alpha_cpy[newDataPtr]  = newAlpha;
        mog_cpy[newDataPtr]    = empty_mog;
        numobs_cpy[newDataPtr] = zeros;
      }
      else
      {
        //find parent in old tree
        int valid_parent_bit = pj;
        while ( valid_parent_bit !=0 && !unrefined_time_tree.bit_at( unrefined_time_tree.parent_index(valid_parent_bit) ) )
          valid_parent_bit = unrefined_time_tree.parent_index(valid_parent_bit);
        oldDataPtr = unrefined_time_tree.get_data_index(valid_parent_bit);

        typename bstm_data_traits<NOBS_DATA_TYPE>::datatype zeros( (unsigned short)0 );


        //copy data
        alpha_cpy[newDataPtr]= alpha_[oldDataPtr];
        mog_cpy[newDataPtr]  = mog_[oldDataPtr];
        numobs_cpy[newDataPtr] = zeros;
      }
    }
  }
}

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
bool bstm_refine_blk_in_time_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::should_refine_tt(int data_ptr)
{
  if(change_[data_ptr] > change_prob_t_)
    return true;
  else
    return false;
}

#endif
