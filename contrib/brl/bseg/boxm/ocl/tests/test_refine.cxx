#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "open_cl_test_data.h"
#include <boxm/ocl/boxm_refine_manager.h>
#include <boxm/boxm_scene.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_vector.h>
#include <vcl_stack.h>
#include <boxm/ocl/boxm_ocl_utils.h>

//TODO verify that data_array_ is also in a good format...

//: Verifies that a tree is in the canonical format in the array
// depth first search
bool verify_format(vcl_vector<vnl_vector_fixed<int, 4> > cell_array){
  unsigned curr_index = 0;
  vcl_stack<int> open;
  open.push(0);
  while(!open.empty()){
    
    int currNode = open.top();
    open.pop();
    int child_ptr = cell_array[currNode][1];
    
    //if the current node has no children, nothing to verify
    if(child_ptr < 0) {
      continue; 
    }
    //if child pointer isn't to teh right place..
    if(child_ptr != curr_index+1){
      return false;
    }  

    //push children on stack in reverse order
    for(int i=7; i>=0; i--){
      open.push(child_ptr+i);
    }
    curr_index += 8;
  }
  return true;
}

bool test_refine_simple_scene()
{
  // Set up test tree
  boct_tree<short,float>* tree = open_cl_test_data::tree<float>();
  float prob_thresh = .3;
  unsigned max_level = 3;
  
  // refine the scene using the opencl refine manager
  boxm_refine_manager<float>* mgr = boxm_refine_manager<float>::instance();
  if(!mgr->init(tree, prob_thresh, max_level)) {
    TEST("Error : boxm_refine : mgr->init() failed\n", false, true);
    return false;
  }
  if(!mgr->run_tree()) {
    TEST("Error : boxm_refine : mgr->run_tree() failed\n", false, true);
    return false;
  }
  
  
  //extract the output scene from the manager
  int* tree_array = mgr->get_tree();
  int  tree_size = mgr->get_tree_size();
  float* data = mgr->get_data();
  if(!tree_array) {
    TEST("Error : boxm_refine : mgr->get_tree() returned NULL\n", false, true);
    return false;
  }
  if(!tree_size) {
    TEST("ERROR : boxm_refine : mgr->get_tree_size() returned NULL\n", false, true);
    return false;
  }
  
  //make sure the array is in the right order
  vcl_vector<vnl_vector_fixed<int,4> > tree_vector;
  for(unsigned i=0,j=0; j<tree_size; i+=4,j++){
    vnl_vector_fixed<int,4> cell;
    for(unsigned k=0; k<4; k++){
      cell[k] = tree_array[i+k];
    }
    tree_vector.push_back(cell);
  }
  
  bool correctFormat = verify_format(tree_vector);
  TEST("test_refine_simple_scene output format", correctFormat, true);

  //free memory used by the manager
  mgr->clean_refine();
}

static void test_refine()
{
  if (test_refine_simple_scene())
    vcl_cout<<"test_refine, simple scene"<<vcl_endl;
}

TESTMAIN(test_refine);
