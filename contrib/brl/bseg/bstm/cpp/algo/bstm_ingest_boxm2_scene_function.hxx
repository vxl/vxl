#ifndef bstm_ingest_boxm2_scene_function_hxx_
#define bstm_ingest_boxm2_scene_function_hxx_

#include "bstm_ingest_boxm2_scene_function.h"

//#define ALPHA_SCALING
#define DEBUG

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::
    bstm_ingest_boxm2_scene_function(
        bstm_block *blk,
        bstm_time_block *blk_t,
        std::map<std::string, bstm_data_base *> &datas,
        boxm2_block *boxm2_blk,
        std::map<std::string, boxm2_data_base *> &boxm2_datas,
        double local_time,
        double p_threshold,
        double app_threshold) {
  p_threshold_ = p_threshold;
  app_threshold_ = app_threshold;

  init_data(blk, blk_t, datas, boxm2_blk, boxm2_datas, local_time);
  conform();
  ingest();
}

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
bool bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::init_data(
    bstm_block *blk,
    bstm_time_block *blk_t,
    std::map<std::string, bstm_data_base *> &datas,
    boxm2_block *boxm2_blk,
    std::map<std::string, boxm2_data_base *> &boxm2_datas,
    double local_time) {
  local_time_ = local_time;

  // store block and pointer to uchar16 3d block
  blk_ = blk;
  blk_t_ = blk_t;
  boxm2_blk_ = boxm2_blk;

  // store data buffers
  for (std::map<std::string, bstm_data_base *>::const_iterator iter =
           datas.begin();
       iter != datas.end();
       iter++) {
    if (iter->first == bstm_data_traits<BSTM_ALPHA>::prefix("")) { // if alpha,
      alpha_ =
          (bstm_data_traits<BSTM_ALPHA>::datatype *)iter->second->data_buffer();
    } else { // if app model
      apm_model_ = (typename bstm_data_traits<APM_TYPE>::datatype *)
                       iter->second->data_buffer();
    }
  }

  // get boxm2 data buffers
  for (std::map<std::string, boxm2_data_base *>::const_iterator iter =
           boxm2_datas.begin();
       iter != boxm2_datas.end();
       iter++) {
    if (iter->first == boxm2_data_traits<BOXM2_ALPHA>::prefix("")) // if alpha,
      boxm2_alpha_ = (boxm2_data_traits<BOXM2_ALPHA>::datatype *)
                         iter->second->data_buffer();
    else // if app model
      boxm2_apm_model_ =
          (typename boxm2_data_traits<BOXM2_APM_TYPE>::datatype *)
              iter->second->data_buffer();
  }

  // block max level
  max_level_ = blk_->max_level();
  max_level_t_ = blk_t_->max_level();

  // length of one side of a sub block
  block_len_ = blk->sub_block_dim().x();

  // number of time trees
  sub_block_num_t_ = blk_t_->sub_block_num();

  // USE rootlevel to determine MAX_INNER and MAX_CELLS
  if (max_level_t_ == 1) {
    std::cout << "Trying to refine scene with max level 1" << std::endl;
    return true;
  } else if (max_level_t_ == 2) {
    MAX_INNER_CELLS_T_ = 1, MAX_CELLS_T_ = 3;
  } else if (max_level_t_ == 3) {
    MAX_INNER_CELLS_T_ = 3, MAX_CELLS_T_ = 7;
  } else if (max_level_t_ == 4) {
    MAX_INNER_CELLS_T_ = 7, MAX_CELLS_T_ = 15;
  } else if (max_level_t_ == 5) {
    MAX_INNER_CELLS_T_ = 15, MAX_CELLS_T_ = 31;
  } else if (max_level_t_ == 6) {
    MAX_INNER_CELLS_T_ = 31, MAX_CELLS_T_ = 63;
  } else
    std::cerr << "ERROR! No max_level_t_\n";

  // USE rootlevel to determine MAX_INNER and MAX_CELLS
  if (max_level_ == 1) {
    std::cout << "Trying to refine scene with max level 1" << std::endl;
    return true;
  } else if (max_level_ == 2) {
    MAX_INNER_CELLS_ = 1, MAX_CELLS_ = 9;
  } else if (max_level_ == 3) {
    MAX_INNER_CELLS_ = 9, MAX_CELLS_ = 73;
  } else if (max_level_ == 4) {
    MAX_INNER_CELLS_ = 73, MAX_CELLS_ = 585;
  } else
    std::cerr << "ERROR! No max_level_\n";

  // for debugging
  num_split_ = 0;

  return true;
}

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
bool bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::conform() {
  boxm2_array_3d<uchar16> &trees = blk_->trees();
  boxm2_array_3d<uchar16> boxm2_trees = boxm2_blk_->trees_copy();
  uchar16 *trees_copy = new uchar16[trees.size()]; // copy of those trees
  int *dataIndex = new int[trees.size()]; // data index for each new tree
  int currIndex = 0;                      // curr tree being looked at
  int dataSize = 0;                       // running sum of data size

  // 1. loop over each tree, refine it in place
  boxm2_array_3d<uchar16>::iterator blk_iter;
  boxm2_array_3d<uchar16>::const_iterator boxm2_blk_iter;
  for (blk_iter = trees.begin(), boxm2_blk_iter = boxm2_trees.begin();
       blk_iter != trees.end();
       ++blk_iter, ++boxm2_blk_iter, ++currIndex) {
    // 0. store data index for eahc tree.
    dataIndex[currIndex] = dataSize;

    // 1. get current tree information
    uchar16 tree = (*blk_iter);
    boct_bit_tree curr_tree((unsigned char *)tree.data_block(), max_level_);

    uchar16 boxm2_tree = (*boxm2_blk_iter);
    boct_bit_tree boxm2_curr_tree((unsigned char *)boxm2_tree.data_block(),
                                  max_level_);

    // 2. make sure the bstm tree is as refined as boxm2 tree
    boct_bit_tree refined_tree = this->conform_tree(curr_tree, boxm2_curr_tree);
    int newSize = refined_tree.num_cells();

    // cache refined tree
    std::memcpy(
        trees_copy[currIndex].data_block(), refined_tree.get_bits(), 16);
    dataSize += newSize;
  }

  // 2. allocate new time blk of the appropriate size
  bstm_block_id id = blk_->block_id();
  bstm_block_metadata m_data;
  m_data.init_level_t_ = blk_t_->init_level();
  m_data.max_level_t_ = blk_t_->max_level();
  m_data.sub_block_num_t_ = blk_t_->sub_block_num();
  bstm_time_block *newTimeBlk =
      new bstm_time_block(id, m_data, dataSize); // create empty time block

  boxm2_array_1d<uchar8> &new_time_trees =
      newTimeBlk->time_trees(); // refined trees
  // std::cout<<"Number of new time trees: "<<  new_time_trees.size() -
  // blk_t_->time_trees().size() <<std::endl;

  // allocate buffer to hold depth differences, one difference per time tree
  // will be used to scale alpha later on
  char *depth_diff = new char[new_time_trees.size()];
  std::memset(
      depth_diff, 0, sizeof(char) * new_time_trees.size()); // zero out buffer

  // 3. loop through trees again, putting the time trees in the right place
  int newInitCount = 0;
  currIndex = 0;
  for (blk_iter = trees.begin(); blk_iter != trees.end();
       ++blk_iter, ++currIndex) {
    // 1. get current tree information
    uchar16 tree = (*blk_iter);
    boct_bit_tree old_tree((unsigned char *)tree.data_block(), max_level_);

    // 2. refine tree locally (only updates refined_tree and returns new tree
    // size)
    boct_bit_tree refined_tree(
        (unsigned char *)trees_copy[currIndex].data_block(), max_level_);

    // 2.5 pack data bits into refined tree
    // store data index in bits [10, 11, 12, 13] ;
    int root_index = dataIndex[currIndex];
    refined_tree.set_data_ptr(root_index, false); // is not random

    // 3. swap data from old location to new location
    newInitCount +=
        this->move_time_trees(old_tree, refined_tree, newTimeBlk, depth_diff);

    // 4. store old tree in new tree, swap data out
    std::memcpy(blk_iter, refined_tree.get_bits(), 16);
  }

  delete[] dataIndex;
  delete[] trees_copy;

  // 4.  loop over time trees to calculate the size of new data blocks
  //    keep index of data sizes, but don't save into them just yet
  dataIndex = new int[new_time_trees.size()]; // data index for each new tree
  currIndex = 0;                              // curr tree being looked at
  dataSize = 0;                               // running sum of data size
  boxm2_array_1d<uchar8>::iterator time_trees_iter;
  for (time_trees_iter = new_time_trees.begin();
       time_trees_iter != new_time_trees.end();
       ++time_trees_iter, ++currIndex) {
    // 0. store data index for each tree.
    dataIndex[currIndex] = dataSize;

    // 1. get refined tree
    bstm_time_tree new_time_tree(
        (unsigned char *)(*time_trees_iter).data_block(), max_level_t_);

    // 2. save its new size
    int newSize = new_time_tree.num_leaves(); // number of leaves, not all
                                              // cells.
    dataSize += newSize;
  }

  // 5. alloc new data buffers with appropriate size
  bstm_data_base *newA = new bstm_data_base(
      new char[dataSize * bstm_data_traits<BSTM_ALPHA>::datasize()],
      dataSize *bstm_data_traits<BSTM_ALPHA>::datasize(),
      id);

  bstm_data_base *newM = new bstm_data_base(
      new char[dataSize * bstm_data_traits<APM_TYPE>::datasize()],
      dataSize *bstm_data_traits<APM_TYPE>::datasize(),
      id);

  bstm_data_traits<BSTM_ALPHA>::datatype *alpha_cpy =
      (bstm_data_traits<BSTM_ALPHA>::datatype *)newA->data_buffer();
  typename bstm_data_traits<APM_TYPE>::datatype *mog_cpy =
      (typename bstm_data_traits<APM_TYPE>::datatype *)newM->data_buffer();

  // 6. copy data from old data to new buffers
  currIndex = 0;
  for (time_trees_iter = new_time_trees.begin();
       time_trees_iter != new_time_trees.end();
       ++time_trees_iter, ++currIndex) {
    // 1. get refined tree
    bstm_time_tree new_time_tree(
        (unsigned char *)(*time_trees_iter).data_block(), max_level_t_);

    // 2. get data pointer
    int old_data_ptr = new_time_tree.get_data_ptr();
    int new_data_ptr = dataIndex[currIndex];

    // 3. copy data using old ptr to new data buffers
    std::memcpy(
        &(alpha_cpy[new_data_ptr]),
        &(alpha_[old_data_ptr]),
        bstm_data_traits<BSTM_ALPHA>::datasize() *
            new_time_tree.num_leaves()); // number of leaves, not all cells.
    std::memcpy(
        &(mog_cpy[new_data_ptr]),
        &(apm_model_[old_data_ptr]),
        bstm_data_traits<APM_TYPE>::datasize() *
            new_time_tree.num_leaves()); // number of leaves, not all cells.

// scale alpha data using the depth they were copied from
// to be consistent.
#ifdef ALPHA_SCALING
    if (depth_diff[currIndex] > 0)
      for (int i = 0; i < new_time_tree.num_cells(); i++)
        alpha_cpy[new_data_ptr + i] *= float(1 << (int)depth_diff[currIndex]);
#endif

    // 4. correct data ptr
    new_time_tree.set_data_ptr(dataIndex[currIndex]);

    // 5. save it back
    std::memcpy(time_trees_iter, new_time_tree.get_bits(), TT_NUM_BYTES);
  }

  delete[] dataIndex;
  delete[] depth_diff;

  // 7. update cache, replace time trees
  bstm_cache_sptr cache = bstm_cache::instance();
  cache->replace_time_block(id, newTimeBlk);
  cache->replace_data_base(id, bstm_data_traits<BSTM_ALPHA>::prefix(), newA);
  cache->replace_data_base(id, bstm_data_traits<APM_TYPE>::prefix(), newM);

  blk_t_ = newTimeBlk;
  alpha_ = alpha_cpy;
  apm_model_ = mog_cpy;

  return true;
}

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
int bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::move_time_trees(
    boct_bit_tree &unrefined_tree,
    boct_bit_tree &refined_tree,
    bstm_time_block *newTimeBlk,
    char *depth_diff) {
  int newSize = refined_tree.num_cells();

  // zip through each leaf cell and
  int oldDataPtr = unrefined_tree.get_data_ptr(false);
  int newDataPtr = refined_tree.get_data_ptr(false);
  int newInitCount = 0;
  int cellsMoved = 0;

  for (int j = 0; j < MAX_CELLS_ && cellsMoved < newSize; ++j) {
    // if parent bit is 1, then you're a valid cell
    int pj = unrefined_tree.parent_index(j); // Bit_index of parent bit
    bool validCellOld = (j == 0) || unrefined_tree.bit_at(pj);
    bool validCellNew = (j == 0) || refined_tree.bit_at(pj);
    if (validCellOld && validCellNew) {
      // move data to new location

      boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > old_time_trees =
          blk_t_->get_cell_all_tt(oldDataPtr); // get all tt from prev. loc
      // newTimeBlk->set_cell_all_tt(newDataPtr,old_time_trees); //set all tt to
      // new loc

      if (!refined_tree.is_leaf(j)) // if the time trees being copied belong to
                                    // an inner cell, erase their time trees
      {
        vnl_vector_fixed<unsigned char, 8> *erased_old_time_trees =
            new vnl_vector_fixed<unsigned char, 8>[sub_block_num_t_];
        for (unsigned int t_idx = 0; t_idx < sub_block_num_t_; ++t_idx) {
          bstm_time_tree tmp_tree(
              old_time_trees[t_idx]
                  .data_block()); // create tree by copying the tree data
          tmp_tree.erase_cells(); // erase all cells except for root.
          erased_old_time_trees[t_idx].set(
              tmp_tree.get_bits()); // copy to erased_old_time_trees
        }
        newTimeBlk->set_cell_all_tt(
            newDataPtr,
            boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> >(
                sub_block_num_t_,
                erased_old_time_trees)); // set all tt to new loc
        delete[] erased_old_time_trees;
      } else // if the time trees being copied belong to a leaf, copy them as
             // they are.
        newTimeBlk->set_cell_all_tt(newDataPtr,
                                    old_time_trees); // set all tt to new loc

      // increment
      ++oldDataPtr;
      ++newDataPtr;
      ++cellsMoved;
    }
    // case where it's a new leaf...
    else if (validCellNew) {
      // find parent in old tree
      int valid_parent_bit = pj;
      while (
          valid_parent_bit != 0 &&
          !unrefined_tree.bit_at(unrefined_tree.parent_index(valid_parent_bit)))
        valid_parent_bit = unrefined_tree.parent_index(valid_parent_bit);

      int parent_dataPtr =
          unrefined_tree.get_data_index(valid_parent_bit, false);

      // move root data to new location
      boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > old_time_trees =
          blk_t_->get_cell_all_tt(parent_dataPtr); // get all tt from root loc
      newTimeBlk->set_cell_all_tt(
          newDataPtr, old_time_trees); // set all tt to new loc(child)

      // save depth differences
      for (unsigned int i = 0; i < sub_block_num_t_; ++i)
        depth_diff[newDataPtr + i] =
            char(refined_tree.depth_at(j) -
                 unrefined_tree.depth_at(valid_parent_bit));

      // update new data pointer
      ++newDataPtr;
      ++newInitCount;
      ++cellsMoved;
    }
  }

  return newInitCount;
}

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
boct_bit_tree
bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::conform_tree(
    boct_bit_tree curr_tree, boct_bit_tree boxm2_curr_tree) {
  boct_bit_tree refined_tree(curr_tree.get_bits(),
                             max_level_); // initialize tree to return

  for (int i = 0; i < MAX_INNER_CELLS_;
       ++i) //(iterate through the max number of inner cells)
    if (boxm2_curr_tree.bit_at(i) == 1 &&
        refined_tree.bit_at(i) ==
            0) // if boxm2 cell is divided, also divide bstm cell
      refined_tree.set_bit_at(i, true);

  return refined_tree;
}

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
bool bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::ingest() {
  // loop over each tree in blk and boxm2_blk.
  //--loop over each cell in the bstm tree
  //----loop over cell's time trees.
  //------if time tree does contain local_time,
  //--------refine time tree(boxm2 dataptr, time tree)
  //----keep record of the datasize for alpha, mog, numobs etc.

  const boxm2_array_3d<uchar16> &trees = blk_->trees();
  const boxm2_array_3d<uchar16> &boxm2_trees = boxm2_blk_->trees();
  boxm2_array_1d<uchar8> &time_trees =
      blk_t_->time_trees(); // time trees to refine

  // make a copy of the time trees in blk_t_
  uchar8 *time_tree_copy_buffer = new uchar8[time_trees.size()];
  boxm2_array_1d<uchar8> time_trees_blk_copy(time_trees.size(),
                                             time_tree_copy_buffer);
  for (unsigned int i = 0; i < time_trees.size(); ++i)
    std::memcpy(time_trees_blk_copy[i].data_block(),
                time_trees[i].data_block(),
                TT_NUM_BYTES);

  int *dataIndex = new int[time_trees.size()]; // data index for each new tree
  int dataSize = 0;                            // running sum of data size
  int currIndex = 0;

  // alloc new data buffers with appropriate size
  bstm_block_id id = blk_->block_id();
  bstm_cache_sptr cache = bstm_cache::instance();
  bstm_data_base *change_buffer = cache->get_data_base_new(
      id,
      bstm_data_traits<BSTM_CHANGE>::prefix(),
      blk_t_->tree_buff_length() * bstm_data_traits<BSTM_CHANGE>::datasize());

  change_array_ =
      (bstm_data_traits<BSTM_CHANGE>::datatype *)change_buffer->data_buffer();

  int tree_index = 0;
  boxm2_array_3d<uchar16>::const_iterator blk_iter, boxm2_blk_iter;
  for (blk_iter = trees.begin(), boxm2_blk_iter = boxm2_trees.begin();
       blk_iter != trees.end();
       ++blk_iter, ++boxm2_blk_iter) {
    // load boct trees from both blk and boxm2_blk
    uchar16 tree = (*blk_iter);
    boct_bit_tree curr_tree((unsigned char *)tree.data_block(), max_level_);

    uchar16 boxm2_tree = (*boxm2_blk_iter);
    boct_bit_tree boxm2_curr_tree((unsigned char *)boxm2_tree.data_block(),
                                  max_level_);

    int num_cells = curr_tree.num_cells();
    int num_processed_cells = 0;
    for (int i = 0; i < MAX_CELLS_ && num_processed_cells < num_cells; ++i) {
      int pi = (i - 1) >> 3; // Bit_index of parent bit
      if ((i == 0) || curr_tree.bit_at(pi)) {
        bool is_leaf = !curr_tree.bit_at(i);

        // it might be that the bstm cell is further divided than the boxm2
        // cell.
        int i_boxm2 = i;
        while (i_boxm2 != 0 &&
               !boxm2_curr_tree.bit_at(boxm2_curr_tree.parent_index(i_boxm2)))
          i_boxm2 = boxm2_curr_tree.parent_index(i_boxm2);

        // get data ptr for both trees
        int bstm_data_offset = curr_tree.get_data_index(i, false);
        int boxm2_data_offset = boxm2_curr_tree.get_data_index(i_boxm2, false);

        // refine all the time trees associated with curr cell.
        this->refine_all_time_trees(bstm_data_offset,
                                    boxm2_data_offset,
                                    dataIndex,
                                    currIndex,
                                    dataSize,
                                    curr_tree.depth_at(i),
                                    boxm2_curr_tree.depth_at(i_boxm2),
                                    is_leaf);
        num_processed_cells++;
      }
    }
    tree_index++;
  }

  // std::cout << "New data size is " << dataSize << std::endl;

  bstm_data_base *newA = new bstm_data_base(
      new char[dataSize * bstm_data_traits<BSTM_ALPHA>::datasize()],
      dataSize *bstm_data_traits<BSTM_ALPHA>::datasize(),
      id);
  bstm_data_base *newM = new bstm_data_base(
      new char[dataSize * bstm_data_traits<APM_TYPE>::datasize()],
      dataSize *bstm_data_traits<APM_TYPE>::datasize(),
      id);
  bstm_data_traits<BSTM_ALPHA>::datatype *alpha_cpy =
      (bstm_data_traits<BSTM_ALPHA>::datatype *)newA->data_buffer();
  typename bstm_data_traits<APM_TYPE>::datatype *apm_cpy =
      (typename bstm_data_traits<APM_TYPE>::datatype *)newM->data_buffer();

  // loop over each tree in blk and boxm2_blk.
  //--loop over each time tree
  //----copy everything from old tree's data
  //----except for the current cell, copy that from boxm2 data.
  currIndex = 0;
  int newInitCount = 0;

  for (blk_iter = trees.begin(), boxm2_blk_iter = boxm2_trees.begin();
       blk_iter != trees.end();
       ++blk_iter, ++boxm2_blk_iter) {
    // load boct trees from both blk and boxm2_blk
    uchar16 tree = (*blk_iter);
    boct_bit_tree curr_tree((unsigned char *)tree.data_block(), max_level_);

    uchar16 boxm2_tree = (*boxm2_blk_iter);
    boct_bit_tree boxm2_curr_tree((unsigned char *)boxm2_tree.data_block(),
                                  max_level_);

    int num_cells = curr_tree.num_cells();
    int num_processed_cells = 0;
    for (int i = 0; i < MAX_CELLS_ && num_processed_cells < num_cells; ++i) {
      int pi = (i - 1) >> 3; // Bit_index of parent bit
      if ((i == 0) || curr_tree.bit_at(pi)) {
        // it might be that the bstm cell is further divided than the boxm2
        // cell.
        int i_boxm2 = i;
        while (i_boxm2 != 0 &&
               !boxm2_curr_tree.bit_at(boxm2_curr_tree.parent_index(i_boxm2)))
          i_boxm2 = boxm2_curr_tree.parent_index(i_boxm2);

        // get data ptr for both trees
        int bstm_data_offset = curr_tree.get_data_index(i, false);
        int boxm2_data_offset = boxm2_curr_tree.get_data_index(i_boxm2, false);

        int depth_diff =
            curr_tree.depth_at(i) - boxm2_curr_tree.depth_at(i_boxm2);
        newInitCount += move_all_time_trees_data(time_trees_blk_copy,
                                                 bstm_data_offset,
                                                 boxm2_data_offset,
                                                 dataIndex,
                                                 currIndex,
                                                 alpha_cpy,
                                                 apm_cpy,
                                                 depth_diff);
        num_processed_cells++;
      }
    }
  }

  // std::cout<<"Number of new cells: "<<newInitCount<<std::endl;

  // replace databases
  cache->replace_data_base(id, bstm_data_traits<BSTM_ALPHA>::prefix(), newA);
  cache->replace_data_base(id, bstm_data_traits<APM_TYPE>::prefix(), newM);

  delete[] time_tree_copy_buffer;
  delete[] dataIndex;

  return true;
}

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
int bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::
    move_all_time_trees_data(
        boxm2_array_1d<uchar8> &time_trees_blk_copy,
        int bstm_data_offset,
        int boxm2_data_offset,
        int *dataIndex,
        int &currIndex,
        bstm_data_traits<BSTM_ALPHA>::datatype *alpha_cpy,
        typename bstm_data_traits<APM_TYPE>::datatype *apm_cpy,
        int depth_diff) {
  // load original time trees
  boxm2_array_1d<uchar8> time_trees_copy(
      sub_block_num_t_,
      &(time_trees_blk_copy[bstm_data_offset * sub_block_num_t_]));

  boxm2_array_1d<uchar8> time_trees_refined =
      blk_t_->get_cell_all_tt(bstm_data_offset);

  // zip thru time trees
  int newInitCount = 0;
  for (unsigned int t = 0; t < time_trees_refined.size(); ++t) {
    bstm_time_tree refined_tree(time_trees_refined[t].data_block(),
                                max_level_t_);
    bstm_time_tree original_tree(time_trees_copy[t].data_block(), max_level_t_);

    // set the root data pointer of the refined tree
    int root_index = dataIndex[currIndex];
    refined_tree.set_data_ptr(root_index);

    newInitCount +=
        this->move_data(original_tree, refined_tree, alpha_cpy, apm_cpy);

    if (t ==
        blk_t_->tree_index(
            local_time_)) // if this time tree contains the queried time
    {
      float cell_min, cell_max;
      refined_tree.cell_range(
          refined_tree.traverse(local_time_ - blk_t_->tree_index(local_time_)),
          cell_min,
          cell_max);
      if (cell_min ==
          local_time_ -
              blk_t_->tree_index(local_time_)) { // if the current time is the
                                                 // start of a cell in which new
                                                 // data will be placed
        this->place_curr_data(
            refined_tree, boxm2_data_offset, alpha_cpy, apm_cpy, depth_diff);
      }
    }

    // make sure to write the refined bits to blk_t_
    uchar8 refined_bits(refined_tree.get_bits());
    blk_t_->set_cell_tt(bstm_data_offset, refined_bits, t);

    currIndex++;
  }
  return newInitCount;
}

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
bool bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::
    refine_all_time_trees(int bstm_data_offset,
                          int boxm2_data_offset,
                          int *dataIndex,
                          int &currIndex,
                          int &dataSize,
                          int currDepth,
                          int currDepth_boxm2,
                          bool is_leaf) {
  bool refined_any_time_tree = false;
  // zip thru time trees
  int newSize = 0;
  boxm2_array_1d<uchar8> all_time_trees =
      blk_t_->get_cell_all_tt(bstm_data_offset);
  for (unsigned int t = 0; t < all_time_trees.size(); ++t) {
    dataIndex[currIndex] = dataSize;

    bstm_time_tree tmp_tree(all_time_trees[t].data_block(), max_level_t_);
    // if this time tree contains the queried time and it is a leaf.
    // if not a leaf, don't bother refining its time tree to save space.
    if (t == blk_t_->tree_index(local_time_) && is_leaf) {
      bstm_time_tree refined_t_tree = this->refine_time_tree(tmp_tree,
                                                             bstm_data_offset,
                                                             boxm2_data_offset,
                                                             currDepth,
                                                             currDepth_boxm2);
      uchar8 refined_bits(refined_t_tree.get_bits());
      blk_t_->set_cell_tt(
          bstm_data_offset, refined_bits, t); // save it in time block
      newSize =
          refined_t_tree.num_leaves(); // count up the number of cells needed
    } else
      newSize = tmp_tree.num_leaves(); // count up the number of cells needed

    if (tmp_tree.num_cells() != newSize)
      refined_any_time_tree = true;

    dataSize += newSize;
    currIndex++;
  }
  return refined_any_time_tree;
}

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
void bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::
    place_curr_data(bstm_time_tree &refined_tree,
                    int boxm2_data_offset,
                    bstm_data_traits<BSTM_ALPHA>::datatype *alpha_cpy,
                    typename bstm_data_traits<APM_TYPE>::datatype *apm_cpy,
                    int
#ifdef ALPHA_SCALING
                        depth_diff
#endif
                    ) {
  float trees_local_time = local_time_ - blk_t_->tree_index(local_time_);
  int new_ptr =
      refined_tree.get_data_index(refined_tree.traverse(trees_local_time));

#ifdef DEBUG
  if (new_ptr == -1)
    std::cerr << "ERROR, data index is -1\n";
#endif

  alpha_cpy[new_ptr] = boxm2_alpha_[boxm2_data_offset];
  apm_cpy[new_ptr] = boxm2_apm_model_[boxm2_data_offset];

#ifdef ALPHA_SCALING
  alpha_cpy[new_ptr] *= float(
      1 << (int)depth_diff); // scale alpha to be consistent with its depth.
#endif
}

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
bstm_time_tree
bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::refine_time_tree(
    const bstm_time_tree &input_tree,
    int bstm_data_offset,
    int boxm2_data_offset,
    int currDepth,
    int currDepth_boxm2) {
  // initialize tree to return
  bstm_time_tree refined_tree(input_tree.get_bits(), max_level_t_);

  float trees_local_time = local_time_ - blk_t_->tree_index(local_time_);

  if (currDepth < currDepth_boxm2)
    std::cout << "ERROR: boxm2 and bstm depths don't match!" << std::endl;

  // first, query for boxm2 data
  double side_len_boxm2 = block_len_ / double(1 << currDepth_boxm2);
  boxm2_data_traits<BOXM2_ALPHA>::datatype boxm2_curr_alpha =
      boxm2_alpha_[boxm2_data_offset];
  float boxm2_p = 1 - std::exp(-boxm2_curr_alpha * side_len_boxm2);
  typename boxm2_data_traits<BOXM2_APM_TYPE>::datatype boxm2_mog =
      boxm2_apm_model_[boxm2_data_offset];

  // and then bstm data
  double side_len = block_len_ / double(1 << currDepth);
  int data_offset =
      refined_tree.get_data_index(refined_tree.traverse(trees_local_time));

#ifdef DEBUG
  if (data_offset == -1)
    std::cerr << "ERROR, data index is -1!\n";
#endif

  bstm_data_traits<BSTM_ALPHA>::datatype alpha = alpha_[data_offset];
  float p = 1 - std::exp(-alpha * side_len);
  typename bstm_data_traits<APM_TYPE>::datatype mog = apm_model_[data_offset];

  // save change of probabilities
  change_array_[bstm_data_offset] = std::fabs(boxm2_p - p);

  if (is_similar(p, mog, boxm2_p, boxm2_mog))
    return refined_tree;
  else // need to refine time tree
  {
    bool split_complete = false;
    while (!split_complete) {
      int curr_cell = refined_tree.traverse(trees_local_time);
      int currDepth = refined_tree.depth_at(curr_cell);

      float cell_min, cell_max;
      refined_tree.cell_range(curr_cell, cell_min, cell_max);
      if (cell_min == trees_local_time) // found cell starting at queried time.
        split_complete = true;          // we're done here.
      else if (currDepth < TT_NUM_LVLS - 1) {
        refined_tree.set_bit_at(curr_cell, true); // split curr_cell
        ++num_split_t_;
      } else // reached end of tree...
        split_complete = true;
    }

    return refined_tree;
  }
}

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
bool bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::is_similar(
    float p,
    typename bstm_data_traits<APM_TYPE>::datatype mog,
    float boxm2_p,
    typename boxm2_data_traits<BOXM2_APM_TYPE>::datatype boxm2_mog) {
  return bstm_similarity_traits<APM_TYPE, BOXM2_APM_TYPE>::is_similar(
      mog, boxm2_mog, p, boxm2_p, p_threshold_, app_threshold_);
}

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
int bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::move_data(
    bstm_time_tree &unrefined_tree,
    bstm_time_tree &refined_tree,
    bstm_data_traits<BSTM_ALPHA>::datatype *alpha_cpy,
    typename bstm_data_traits<APM_TYPE>::datatype *apm_cpy) {
  int newInitCount = 0;

  std::vector<int> new_leaves = refined_tree.get_leaf_bits();
  std::vector<int> old_leaves = unrefined_tree.get_leaf_bits();

  for (int & new_leave : new_leaves) {
    // get new data ptr
    int newDataPtr = refined_tree.get_data_index(new_leave);

    // find out if this leaf exists in the unrefined tree as well
    int pj = unrefined_tree.parent_index(new_leave); // Bit_index of parent bit
    bool validCellOld = (new_leave == 0) || unrefined_tree.bit_at(pj);

    int oldDataPtr;
    if (validCellOld) // if they both exist
      oldDataPtr = unrefined_tree.get_data_index(new_leave);
    else {
      // find parent in old tree
      int valid_parent_bit = pj;
      while (
          valid_parent_bit != 0 &&
          !unrefined_tree.bit_at(unrefined_tree.parent_index(valid_parent_bit)))
        valid_parent_bit = unrefined_tree.parent_index(valid_parent_bit);

      oldDataPtr = unrefined_tree.get_data_index(valid_parent_bit);

      // increment new cell count
      newInitCount++;
    }
    // copy data
    alpha_cpy[newDataPtr] = alpha_[oldDataPtr];
    apm_cpy[newDataPtr] = apm_model_[oldDataPtr];
  }
  return newInitCount;
}

// OLD DEPRECATED CODE
#if 0

template <bstm_data_type APM_TYPE, boxm2_data_type BOXM2_APM_TYPE>
int bstm_ingest_boxm2_scene_function<APM_TYPE, BOXM2_APM_TYPE>::move_data(bstm_time_tree& unrefined_tree, bstm_time_tree& refined_tree,
                                                                          bstm_data_traits<BSTM_ALPHA>::datatype*  alpha_cpy,
                                                                          typename bstm_data_traits<APM_TYPE>::datatype * apm_cpy )
{
  int newSize = refined_tree.num_cells();

  //zip through each leaf cell and
  int oldDataPtr = unrefined_tree.get_data_ptr();
  int newDataPtr = refined_tree.get_data_ptr();
  int newInitCount = 0;
  int cellsMoved = 0;
  for (int j=0; j<MAX_CELLS_T_ && cellsMoved<newSize; ++j)
  {
    //--------------------------------------------------------------------
    //4 Cases:
    // - Old cell and new cell exist - transfer data over
    // - new cell exists, old cell doesn't - create new occupancy based on depth
    // - old cell exists, new cell doesn't - uh oh this is bad news
    // - neither cell exists - do nothing and carry on
    //--------------------------------------------------------------------
    //if parent bit is 1, then you're a valid cell
    int pj = unrefined_tree.parent_index(j);           //Bit_index of parent bit
    bool validCellOld = (j==0) || unrefined_tree.bit_at(pj);
    bool validCellNew = (j==0) || refined_tree.bit_at(pj);
    if (validCellOld && validCellNew) {
      //move root data to new location
      alpha_cpy[newDataPtr]  = alpha_[oldDataPtr];
      apm_cpy[newDataPtr]  = apm_model_[oldDataPtr];

      //increment
      ++oldDataPtr;
      ++newDataPtr;
      ++cellsMoved;
    }
    //case where it's a new leaf...
    else if (validCellNew) {
      //find parent in old tree
      int valid_parent_bit = pj;
      while ( valid_parent_bit !=0 && !unrefined_tree.bit_at( unrefined_tree.parent_index(valid_parent_bit) ) )
        valid_parent_bit = unrefined_tree.parent_index(valid_parent_bit);

      int parent_data_ptr = unrefined_tree.get_data_index(valid_parent_bit);

#ifdef DEBUG
  if (parent_data_ptr == -1)
    std::cerr << "ERROR, data index is -1\n";
#endif

      alpha_cpy[newDataPtr]    = alpha_[ parent_data_ptr ];
      apm_cpy[newDataPtr]      = apm_model_[parent_data_ptr];

      //update new data pointer
      ++newDataPtr;
      ++newInitCount;
      ++cellsMoved;
    }
  }
  return newInitCount;
}
#endif

#endif // bstm_ingest_boxm2_scene_function_hxx_
