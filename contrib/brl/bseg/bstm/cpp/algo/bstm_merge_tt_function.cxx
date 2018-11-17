#include <iostream>
#include <set>
#include "bstm_merge_tt_function.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool bstm_merge_tt_function::init_data(bstm_time_block* blk_t, bstm_block* blk, std::vector<bstm_data_base*> & datas, float prob_thresh)
{
  //store block and pointer to uchar16 3d block
   blk_   = blk;
   blk_t_ = blk_t;

   //store data buffers
   int i=0;
   alpha_   = (bstm_data_traits<BSTM_ALPHA>::datatype*)   datas[i++]->data_buffer();
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

   prob_t_ = prob_thresh;

   return true;
}


bool bstm_merge_tt_function::merge(std::vector<bstm_data_base*>&  /*datas*/)
{
  //1. loop over each tree to save spatial depth of each time tree
  boxm2_array_1d<uchar8>&  old_time_trees = blk_t_->time_trees();    //old time trees
  char* depths = new char[old_time_trees.size()];
  boxm2_array_3d<uchar16>&  trees = blk_->trees();  //trees to refine
  boxm2_array_3d<uchar16>::iterator blk_iter;
  for (blk_iter = trees.begin(); blk_iter != trees.end(); ++blk_iter)
  {
      //1. get current tree information
      uchar16 tree  = (*blk_iter);
      boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), max_level_);

      int cellsHit = 0;
      for (int i=0; i<MAX_CELLS_ && cellsHit < curr_tree.num_cells(); ++i)
      {
        //if current bit is 0 and parent bit is 1, you're at a leaf
        int pi = (i-1)>>3;           //Bit_index of parent bit
        bool validParent = curr_tree.bit_at(pi) || (i==0); // special case for root
        if (validParent)
        {
          depths[curr_tree.get_data_index(i, false) ] = curr_tree.depth_at(i);
          cellsHit++;
        }
      }
  }


  //2. loop over time trees to merge them
  boxm2_array_1d<uchar8>::iterator old_time_trees_iter;
  auto* trees_copy = new uchar8[old_time_trees.size()];  //copy of time trees
  int* dataIndex = new int[old_time_trees.size()]; //data index for each new tree
  int currIndex = 0;                                //curr tree being looked at
  int dataSize = 0;                                 //running sum of data size
  int old_dataSize = 0;
  for (old_time_trees_iter = old_time_trees.begin(); old_time_trees_iter != old_time_trees.end(); ++old_time_trees_iter, ++currIndex)
  {
      //0. store data index for each tree.
      dataIndex[currIndex] = dataSize;
      //1. get old time tree
      bstm_time_tree old_time_tree((unsigned char*) (*old_time_trees_iter).data_block(), max_level_t_);
      //2. merge time tree
      bstm_time_tree new_time_tree = this->merge_tt(old_time_tree, depths[currIndex]);
      //3. copy new tree into trees_copy
      std::memcpy (trees_copy[currIndex].data_block(), new_time_tree.get_bits(), TT_NUM_BYTES);
      //4. account new datasize
      dataSize += new_time_tree.num_leaves();
      old_dataSize += old_time_tree.num_leaves();
  }

  //3. alloc new buffers
  bstm_block_id id = blk_->block_id();
  bstm_data_base* newA = new bstm_data_base(new char[dataSize * bstm_data_traits<BSTM_ALPHA>::datasize() ],
                                                      dataSize * bstm_data_traits<BSTM_ALPHA>::datasize(), id);
  bstm_data_base* newM = new bstm_data_base(new char[dataSize * bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datasize() ],
                                                      dataSize * bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datasize() , id);
  bstm_data_base* newN = new bstm_data_base(new char[dataSize * bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datasize() ],
                                                      dataSize * bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datasize(), id);
  auto *   alpha_cpy = (bstm_data_traits<BSTM_ALPHA>::datatype *) newA->data_buffer();
  auto *  mog_cpy = (bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype *) newM->data_buffer();
  auto *  numobs_cpy = (bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datatype *) newN->data_buffer();



  std::cout << "Num elements saved: " << old_dataSize - dataSize << "." << std::endl;

  //43. loop through trees again, putting the time trees in the right place as well as refining the time trees
  currIndex = 0;
  for (old_time_trees_iter = old_time_trees.begin(); old_time_trees_iter != old_time_trees.end(); ++old_time_trees_iter, ++currIndex)
  {
      //1. get current tree information
      bstm_time_tree old_tree( (unsigned char*) (*old_time_trees_iter).data_block(), max_level_t_);

      //2. merged tree
      bstm_time_tree merged_tree( (unsigned char*) trees_copy[currIndex].data_block(), max_level_t_);

      //2.5 pack data bits into merged tree
      //store data index in bits [10, 11, 12, 13] ;
      int root_index = dataIndex[currIndex];
      merged_tree.set_data_ptr(root_index);

      //3. swap data from old location to new location
      this->move_data(old_tree, merged_tree,  depths[currIndex], alpha_cpy, mog_cpy, numobs_cpy);

      //4. store old tree in new tree, swap data out
      std::memcpy(old_time_trees_iter, merged_tree.get_bits(), TT_NUM_BYTES);
  }


  //5. update cache, replace time trees
  bstm_cache_sptr cache = bstm_cache::instance();
  cache->replace_data_base(id, bstm_data_traits<BSTM_ALPHA>::prefix(), newA);
  cache->replace_data_base(id, bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix(), newM);
  cache->replace_data_base(id, bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix(), newN);

  delete[] trees_copy;
  delete[] dataIndex;
  delete[] depths;
  return true;
}

void bstm_merge_tt_function::move_data(const bstm_time_tree& old_tree, const bstm_time_tree& merged_tree,  int depth, bstm_data_traits<BSTM_ALPHA>::datatype* alpha_cpy,
                                            bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype* mog_cpy, bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datatype* numobs_cpy)
{
  std::vector<int> merged_tree_leaves = merged_tree.get_leaf_bits();

  for (int & merged_tree_leave : merged_tree_leaves)
  {
    //get new data ptr
    int newDataPtr = merged_tree.get_data_index(merged_tree_leave);

    if (old_tree.is_leaf(merged_tree_leave) ) { //if they both exist
      int oldDataPtr = old_tree.get_data_index(merged_tree_leave);

      //copy data
      alpha_cpy[newDataPtr]= alpha_[oldDataPtr];
      mog_cpy[newDataPtr]  = mog_[oldDataPtr];
      numobs_cpy[newDataPtr] = num_obs_[oldDataPtr];
    }
    else
    {
      if(old_tree.is_leaf(old_tree.child_index(merged_tree_leave) )==0 || old_tree.is_leaf(old_tree.child_index(merged_tree_leave) + 1 ) == 0)
      {
        std::cout << "Bit " << merged_tree_leave << " and childs: " << old_tree.child_index(merged_tree_leave) << " and " << old_tree.child_index(merged_tree_leave) + 1 << std::endl;
        std::cout << "Valid cells " << old_tree.valid_cell(old_tree.child_index(merged_tree_leave) ) << " and " << old_tree.valid_cell(old_tree.child_index(merged_tree_leave) + 1 ) << std::endl;
        std::cout << "Valid leaves " << old_tree.is_leaf(old_tree.child_index(merged_tree_leave) ) << " and " << old_tree.is_leaf(old_tree.child_index(merged_tree_leave) + 1 ) << std::endl;
        std::cout << "Num leaves in old tree " << old_tree.num_leaves() << " and in the new one: " << merged_tree.num_leaves() << std::endl;
      }

      //find children in old tree
      int oldDataPtr_left_child = old_tree.get_data_index( old_tree.child_index(merged_tree_leave) );
      int oldDataPtr_right_child = old_tree.get_data_index( old_tree.child_index(merged_tree_leave ) + 1 );

      //avg alpha
      float alpha_left_child = alpha_[oldDataPtr_left_child];
      float alpha_right_child = alpha_[oldDataPtr_right_child];
      float side_len = (float)block_len_ / float(1<<depth);
      float left_child_p =  1.0f - (float)std::exp(-alpha_left_child * side_len);
      float right_child_p =  1.0f - (float)std::exp(-alpha_right_child * side_len);
      float avg_p = (left_child_p + right_child_p) / 2;
      alpha_cpy[newDataPtr]  = -std::log(1- avg_p) / side_len;

      //avg mog
      bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype left_mog = mog_[oldDataPtr_left_child];
      bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype right_mog = mog_[oldDataPtr_right_child];
      bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype avg_mog;
      for(int i = 0; i < 8; i++) {
        float left_mean = left_mog[2*i];
        float right_mean = right_mog[2*i];
        float left_std = (float)(left_mog[2*i + 1]) / 255.0f;
        float right_std = (float)(right_mog[2*i + 1]) / 255.0f;
        avg_mog[2*i] = (unsigned char)( (left_mean + right_mean) / 2);
        avg_mog[2*i + 1] = (unsigned char) ( 255.0f * std::sqrt( (left_std*left_std + right_std*right_std) / 4 ) );


//        float left_std = (float)(left_mog[2*i + 1]);
//        float right_std = (float)(right_mog[2*i + 1]) ;
//        avg_mog[2*i] = ( (left_mean + right_mean) / 2);
//        avg_mog[2*i + 1] = ( std::sqrt( (left_std*left_std + right_std*right_std) / 4 ) );


      }
      mog_cpy[newDataPtr]= avg_mog;


      //avg num obs
      bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datatype left_num_obs= num_obs_ [oldDataPtr_left_child];
      bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datatype right_num_obs = num_obs_[oldDataPtr_right_child];
      bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datatype avg_num_obs;
      for(int i = 0; i < 8; i++) {
        avg_num_obs[i] = (left_num_obs[i] + right_num_obs[i]) / 2;
      }
      numobs_cpy[newDataPtr] = avg_num_obs;

    }
  }
}


bstm_time_tree bstm_merge_tt_function::merge_tt(const bstm_time_tree& old_tree, int curr_depth)
{
  std::vector<int> old_leaves = old_tree.max_depth_leaves();
  std::set<int> parents;
  bstm_time_tree merged_tree(old_tree.get_bits(), max_level_t_);
  //get parents of current leaves
  for (int & old_leave : old_leaves) {
    if(old_leave > 0)
      parents.insert(old_tree.parent_index(old_leave));
  }

  for(const auto & parent : parents)
  {
    int bit_index_left_child = old_tree.child_index(parent);
    int bit_index_right_child = bit_index_left_child + 1;
    //check if they can be merged
    float side_len = (float)block_len_ / float(1<<curr_depth);
    float left_child_p =  1.0f - (float)std::exp(-alpha_[old_tree.get_data_index(bit_index_left_child)] * side_len);
    float right_child_p =  1.0f - (float)std::exp(-alpha_[old_tree.get_data_index(bit_index_right_child)] * side_len);
    bool merge = std::abs(left_child_p - right_child_p ) < prob_t_; //merge decision |p_left - p_right| < p_t
    if(merge)
      merged_tree.set_bit_at(parent,false);
  }
  return merged_tree;
}


////////////////////////////////////////////////////////////////////////////////
//MAIN MERGE FUNCTION
////////////////////////////////////////////////////////////////////////////////
void bstm_merge_tt_blk(bstm_time_block* t_blk, bstm_block* blk,
                          std::vector<bstm_data_base*> & datas,
                          float prob_thresh)
{
  bstm_merge_tt_function merge_block;
  merge_block.init_data(t_blk, blk, datas, prob_thresh);
  merge_block.merge(datas);
}
