//------------------------------------------------------------------------------
// Refine bit scene
// NOTE: relies on preprocessor macro "MAXINNER" to be defined
// in the host "build_refine_program" function.
// Also macro "MAXCELLS" must be defined for moving refined data
//------------------------------------------------------------------------------
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

/////////////////////////////////////////////////////////////////
////Refine Tree (refines local tree)
// Refines tree using new data indexing scheme
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
  float max_alpha_int = -log(1.0f - prob_thresh);

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
      int dataIndex = data_index_cached(unrefined, i, bit_lookup, cumsum, &cumIndex) + data_index_root(unrefined); //gets absolute position
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
// Move data now uses a BFS (was faster than traversing bits)
//////////////////////////////////////////////////////////////////
int move_data(__constant RenderSceneInfo * linfo,
              __global   MOG_TYPE        * src,
              __global   MOG_TYPE        * dest,
              __local    uchar16         * unrefined_tree,
              __local    uchar16         * refined_tree,
              __local    char            * list_mem,
              __constant uchar           * bit_lookup,
                         float             prob_thresh,
                         bool              is_alpha,
                         bool              copy_parent)
{
  //only used in alpha case
  float max_alpha_int = -log(1.0f - prob_thresh);

  //zip through each leaf cell and
  int oldDataPtr = 0, newDataPtr = 0;
  int newTreeSize = num_cells(refined_tree);

  ////////////////////////////////////////////////////////////////////
  // Begin octree traversal
  //load current block/tree, initialize cumsum buffer and cumIndex
  linked_list toVisit = new_linked_list(list_mem, 73);
  push_back( &toVisit, -1 );

  //iterate through tree if there are children to get to
  while ( !empty(&toVisit) ) {

    //get front node off the top of the list, do an intersection for all 8 children
    int pindex = (int) pop_front( &toVisit );
    for(int j=8*pindex + 1; j < 8*pindex + 9; ++j) {
      if(j < 0) continue;
      if(tree_bit_at(refined_tree, j))
        push_back(&toVisit, j);

      //--- SWAP CODE ----------------------
      //if parent bit is 1, then you're a valid cell
      bool validCellOld = (j==0) || tree_bit_at(unrefined_tree, pindex);
      bool validCellNew = (j==0) || tree_bit_at(refined_tree, pindex);
      if(validCellOld && validCellNew) {
        //move root data to new location
        dest[newDataPtr]   = src[oldDataPtr];

        //increment
        oldDataPtr++;
        newDataPtr++;
      }
      //case where it's a new leaf...
      else if(validCellNew) {
        //calc new alpha
        MOG_TYPE new_value;
        if(is_alpha) {
          int parentLevel = get_depth(pindex);
          float side_len = linfo->block_len / (float) (1<<parentLevel);
          float newAlpha = (max_alpha_int / side_len);

          //cast as float
          __global float* dest_f = (__global float*) dest;
          dest_f[newDataPtr] = newAlpha;
        }
        else if(copy_parent && pindex >= 0) {
          int pDataPtr = data_index_relative(unrefined_tree, pindex, bit_lookup);
          dest[newDataPtr] = src[pDataPtr];
        }
        else {
          dest[newDataPtr] = (MOG_TYPE) 0;
        }

        //update new data pointer
        newDataPtr++;
      }
      //--- END SWAP CODE ----------------------

    }
  }
  return newDataPtr;
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
  __local uchar16* local_tree   = &all_local_tree[llid];
  __local uchar16* refined_tree = &all_refined_tree[llid];
  __local uchar*   csum         = &cumsum[10*llid];

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
    int newSize = refine_tree(linfo,
                              local_tree,
                              refined_tree,
                              currTreeSize,
                              0,
                              alpha_array,
                              prob_thresh,
                              csum,
                              bit_lookup,
                              MAX_INNER_CELLS,
                              output);

    tree_sizes[gid] = newSize;
    trees_cpy[gid] = as_int4(*refined_tree);
  }
}

////////////////////////////////////////////
// New Refine, pass one, refines trees to copy
// SPECIAL CASE: if the argument "prob_thresh_t" is less than 0, then
// this kernel is refining an appearance data, not alpha
// if it is refining alpha, ste the init_cell to the correct alpha each time
///////////////////////////////////////////
__kernel void refine_data( __constant RenderSceneInfo * linfo,
                           __global   uchar16         * trees,
                           __global   uchar16         * trees_refined,        // refined trees
                           __global   int             * tree_sizes,       // tree size for each block
                           __global   MOG_TYPE        * data,             // Data array to be copied
                           __global   MOG_TYPE        * data_cpy,         // data array to be copied into, size(data_cpy) = tree_sizes[last] + sizeof(last_tree);
                           __global   float           * prob_thresh_t,    //IF THIS VALUE IS less than 0, then it is not the alpha pass
                           __global   bool            * is_alpha_t,       //this is true if you're refining alpha, otherwise it should be false
                           __global   bool            * copy_parent_t,    //if this is true, then new cells should copy parent values
                           __constant uchar           * bit_lookup,       // used to get data_index
                           __global   float           * output,
                           __local    uchar16         * all_local_tree,      // cache current tree into local memory
                           __local    uchar16         * all_refined_tree,
                           __local    char            * all_list_mem )
{
  //make sure local_tree points to the right one in shared memory
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  __local uchar16* local_tree = &all_local_tree[llid];
  __local uchar16* refined_tree = &all_refined_tree[llid];
  __local char*    list_mem = &all_list_mem[73*llid];

  //global id should be the same as treeIndex
  unsigned gid = get_global_id(0);

  //tree Index is global id
  unsigned treeIndex = gid;
  int numTrees = linfo->dims.x * linfo->dims.y * linfo->dims.z;
  if(gid < numTrees) {

    //USE rootlevel to determine MAX_INNER and MAX_CELLS
    //1. get current tree information
    uchar16 currTree = as_uchar16(trees[treeIndex]);
    (*local_tree)    = currTree;
    int currTreeSize = num_cells(local_tree);

    uchar16 newTree  = as_uchar16(trees_refined[treeIndex]);
    (*refined_tree)  = newTree;
    int newTreeSize  = num_cells(refined_tree); //this should also equal to tree_sizes[treeIndex+1]-tree_sizes[treeIndex];

    //6a. update local tree's data pointer (store it back tree buffer)
    int data_ptr = tree_sizes[treeIndex];
    set_data_index_root(refined_tree, data_ptr);

    //if this is updating the ALPHA pass and is therefore the last one, write to new block
    if( *is_alpha_t ) {
      trees[treeIndex] = (*refined_tree);
    }

    //6b. get old data pointer
    int old_data_ptr = data_index_root(local_tree);

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
      bool copy_parent = (*copy_parent_t);

      //do some Pointer arithmetic to pass in aligned data
      int numNew = move_data(linfo,
                             &data[old_data_ptr],
                             &data_cpy[data_ptr],
                             local_tree,
                             refined_tree,
                             list_mem,
                             bit_lookup,
                             prob_thresh,
                             is_alpha,
                             copy_parent );

    }
  }
}

