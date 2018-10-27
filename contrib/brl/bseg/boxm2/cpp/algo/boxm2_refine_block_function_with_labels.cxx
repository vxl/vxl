#include <cstdlib>
#include "boxm2_refine_block_function_with_labels.h"
//:
// \file

#define copy_parent_data_ 1

//: initialize generic data base pointers as their data type
template <class T>
bool boxm2_refine_block_function_with_labels<T>::init_data(boxm2_block* blk, std::vector<boxm2_data_base*> & datas, float prob_thresh,
                std::string app_type,std::string flow_prefix)
{
    //store block and pointer to uchar16 3d block
    blk_   = blk;
    app_type_= app_type;
    //store data buffers
    int i=0;
    flow_prefix_= flow_prefix;
    app_type_size_ = sizeof(T);

    alpha_   = (float*)   datas[i++]->data_buffer();

    mog_     = (T*) datas[i++]->data_buffer();

    labels_ =  (short*)   datas[i++]->data_buffer();


    if(datas.size()>=4)
        flow_res_   =  (vnl_vector_fixed<float,4>*)   datas[i++]->data_buffer();
    else
        flow_res_ = nullptr;

    if(datas.size()>=5)
        alpha_sav_   =  (float*)   datas[i++]->data_buffer();
    else
        alpha_sav_ = nullptr;

     if(datas.size()>=6)
        mog_sav_   =  (T*)   datas[i++]->data_buffer();
    else
        mog_sav_ = nullptr;

     if(datas.size()>=7)
        flow_   =  (vnl_vector_fixed<float,4>*)   datas[i++]->data_buffer();
    else
        flow_ = nullptr;



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
template <class T>
bool boxm2_refine_block_function_with_labels<T>::refine_deterministic(std::vector<boxm2_data_base*>& datas)
{
  std::cout<<"CPU deterministic refine:"<<std::endl;

  //loop over each tree, refine it in place (keep a vector of locations for
  // posterities sake
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

      //3. refine tree locally (only updates refined_tree and returns new tree size)
      boct_bit_tree refined_tree = this->refine_bit_tree(curr_tree, currIndex, false);  //i.e. is not random
      int newSize = refined_tree.num_cells();

      //cache refined tree
      std::memcpy (trees_copy[currIndex].data_block(), refined_tree.get_bits(), 16);
      dataSize += newSize;
  }


  //2. allocate new data arrays of the appropriate size
  std::cout<<"Allocating new data blocks"<<std::endl;
  boxm2_cache_sptr cache = boxm2_cache::instance();
  boxm2_scene_sptr scene = cache->get_scenes()[0];
  boxm2_block_id id = datas[0]->block_id();
  char* buf = new char[dataSize * app_type_size_];
  std::cout << "data size is "<<app_type_size_<<" , "<<sizeof(T)<<" and id is "<<app_type_<<std::endl;
  boxm2_data_base* newA = new boxm2_data_base(new char[dataSize * sizeof(float) ], dataSize * sizeof(float), id);
  auto* newM = new boxm2_data_base(new char[dataSize * app_type_size_], dataSize * app_type_size_, id);
  boxm2_data_base* newF = nullptr;
  boxm2_data_base* newF_res = nullptr;
  boxm2_data_base* newA_sav = nullptr;
  boxm2_data_base* newM_sav = nullptr;
  T fills;
  fills.fill(0);


    if (flow_)
          newF = new boxm2_data_base(new char[dataSize * sizeof(float4)], dataSize * sizeof(float4), id);
  if (flow_res_)
          newF_res = new boxm2_data_base(new char[dataSize * sizeof(float4)], dataSize * sizeof(float4), id);
  if (alpha_sav_)
          newA_sav =  new boxm2_data_base(new char[dataSize * sizeof(float) ], dataSize * sizeof(float), id);
  if(mog_sav_)
          newM_sav  = new boxm2_data_base(new char[dataSize * app_type_size_] , dataSize * app_type_size_, id);

  if(flow_res_){

          std::cout<<"optical flow will be moved"<<std::endl;
  }

  if(alpha_sav_ && mog_sav_){

          std::cout<<"saved buffers will be moved"<<std::endl;
  }

  auto*   alpha_cpy = (float*) newA->data_buffer();
  T* mog_cpy   = (T*) newM->data_buffer();
  float4*   flow_cpy = nullptr;
  float4*  flow_res_cpy =nullptr;
  float*   alpha_sav_cpy = nullptr;
  T*  mog_sav_cpy = nullptr;
  if(flow_)
          flow_cpy =(float4*)newF->data_buffer();
  if(flow_res_)
          flow_res_cpy =(float4*)newF_res->data_buffer();
  if(alpha_sav_)
          alpha_sav_cpy = (float*)newA_sav->data_buffer();
  if(mog_sav_)
          mog_sav_cpy = (T*)newM_sav->data_buffer();

  //3. loop through tree again, putting the data in the right place
  std::cout<<"Swapping data into new blocks..."<<std::endl;
  int newInitCount = 0;
  currIndex = 0;
  int count_original=0;
  int count_new = 0;
  int count_9 = 0;
  for (blk_iter = trees.begin(); blk_iter != trees.end(); ++blk_iter, ++currIndex)
  {
      //1. get current tree information
      uchar16 tree  = (*blk_iter);
      boct_bit_tree old_tree( (unsigned char*) tree.data_block(), max_level_);
      count_original += old_tree.num_cells();
      //2. refine tree locally (only updates refined_tree and returns new tree size)
      boct_bit_tree refined_tree( (unsigned char*) trees_copy[currIndex].data_block(), max_level_);

      //2.5 pack data bits into refined tree
      //store data index in bits [10, 11, 12, 13] ;
      int root_index = dataIndex[currIndex];
      refined_tree.set_data_ptr(root_index, false); //is not random

      //3. swap data from old location to new location
      newInitCount += this->move_data(old_tree, refined_tree, alpha_cpy, alpha_sav_cpy,mog_cpy,mog_sav_cpy,flow_cpy,flow_res_cpy);

      //4. store old tree in new tree, swap data out
      std::memcpy(blk_iter, refined_tree.get_bits(), 16);

      int index_x = currIndex/(trees.get_row2_count() * trees.get_row3_count());
      int rem_x= currIndex- index_x*(trees.get_row2_count() * trees.get_row3_count());
      int index_y = rem_x/trees.get_row3_count();
      int rem_y =  rem_x - index_y*trees.get_row3_count();
      int index_z =rem_y;

      count_new+=refined_tree.num_cells();
  }
  blk_->set_trees(trees);

  std::cout<<"Number of new cells: "<<newInitCount<<std::endl;

  std::cout<<"old and new counts "<<count_original<<" "<<count_new<<" "<<std::endl;

  //3. Replace data in the cache



  cache->replace_data_base(scene,id, boxm2_data_traits<BOXM2_ALPHA>::prefix(), newA);
  cache->replace_data_base(scene,id,  app_type_, newM);
  if (flow_)
          cache->replace_data_base(scene,id, boxm2_data_traits<BOXM2_NORMAL>::prefix("lvl"), newF);
  if (flow_res_)
          cache->replace_data_base(scene,id, boxm2_data_traits<BOXM2_NORMAL>::prefix(flow_prefix_), newF_res);
  if(alpha_sav_)
          cache->replace_data_base(scene,id, boxm2_data_traits<BOXM2_ALPHA>::prefix("sav"), newA_sav);
  if(mog_sav_)
          cache->replace_data_base(scene,id, app_type_+"_sav", newM_sav);


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
template <class T>
boct_bit_tree boxm2_refine_block_function_with_labels<T>::refine_bit_tree(boct_bit_tree& unrefined_tree,
                                                           int  /*buff_offset*/,
                                                           bool is_random)
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
      double side_len = block_len_/ double(1<<currDepth);

      //get alpha value for this cell;
      int dataIndex = unrefined_tree.get_data_index(i, is_random);

      float alpha = alpha_[dataIndex];
      float prob = 1 - exp(- alpha * side_len);
      short curr_label = labels_[dataIndex];
      //if (curr_label !=0)

      //integrate alpha value
      float alpha_int = alpha * float(side_len);

      //IF label calls for  split, tack on 8 children to end of tree array
      if (curr_label != 0 && currDepth < max_level_-1)
      {
        //change value of bit_at(i) to 1;
        refined_tree.set_bit_at(i, true);
        //this->recursive_refine(refined_tree,i);


        //std::cout<<"before refinement have label "<<curr_label<<" at "<<dataIndex<<" and depth is "<<currDepth<< " max lvl is "<<max_level_<<std::endl;

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
template <class T>
void boxm2_refine_block_function_with_labels<T>::recursive_refine(boct_bit_tree & tree,int i){

        std::vector<int> leafs= tree.get_leaf_bits(i);
        int count =0;

        for (auto it= leafs.begin();it!=leafs.end();it++,count++){
                if (tree.depth_at(*it)>=2)
                        return;
                tree.set_bit_at(*it,true);
                ++num_split_;
                this->recursive_refine(tree,*it);

        }
        if(count!=8)
                std::cout<<"count was found to be "<<count<<" at level "<<tree.depth_at(i)<<std::endl;
}

//Deterministic move data
//moves data from src to destination
//returns the number of split nodes for this tree (for assertions)
template <class T>
int boxm2_refine_block_function_with_labels<T>::move_data(boct_bit_tree& unrefined_tree,
                                           boct_bit_tree& refined_tree,
                                           float*  alpha_cpy,
                                           float*  alpha_sav_cpy,
                                           T*  mog_cpy,
                                           T*  mog_sav_cpy,
                                           float4* flow_cpy,
                                           float4* flow_res_cpy)
{
  int newSize = refined_tree.num_cells();
/*
  if (newSize!=1)
          std::cout<<"new size is "<<newSize<<std::endl;
*/


  //zip through each leaf cell and
  int oldDataPtr = unrefined_tree.get_data_ptr(false);
  int newDataPtr = refined_tree.get_data_ptr(false);
  int newInitCount = 0;
  int cellsMoved = 0;

  for (int j=0; j<MAX_CELLS_ && newSize<<cellsMoved; ++j)
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
      if(flow_cpy)
          flow_cpy[newDataPtr]    = flow_[oldDataPtr];
      if(flow_res_cpy)
          flow_res_cpy[newDataPtr] = flow_res_[oldDataPtr];
      if(mog_sav_cpy)
          mog_sav_cpy[newDataPtr] = mog_sav_[oldDataPtr];
      if(alpha_sav_cpy)
          alpha_sav_cpy[newDataPtr]= alpha_sav_[oldDataPtr];


      //increment
      ++oldDataPtr;
      ++newDataPtr;
      ++cellsMoved;

    }
    //case where it's a new leaf...
    else if (validCellNew) {
      //move root data to new location
      int parentLevel = unrefined_tree.depth_at(pj);
      double side_len = block_len_ / double(1<<(parentLevel+1));

      int dataIndex = unrefined_tree.get_data_index(pj, false);

      replace_ = true;

      if(labels_[dataIndex] == 1){
          if(replace_){
                  alpha_cpy[newDataPtr]  = float(alpha_[dataIndex] * 2); // (float(-std::log(1.0f - p_init_) / side_len));
                  mog_cpy[newDataPtr]    = mog_[dataIndex];
                  if (flow_cpy)
                          flow_cpy[newDataPtr]    = flow_[dataIndex];
                  if (flow_res_cpy)
                          flow_res_cpy[newDataPtr]= flow_res_[dataIndex];
                  if(mog_sav_cpy)
                          mog_sav_cpy[newDataPtr] = mog_sav_[dataIndex];
                  if(alpha_sav_cpy)
                          alpha_sav_cpy[newDataPtr]= alpha_sav_[dataIndex];
          }

      }else if(labels_[dataIndex] == 2){
          alpha_cpy[newDataPtr]  = float(alpha_[dataIndex] * 2);
          mog_cpy[newDataPtr]    = mog_[dataIndex];
      }
      //update new data pointer
      ++newDataPtr;
      ++newInitCount;
      ++cellsMoved;
    }
  }
  return newInitCount;
}


template <class T>
int boxm2_refine_block_function_with_labels<T>::free_space(int startPtr, int endPtr)
{
  int freeSpace = (startPtr >= endPtr)? startPtr-endPtr : data_len_ - (endPtr-startPtr);
  return freeSpace;
}
////////////////////////////////////////////////////////////////////////////////
//MAIN REFINE FUNCTION
////////////////////////////////////////////////////////////////////////////////


template class boxm2_refine_block_function_with_labels<uchar16>;
template class boxm2_refine_block_function_with_labels<int16>;
template class boxm2_refine_block_function_with_labels<uchar8>;
