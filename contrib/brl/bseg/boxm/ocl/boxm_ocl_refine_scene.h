#ifndef boxm_ocl_refine_scene_h_
#define boxm_ocl_refine_scene_h_
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_rt_sample.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <vil/vil_transform.h>

#include <bocl/bocl_cl.h>
#include <boxm/ocl/boxm_refine_manager.h>

#include <vcl_where_root_dir.h>
#include <vcl_iostream.h>

//includes for comparison to cpu
#include <vcl_stack.h>
#include <vgl/vgl_box_3d.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vul/vul_timer.h>

//comparison function
template <boxm_apm_type APM>
void boxm_ocl_refine_scene_cpu(boct_tree<short, boxm_sample<APM> >* tree, float prob_thresh);


template <boxm_apm_type APM>
void boxm_ocl_refine_scene(boxm_scene<boct_tree<short, boxm_sample<APM> > >* scene,
                                     float prob_thresh)
{
  typedef boxm_sample<APM> data_type; 
  typedef boct_tree<short, data_type > tree_type;

  // render the image using the opencl raytrace manager
  boxm_refine_manager<data_type >* mgr = boxm_refine_manager<data_type >::instance();
  
  //loop through the blocks of scene, refine each tree
  boxm_block_iterator<tree_type > iter(scene);
  int count = 0;
  for(iter.begin(); !iter.end(); iter++) {
    scene->load_block(iter.index());
    boxm_block<tree_type >* block = scene->get_active_block();
    tree_type* tree = block->get_tree();    
  
    //initialize the manager
    if(!mgr->init(tree, prob_thresh)) {
      vcl_cout<<"ERROR : boxm_refine : mgr->init() failed"<<vcl_endl;
      return;
    }
    
    //run the refine method
    if (!mgr->run_tree()) {
      vcl_cout<<"ERROR : boxm_refine : mgr->run_tree() failed"<<vcl_endl;
      return;
    }
    
    
    // extract the output scene from the manager
    int* tree_array = mgr->get_tree();
    int  tree_size = mgr->get_tree_size();
    float* data = mgr->get_data();
    int  data_size = mgr->get_data_size();
    
    //////////////////////////////////////////
    //Do something with tree array here
    /////////////////////////////////////////
    vcl_cout<<"REFINED TREE! -> size:"<<tree_size<<vcl_endl;
      

    //clean up after each tree
    mgr->clean_refine();    
      
      
    //refine using cpu for comparison
    if(count == 1)
      boxm_ocl_refine_scene_cpu<APM>(tree, prob_thresh);
    count++;
  }
}


/////////////////////////////////////////////////
//COMPARISON METHODS
//Compares CPU Swapping time to GPU swapping time
////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Swaps 8 blocks of data from index A to index B
// makes sure to switch A and B's parents to point to the new A and B locations
// makes sure to switch A and B's children to point to the new A and B locations
// makes sure to update the location of nodes in the STACK
///////////////////////////////////////////////////////////////////////
void swap_eight(vcl_vector<vnl_vector_fixed<int, 4> > &tree, int a, int b)
{
  //update A and B's Parent's child pointer first
  int parent_ptr = tree[a][0];
  tree[parent_ptr][1] = b;
  parent_ptr = tree[b][0];
  tree[parent_ptr][1] = a; 
  
  //copy B to a buffer
  vnl_vector_fixed<int, 4> buff[8];
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
    int childA = tree[a+i][1];
    int childB = tree[b+i][1];
    for(int j=0; j<8; j++){
      tree[childA+j][0] = a+i;
      tree[childB+j][0] = b+i; 
    }
  }
}
 
////////////////////////////////////////////
//reformat tree method
///////////////////////////////////////////
void reformat_tree(vcl_vector<vnl_vector_fixed<int, 4> > &tree)
{
  unsigned curr_index = 0;
  vcl_stack<int> open;
  open.push(0);
 
  while(!open.empty()){
    
    //examine node at top of stack
    int currNode = open.top(); open.pop();
    //recall tree's are organized as such:
    //1) parent pointer, 2) child pointer 3) data pointer 4) nothing right now
    //if the current node has no children, nothing to verify
    int child_ptr = tree[currNode][1];
    bool isleaf = (child_ptr < 0);
    if(!isleaf){
      //if child pointer isn't to the right place..
      if(child_ptr != curr_index+1){          
        //-- need to swap 8 nodes at currNode[1] to curr_index+1 --//
        swap_eight(tree, child_ptr, curr_index+1);
        child_ptr = curr_index+1;  
      } 
      //push children on stack (in reverse order)
      for(int i=7; i>=0; i--){
        open.push(child_ptr+i);
      }
      curr_index += 8;
    }
  }
}
 


//Comparison method - does the swapping algorithm on the CPU and outputs times
template <boxm_apm_type APM>
void boxm_ocl_refine_scene_cpu(boct_tree<short, boxm_sample<APM> >* tree, float prob_thresh)
{
  
  //use vectors to build the tree up
  vcl_vector<vnl_vector_fixed<int, 4> > cells;
  vcl_vector<vnl_vector_fixed<float, 16>  > data;

  // put the root into the cell array and its data in the data array
  boct_tree_cell<short, boxm_sample<APM> >* root = tree->root();
  int cell_ptr = 0;
  vnl_vector_fixed<int, 4> root_cell(0);
  root_cell[0]=-1; // no parent
  root_cell[1]=-1; // no children at the moment
  root_cell[2]=-1; // no data at the moment
  cells.push_back(root_cell);
  boxm_ocl_convert<boxm_sample<APM> >::copy_to_arrays(root, cells, data, cell_ptr);  
  
  vul_timer t;
  //////////////////////////////////////////
  //begin the refinement
  //////////////////////////////////////////
  vgl_box_3d<double> bbox = tree->bounding_box();
  float boxLen = (float) (bbox.max_z() - bbox.min_z());
  float max_alpha_int = (-1)*log(1.0 - prob_thresh);
  int maxLevel = tree->number_levels();
  int tSize = cells.size();
  int dSize = data.size();
  int popCounts[11];
  for(int i=0; i<11; i++) popCounts[i]=0;
  int currLevel = 0;
 
  ////stack for depth first traversal
  vcl_stack<int> open;
  open.push(0);
  while(!open.empty()){
  
    //figure out the current level
    while(popCounts[currLevel]>=8) {
      popCounts[currLevel] = 0;
      currLevel--;
    }
   
    //examine node at top of stack (and keep track of popping)
    int currNode = open.top(); open.pop();
    popCounts[currLevel]++;
    
    //if the current node has no children, it's a leaf -> check if it needs to be refined
    int child_ptr = cells[currNode][1];
    if(child_ptr < 0){
      
      ////////////////////////////////////////
      //INSERT LEAF SPECIFIC CODE HERE
      ////////////////////////////////////////
      //find side length for cell of this level (bit shift: two_pow_level = 1<<currLevel;)
      //int two_pow_level = pow(2,currLevel);
      unsigned two_pow_level = 1<<currLevel;
      float side_len = boxLen/two_pow_level;
     
      //get alpha value for this cell;
      int dataIndex = cells[currNode][2];
      vnl_vector_fixed<float, 16>  datum = data[dataIndex];
      float alpha = datum[0];
     
      //integrate alpha value
      float alpha_int = alpha * side_len;

      //IF alpha value triggers split, tack on 8 children to end of tree array
      //make sure the PARENT cell for each of the new children points to i
      //ALSO make sure currLevel is less than MAX_LEVELS
      if(alpha_int > max_alpha_int && currLevel < maxLevel)  {
        //new alpha for the child nodes
        float new_alpha = max_alpha_int / side_len;  
        
        //node I points to tSize - the place where it's children will be tacked on
        cells[currNode][1] = tSize;
        for(int j=0; j<8; j++){
          vnl_vector_fixed<int, 4> newCell(currNode, -1, dSize+j, 0);
          cells.push_back(newCell);
         
          //copy data to new children, along with new alpha
          vnl_vector_fixed<float, 16> newData;
          newData[0] = new_alpha;
          data.push_back(newData);
        }
        tSize += 8; //update tree size
        dSize += 8; //update data buffer size
     
        //reset data for curent node
        vnl_vector_fixed<float, 16> zeroDat;
        data[dataIndex] =  zeroDat; 
      }
      ////////////////////////////////////////////
      //END LEAF SPECIFIC CODE
      ////////////////////////////////////////////
     
    }
    //for inner nodes
    else {                      
      for(int i=7; i>=0; i--){
        open.push(child_ptr+i);
      }
      currLevel++;
    }
  }
  
  float refine_time = (float) t.all()/1e3f;
  vcl_cout<<"refine time: "<<refine_time<<vcl_endl;
  vcl_cout<<"num cells now: "<<cells.size()<<vcl_endl;

  //after splitting reformat
  reformat_tree(cells);

  float cpu_time = (float) t.all()/1e3f;

  //make sure it's correct
  bool isCorrect = boxm_ocl_utils::verify_format(cells);
  
  //OUTPUT STATS  
  int numCells = tree->all_cells().size();
  vcl_cout<<vcl_endl
          <<"----CPU SWAPPING STATS---------------"<<vcl_endl
          <<"tree size (numcells): "<<numCells<<vcl_endl
          <<"cpu time : "<<cpu_time<<" s"<<vcl_endl
          <<"format correct : "<< (isCorrect?" yes":" no") <<vcl_endl
          <<"-------------------------------------"<<vcl_endl; 

}



#endif

