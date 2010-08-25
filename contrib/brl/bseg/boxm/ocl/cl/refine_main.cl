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
void swap_eight(__global int4 *tree, int a, int b, __global float* output)
{

  //update A and B's Parent's child pointer first
  int parent_ptr = tree[a].x;
  tree[parent_ptr].y = b;
  parent_ptr = tree[b].x;
  tree[parent_ptr].y = a; 
  
  //copy B to a buffer
  int4 buff[8];
  for(int i=0; i<8; i++)
    buff[i] = tree[b+i];
  
  //copy A into B
  for(int i=0; i<8; i++)
    tree[b+i] = tree[a+i];
  
  //copy buffer into A
  for(int i=0; i<8; i++)
    tree[a+i] = buff[i];

  //for each child of B and A, update the parent pointer
  for(int i=0; i<8; i++){
    int childA = tree[a+i].y;
    int childB = tree[b+i].y;
    if(childA > 0) {
      for(int j=0; j<8; j++)
        tree[childA+j].x = a+i;
    }
    if(childB > 0) {
      for(int j=0; j<8; j++)
        tree[childB+j].x = b+i; 
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
 
  int count = 0;
  while(!empty(&open)){
    count++;
    //examine node at top of stack
    int currNode = pop(&open);
    //recall tree's are organized as such:
    //1) parent pointer, 2) child pointer 3) data pointer 4) nothing right now
    //if the current node has no children, nothing to verify
    int child_ptr = tree[currNode].y;
    bool isleaf = (child_ptr < 0);
    if(!isleaf){
      //if child pointer isn't to the right place..
      if(child_ptr != curr_index+1){      
        (*output) = curr_index+1;    
        //-- need to swap 8 nodes at currNode[1] to curr_index+1 --//
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
 
///////////////////////////////////////////
//REFINE MAIN
//TODO include CELL LEVEL SOMEHOW to make sure cells don't over split
//TODO include a debug print string at the end to know what the hell is going on.
///////////////////////////////////////////
__kernel
void
refine_main(__global int4     *tree,           //tree structure
            __global unsigned *tree_size,      //current size of tree buffer
            __global unsigned *tree_max_size,  //max size for tree buffer
            __global float16  *data,           //tree data
            __global unsigned *data_size,      //current size of data buffer
            __global unsigned *data_max_size,  //max size for data buffer
            __global float    *prob_thresh,    //refinement threshold
            __global unsigned *max_level,      //maximum number of levels for tree
            __global float    *bbox_len,       //
            __global float      *output)       //TODO delete me later
{
  unsigned gid = get_global_id(0);
  unsigned lid = get_local_id(0);
 
  //Threshold thing for now
  float max_alpha_int = (-1)*log(1.0 - (*prob_thresh));
  unsigned tSize = (*tree_size);
  unsigned dSize = (*data_size);
  unsigned maxLevel = (*max_level);
  float boxLen = (*bbox_len);
 
  if(gid==0) { //only do it on one core
   
    ///////////////////////////////////////////////////////////////
    //SPLIT TREE
    //Depth first search iteration of the tree (keeping track of node level)
    //1) parent pointer, 2) child pointer 3) data pointer 4) nothing right now
    ///////////////////////////////////////////////////////////////
    //need to keep track of current level and pops for each level
    (*output) = 0;
    int popCounts[11];
    for(int i=0; i<11; i++) popCounts[i]=0;
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
        ////////////////////////////////////////
        //INSERT LEAF SPECIFIC CODE HERE
        ////////////////////////////////////////
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
          int4 treecell=tree[currNode];
          treecell.y= tSize;
          tree[currNode]=treecell;
          for(int j=0; j<8; j++){
            int4 tcell = (int4) (currNode, -1, (int)dSize+j, 0);
            tree[tSize+j] = tcell;   //PARENT POINTS TO NODE THAT SPLIT
            //tree[tSize+j]   //HAS NO CHILDREN
            //tree[tSize+j]   //point to next piece of data
           
            //copy data to new children, along with new alpha
            float16 newData = datum;
            newData.s0 = new_alpha;
            data[dSize+j] = newData;
          }
          tSize += 8; //update tree size
          dSize += 8; //update data buffer size
       
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
    //tree and data size output
    tree_size[0] = tSize;
    data_size[0] = dSize;
        
    ///////////////////////////////////////////////////////////////////
    ////REFORMAT TREE into cannonical order
    ///////////////////////////////////////////////////////////////////
    reformat_tree(tree, output); 
  }
 
}
 
 
 
 
