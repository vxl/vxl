/////////////////////////////////////////
// STACK IMPLEMENTATION METHODS:
// the necessary stack size will be about 8*num_levels
/////////////////////////////////////////
typedef struct {
  int data[1000];
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
  
//////////////////////////////////////////////
// Swaps 8 blocks of data from index A to index B
// makes sure to switch A and B's parents to point to the new A and B locations
// makes sure to switch A and B's children to point to the new A and B locations
// makes sure to update the location of nodes in the STACK
//////////////////////////////////////////////
void swap_eight(__global int4 *tree, int a, int b)
{
  //store data at B in some buffer
  unsigned i;
  int4 buff[8];
  for(i=0; i<8; i++){
    buff[i] = tree[b+i];
  }
    
  //move data at A into B's spot (make sure A's parent updates it's child pointer)
  int parent_ptr = tree[a].x;
  tree[parent_ptr].y = b;
  for(i=0; i<8; i++){
    tree[b+i] = tree[a+i];
    
    //update the parent pointers for each child
    int childIndex = tree[b+i].y;
    if(childIndex >= 0){        //if it's not a leaf
      for(int j=0; j<8; j++)
        tree[childIndex+j].x = b+i;
    }
  }

  //move buffer (B's data) into A's spot (make sure B's parent updates it's child pointer)
  parent_ptr = buff[0].x;
  tree[parent_ptr].y = a;
  for(i=0; i<8; i++){
    tree[a+i] = buff[i];
    
    //update the parent pointers for each child
    int childIndex = tree[a+i].y;
    if(childIndex >= 0){
      for(int j=0; j<8; j++)
        tree[childIndex+j].x = a+i;
    }
  }
}

////////////////////////////////////////////
//reformat tree method
///////////////////////////////////////////
void reformat_tree(__global int4 *tree, __global float* output)
{
  unsigned curr_index = 0;
  ocl_stack open;
  init_ocl_stack(&open);
  push(&open, 0);

  int bugcount = 0;

  while(!empty(&open)){
    //examine node at top of stack
    int currNode = pop(&open);

    //recall tree's are organized as such:
    //1) parent pointer, 2) child pointer 3) data pointer 4) nothing right now
    //if the current node has no children, nothing to verify
    int child_ptr = tree[currNode].y;
    if(child_ptr < 0){
      continue;
    }

    //if child pointer isn't to the right place..
    if(child_ptr != curr_index+1){           
      //-- need to swap 8 nodes at currNode[1] to curr_index+1 --//
      swap_eight(tree, child_ptr, curr_index+1);
      child_ptr = curr_index+1;
    }  

    //TODO this should be done all in one go...
    for(int i=7; i>=0; i--){
      push(&open, child_ptr+i);
    }
    curr_index += 8;
  }
  
}

///////////////////////////////////////////
//REFINE MAIN
//TODO include CELL LEVEL SOMEHOW to make sure cells don't over split
//TODO include a debug print string at the end to know what the hell is going on.
///////////////////////////////////////////
__kernel
void
refine_main(__global int4     *tree,          //tree structure
            __global unsigned *tree_size,
            __global unsigned *tree_max_size,
            __global float16  *data,           //tree data
            __global unsigned *data_size,
            __global float    *prob_thresh,
            __global float      *output)    //surface probability threshold for refinement
{
  unsigned gid = get_global_id(0);
  unsigned lid = get_local_id(0);
 
  
  //Threshold thing for now
  float max_alpha_int = (-1)*log(1.0 - (*prob_thresh));
  unsigned tSize = (*tree_size);
 
  if(gid==0) { //only do it on one core
    
    /////////////////////////////////////////////////////////////////
    //SPLIT TREE
    //Depth first search iteration of the tree (keeping track of node level)
    //1) parent pointer, 2) child pointer 3) data pointer 4) nothing right now
    /////////////////////////////////////////////////////////////////
    //need to keep track of current level and pops for each level
    int popCounts[10];
    for(int i=0; i<10; i++) popCounts[i]=0; 
    int currLevel = 0;

    //stack for depth first traversal
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
        
        ////////////////////////////////////////
        //INSERT LEAF SPECIFIC CODE HERE
        ////////////////////////////////////////
        //find side length for cell of this level (bit shift: two_pow_level = 1<<currLevel;)
        //int two_pow_level = pow(2,currLevel);
        unsigned two_pow_level = 1<<currLevel;
        float side_len = 1.0/two_pow_level;
        
        //get alpha value for this cell;
        int dataIndex = tree[currNode].z;
        float16 datum = data[dataIndex];
        float alpha = datum.s0;
        
        //integrate alpha value
        float alpha_int = alpha * side_len;

        //IF alpha value triggers split, tack on 8 children to end of tree array
        //make sure the PARENT cell for each of the new children points to i
        //ALSO make sure currLevel is less than MAX_LEVELS
        if(alpha_int > max_alpha_int)  {
          //node I points to tSize - the place where it's children will be tacked on
          tree[currNode].y = tSize;
          for(int j=0; j<8; j++){
            tree[tSize+j].x = currNode;   //PARENT POINTS TO NODE THAT SPLIT
            tree[tSize+j].y = -1;         //HAS NO CHILDREN
            tree[tSize+j].z = 0;          //NO DATA FOR NOW
          }
          tSize += 8; //update tree size 
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
    tree_size[0] = tSize;
     
    
    /////////////////////////////////////////////////////////////////
    //REFORMAT TREE into cannonical order
    /////////////////////////////////////////////////////////////////
    reformat_tree(tree, output);  
  }
  
}




