#ifndef bstm_copy_data_to_future_function_hxx_
#define bstm_copy_data_to_future_function_hxx_

#include "bstm_copy_data_to_future_function.h"
//:
// \file

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
bstm_copy_data_to_future_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::bstm_copy_data_to_future_function(
                                  bstm_time_block* t_blk, bstm_block* blk, std::vector<bstm_data_base*> & datas, float time)
{
  init_data(t_blk, blk, datas ,time);
  copy();
}

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
bool bstm_copy_data_to_future_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::init_data(bstm_time_block* blk_t, bstm_block* blk, std::vector<bstm_data_base*> & datas,  float local_time)
{
  //store block and pointer to uchar16 3d block
   blk_   = blk;
   blk_t_ = blk_t;

  //store data buffers
  int i=0;
  alpha_   = (float*)   datas[i++]->data_buffer();
  mog_     = (typename bstm_data_traits<APM_DATA_TYPE>::datatype*)   datas[i++]->data_buffer();
  num_obs_     = (typename bstm_data_traits<NOBS_DATA_TYPE>::datatype*)   datas[i++]->data_buffer();

  num_el_ = datas[0]->buffer_length() / bstm_data_traits<BSTM_ALPHA>::datasize();
  local_time_ = local_time - blk_t_->tree_index(local_time);

   return true;
}


template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
bool bstm_copy_data_to_future_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::copy()
{
  bstm_block_id id = blk_->block_id();

  //1. loop over each time tree
  boxm2_array_1d<uchar8>&  time_trees = blk_t_->time_trees();    //old time trees
  boxm2_array_1d<uchar8>::iterator time_trees_iter;
  for (time_trees_iter = time_trees.begin(); time_trees_iter != time_trees.end(); ++time_trees_iter)
  {
    bstm_time_tree time_tree((unsigned char*) (*time_trees_iter).data_block());
    move_data(time_tree);
  }

  return true;
}


template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
void bstm_copy_data_to_future_function<APM_DATA_TYPE, NOBS_DATA_TYPE>::move_data(bstm_time_tree& time_tree)
{
  std::vector<int> leaves = time_tree.get_leaf_bits();

  int num_cells = 0;
  int curr_leaf_bit = time_tree.traverse(local_time_);
  int oldDataPtr = time_tree.get_data_index(curr_leaf_bit);
  for (int & leave : leaves)
  {

    float cell_min;
    float cell_max;
    time_tree.cell_range(leave, cell_min, cell_max);

    if(cell_min > local_time_ && cell_max >= local_time_)
    {
      int newDataPtr = time_tree.get_data_index(leave);

//      //copy data
      alpha_[newDataPtr]= alpha_[oldDataPtr];
      mog_[newDataPtr]  = mog_[oldDataPtr];
      num_obs_[newDataPtr] = num_obs_[oldDataPtr];
    }


  }

}


#endif
