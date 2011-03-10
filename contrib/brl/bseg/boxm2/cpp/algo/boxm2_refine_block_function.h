#ifndef boxm2_refine_block_function_h
#define boxm2_refine_block_function_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boct/boct_bit_tree2.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>
#include <boxm2/io/boxm2_cache.h>

class boxm2_refine_block_function
{
 public:
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  //: "default" constructor
  boxm2_refine_block_function() {}

  //: initialize generic data base pointers as their data type
  bool init_data(boxm2_block* blk, vcl_vector<boxm2_data_base*> & datas, float prob_thresh);

  //: refine function;
  bool refine();
  bool refine_deterministic(vcl_vector<boxm2_data_base*>& datas); 

  //: refine bit tree
  boct_bit_tree2 refine_bit_tree(boct_bit_tree2& curr_tree,
                                 int buff_offset, 
                                 bool is_random=true);
      
  //: move data into new location
  int move_data(boct_bit_tree2& unrefined_tree, 
                boct_bit_tree2& refined_tree, 
                float*  alpha_cpy,
                uchar8*  mog_cpy,
                ushort4* num_obs_cpy );

  //: helper method
  int free_space(int startPtr, int endPtr);

 private:
  boxm2_block* blk_;
  uchar16*     trees_;

  float*       alpha_;
  uchar8*      mog_;
  ushort4*     num_obs_;

  //block max level
  int max_level_;
  int MAX_INNER_CELLS_;
  int MAX_CELLS_;

  //max alpha integrated
  float max_alpha_int_;

  //length of data buffer
  int data_len_;

  //length of one side of a sub block
  double block_len_;

  int num_split_;
};

//: initialize generic data base pointers as their data type
bool boxm2_refine_block_function::init_data(boxm2_block* blk, vcl_vector<boxm2_data_base*> & datas, float prob_thresh)
{
    //store block and pointer to uchar16 3d block
    blk_   = blk;
    trees_ = blk_->trees().data_block();

    //store data buffers
    int i=0;
    alpha_   = (float*)   datas[i++]->data_buffer();
    mog_     = (uchar8*)  datas[i++]->data_buffer();
    num_obs_ = (ushort4*) datas[i++]->data_buffer();

    //block max level
    max_level_ = blk_->max_level();

    //max alpha integrated
    max_alpha_int_ = -vcl_log(1.f - prob_thresh);

    //Data length now is constant
    data_len_ = 65536;

    //length of one side of a sub block
    block_len_ = blk_->sub_block_dim().x();

    //USE rootlevel to determine MAX_INNER and MAX_CELLS
    if (max_level_ == 1) {
      vcl_cout<<"Trying to refine scene with max level 1"<<vcl_endl;
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

    vcl_cout<<"Refine Info: [blk "<<blk->block_id()
            <<"] [blk_len "<<block_len_
            <<"] [data_len "<<data_len_
            <<"] [max_alpha_int "<<max_alpha_int_
            <<"] [max level "<<max_level_
            <<']'<<vcl_endl;

    //for debugging
    num_split_ = 0;

    return true;
}

bool boxm2_refine_block_function::refine()
{
  //Go through each buffer, updating the tree pointers
  for (int buffIndex = 0; buffIndex < blk_->num_buffers(); ++buffIndex)
  {
    //cache some buffer variables in registers:
    int numBlocks = blk_->trees_in_buffers()[buffIndex]; //number of blocks in this buffer;
    int startPtr  = blk_->mem_ptrs()[buffIndex][0];       //points to first element in data buffer
    int endPtr    = blk_->mem_ptrs()[buffIndex][1];       //points to TWO after the last element in data buffer

    //---- special case that may not be necessary ----------------------------
    //0. if there aren't 585 cells in this buffer, quit refining
    int preFreeSpace = free_space(startPtr, endPtr);
    if (preFreeSpace < 585) {
      vcl_cout<<"Skipping because of pre free space"<<vcl_endl;
      continue;
    }
    //------------------------------------------------------------------------

    //Iterate over each tree in buffer=gid
    for (int subIndex=0; subIndex<numBlocks; ++subIndex)
    {
      //1. get current tree information
      int treeIndex = blk_->tree_ptrs()[buffIndex][subIndex];
      uchar16 tree  = trees_[treeIndex];

      boct_bit_tree2 curr_tree( (unsigned char*) tree.data_block(), max_level_);
      int currTreeSize = curr_tree.num_cells();

      //3. refine tree locally (only updates refined_tree and returns new tree size)
      boct_bit_tree2 refined_tree = this->refine_bit_tree(curr_tree,
                                                          buffIndex*data_len_);
      int newSize = refined_tree.num_cells();

      //4. update start pointer (as data will be moved up to the end)
      startPtr = (startPtr+currTreeSize) % data_len_;

      //5. if there's enough space, move tree
      int freeSpace = free_space(startPtr, endPtr);

      //6. if the tree was refined (and it fits)
      if (newSize > currTreeSize && newSize <= freeSpace)
      {
        //6a. update local tree's data pointer (store it back tree buffer)
        int buffOffset = (endPtr-1 + data_len_) % data_len_;
        refined_tree.set_data_ptr(buffOffset);

        //store this refined tree in blocks' tree at treeIndex (make this legal somehow)
        vcl_memcpy(&trees_[treeIndex], refined_tree.get_bits(), sizeof(uchar16));

        //cache old data pointer and new data pointer
        int oldDataPtr = curr_tree.get_data_index(0);
        int newDataPtr = buffOffset;                                       //new root offset within buffer

        //next start moving cells, must zip through max number of cells
        int offset = buffIndex*data_len_;                   //absolute buffer offset
        int newInitCount = 0; int oldCount = 0;             //counts used for assertions
        for (int j=0; j<MAX_CELLS_; j++)
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
          bool validCellOld = (j==0) || curr_tree.bit_at(pj);
          bool validCellNew = (j==0) || refined_tree.bit_at(pj);
          if (validCellOld && validCellNew) {
            oldCount++;

            //move root data to new location
            alpha_  [offset + newDataPtr] = alpha_  [offset + oldDataPtr];
            mog_    [offset + newDataPtr] = mog_    [offset + oldDataPtr];
            num_obs_[offset + newDataPtr] = num_obs_[offset + oldDataPtr];

            //increment
            oldDataPtr = (oldDataPtr+1) % data_len_;
            newDataPtr = (newDataPtr+1) % data_len_;
          }
          //case where it's a new leaf...
          else if (validCellNew) {
            newInitCount++;

            //calc new alpha
            int currLevel = curr_tree.depth_at(j);
            float side_len = (float)block_len_ / (float)(1<<currLevel);
            float new_alpha = max_alpha_int_ / side_len;
            alpha_[offset+newDataPtr] = new_alpha;

            //store parent's data in child cells
            mog_    [offset+newDataPtr].fill(0);
            num_obs_[offset+newDataPtr].fill(0);

            //update new data pointer
            newDataPtr = (newDataPtr+1) % data_len_;
          }
        }

        //6c. update endPtr
        endPtr = (newDataPtr+1) % data_len_;
      }
      //otherwise just move the unrefined tree quickly
      else if (currTreeSize <= freeSpace)
      {
        //6a. update local tree's data pointer (store it back tree buffer)
        int buffOffset = (endPtr-1 + data_len_) % data_len_;
        int oldDataPtr = curr_tree.get_data_index(0);
        curr_tree.set_data_ptr(buffOffset);

        //store this refined tree in blocks' tree at treeIndex (make this legal somehow)
        vcl_memcpy(&trees_[treeIndex], curr_tree.get_bits(), sizeof(uchar16));

        //6b. move data up to end pointer
        int newDataPtr = buffOffset;
        int offset = buffIndex * data_len_;                   //absolute buffer offset
        for (int j=0; j<currTreeSize; j++) {
          alpha_  [offset + newDataPtr] = alpha_  [offset + oldDataPtr];
          mog_    [offset + newDataPtr] = mog_    [offset + oldDataPtr];
          num_obs_[offset + newDataPtr] = num_obs_[offset + oldDataPtr];

          //increment
          oldDataPtr = (oldDataPtr+1)%data_len_;
          newDataPtr = (newDataPtr+1)%data_len_;
        }

        //6c. update endPtr
        endPtr = (endPtr+currTreeSize) % data_len_;
      }
      //THIS SHOULDN"T EVER HAPPEN, buffer is full even though the tree didn't refine!
      else {
        //move start pointer back
        startPtr = (startPtr - currTreeSize + data_len_)%data_len_;
        break;
      }
    } //end for loop

    //update mem pointers before returning
    blk_->mem_ptrs()[buffIndex][0] = startPtr;
    blk_->mem_ptrs()[buffIndex][1] = endPtr;
  } //end if (gid < num_buffer)

  vcl_cout<<"NUm SPLIT: "<<num_split_<<vcl_endl;
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
bool boxm2_refine_block_function::refine_deterministic(vcl_vector<boxm2_data_base*>& datas)
{
  vcl_cout<<"CPU Non deterministic refine: "<<vcl_endl;
  
  //loop over each tree, refine it in place (keep a vector of locations for 
  // posterities sake
  boxm2_array_3d<uchar16>&  trees = blk_->trees();  //trees to refine
  uchar16* trees_copy = new uchar16[trees.size()];  //copy of those trees
  int* dataIndex = new int[trees.size()];           //data index for each new tree
  int currIndex = 0;                                //curr tree being looked at
  int dataSize = 0;                                 //running sum of data size
  boxm2_array_3d<uchar16>::iterator blk_iter;  
  for(blk_iter = trees.begin(); blk_iter != trees.end(); ++blk_iter, ++currIndex)
  {
      //0. store data index for eahc tree. 
      dataIndex[currIndex] = dataSize; 

      //1. get current tree information
      uchar16 tree  = (*blk_iter);
      boct_bit_tree2 curr_tree( (unsigned char*) tree.data_block(), max_level_);
      int currTreeSize = curr_tree.num_cells();

      //3. refine tree locally (only updates refined_tree and returns new tree size)
      boct_bit_tree2 refined_tree = this->refine_bit_tree(curr_tree, 0, false);  //i.e. is not random
      int newSize = refined_tree.num_cells();
    
      //cache refined tree
      vcl_memcpy (trees_copy[currIndex].data_block(), refined_tree.get_bits(), 16); 
      dataSize += newSize; 
  }
  
  //2. allocate new data arrays of the appropriate size
  vcl_cout<<"Allocating new data blocks"<<vcl_endl;
  boxm2_block_id id = datas[0]->block_id();
  boxm2_data_base* newA = new boxm2_data_base(new char[dataSize * sizeof(float) ], dataSize * sizeof(float), id);
  boxm2_data_base* newM = new boxm2_data_base(new char[dataSize * sizeof(uchar8)], dataSize * sizeof(uchar8), id);
  boxm2_data_base* newN = new boxm2_data_base(new char[dataSize * sizeof(ushort4)], dataSize * sizeof(ushort4), id);
  float*   alpha_cpy = (float*) newA->data_buffer(); 
  uchar8*  mog_cpy   = (uchar8*) newM->data_buffer();
  ushort4* num_obs_cpy = (ushort4*) newN->data_buffer();
  
  //3. loop through tree again, putting the data in the right place
  vcl_cout<<"Swapping data into new blocks... "<<vcl_endl;
  currIndex = 0; 
  for(blk_iter = trees.begin(); blk_iter != trees.end(); ++blk_iter, ++currIndex)
  {
      //1. get current tree information
      uchar16 tree  = (*blk_iter);
      boct_bit_tree2 old_tree( (unsigned char*) tree.data_block(), max_level_);

      //2. refine tree locally (only updates refined_tree and returns new tree size)
      boct_bit_tree2 refined_tree( (unsigned char*) trees_copy[currIndex].data_block(), max_level_);  

      //2.5 pack data bits into refined tree
      //store data index in bits [10, 11, 12, 13] ;
      int root_index = dataIndex[currIndex]; 
      refined_tree.set_data_ptr(root_index, false); //is not random 

      //3. swap data from old location to new location
      this->move_data(old_tree, refined_tree, alpha_cpy, mog_cpy, num_obs_cpy); 
      
      //4. store old tree in new tree, swap data out
      vcl_memcpy(blk_iter, refined_tree.get_bits(), 16); 

  }

  //3. Replace data in the cache
  boxm2_cache* cache = boxm2_cache::instance();    
  cache->replace_data_base(id, boxm2_data_traits<BOXM2_ALPHA>::prefix(), newA); 
  cache->replace_data_base(id, boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(), newM); 
  cache->replace_data_base(id, boxm2_data_traits<BOXM2_NUM_OBS>::prefix(), newN); 

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
boct_bit_tree2 boxm2_refine_block_function::refine_bit_tree(boct_bit_tree2& unrefined_tree,
                                                            int buff_offset, 
                                                            bool is_random)
{
  //initialize tree to return
  boct_bit_tree2 refined_tree(unrefined_tree.get_bits(), max_level_);

  //no need to do depth first search, just iterate and check each node along the way
  //(iterate through the max number of inner cells)
  for (int i=0; i<MAX_INNER_CELLS_; i++)
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
      float side_len = (float)block_len_/(float)(1<<currDepth);

      //get alpha value for this cell;
      int dataIndex; 
      float alpha; 
      if(is_random) {
        dataIndex = unrefined_tree.get_data_index(i, is_random) % data_len_;             //gets offset within buffer
        alpha   = alpha_[buff_offset + dataIndex];
      }
      else {
        dataIndex = unrefined_tree.get_data_index(i, is_random); 
        alpha   = alpha_[dataIndex]; 
      }
    
      //integrate alpha value
      float alpha_int = alpha * side_len;

      //IF alpha value triggers split, tack on 8 children to end of tree array
      if (alpha_int > max_alpha_int_ && currDepth < max_level_-1)
      {
        //change value of bit_at(i) to 1;
        refined_tree.set_bit_at(i, true);

        //keep track of number of nodes that split
        num_split_++;
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
int boxm2_refine_block_function::move_data(boct_bit_tree2& unrefined_tree, 
                                  boct_bit_tree2& refined_tree, 
                                  float*  alpha_cpy,
                                  uchar8*  mog_cpy,
                                  ushort4* num_obs_cpy )
{
  
  //zip through each leaf cell and 
  int oldDataPtr = unrefined_tree.get_data_ptr(false); 
  int newDataPtr = refined_tree.get_data_ptr(false);
  int newInitCount = 0; int oldCount = 0;            
  for(int j=0; j<MAX_CELLS_; ++j) {

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
    if(validCellOld && validCellNew) {
      
      //move root data to new location
      alpha_cpy[newDataPtr]  = alpha_[oldDataPtr];
      mog_cpy[newDataPtr]    = mog_[oldDataPtr];
      num_obs_cpy[newDataPtr]= num_obs_[oldDataPtr];

      //increment 
      oldDataPtr++; 
      newDataPtr++; 
    } 
    //case where it's a new leaf...
    else if(validCellNew) {
      newInitCount++; 

      //move root data to new location
      int currLevel = unrefined_tree.depth_at(j);
      float side_len = block_len_ / (float) (1<<currLevel);
      alpha_cpy[newDataPtr]  = (max_alpha_int_ / side_len);
      mog_cpy[newDataPtr]    = uchar8((uchar) 0);
      num_obs_cpy[newDataPtr]= ushort4((ushort) 0);

      //update new data pointer
      newDataPtr++; 
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
void boxm2_refine_block( boxm2_block* blk,
                         vcl_vector<boxm2_data_base*> & datas,
                         float prob_thresh,
                         bool is_random = true)
{
  boxm2_refine_block_function refine_block;
  refine_block.init_data(blk, datas, prob_thresh);
  
  if(is_random)
    refine_block.refine();
  else
    refine_block.refine_deterministic(datas); 
}

#endif
