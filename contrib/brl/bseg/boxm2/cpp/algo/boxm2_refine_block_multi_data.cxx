#include <iostream>
#include <algorithm>
#include "boxm2_refine_block_multi_data.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_data_traits.h>
//:
// \file
// need to define again for return value
typedef vnl_vector_fixed<unsigned char, 16> uchar16;

#define COPY_PARENT_DATA 1

uchar16 boxm2_refine_block_multi_data::fully_refined(int depth, int& data_size){
  uchar16 ret; data_size = 0;
  ret.fill((unsigned char)(0));
  if(depth >3){
    std::cout << "FATAL! - depth > 3\n";
    return ret;
  }
  if(depth == 0){
    data_size = 1;
    return ret;
  }
  if(depth == 1){
    ret[0]=(unsigned char)(1);
    data_size = 9;
    return ret;
  }
  if(depth ==2){
    ret[0]=(unsigned char)(1);
    ret[1]=(unsigned char)(255);
    data_size = 73;
    return ret;
  }

  ret[0]=(unsigned char)(1);
  for(unsigned i=1; i<10; ++i)
    ret[i]=(unsigned char)(255);
  data_size = 585;
  return ret;

}

//: initialize data base pointers and associated data buffers for each prefix (data type)
bool boxm2_refine_block_multi_data::init_data(boxm2_scene_sptr scene, const boxm2_block_sptr& blk, std::vector<std::string> const& prefixes, float prob_thresh)
{
  //the prefix set must include "alpha" , occupation density
  std::vector<std::string>::const_iterator pit;
  pit = std::find(prefixes.begin(), prefixes.end(),"alpha");
  if(pit == prefixes.end()){
    std::cerr << "FATAL! In refine - the data base for alpha must be present\n" << std::endl;
    return false;
  }
  // the index in the prefix list corresponding to alpha
  alpha_index_ = static_cast<int>(pit-prefixes.begin());

  //store block and pointer to uchar16 3d block
  scene_ = scene;
  blk_   = blk;
  boxm2_block_id& id = blk_->block_id();

  // cache the databases and data buffers before refine
  dbs_.clear();
  old_bufs_.clear();
  for(const auto & prefixe : prefixes){
    boxm2_data_base* db = boxm2_cache::instance()->get_data_base(scene,id,prefixe);
    if(prefixe == "alpha")
      alpha_   = (float*)   db->data_buffer();
    db->enable_write();
    dbs_.push_back(db);
    old_bufs_.push_back(db->data_buffer());
  }

  //block max level corrsponds to max depth + 1
  max_level_ = blk_->max_level();

  //max alpha integrated,  p_occlusion = (1 - e^-(alpha*length))
  // for length == 1, alpha = -log((1- p_occlusion) )
  max_alpha_int_ = -std::log(1.f - prob_thresh);

  //Data length now is constant
  data_len_ = 65536;

  //length of one side of a sub block
  block_len_ = blk_->sub_block_dim().x();

  //USE rootlevel to determine MAX_INNER and MAX_CELLS
  if (max_level_ == 1) {
    std::cout<<"Trying to refine scene with max level 1"<<std::endl;
    return false;
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

// the term deterministic is anachronistic but kept for historical reasons
// earlier forms of refine assigned data locations randomly vs. determinisically
bool boxm2_refine_block_multi_data::refine_deterministic(std::vector<std::string> const& prefixes)
{
  std::cout<<"CPU deterministic refine:"<<std::endl;
  // get a copy of the blocks's trees
  //loop over each tree, refine it in place
  boxm2_array_3d<uchar16> trees = blk_->trees_copy();  //trees to refine
  auto* trees_copy = new uchar16[trees.size()];  //copy of those trees
  int* dataIndex = new int[trees.size()];           //data index for each new tree
  int currIndex = 0;                                //curr tree being looked at
  int dataSize = 0;                                 //running sum of data size
  boxm2_array_3d<uchar16>::iterator blk_iter;
  for (blk_iter = trees.begin(); blk_iter != trees.end(); ++blk_iter, ++currIndex)
  {
      //0. store data index for each tree.
      dataIndex[currIndex] = dataSize;

      //1. get current tree information
      uchar16 tree  = (*blk_iter);
      boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), max_level_);

      //3. refine tree locally (only updates refined_tree and returns new tree size)
      boct_bit_tree refined_tree = this->refine_bit_tree(curr_tree, 0);
      int newSize = refined_tree.num_cells();

      //cache refined tree
      std::memcpy (trees_copy[currIndex].data_block(), refined_tree.get_bits(), 16);
      dataSize += newSize;
  }


  //2. allocate new data arrays of the appropriate size
  std::cout<<"Allocating new data blocks"<<std::endl;
  boxm2_block_id id = dbs_[0]->block_id();
  std::vector<boxm2_data_base *> new_dbs;
  std::vector<std::size_t> type_sizes;
  // get data buffers for old and new databases
  std::vector<char*> new_bufs;
  auto pit = prefixes.begin();
  auto dit = dbs_.begin();
  for(; dit!=dbs_.end(); ++dit, ++pit){
    std::size_t type_size = boxm2_data_info::datasize(*pit);
    type_sizes.push_back(type_size);
    boxm2_data_base* new_db = new boxm2_data_base(new char[dataSize *type_size ], dataSize * type_size, id);
    new_db->enable_write();
    new_bufs.push_back(new_db->data_buffer());
    new_dbs.push_back(new_db);
  }
  //3. loop through tree again, putting the data in the right place
  std::cout<<"Swapping data into new databases..."<<std::endl;
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
      newInitCount += this->move_data(old_tree, refined_tree, type_sizes, new_bufs);

      //4. store old tree in new tree, swap data out
      std::memcpy(blk_iter, refined_tree.get_bits(), 16);
  }
  blk_->set_trees(trees);
  std::cout<<"Number of new cells for alpha refine: "<<newInitCount<<std::endl;

  //3. Replace data in the cache
  boxm2_cache_sptr cache = boxm2_cache::instance();
  dit = new_dbs.begin();
  pit = prefixes.begin();
  for(; dit!=new_dbs.end(); ++dit, ++pit){
    cache->replace_data_base(scene_, id, *pit, *dit);
}
 return true;
}

/////////////////////////////////////////////////////////////////
////Refine Tree (refines local tree based on an alpha threshold)
////Depth first search iteration of the tree (keeping track of node level)
////1) parent pointer, 2) child pointer 3) data pointer 4) nothing right now
// Kind of a weird mix of functions - the tree structure is modified locally,
// so no tree_buffer information is needed, whereas the data is modified
// on the global level, so buffers, offsets are used
/////////////////////////////////////////////////////////////////
boct_bit_tree boxm2_refine_block_multi_data::refine_bit_tree(boct_bit_tree& unrefined_tree,
                                                                      int  /*buff_offset*/)
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
      int dataIndex = unrefined_tree.get_data_index(i);

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
// fully refine each tree to match the depth specified by depths_to_match
// create new databases to hold the increased data size
// copy the data from the unrefined cells to the new cells if COPY_PARENT_DATA !=0 othewise set the new data elements to zero
bool boxm2_refine_block_multi_data::match_refine(std::vector<std::string> const& prefixes,vbl_array_3d<int> const& depths_to_match){
  // get a copy of the block trees
  boxm2_array_3d<uchar16> trees = blk_->trees_copy();
  int dataIndex = 0;
  int dataSize = 0;                                 //running sum of data size
  int max_level = blk_->max_level();
  // loop over the trees and refine to the specified level
  int nx = static_cast<int>(trees.get_row1_count()), ny = static_cast<int>(trees.get_row2_count()), nz = static_cast<int>(trees.get_row3_count());
  for(int ix = 0; ix<nx; ++ix)
    for(int iy = 0; iy<ny; ++iy)
      for(int iz = 0; iz<nz; ++iz){
        int dsize = 0;
        dataIndex = dataSize; // current data pointer
        // get tree from block
        uchar16& tree_bits  = trees(ix, iy, iz);
        boct_bit_tree cur_tree( (unsigned char*)tree_bits.data_block(), max_level); // the bit tree with the required refinement
        int required_depth = depths_to_match(ix, iy, iz);
        int cur_depth = cur_tree.depth();
        //if the tree refinement is the same,just update the data pointer bits
        if(cur_depth == required_depth){
          cur_tree.set_data_ptr(dataIndex, false);
          std::memcpy(tree_bits.data_block(), cur_tree.get_bits(), 16);
          dataSize += cur_tree.num_cells();
          continue;
        }
        //else set the tree bits to the required refinement and also update the data pointer bits
        uchar16 refined_bits = fully_refined(required_depth, dsize);
        boct_bit_tree refined_tree( (unsigned char*)refined_bits.data_block(), max_level);
        refined_tree.set_data_ptr(dataIndex, false);
        std::memcpy(tree_bits.data_block(), refined_tree.get_bits(), 16);
        dataSize += dsize;
      }
  //2. allocate new data arrays of the appropriate size for the block database
  std::cout<<"Allocating new data blocks"<<std::endl;
  boxm2_block_id id = dbs_[0]->block_id();
  std::vector<boxm2_data_base *> new_dbs;
  std::vector<std::size_t> type_sizes;
  // get data buffers for old and new databases
  std::vector<char*> new_bufs;
  auto pit = prefixes.begin();
  auto dit = dbs_.begin();
  for(; dit!=dbs_.end(); ++dit, ++pit){
    std::size_t type_size = boxm2_data_info::datasize(*pit);
    type_sizes.push_back(type_size);
    boxm2_data_base* new_db = new boxm2_data_base(new char[dataSize *type_size ], dataSize * type_size, id);
    new_db->enable_write();
    new_bufs.push_back(new_db->data_buffer());
    new_dbs.push_back(new_db);
  }
  //3. loop through tree again, putting the data in the right place
  int newInitCount = 0;
  std::cout<<"Swapping data into new blocks..."<<std::endl;
  boxm2_array_3d<uchar16> old_trees = blk_->trees_copy();
  for(int ix = 0; ix<nx; ++ix)
    for(int iy = 0; iy<ny; ++iy)
      for(int iz = 0; iz<nz; ++iz){
        uchar16 old_tree_bits  = old_trees(ix, iy, iz);
        boct_bit_tree old_tree( (unsigned char*) old_tree_bits.data_block(), max_level);
        uchar16 new_tree_bits = trees(ix, iy, iz);
        boct_bit_tree new_tree( (unsigned char*) new_tree_bits.data_block(), max_level);
        newInitCount += this->move_data(old_tree, new_tree, type_sizes, new_bufs);
      }
  blk_->set_trees(trees);
  //3. Replace data in the cache
  boxm2_cache_sptr cache = boxm2_cache::instance();
  dit = new_dbs.begin();
  pit = prefixes.begin();
  for(; dit!=new_dbs.end(); ++dit, ++pit){
    cache->replace_data_base(scene_, id, *pit, *dit);
  }
  std::cout<<"Number of new cells for match refine: "<<newInitCount<<std::endl;
 return true;
}

//moves data from unrefined tree to refined tree
//returns the number of split nodes for this tree (for assertions)
//vector of old databases is the member dbs_;
//new_dbs will have data for the refined tree
// if the symbol COPY_PARENT_DATA is defined as non-zero the parent data will be copied
// otherwise the cell data is set to zero
int boxm2_refine_block_multi_data::move_data(boct_bit_tree& unrefined_tree,
                                                      boct_bit_tree& refined_tree,
                                                      std::vector<std::size_t> const& type_sizes,
                                                      std::vector<char*>& new_bufs)
{
  std::size_t n = type_sizes.size();
  if(n != old_bufs_.size()||n!=new_bufs.size()){
    std::cout << "!FATAL - n type sizes not equal to number of buffers\n";
    return -1;
  }
  // get data indexes
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
      for(int i = 0; i<static_cast<int>(n); ++i){
        char* old_buf = old_bufs_[i];
        char* new_buf = new_bufs[i];
        int ptr_inc = static_cast<int>(type_sizes[i]);
        //move root data to new location copying the number of bytes appropriate
        //for each database type (ptr_inc)
        char* old_byte_ptr = old_buf + oldDataPtr*ptr_inc;
        char* new_byte_ptr = new_buf + newDataPtr*ptr_inc;
        std::memcpy(new_byte_ptr, old_byte_ptr, ptr_inc);
      }
      //increment data pointers
      ++oldDataPtr;
      ++newDataPtr;
      ++cellsMoved;
    }
    //case where it's a new leaf, so copy the parent data if enabled
    //the alpha value is not copied but assigned a new value
    //appropriate for the length of the new cell
    else if (validCellNew) {
      //move root data to new location
      int parentLevel = unrefined_tree.depth_at(pj);
      double side_len = block_len_ / double(1<<parentLevel);
      int dataIndex = unrefined_tree.get_data_index(pj, false);
      auto nalpha = static_cast<float>(max_alpha_int_ / side_len);
      char* new_alpha = reinterpret_cast<char*>(&nalpha);
#if COPY_PARENT_DATA
      for(int i = 0; i<static_cast<int>(n); ++i){
        char* old_buf = old_bufs_[i];
        char* new_buf = new_bufs[i];
        int ptr_inc = static_cast<int>(type_sizes[i]);
        //move root data to new location
        //note the old data pointed to by dataIndex, not oldDataPtr
        //thus getting the data of the parent
        char* old_byte_ptr = old_buf + dataIndex*ptr_inc;
        char* new_byte_ptr = new_buf + newDataPtr*ptr_inc;
        if(i == alpha_index_)
          std::memcpy(new_byte_ptr, new_alpha, ptr_inc);
        else{
          std::memcpy(new_byte_ptr, old_byte_ptr, ptr_inc);
        }
      }
#else//don't copy parent data
      char* zero_data = new char[ptr_inc];
      for(int i=0;i<ptr_inc; ++i)
        zero_data[i]=unsigned char(0);
      for(int i = 0; i<static_cast<int>(n); ++i){
        char* new_buf = new_bufs[i];
        int ptr_inc = static_cast<int>(type_sizes[i]);
        int new_byte_ptr = new_buf + newDataPtr*ptr_inc;
        if(i == alpha_index_)
          std::memcpy(new_byte_ptr, new_alpha, ptr_inc);
        else
          std::memcpy(new_byte_ptr, zero_data, ptr_inc);
      }
      delete [] zero_data;
#endif
      //update new data pointer
      ++newDataPtr;
      ++newInitCount;
      ++cellsMoved;
    }
  }
  return newInitCount;
}


int boxm2_refine_block_multi_data::free_space(int startPtr, int endPtr)
{
  int freeSpace = (startPtr >= endPtr)? startPtr-endPtr : data_len_ - (endPtr-startPtr);
  return freeSpace;
}


////////////////////////////////////////////////////////////////////////////////
//REFINE FUNCTION BASED ON ALPHA
////////////////////////////////////////////////////////////////////////////////
void boxm2_refine_block_multi_data_function( const boxm2_scene_sptr& scene,
                                             const boxm2_block_sptr& blk,
                                             std::vector<std::string> const& prefixes,
                                             float prob_thresh)
{
  boxm2_refine_block_multi_data refine_block;
  if(!refine_block.init_data(scene, blk, prefixes, prob_thresh))
    return;
  refine_block.refine_deterministic(prefixes);
}
////////////////////////////////////////////////////////////////////////////////
//REFINE FUNCTION TO MATCH SPECIFIED DEPTHS
////////////////////////////////////////////////////////////////////////////////

void boxm2_refine_block_multi_data_function( const boxm2_scene_sptr& scene,
                                             const boxm2_block_sptr& blk,
                                             std::vector<std::string> const& prefixes,
                                             vbl_array_3d<int> const& depths_to_match){
  boxm2_refine_block_multi_data refine_block;
  if(!refine_block.init_data(scene, blk, prefixes, 0.99f))
    return;
  refine_block.match_refine(prefixes, depths_to_match);
}
