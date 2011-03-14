//------------------------------------------------------------------------------
// Refine bit scene 
// NOTE: relies on preprocessor macro "MAXINNER" to be defined 
// in the host "build_refine_program" function.  
// Also macro "MAXCELLS" must be defined for moving refined data
//------------------------------------------------------------------------------
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#ifdef MOG_TYPE_16 
    #define MOG_TYPE ushort8
#endif
#ifdef MOG_TYPE_8 
   #define MOG_TYPE uchar8
#endif
#ifdef MOG_TYPE_4
   #define MOG_TYPE uchar4
#endif

/////////////////////////////////////////////////////////////////
////Refine Tree (refines local tree)
////Depth first search iteration of the tree (keeping track of node level)
////1) parent pointer, 2) child pointer 3) data pointer 4) nothing right now
// Kind of a wierd mix of functions - the tree structure is modified locally, 
// so no tree_buffer information is needed, whereas the data is modified 
// on the global level, so buffers, offsets are used
/////////////////////////////////////////////////////////////////
int refine_tree(__constant RenderSceneInfo * linfo, 
                __local    uchar16         * unrefined_tree,
                __local    uchar16         * refined_tree,
                           int               tree_size, 
                           int               blockIndex,
                __global   float           * alpha_array,
                           float             prob_thresh, 
                __local    uchar           * cumsum, 
                __constant uchar           * bit_lookup,       // used to get data_index
                           int               MAX_INNER_CELLS,
                __global   float           * output)
{
  unsigned gid = get_group_id(0);
  unsigned lid = get_local_id(0);

  //cast local pointers to uchar arrays
  __local uchar* unrefined = (__local uchar*) unrefined_tree; 
  __local uchar* refined   = (__local uchar*) refined_tree; 

  //max alpha integrated
  float max_alpha_int = (-1)*log(1.0 - prob_thresh);      
  
  //initialize cumsum buffer and cumIndex
  cumsum[0] = (*unrefined_tree).s0;
  int cumIndex = 1;
  int numSplit = 0;
  
  //no need to do depth first search, just iterate and check each node along the way
  //(iterate through the max number of inner cells)
  for(int i=0; i<MAX_INNER_CELLS; i++) {
    
    //if current bit is 0 and parent bit is 1, you're at a leaf
    int pi = (i-1)>>3;           //Bit_index of parent bit    
    bool validParent = tree_bit_at(unrefined, pi) || (i==0); // special case for root
    if(validParent && tree_bit_at(unrefined, i)==0) {
    
      //////////////////////////////////////////////////
      //LEAF CODE HERE
      //////////////////////////////////////////////////
      //find side length for cell of this level = block_len/2^currDepth
      int currDepth = get_depth(i);
      float side_len = linfo->block_len/(float) (1<<currDepth);
     
      //get alpha value for this cell;
      int dataIndex = data_index_cached(unrefined, i, bit_lookup, cumsum, &cumIndex, linfo->data_len); //gets offset within buffer
      float alpha   = alpha_array[gid*linfo->data_len + dataIndex];
         
      //integrate alpha value
      float alpha_int = alpha * side_len;
      
      //IF alpha value triggers split, tack on 8 children to end of tree array
      if(alpha_int > max_alpha_int && currDepth<linfo->root_level)  {
       
        //change value of bit_at(i) to 1;
        set_tree_bit_at(refined, i, true);
       
        //keep track of number of nodes that split
        numSplit++;
        //output[gid]++;
      }
      ////////////////////////////////////////////
      //END LEAF SPECIFIC CODE
      ////////////////////////////////////////////
      
    }
  }
  
  //tree and data size output
  tree_size += numSplit * 8;
  return tree_size;
}

/////////////////////////////////////////////////////////////////
////Refine Tree (refines local tree)
// Refines tree using new data indexing scheme
/////////////////////////////////////////////////////////////////
int refine_tree2(__constant RenderSceneInfo * linfo, 
                __local    uchar16         * unrefined_tree,
                __local    uchar16         * refined_tree,
                           int               tree_size, 
                           int               blockIndex,
                __global   float           * alpha_array,
                           float             prob_thresh, 
                __local    uchar           * cumsum, 
                __constant uchar           * bit_lookup,       // used to get data_index
                           int               MAX_INNER_CELLS,
                __global   float           * output)
{
  unsigned gid = get_group_id(0);
  unsigned lid = get_local_id(0);

  //cast local pointers to uchar arrays
  __local uchar* unrefined = (__local uchar*) unrefined_tree; 
  __local uchar* refined   = (__local uchar*) refined_tree; 

  //max alpha integrated
  float max_alpha_int = (-1)*log(1.0 - prob_thresh);      
  
  //initialize cumsum buffer and cumIndex
  cumsum[0] = (*unrefined_tree).s0;
  int cumIndex = 1;
  int numSplit = 0;
  
  //no need to do depth first search, just iterate and check each node along the way
  //(iterate through the max number of inner cells)
  for(int i=0; i<MAX_INNER_CELLS; i++) {
    
    //if current bit is 0 and parent bit is 1, you're at a leaf
    int pi = (i-1)>>3;           //Bit_index of parent bit    
    bool validParent = tree_bit_at(unrefined, pi) || (i==0); // special case for root
    if(validParent && tree_bit_at(unrefined, i)==0) {
    
      //////////////////////////////////////////////////
      //LEAF CODE HERE
      //////////////////////////////////////////////////
      //find side length for cell of this level = block_len/2^currDepth
      int currDepth = get_depth(i);
      float side_len = linfo->block_len/(float) (1<<currDepth);
     
      //get alpha value for this cell;
      int dataIndex = data_index_cached2(unrefined, i, bit_lookup, cumsum, &cumIndex); //gets absolute position
      float alpha   = alpha_array[dataIndex];
         
      //integrate alpha value
      float alpha_int = alpha * side_len;
      
      //IF alpha value triggers split, tack on 8 children to end of tree array
      if(alpha_int > max_alpha_int && currDepth<linfo->root_level)  {
       
        //change value of bit_at(i) to 1;
        set_tree_bit_at(refined, i, true);
       
        //keep track of number of nodes that split
        numSplit++;
      }
      ////////////////////////////////////////////
      //END LEAF SPECIFIC CODE
      ////////////////////////////////////////////
      
    }
  }
  
  //tree and data size output
  tree_size += numSplit * 8;
  return tree_size;
}

 
//////////////////////////////////////////////////////////////////
// Moves data into dest from src
////////////////////////////////////////////////////////////////// 
int move_data(__constant RenderSceneInfo * linfo,
              __global   MOG_TYPE        * src, 
              __global   MOG_TYPE        * dest, 
              __local    uchar16         * unrefined_tree,
              __local    uchar16         * refined_tree,
                         int               MAX_CELLS, 
              __constant uchar           * bit_lookup,
                         float             prob_thresh,
                         bool              is_alpha)
{
  //only used in alpha case
  float max_alpha_int = (-1)*log(1.0 - prob_thresh);      

  //zip through each leaf cell and 
  int oldDataPtr = 0; int newDataPtr = 0; 
  int newInitCount = 0; int oldCount = 0;       
  int newTreeSize = num_cells(refined_tree); 
  for(int j=0; j<MAX_CELLS && newDataPtr<newTreeSize; ++j) {

    //--------------------------------------------------------------------
    //4 Cases:
    // - Old cell and new cell exist - transfer data over
    // - new cell exists, old cell doesn't - create new occupancy based on depth
    // - old cell exists, new cell doesn't - uh oh this is bad news
    // - neither cell exists - do nothing and carry on
    //--------------------------------------------------------------------
    //if parent bit is 1, then you're a valid cell
    int pj = (j-1)>>3;           //Bit_index of parent bit    
    bool validCellOld = (j==0) || tree_bit_at(unrefined_tree, pj); 
    bool validCellNew = (j==0) || tree_bit_at(refined_tree, pj); 
    if(validCellOld && validCellNew) {
      //move root data to new location
      dest[newDataPtr]   = src[oldDataPtr];

      //increment 
      oldDataPtr++; 
      newDataPtr++; 
    } 
    //case where it's a new leaf...
    else if(validCellNew) {
      newInitCount++; 

      //calc new alpha
      MOG_TYPE new_value;
      if(is_alpha) {
        int currLevel = get_depth(j);
        float side_len = linfo->block_len / (float) (1<<currLevel);
        float newAlpha = (max_alpha_int / side_len); 
        
        //cast as float 
        __global float* dest_f = (__global float*) dest; 
        dest_f[newDataPtr] = newAlpha;  
      }
      else {
        //int pDataPtr = data_index(0, unrefined_tree, pj, bit_lookup);
        dest[newDataPtr] = (MOG_TYPE) 0;
      }

      //update new data pointer
      newDataPtr++; 
    }
  }       
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// REFINE KERNELS
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////// 
 
////////////////////////////////////////////
// New Refine, pass one, refines trees to copy
/////////////////////////////////////////// 
__kernel void refine_trees(__constant RenderSceneInfo * linfo,
                           __global   int4            * trees,
                           __global   int4            * trees_cpy,        // refined trees
                           __global   float           * alpha_array,      // alpha for each block
                           __global   int             * tree_sizes,       // tree size for each block
                           __global   float           * prob_thresh_t,    //refinement threshold
                           __constant uchar           * bit_lookup,       // used to get data_index                  
                           __global   float           * output, 
                           __local    uchar           * cumsum,
                           __local    uchar16         * all_local_tree,      // cache current tree into local memory
                           __local    uchar16         * all_refined_tree )
{
  //make sure local_tree points to the right one in shared memory
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  __local uchar16* local_tree = &all_local_tree[llid]; 
  __local uchar16* refined_tree = &all_refined_tree[llid]; 

  //global id should be the same as treeIndex
  unsigned gid = get_global_id(0);
  int numTrees = linfo->dims.x * linfo->dims.y * linfo->dims.z; 
  if(gid < numTrees) {
    //USE rootlevel to determine MAX_INNER and MAX_CELLS
    int MAX_INNER_CELLS, MAX_CELLS;
    if(linfo->root_level == 1) 
      MAX_INNER_CELLS=1, MAX_CELLS=9; 
    else if (linfo->root_level == 2) 
      MAX_INNER_CELLS=9, MAX_CELLS=73; 
    else if (linfo->root_level == 3) 
      MAX_INNER_CELLS=73, MAX_CELLS=585; 
  
    //locally cache prob_thresh
    float prob_thresh = *prob_thresh_t;

    //1. get current tree information
    uchar16 currTree = as_uchar16(trees[gid]);
    (*local_tree)    = currTree; 
    (*refined_tree)  = currTree;
    int currTreeSize = num_cells(local_tree);
    
    //3. refine tree locally (only updates refined_tree and returns new tree size)
    //NEEDS TO USE NEW VERSION OF DATA INDEXING... 
    //TODO FIX THIS REFINE TREE METHOD
    int newSize = refine_tree2(linfo, 
                              local_tree,
                              refined_tree, 
                              currTreeSize, 
                              0,
                              alpha_array,
                              prob_thresh, 
                              cumsum,
                              bit_lookup,
                              MAX_INNER_CELLS,
                              output);
    
    tree_sizes[gid] = newSize; 
    trees_cpy[gid] = as_int4(*refined_tree); 
  }
}
 
////////////////////////////////////////////
// New Refine, pass two, refines scan to copy
/////////////////////////////////////////// 
__kernel void refine_scan( __constant RenderSceneInfo * linfo,
                           __global   int             * tree_sizes)
{
  //global id will be the tree buffer
  unsigned gid = get_group_id(0);
  
  //implement scan sum here....
  
  
}

////////////////////////////////////////////
// New Refine, pass one, refines trees to copy
// SPECIAL CASE: if the argument "prob_thresh_t" is less than 0, then 
// this kernel is refining an appearance data, not alpha
// if it is refining alpha, ste the init_cell to the correct alpha each time
/////////////////////////////////////////// 
__kernel void refine_data( __constant RenderSceneInfo * linfo,
                           __global   int4            * trees,
                           __global   int4            * trees_refined,        // refined trees
                           __global   int             * tree_sizes,       // tree size for each block
                           __global   MOG_TYPE        * data,             // Data array to be copied
                           __global   MOG_TYPE        * data_cpy,         // data array to be copied into, size(data_cpy) = tree_sizes[last] + sizeof(last_tree); 
                           __global   float           * prob_thresh_t,    //IF THIS VALUE IS less than 0, then it is not the alpha pass
                           __global   bool            * is_alpha_t,       //this is true if you're refining alpha, otherwise it should be false
                           __constant uchar           * bit_lookup,       // used to get data_index                  
                           __global   float           * output, 
                           __local    uchar           * cumsum,
                           __local    uchar16         * all_local_tree,      // cache current tree into local memory
                           __local    uchar16         * all_refined_tree )
{
  //make sure local_tree points to the right one in shared memory
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  __local uchar16* local_tree = &all_local_tree[llid]; 
  __local uchar16* refined_tree = &all_refined_tree[llid]; 

  //global id should be the same as treeIndex
  unsigned gid = get_global_id(0);
  
  //tree Index is global id 
  unsigned treeIndex = gid; 
  int numTrees = linfo->dims.x * linfo->dims.y * linfo->dims.z; 
  if(gid < numTrees) {
    
    //USE rootlevel to determine MAX_INNER and MAX_CELLS
    int MAX_INNER_CELLS, MAX_CELLS;
    if(linfo->root_level == 1) 
      MAX_INNER_CELLS=1, MAX_CELLS=9; 
    else if (linfo->root_level == 2) 
      MAX_INNER_CELLS=9, MAX_CELLS=73; 
    else if (linfo->root_level == 3) 
      MAX_INNER_CELLS=73, MAX_CELLS=585; 
        
    //1. get current tree information
    uchar16 currTree = as_uchar16(trees[treeIndex]);
    (*local_tree)    = currTree; 
    int currTreeSize = num_cells(local_tree);
    
    uchar16 newTree  = as_uchar16(trees_refined[treeIndex]); 
    (*refined_tree)  = newTree;
    int newTreeSize  = num_cells(refined_tree); //this should also equal to tree_sizes[treeIndex+1]-tree_sizes[treeIndex]; 
     
    //6a. update local tree's data pointer (store it back tree buffer)
    int data_ptr = tree_sizes[treeIndex]; 
    uchar4 data_chars = as_uchar4(data_ptr);
    (*refined_tree).sA = data_chars.x; 
    (*refined_tree).sB = data_chars.y; 
    (*refined_tree).sC = data_chars.z; 
    (*refined_tree).sD = data_chars.w; 
    
    //if this is updating the ALPHA pass and is therefore the last one, write to new block
    if( *is_alpha_t ) {
      trees[treeIndex] = as_int4(*refined_tree);
    }
    
    //6b. get old data pointer
    //as_int((uchar4) (tree[10], tree[11], tree[12], tree[13]));
    int old_data_ptr = as_int((uchar4) ((*local_tree).sA,(*local_tree).sB,(*local_tree).sC, (*local_tree).sD)); 
  
    //6. if the tree was not refined (simple case) just move it on into the right slot
    if(newTreeSize == currTreeSize) {
      
      //6b. move data into new copy (parallelize this)
      for(int j=0; j<newTreeSize; ++j) 
        data_cpy[data_ptr + j] = data[old_data_ptr + j]; 

    }
    //otherwise if it was refined, then you got some work to do...
    else if(newTreeSize > currTreeSize) {
      
      //locally cache prob_thresh
      float prob_thresh = *prob_thresh_t;
      bool is_alpha = (*is_alpha_t); 

      //do some Pointer arithmetic to pass in aligned data
      int numNew = move_data(linfo, 
                             &data[old_data_ptr], 
                             &data_cpy[data_ptr], 
                             local_tree, 
                             refined_tree, 
                             MAX_CELLS,  
                             bit_lookup, 
                             prob_thresh,
                             is_alpha ); 
                             
    }
  }
}
 
///////////////////////////////////////////
//REFINE MAIN (old, randomly distributed)
//TODO include CELL LEVEL SOMEHOW to make sure cells don't over split
//TODO include a debug print string at the end to know what the hell is going on.
///////////////////////////////////////////
__kernel
void
refine_bit_scene(__constant  RenderSceneInfo    * linfo,
                 __global    int4               * trees,            // 3d array structure of trees
                 __global    int                * tree_ptrs,        // tree pointers in a 2d buffer
                 __global    ushort2            * mem_ptrs,         // denotes occupied space in each data buffer
                 __global    ushort             * blocks_in_buffers,// number of blocks in each buffers
                
                 __global    float              * alpha_array,      // alpha for each block
                 __global    MOG_TYPE           * mixture_array,    // mixture for each block
                 __global    ushort4            * num_obs_array,    // num obs for each block
                 
                 __constant  uchar              * bit_lookup,       // used to get data_index                  
                 __global    float              * prob_thresh_t,    //refinement threshold
                 __global    float              * output, 
                 __local     uchar              * cumsum,
                 __local     uchar16            * local_tree,      // cache current tree into local memory
                 __local     uchar16            * refined_tree)    // refined tree (need old tree to move data over))  
{

  //global id will be the tree buffer
  unsigned gid = get_group_id(0);
  unsigned lid = get_local_id(0);
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  //USE rootlevel to determine MAX_INNER and MAX_CELLS
  int MAX_INNER_CELLS, MAX_CELLS;
  if(linfo->root_level == 1) 
    MAX_INNER_CELLS=1, MAX_CELLS=9; 
  else if (linfo->root_level == 2) 
    MAX_INNER_CELLS=9, MAX_CELLS=73; 
  else if (linfo->root_level == 3) 
    MAX_INNER_CELLS=73, MAX_CELLS=585; 
      
  //locally cache prob_thresh
  float prob_thresh = *prob_thresh_t;

  //go through the tree array and refine it...
  if(gid < linfo->num_buffer && llid==0) 
  {
    output[gid] == 0.0;

    //cache some buffer variables in registers:
    int numBlocks = convert_int(blocks_in_buffers[gid]); //number of blocks in this buffer;
    int startPtr  = convert_int(mem_ptrs[gid].x);         //points to first element in data buffer
    int endPtr    = convert_int(mem_ptrs[gid].y);         //points to TWO after the last element in data buffer

    //get the (absolute) index of the start and end pointers
    int preRefineStart = startPtr;
    int preRefineEnd   = endPtr;

    //---- special case that may not be necessary ----------------------------
    //0. if there aren't 585 cells in this buffer, quit refining 
    int preFreeSpace = (startPtr >= endPtr) ? startPtr-endPtr : linfo->data_len - (endPtr-startPtr);
    if(preFreeSpace < 585) {
      output[gid] = -665;       
      return;
    }
    //------------------------------------------------------------------------

    //Iterate over each tree in buffer=gid      
    for(int subIndex=0; subIndex<numBlocks; subIndex++) {
      
      //1. get current tree information
      int treeIndex = tree_ptrs[gid*linfo->tree_len + subIndex]; 
      uchar16 currTree = as_uchar16(trees[treeIndex]);
      (*local_tree)    = currTree; 
      (*refined_tree)  = currTree;
      int currTreeSize = num_cells(local_tree);

      //2. determine number of data cells used, datasize = occupied space
      int dataSize = (endPtr > startPtr)? (endPtr-1)-startPtr: linfo->data_len - (startPtr-endPtr)-1;

      //3. refine tree locally (only updates refined_tree and returns new tree size)
      int newSize = refine_tree(linfo, 
                                local_tree,
                                refined_tree, 
                                currTreeSize, 
                                subIndex,
                                alpha_array,
                                prob_thresh, 
                                cumsum,
                                bit_lookup,
                                MAX_INNER_CELLS,
                                output);
      //!!! assert that the refined tree matches the newsize !!!
      if(newSize != num_cells(refined_tree)) {
        output[gid] = -663; 
        return;
      }

      //4. update start pointer (as data will be moved up to the end)
      startPtr = (startPtr+currTreeSize)%linfo->data_len;
      
      //5. if there's enough space, move tree
      int freeSpace = (startPtr >= endPtr)? startPtr-endPtr : linfo->data_len - (endPtr-startPtr);

      //6. if the tree was refined (and it fits)
      if(newSize > currTreeSize && newSize <= freeSpace) {

        //6a. update local tree's data pointer (store it back tree buffer)
        ushort buffOffset = convert_ushort( (endPtr-1 + linfo->data_len)%linfo->data_len );
        uchar hi = (uchar)(buffOffset >> 8);
        uchar lo = (uchar)(buffOffset & 255);
        (*refined_tree).sa = hi; 
        (*refined_tree).sb = lo;
        trees[treeIndex] = as_int4((*refined_tree));
                        
        //cache old data pointer and new data pointer
        int oldDataPtr = data_index(0, local_tree, 0, bit_lookup);
        int newDataPtr = convert_int(buffOffset);                                       //new root offset within buffer
        
        //next start moving cells, must zip through max number of cells
        int offset = gid*linfo->data_len;                   //absolute buffer offset
        float max_alpha_int = (-1)*log(1.0 - prob_thresh);  //used for new leaves...
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
          bool validCellOld = (j==0) || tree_bit_at(local_tree, pj); 
          bool validCellNew = (j==0) || tree_bit_at(refined_tree, pj); 
          if(validCellOld && validCellNew) {
            oldCount++;
          
            //move root data to new location
            alpha_array[offset + newDataPtr]   = alpha_array[offset + oldDataPtr];
            mixture_array[offset + newDataPtr] = mixture_array[offset + oldDataPtr];
            num_obs_array[offset + newDataPtr] = num_obs_array[offset + oldDataPtr];

            //increment 
            oldDataPtr = (oldDataPtr+1)%linfo->data_len;
            newDataPtr = (newDataPtr+1)%linfo->data_len;
          } 
          //case where it's a new leaf...
          else if(validCellNew) {
            newInitCount++; 
  
            //calc new alpha
            int currLevel = get_depth(j);
            //int dataIndex = data_index_cached(local_tree, pj, bit_lookup, cumsum, &cumIndex, linfo->data_len); //gets offset within buffer
            int pDataPtr = data_index(0, local_tree, pj, bit_lookup);

            float side_len = linfo->block_len / (float) (1<<currLevel);
            float new_alpha = max_alpha_int / side_len;              
            alpha_array[offset+newDataPtr] = new_alpha;

            //store parent's data in child cells
            mixture_array[offset+newDataPtr] = (MOG_TYPE) mixture_array[offset+pDataPtr];
            num_obs_array[offset+newDataPtr] = (ushort4) (0,0,0,100);

            //update new data pointer
            newDataPtr = (newDataPtr+1)%linfo->data_len;
          }       
        }
        
        //!!! Assert that the number of new cells matches the difference in tree sizes !!!
        if( (newSize-currTreeSize) != newInitCount) {
          output[gid] = -662; 
          return;
        }
        //!!! assert that newSize is equal to oldCells + newCells
        if( newSize != oldCount+newInitCount) {
          output[gid] = -661; 
          return;
        }
        
        //6c. update endPtr
        endPtr = (newDataPtr+1)%linfo->data_len;
        
        //!!! assert that end pointer+newsize = newDataPtr+1...
        if(endPtr != (newDataPtr+1)%linfo->data_len) {
          output[gid] = -660;
          return;
        }
        
      }
      //otherwise just move the unrefined tree quickly
      else if(currTreeSize <= freeSpace) {

        //6a. update local tree's data pointer (store it back tree buffer)
        ushort buffOffset = convert_ushort((endPtr-1 + linfo->data_len)%linfo->data_len);
        int oldDataPtr = data_index(0, local_tree, 0, bit_lookup);
        uchar hi = (uchar)(buffOffset >> 8);
        uchar lo = (uchar)(buffOffset & 255);
        (*local_tree).sa = hi; 
        (*local_tree).sb = lo;
        trees[treeIndex] = as_int4((*local_tree));

        //6b. move data up to end pointer
        int newDataPtr = buffOffset;   
        int offset = gid*linfo->data_len;                   //absolute buffer offset
        for(int j=0; j<currTreeSize; j++) {
            alpha_array[offset + newDataPtr]   = alpha_array[offset + oldDataPtr];
            mixture_array[offset + newDataPtr] = mixture_array[offset + oldDataPtr];
            num_obs_array[offset + newDataPtr] = num_obs_array[offset + oldDataPtr];

            //increment 
            oldDataPtr = (oldDataPtr+1)%linfo->data_len;
            newDataPtr = (newDataPtr+1)%linfo->data_len;
        }

        //6c. update endPtr
        endPtr = (endPtr+currTreeSize)%linfo->data_len;
        
        //!!! assert that end pointer+newsize = newDataPtr+1...
        if(endPtr != (newDataPtr+1)%linfo->data_len) {
          output[gid] = -559;
          return;
        }
      }
#if 1
      //THIS SHOULDN"T EVER HAPPEN, buffer is full even though the tree didn't refine! 
      else {
        //move start pointer back
        startPtr = (startPtr - currTreeSize + linfo->data_len)%linfo->data_len;
        output[gid] = -666;         //signal for buffer full
        break;
      }
#endif

    } //end for loop

    //update mem pointers before returning
    mem_ptrs[gid].x = startPtr;
    mem_ptrs[gid].y = endPtr; 
    
  } //end if(gid < num_buffer)
  
}

 
 
 
