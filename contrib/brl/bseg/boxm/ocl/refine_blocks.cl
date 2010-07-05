

/////////////////////////////////////////////////////////////////////
// STACK IMPLEMENTATION METHODS:
// the necessary stack size will be about 8*num_levels
/////////////////////////////////////////////////////////////////////
typedef struct {
  int data[999];
  int top;
} ocl_stack;
void init_ocl_stack(ocl_stack *stack) { stack->top = 0; }
bool empty(ocl_stack *stack) { return stack->top <= 0; }
void push(ocl_stack *stack, int a) {
  stack->data[stack->top] = a;
  stack->top++;
}
int pop(ocl_stack *stack) {
  int buff = stack->data[stack->top-1];
  stack->top--;
  return buff;
}
 
///////////////////////////////////////////////////////////////////////
// Swaps 8 blocks of data from index A to index B
// makes sure to switch A and B's parents to point to the new A and B locations
// makes sure to switch A and B's children to point to the new A and B locations
// makes sure to update the location of nodes in the STACK
///////////////////////////////////////////////////////////////////////
void swap_eight(__local int4 *tree, int a, int b, __global float* output)
{
  
  unsigned lid = get_local_id(0);
  unsigned gid = get_global_id(0);

  //update A and B's Parent's child pointer first
  if(gid==0) {
    int parent_ptr = tree[a].x;
    tree[parent_ptr].y = b;
    parent_ptr = tree[b].x;
    tree[parent_ptr].y = a; 
  }

  //copy b to a buffer
  int4 buff;
  buff = tree[b+lid];

  //copy A to B
  tree[b+lid] = tree[a+lid];
  
  //copy Buffer to A
  tree[a+lid] = buff;
  
  //for each child of B and A update the parent pointer
  int childA = tree[a+lid].y;
  int childB = tree[b+lid].y;
  if(childA > 0) {
    for(int j=0; j<8; j++)
      tree[childA+j].x = a+lid;
  } 
  if(childB > 0) {
    for(int j=0; j<8; j++)
      tree[childB+j].x = b+lid;    
  }
}
 
////////////////////////////////////////////
//reformat tree method
//Tree cell organization: (parent ptr, child ptr, data ptr, nada)
///////////////////////////////////////////
void reformat_tree(__local int4 *tree, __global float* output)
{
  unsigned gid = get_global_id(0);
  
  unsigned curr_index = 0;
  ocl_stack open;
  init_ocl_stack(&open);
  push(&open, 0);
 
  while(!empty(&open)){
    //examine node at top of stack
    int currNode = pop(&open);
    //if the current node has no children, nothing to verify
    int child_ptr = tree[currNode].y;
    bool isleaf = (child_ptr < 0);
    if(!isleaf){
      //if child pointer isn't to the right place..
      if(child_ptr != curr_index+1){          
        //-- need to swap 8 nodes at currNode[1] to curr_index+1 --//
        barrier(CLK_GLOBAL_MEM_FENCE);
        swap_eight(tree, child_ptr, curr_index+1, output);
        child_ptr = curr_index+1;  //verify that tree[currNode].y is equal to curr_index+1;
      } 
      //push children on stack (in reverse order)
      for(int i=7; i>=0; i--){
        push(&open, child_ptr+i);
      }
      curr_index += 8;
    }
  }
}



/////////////////////////////////////////////////////////////////
////SPLIT TREE
////Depth first search iteration of the tree (keeping track of node level)
////1) parent pointer, 2) child pointer 3) data pointer 4) nothing right now
/////////////////////////////////////////////////////////////////
int refine_tree(__local int4    *tree, 
                        int      tree_size, 
               __global float16 *data, 
                        int      data_size,
                        float    prob_thresh, 
                        float    boxLen, 
                        int      maxLevel, 
               __global float   *output)
{
  unsigned gid = get_global_id(0);
  unsigned lid = get_local_id(0);

  //max alpha integrated
  float max_alpha_int = (-1)*log(1.0 - prob_thresh);

  //need to keep track of current level and pops for each level
  int popCounts[5];
  for(int i=0; i<5; i++) popCounts[i]=0;
  int currLevel = 0;

  ////stack for depth first traversal
  ocl_stack open;
  init_ocl_stack(&open);
  push(&open, 0);
  while(!empty(&open)){
    //figure out the current level
    while(popCounts[currLevel]>=8) {
      popCounts[currLevel] = 0;
      currLevel--;
    }
   
    //examine node at top of stack (and keep track of popping)
    int currNode = pop(&open);
    popCounts[currLevel]++;
    
    //if the current node has no children, it's a leaf -> check if it needs to be refined
    int child_ptr = tree[currNode].y;
    if(child_ptr < 0){
   
      //////////////////////////////////////////////////
      //INSERT LEAF SPECIFIC CODE HERE
      //////////////////////////////////////////////////
      //find side length for cell of this level (bit shift: two_pow_level = 1<<currLevel;)
      //int two_pow_level = pow(2,currLevel);
      unsigned two_pow_level = 1<<currLevel;
      float side_len = boxLen/two_pow_level;
     
      //get alpha value for this cell;
      int dataIndex = tree[currNode].z;
      float16 datum = data[dataIndex];
      float alpha = datum.s0;
     
      //integrate alpha value
      float alpha_int = alpha * side_len;

      //IF alpha value triggers split, tack on 8 children to end of tree array
      //make sure the PARENT cell for each of the new children points to i
      //ALSO make sure currLevel is less than MAX_LEVELS
      if(alpha_int > max_alpha_int && currLevel < maxLevel)  {

        //new alpha for the child nodes
        float new_alpha = max_alpha_int / side_len;  
        
        //node I points to tSize - the place where it's children will be tacked on
        tree[currNode].y = tree_size;
        
        //each child points to the currNode, has no children, 
        barrier(CLK_GLOBAL_MEM_FENCE);
        if(lid < 8) {
          int4 tcell = (int4) (currNode, -1, (int)data_size+lid, 0);
          tree[tree_size+lid] = tcell;
        }
        
        //copy data for new children
        float16 newData = datum;
        newData.s0 = new_alpha;
        data[data_size+lid] = newData;

        //update tree and buffer size
        tree_size += 8; 
        data_size += 8;
     
        //reset data for curent node
        float16 zeroDat = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        data[dataIndex] =  zeroDat;    
      }
      ////////////////////////////////////////////
      //END LEAF SPECIFIC CODE
      ////////////////////////////////////////////
     
    }
    //for inner nodes
    else {                      
      for(int i=7; i>=0; i--){
        push(&open, child_ptr+i);
      }
      currLevel++;
    }
  }      

  ///////////////////////////////////////////////////////////////////
  ////REFORMAT TREE into cannonical order
  ///////////////////////////////////////////////////////////////////
  reformat_tree(tree, output); 

  //tree and data size output
  return tree_size;
}
 

 
///////////////////////////////////////////
//REFINE MAIN
//TODO include CELL LEVEL SOMEHOW to make sure cells don't over split
//TODO include a debug print string at the end to know what the hell is going on.
///////////////////////////////////////////
__kernel
void
refine_main(__global  int4     *cells,          //all blocks
            __global  int2     *block_ptrs,     //location and size of blocks
            __private unsigned  num_blocks,     //number of blocks
            __private unsigned  treeBuffSize,   //length of tree buffer (number of int4 cells allocated)
            __global  unsigned *start_ptr,      //start of free memory in treeBuffSize
            __global  unsigned *end_ptr,        //end of free memory in treeBuffSize
            __global  float16  *data,           //tree data
            __private unsigned  data_size,      //number of data items in data buffer
            __private unsigned  dataBuffSize,   //max size for data buffer
            __private float     prob_thresh,    //refinement threshold
            __private unsigned  max_level,      //maximum number of levels for tree (4 for small tree)
            __private float     bbox_len,       //side length of one block
            __local   int4      *local_tree,    // local copy of the tree
            __global  float     *output)        //TODO delete me later
{
  unsigned gid = get_global_id(0);
  unsigned lid = get_local_id(0);
  
  //mem start and end pointers
  int startPtr = (*start_ptr);
  int endPtr = (*end_ptr);
  
    
  //refine each block
  for(int i=0; i<num_blocks; i++){
    
    //clear out local tree
    for(int j=0; j<585; j++){
      int4 buff = (int4) (-1,-1,-1,-1);
      local_tree[j] = buff;
    }
    
    //copy block into local memory
    int currRoot = block_ptrs[i].x;
    int currSize = block_ptrs[i].y;
    for(int j=0; j<currSize; j++) {
      local_tree[j] = cells[currRoot+j];
    }
    
    //free space in global memory
    startPtr = (startPtr+currSize)%treeBuffSize;
    
    //refine tree locally
    int newSize = refine_tree(local_tree, currSize, data, data_size, prob_thresh, bbox_len, max_level, output);
    data_size += newSize-currSize;
    block_ptrs[i].x = endPtr;
    block_ptrs[i].y = newSize;
   
    //check to make sure there's enough buffer space
    int freeSpace = (startPtr >= endPtr)? startPtr-endPtr : treeBuffSize - (endPtr-startPtr);
    if(newSize <= freeSpace) {    
    
      //copy refined tree to global memory 
      for(int j=0; j<newSize; j++) {
        cells[(endPtr+j)%treeBuffSize] = local_tree[j];
      }
    
      //update endPtr
      endPtr = (endPtr+newSize)%treeBuffSize;
    
    }
    else {
      //Do something with emergency space or swapping or something
      (*output) = 666;
    }


  }

}

 
 
 
