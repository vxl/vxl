//------------------------------------------------------------------------------
// Merge bit scene
//------------------------------------------------------------------------------
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

//move behaviors
#define ALPHA_BEHAVIOR 0
#define COPY_PARENT_BEHAVIOR 1
#define COPY_INIT_BEHAVIOR 2
#define ZERO_BEHAVIOR 3

/////////////////////////////////////////////////////////////////
// Given zero based alpha array, merges tree
/////////////////////////////////////////////////////////////////
int merge_tree( __constant RenderSceneInfo * linfo,
                __local    uchar16         * unrefined_tree,
                __local    uchar16         * merged_tree,
                __global   float           * alpha_array,
                           float             prob_thresh,
                __local    char            * listMem,

                // Args used for indexing
                __local    uchar           * cumsum,
                __constant uchar           * bit_lookup,       // used to get data_index
                __global   float           * output)
{
  unsigned gid = get_group_id(0);
  unsigned lid = get_local_id(0);

  //cast local pointers to uchar arrays
  __local uchar* unmerged = (__local uchar*) unrefined_tree;
  __local uchar* merged   = (__local uchar*) merged_tree;
  cumsum[0] = unmerged[0];
  int cumIndex = 1;
  int numSplit = 0;

  //it can't be merged if it's just a root tree
  if (tree_bit_at(unmerged,0)==0)
    return 0;

  //create float array to keep track of probs
  float max_alpha = -log(1.0f - prob_thresh);
  float probs[8];  //one float for each generation (probs are really alphas)

  ////////////////////////////////////////////////////////////////////
  // Begin octree traversal
  //load current block/tree, initialize cumsum buffer and cumIndex
  linked_list toVisit = new_linked_list(listMem, 73);
  push_back( &toVisit, 0 );

  //iterate through tree if there are children to get to
  while ( !empty(&toVisit) )
  {
    //get front node off the top of the list, do an intersection for all 8 children
    int genCounter = 0;
    bool allLeaves = true;
    int pindex = (int) pop_front( &toVisit );
    for (int currBitIndex=8*pindex + 1; currBitIndex < 8*pindex + 9; ++currBitIndex)
    {
      //get alpha value for this cell;
      int dataIndex = data_index_cached(unmerged, currBitIndex, bit_lookup, cumsum, &cumIndex) + data_index_root(unmerged); //gets absolute position
      float alpha   = alpha_array[dataIndex];

      //calculate the theoretical radius of this cell
      int curr_depth = get_depth(currBitIndex);
      float side_len = linfo->block_len / (float) (1<<curr_depth);
      probs[genCounter++] = alpha*side_len;

      //if currBitIndex is not a leaf
      if ( tree_bit_at(unmerged, currBitIndex) ) {
        push_back( &toVisit, currBitIndex);
        allLeaves = false;
      }
    } //end child for loop

    //take a look to see if this parent should be merged
    bool allBelow = true;
    for (int i=0; i<8; ++i)
      allBelow = allBelow && (probs[i] < max_alpha);

    //if all are leaves and all below, then reset the parent index to 0 (merge)
    if (allLeaves && allBelow) {
      set_tree_bit_at(merged, pindex, false);
      numSplit++;
    }
  } //end BFS while
  // end Octree traversal portion of cone ray trace
  ////////////////////////////////////////////////////////////////////

  return numSplit;
}

//////////////////////////////////////////////////////////////////
// Moves data into dest from src
// Moves data with certain "behavior"
// behavior 1
int move_data(__constant RenderSceneInfo * linfo,
              __global   MOG_TYPE        * src,
              __global   MOG_TYPE        * dest,
              __local    uchar16         * old_tree,
              __local    uchar16         * merged_tree,
              __local    char            * listMem,     //buffer of 73 chars
              __constant uchar           * bit_lookup,
                         float             prob_thresh,
                         int               behavior)
{
  //place to get data and place to put it in new buffer
  int oldDataIndex=0, newDataIndex=0;
  float max_alpha = -log(1.0f - prob_thresh);

  ////////////////////////////////////////////////////////////////////
  // Begin octree traversal
  //load current block/tree, initialize cumsum buffer and cumIndex
  linked_list toVisit = new_linked_list(listMem, 73);
  push_back( &toVisit, -1 );
  while ( !empty(&toVisit) )
  {
    //get front node off the top of the list, do an intersection for all 8 children
    int genCounter = 0;
    bool allLeaves = true;
    int pindex = (int) pop_front( &toVisit );

    for (int currBitIndex=8*pindex + 1; currBitIndex < 8*pindex + 9; ++currBitIndex) {
      if (currBitIndex < 0) continue;

      //branch on OLD TREE
      if (tree_bit_at(old_tree, currBitIndex)==1)
        push_back(&toVisit, currBitIndex);

      //first case: gone to leave that doesn't exist in merged, incrememnt old, not new
      if (valid_cell(old_tree, currBitIndex) && !valid_cell(merged_tree, currBitIndex)) {
        oldDataIndex++;
      }
      //second case: found a merged cell in new tree, copy init vals
      else if (is_leaf(merged_tree, currBitIndex) && !is_leaf(old_tree, currBitIndex))
      {
        //different copy behaviors
        if (behavior==ALPHA_BEHAVIOR) {
          int currLevel = get_depth(currBitIndex);
          float side_len = linfo->block_len / (float) (1<<currLevel);
          float newAlpha = (max_alpha / side_len);

          //cast as float
          __global float* dest_f = (__global float*) dest;
          dest_f[newDataIndex] = newAlpha;
        }
        else if (behavior==COPY_PARENT_BEHAVIOR && pindex >= 0) {
          int pDataPtr = data_index_relative(old_tree, pindex, bit_lookup);
          dest[newDataIndex] = src[pDataPtr];
        }
        else if (behavior==ZERO_BEHAVIOR) {
          dest[newDataIndex] = (MOG_TYPE) 0;
        }

        //increment both pointers
        newDataIndex++;
        oldDataIndex++;
      }
      //last case: they are both valid and unchanged, just copy over old
      else
      {
        dest[newDataIndex] = src[oldDataIndex];
        newDataIndex++;
        oldDataIndex++;
      }
    }//end child for
  } //end BFS while
  return oldDataIndex - newDataIndex;
}

////////////////////////////////////////////////////////////////////////////////
// Merge KERNELS
////////////////////////////////////////////////////////////////////////////////

// New merge
__kernel void merge_trees( __constant RenderSceneInfo  * linfo,
                           __global   int4            * trees,
                           __global   int4            * trees_cpy,        // refined trees
                           __global   float           * alpha_array,      // alpha for each block
                           __global   int             * tree_sizes,       // tree size for each block
                           __global   float           * prob_thresh_t,    //refinement threshold
                           __constant uchar           * bit_lookup,       // used to get data_index
                           __global   float           * output,
                           __local    uchar           * all_cumsum,       //10 bytes * workspace size, for cacheing tree indices
                           __local    uchar16         * all_old_tree,     //16 bytes * workspace size, cache current tree into local memory
                           __local    uchar16         * all_merged_tree,  //16 bytes * workspace size, cache next tree into local memory
                           __local    uchar           * all_list_mem )    //73 bytes * workspace size, caches list for BFS
{
  //make sure local_tree points to the right one in shared memory
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  //setup pointers
  __local uchar*   cumsum      = &all_cumsum[10*llid];
  __local uchar16* old_tree    = &all_old_tree[llid];
  __local uchar16* merged_tree = &all_merged_tree[llid];
  __local uchar*   list_mem    = &all_list_mem[73*llid];

  //global id should be the same as treeIndex
  unsigned gid = get_global_id(0);
  int numTrees = linfo->dims.x * linfo->dims.y * linfo->dims.z;
  if (gid < numTrees)
  {
    //locally cache prob_thresh
    float prob_thresh = *prob_thresh_t;

    //1. get current tree information
    uchar16 currTree = as_uchar16(trees[gid]);
    (*old_tree)     = currTree;
    (*merged_tree)  = currTree;
    int currTreeSize = num_cells(old_tree);

    //3. merge tree locally (only updates refined_tree and returns new tree size)
    int numMerged = merge_tree( linfo,
                                old_tree,
                                merged_tree,
                                alpha_array,
                                prob_thresh,
                                list_mem,
                                cumsum,
                                bit_lookup,
                                output );

    //4. store merged tree and size
    tree_sizes[gid] = num_cells(merged_tree); //currTreeSize - 8*numMerged;
    trees_cpy[gid] = as_int4(*merged_tree);
  }
}


// merge data
__kernel void merge_data(__constant RenderSceneInfo * linfo,
                         __global   uchar16         * trees,
                         __global   uchar16         * trees_merged,    // merged trees
                         __global   int             * tree_sizes,       // tree size for each block
                         __global   MOG_TYPE        * data,             // Data array to be copied
                         __global   MOG_TYPE        * data_cpy,         // data array to be copied into, size(data_cpy) = tree_sizes[last] + sizeof(last_tree);

                         __global   float           * prob_thresh_ptr,    // threshold for the merge
                         __global   int             * int_ptr,//this is true if you're refining alpha, otherwise it should be false

                         //local mem and aux data
                         __constant uchar           * bit_lookup,       // used to get data_index
                         __global   float           * output,
                         __local    uchar           * cumsum,
                         __local    char            * all_list_mem,         //73*worksapce sized local mem chunk
                         __local    uchar16         * all_old_tree,      // cache current tree into local memory
                         __local    uchar16         * all_merged_tree )
{
  //make sure local_tree points to the right one in shared memory
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  __local uchar16* old_tree    = &all_old_tree[llid];
  __local uchar16* merged_tree = &all_merged_tree[llid];
  __local char*    list_mem    = &all_list_mem[73*llid];

  //global id should be the same as treeIndex
  unsigned gid = get_global_id(0);

  //tree Index is global id
  unsigned treeIndex = gid;
  int numTrees = linfo->dims.x * linfo->dims.y * linfo->dims.z;
  if (gid < numTrees)
  {
    //1. get current tree information
    uchar16 currTree = as_uchar16(trees[treeIndex]);
    (*old_tree)    = currTree;
    int currTreeSize = num_cells(old_tree);

    uchar16 newTree  = as_uchar16(trees_merged[treeIndex]);
    (*merged_tree)  = newTree;
    int newTreeSize  = num_cells(merged_tree); //this should also equal to tree_sizes[treeIndex+1]-tree_sizes[treeIndex];

    //2a. update local tree's data pointer (store it back tree buffer)
    int data_ptr = tree_sizes[treeIndex];
    set_data_index_root(merged_tree, data_ptr);

    //3a.  figure out move behavior
    int move = (*int_ptr);

    //if this is updating the ALPHA pass and is therefore the last one, write to new block
    if ( move == ALPHA_BEHAVIOR )
      trees[treeIndex] = (*merged_tree);

    //6b. get old data pointer
    //int old_data_ptr = as_int((uchar4) ((*old_tree).sa,(*old_tree).sb,(*old_tree).sc, (*old_tree).sd));
    int old_data_ptr = data_index_root(old_tree);

    //6. if the tree was not refined (simple case) just move it on into the right slot
    if (newTreeSize == currTreeSize) {
      for (int j=0; j<newTreeSize; ++j)
        data_cpy[data_ptr + j] = data[old_data_ptr + j];
    }
    //otherwise if it was merged, then you got some work to do...
    else {
      //locally cache prob_thresh
      float prob_thresh = *prob_thresh_ptr;

      //do some Pointer arithmetic to pass in aligned data
      int numNew = move_data(linfo,
                             &data[old_data_ptr],
                             &data_cpy[data_ptr],
                             old_tree,
                             merged_tree,
                             list_mem,
                             bit_lookup,
                             prob_thresh,
                             move);
    }
  }
}

