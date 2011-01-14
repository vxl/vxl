#ifndef boxm2_refine_block_function_h
#define boxm2_refine_block_function_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/boxm2_cast_ray_function.h>
#include <boxm2/cpp/boxm2_mog3_grey_processor.h>
#include <boct/boct_bit_tree2.h>
#include <vnl/vnl_vector_fixed.h>

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
  bool init_data(boxm2_block* blk, vcl_vector<boxm2_data_base*> & datas, float prob_thresh)
  {
      //store block and pointer to uchar16 3d block
      blk_   = blk;
      trees_ = blk_->trees().data_block(); 
      
      //store data buffers
      int i=0; 
      alpha_   = (float*)   datas[i++]->data_buffer();
      mog_     = (uchar8*)  datas[i++]->data_buffer();
      num_obs_ = (ushort4*) datas[i++]->data_buffer();
      
      //blcok max level
      int max_level_ = blk_->max_level();
      
      //max alpha integrated
      float max_alpha_int_ = (-1)*vcl_log(1.0 - prob_thresh);      
        
      //Data lenght now is constant
      int data_len_ = 65536; 
      
      //length of one side of a sub block
      double block_len_ = blk_->sub_block_dim().x(); 
      
      return true;
  }

  //: refine function; 
  bool refine(); 
  
  //: refine bit tree
  boct_bit_tree2 refine_bit_tree(boct_bit_tree2 curr_tree, 
                                 int buff_offset, 
                                 int MAX_INNER_CELLS); 

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

  //max alpha integrated
  float max_alpha_int_; 
  
  //length of data buffer
  int data_len_;    
  
  //lenght of one side of a sub block
  double block_len_;
};


bool boxm2_refine_block_function::refine()
{

  //USE rootlevel to determine MAX_INNER and MAX_CELLS
  int MAX_INNER_CELLS, MAX_CELLS;
  if(max_level_ == 1)
    return true; 
  else if(max_level_ == 2) 
    MAX_INNER_CELLS=1, MAX_CELLS=9; 
  else if (max_level_ == 3) 
    MAX_INNER_CELLS=9, MAX_CELLS=73; 
  else if (max_level_ == 4) 
    MAX_INNER_CELLS=73, MAX_CELLS=585; 

  
  //Go through each buffer, updating the tree pointers
  for(int buffIndex = 0; buffIndex < blk_->num_buffers(); ++buffIndex)
  {

    //cache some buffer variables in registers:
    int numBlocks = blk_->trees_in_buffers()[buffIndex]; //number of blocks in this buffer;
    int startPtr  = blk_->mem_ptrs()[buffIndex][0];       //points to first element in data buffer
    int endPtr    = blk_->mem_ptrs()[buffIndex][1];       //points to TWO after the last element in data buffer

    //get the (absolute) index of the start and end pointers
    int preRefineStart = startPtr;
    int preRefineEnd   = endPtr;

    //---- special case that may not be necessary ----------------------------
    //0. if there aren't 585 cells in this buffer, quit refining 
    int preFreeSpace = free_space(startPtr, endPtr); 
    if(preFreeSpace < 585) 
      return true;
    //------------------------------------------------------------------------

    //Iterate over each tree in buffer=gid      
    for(int subIndex=0; subIndex<numBlocks; ++subIndex) {
      
      //1. get current tree information
      int treeIndex = blk_->tree_ptrs()[buffIndex][subIndex]; 
      uchar16 tree  = trees_[treeIndex]; 
      
      boct_bit_tree2 curr_tree( (unsigned char*) tree.data_block(), max_level_);
      int currTreeSize = curr_tree.num_cells();

      //3. refine tree locally (only updates refined_tree and returns new tree size)
      boct_bit_tree2 refined_tree = this->refine_bit_tree(curr_tree, 
                                                          buffIndex*data_len_, 
                                                          MAX_INNER_CELLS); 
      int newSize = refined_tree.num_cells(); 

      //4. update start pointer (as data will be moved up to the end)
      startPtr = (startPtr+currTreeSize) % data_len_;
      
      //5. if there's enough space, move tree
      int freeSpace = free_space(startPtr, endPtr);

      //6. if the tree was refined (and it fits)
      if(newSize > currTreeSize && newSize <= freeSpace) {

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
        for(int j=0; j<MAX_CELLS; j++) {

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
          if(validCellOld && validCellNew) {
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
          else if(validCellNew) {
            newInitCount++; 
  
            //calc new alpha
            int currLevel = curr_tree.depth_at(j);
            float side_len = block_len_ / (float) (1<<currLevel);
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
      else if(currTreeSize <= freeSpace) {

        //6a. update local tree's data pointer (store it back tree buffer)
        int buffOffset = (endPtr-1 + data_len_) % data_len_;
        int oldDataPtr = curr_tree.get_data_index(0);
        curr_tree.set_data_ptr(buffOffset); 
        
        //store this refined tree in blocks' tree at treeIndex (make this legal somehow)
        vcl_memcpy(&trees_[treeIndex], refined_tree.get_bits(), sizeof(uchar16)); 

        //6b. move data up to end pointer
        int newDataPtr = buffOffset;   
        int offset = buffIndex * data_len_;                   //absolute buffer offset
        for(int j=0; j<currTreeSize; j++) {
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
    
  } //end if(gid < num_buffer)
  
}

 
 
 

/////////////////////////////////////////////////////////////////
////Refine Tree (refines local tree)
////Depth first search iteration of the tree (keeping track of node level)
////1) parent pointer, 2) child pointer 3) data pointer 4) nothing right now
// Kind of a wierd mix of functions - the tree structure is modified locally, 
// so no tree_buffer information is needed, whereas the data is modified 
// on the global level, so buffers, offsets are used
/////////////////////////////////////////////////////////////////
boct_bit_tree2 boxm2_refine_block_function::refine_bit_tree(boct_bit_tree2 unrefined_tree, 
                                                            int buff_offset, 
                                                            int MAX_INNER_CELLS)
{
  //initialize cumsum buffer and cumIndex
  int numSplit = 0;
  
  //intiialize tree to return
  boct_bit_tree2 refined_tree(unrefined_tree.get_bits(), max_level_); 
  
  //no need to do depth first search, just iterate and check each node along the way
  //(iterate through the max number of inner cells)
  for(int i=0; i<MAX_INNER_CELLS; i++) {
    
    //if current bit is 0 and parent bit is 1, you're at a leaf
    int pi = (i-1)>>3;           //Bit_index of parent bit    
    bool validParent = unrefined_tree.bit_at(pi) || (i==0); // special case for root
    if(validParent && unrefined_tree.bit_at(i)==0) {
    
      //////////////////////////////////////////////////
      //LEAF CODE HERE
      //////////////////////////////////////////////////
      //find side length for cell of this level = block_len/2^currDepth
      int currDepth = unrefined_tree.depth_at(i);
      float side_len = block_len_/(float) (1<<currDepth);
     
      //get alpha value for this cell;
      int dataIndex = unrefined_tree.get_data_index(i) % data_len_;             //gets offset within buffer
      float alpha   = alpha_[buff_offset + dataIndex];
         
      //integrate alpha value
      float alpha_int = alpha * side_len;
      
      //IF alpha value triggers split, tack on 8 children to end of tree array
      if(alpha_int > max_alpha_int_ && currDepth < max_level_-1)  {
       
        //change value of bit_at(i) to 1;
        refined_tree.set_bit_at(i, true);
       
        //keep track of number of nodes that split
        numSplit++;
      }
      ////////////////////////////////////////////
      //END LEAF SPECIFIC CODE
      ////////////////////////////////////////////
      
    }
  }
  
  return refined_tree; 
}
 
 
int boxm2_refine_block_function::free_space(int startPtr, int endPtr)
{
  int freeSpace = (startPtr >= endPtr)? startPtr-endPtr : data_len_ - (endPtr-startPtr);  
  return freeSpace; 
}

////////////////////////////////////////////////////////////////////////////////
//MAIN FUNCTION 
////////////////////////////////////////////////////////////////////////////////
void boxm2_refine_block( boxm2_block* blk,
                         vcl_vector<boxm2_data_base*> & datas, 
                         float prob_thresh )
{
    boxm2_refine_block_function refine_block;
    refine_block.init_data(blk, datas, prob_thresh); 
    refine_block.refine();  
}

#endif
