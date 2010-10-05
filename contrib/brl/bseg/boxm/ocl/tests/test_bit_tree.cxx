//Tests bit tree OpenCL traverse, traverse_force and others
//will run ocl scene comparison as well. 
#include <testlib/testlib_test.h>
#include "open_cl_test_data.h"
#include <boct/boct_bit_tree.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include "bit_tree_test_manager.h"
#include "ocl_scene_test_manager.h"
#include <boxm/ocl/boxm_ocl_bit_scene.h>


#if 0 // most tests are commented out for the time being ...
static void test_loc_code(bit_tree_test_manager* test_mgr)
{
  if (!test_mgr->run_test(0)) {
    TEST("Run Kernel test_loc_code", false, true);
    return;
  }
  else {
    TEST("Run Kernel test_loc_code", true, true);
  }
  cl_int* results = test_mgr->get_output();
  vcl_size_t size = 4*4;
  if (results) {
    int test[]={0,0,0,0,
                2,0,0,0,
                0,2,0,0,
                3,2,3,0};
    bool good = true;
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_loc_code_data", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_loc_code_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
    else
    {
      vcl_cout<<"Time spent test_loc_code "<<test_mgr->gpu_time_/(10000.0)<<" ms"<<vcl_endl;
    }
  }
}
#endif // 0

#if 0 // most tests are commented out for the time being ...
static void test_bit_at(bit_tree_test_manager* test_mgr)
{
  bool good = false;
  TEST("Run Kernel test_bit_at", good=test_mgr->run_test(1), true); 
  if(!good) return;
  
  cl_int* results = test_mgr->get_output();
  vcl_size_t size = 16*4;
  if (results)
  {
    int test[] = {1,
                  1,1,1,1,1,1,1,1,
                  1,0,0,0,0,0,0,0,  1,0,0,0,0,0,0,0,
                  1,0,0,0,0,0,0,0,  1,0,0,0,0,0,0,0,
                  1,0,0,0,0,0,0,0,  1,0,0,0,0,0,0,0,
                  1,0,0,0,0,0,0,0,  1,0,0,0,0,0,0,0
                  };
    bool good = true;
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_bit_at_results", good, true);
    if (!good) {
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_bit_at_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_bit_at_should_be(" << test[i] << ' '
                 << test[i+1] << ' '
                 << test[i+2] << ' '
                 << test[i+3] << ")\n";
    }
  }
}
#endif // 0

#if 0 // most tests are commented out for the time being ...
static void test_traverse(bit_tree_test_manager* test_mgr, boct_tree<short, float>* tree)
{
  bool good = false;
  TEST("Run Kernel test_traverse", good=test_mgr->run_test(2), true); 
  if(!good) return;
  
  cl_int* results = test_mgr->get_output();
  cl_float* bbox_results = test_mgr->get_bbox_output();
 
  //make sure results match boct_tree
  boct_loc_code<short> codes[8]; 
  codes[0].set_code(0,0,0);
  codes[1].set_code(4,0,0);
  codes[2].set_code(5,1,1);   
  codes[3].set_code(6,6,2);  
  codes[4].set_code(1,1,6);   
  codes[5].set_code(4,0,4);   
  codes[6].set_code(4,4,5);   
  codes[7].set_code(6,6,6);
  
  //check codes 
  good = true;
  for(int i=0; i< 8; i++) {
    boct_tree_cell<short,float>* cell = tree->get_cell(codes[i]);
    boct_loc_code<short> code = cell->get_code();
    
    bool same = (code.x_loc_ == results[8*i]) && 
                (code.y_loc_ == results[8*i+1]) &&
                (code.z_loc_ == results[8*i+2]) &&
                (code.level  == results[8*i+3]);
    
    if(!same) {
      vcl_cout << "result: ["
               << results[8*i] << ' '
               << results[8*i+1] << ' '
               << results[8*i+2] << "] level: "
               << results[8*i+3] << " should be: "
               << code << " level: " << code.level << vcl_endl;
    }
    good = good && same;
    
    //check index
    int index = boct_bit_tree::loc_code_to_index(code, 3);
    same = (index == results[8*i+4]);
    if(!same) {
      vcl_cout << "result index [" 
               << results[8*i] << "," << results[8*i+1] << ","
               << results[8*i+2] << "," << results[8*i+3] << "] "
               << "  index: " << results[8*i+4] 
               << " should be: "<<index<< vcl_endl;
    }
    good = good && same;
    
  }
  TEST("Test_traverse code results", good, true);
  
  vcl_cout<<"Time spent test_traverse "<<test_mgr->gpu_time_/(10000.0*8)<<" ms"<<vcl_endl;


  //check BOUNDING BOX results
  vcl_cout<<"BBOX Results--------------------------------------"<<vcl_endl;
  for(int i=0; i<8; i++) {
    vnl_vector_fixed<float,4> cell_min, cell_max, cell_min_opt, cell_max_opt;
    for(int j=0; j<4; j++) {
      cell_min[j] = bbox_results[16*i + j];
      cell_max[j] = bbox_results[16*i + 4 + j];
      cell_min_opt[j] = bbox_results[16*i + 8 + j];
      cell_max_opt[j] = bbox_results[16*i + 12 + j];
    }
    vcl_cout<<"point "<<i<<vcl_endl;
    vcl_cout<<"old bbox: min="<<cell_min<<"  max="<<cell_max<<vcl_endl
            <<" box size: "<<cell_max[0]-cell_min[0]<<vcl_endl;
    vcl_cout<<"new bbox: min="<<cell_min_opt<<"  max="<<cell_max_opt<<vcl_endl
            <<" box size: "<<cell_max_opt[0]-cell_min_opt[0]<<vcl_endl;
  }
  
  //debug print just to see all resutls
  int index = 0;
  for(int i=0; i<16; i++) {
    for(int j=0; j<4; j++) {
      vcl_cout<<results[index++]<<' ';
    }
    vcl_cout<<'\n';
  }
  
}
#endif // 0

#if 0 // most tests are commented out for the time being ...
static void test_traverse_force(bit_tree_test_manager* test_mgr, boct_tree<short, float>* tree)
{
  //test the execution 
  bool good = false;
  TEST("Run Kernel test_traverse_force", good=test_mgr->run_test(3), true); 
  if(!good) return;
  
  //get results 
  cl_int* results = test_mgr->get_output();
  
  //make sure results match boct_tree
  boct_loc_code<short> start_code; start_code.level = 1;
  boct_loc_code<short> target_codes[4]; 
  target_codes[0].set_code(2,0,0); target_codes[0].level = 0;
  target_codes[1].set_code(0,2,0); target_codes[1].level = 0;
  target_codes[2].set_code(0,0,2);  target_codes[2].level = 0;
  target_codes[3].set_code(2,2,2);  target_codes[3].level = 0;

  for(int i=0; i<4; i++) {
    //get start cell; run traverse force on boct_tree
    boct_tree_cell<short,float>* start = tree->get_cell(start_code);
    boct_tree_cell<short,float>* result = start->traverse_force(target_codes[i]);
    boct_loc_code<short> result_code = result->get_code();
    
    //compare 
    bool same = (result_code.x_loc_ == results[8*i]) && 
                (result_code.y_loc_ == results[8*i+1]) &&
                (result_code.z_loc_ == results[8*i+2]) &&
                (result_code.level  == results[8*i+3]);
    
    if(!same) {
      vcl_cout << "result: ["
               << results[8*i] << ' '
               << results[8*i+1] << ' '
               << results[8*i+2] << "] level: "
               << results[8*i+3] << " should be: "
               << result_code << " level: " << result_code.level << vcl_endl;
    }
    good = good && same;
  }
  
  for(int i=0; i<8; i++) {
      vcl_cout << "result: ["
               << results[4*i] << ' '
               << results[4*i+1] << ' '
               << results[4*i+2] << "] level: "
               << results[4*i+3] << vcl_endl;
  }
  TEST("test_traverse_force_data", good, true);
  vcl_cout<<"Time spent traverse_force "<<test_mgr->gpu_time_/(10000.0)<<" ms"<<vcl_endl;
}
#endif // 0

static void compare_traverse()
{  
  boct_tree<short, float>* btree = open_cl_test_data::four_tree<float>();
  bool good = false;
  cl_int* results;
  
  /////////////////////////////////////////////////////////////////////////
  //////// 1. test bit_tree traverse
  /////////////////////////////////////////////////////////////////////////
  bit_tree_test_manager* test_mgr = bit_tree_test_manager::instance();
  if(!test_mgr){
    vcl_cout<<"bit_tree_test instance failed"<<vcl_endl;
    return;
  }
  test_mgr->init_arrays();
  
  //set tree in manager
  boct_bit_tree test_tree = boct_bit_tree(btree);
  test_mgr->set_tree(test_tree.get_bits());
  test_mgr->init_manager();
  
  //run traverse;
  good=test_mgr->run_test(2); 
  TEST("Bit tree traverse runs ", good, true);
  results = test_mgr->get_output();
  for(int i=0; i<16; i++)
    vcl_cout<<results[i]<<vcl_endl;

  
  /////////////////////////////////////////////////////////////////////
  //// 2. test ocl scene tree
  /////////////////////////////////////////////////////////////////////
  //ocl_scene_test_manager* ocl_mgr = ocl_scene_test_manager::instance();
  //if(!ocl_mgr) {
    //vcl_cout<<"OCL Manager instance failed"<<vcl_endl;
    //return;
  //}
  //ocl_mgr->init_arrays();
  
  ////set up tree
  //typedef vnl_vector_fixed<int,4> int4;
  //typedef vnl_vector_fixed<float,16> float16;
  //int4 arr_root(0);
  //arr_root[0] = -1; //no parent for root
  //arr_root[1] = -1; // no children yet
  //vcl_vector<int4> cell_array;
  //cell_array.push_back(arr_root);
  //vcl_vector<float16> data_array;
  //float16 dat_init(0.0);
  //data_array.push_back(dat_init);
  //boxm_ocl_convert<float>::copy_to_arrays(btree->root(), cell_array, data_array, 0);
  //ocl_mgr->set_tree(cell_array);
  
  ////run traverse;
  //ocl_mgr->init_manager();
  //good = ocl_mgr->run_test(0);
  //TEST("Ocl tree traverse runs ", good, true);
  //results = ocl_mgr->get_output();
  //for(int i=0; i<16; i++)
    //vcl_cout<<results[i]<<vcl_endl;
  
}


static void compare_float3()
{
  
  bit_tree_test_manager* test_mgr = bit_tree_test_manager::instance();
  if(!test_mgr){
    vcl_cout<<"bit_tree_test instance failed"<<vcl_endl;
    return;
  }
  test_mgr->init_arrays();
  test_mgr->init_manager();

  //compare two kernel runs
  test_mgr->run_test(4); //test float 4
  vcl_cout<<"Time spent float4 "<<test_mgr->gpu_time_/(100000.0)<<" ms"<<vcl_endl;

  cl_float* bbox_results = test_mgr->get_bbox_output();
  int index = 0;
  for(int i=0; i<2; i++) {
    for(int j=0; j<4; j++) {
      vcl_cout<<bbox_results[index++]<<" ";
    }
    vcl_cout<<'\n';
  }

  test_mgr->run_test(5); //test float 3
  vcl_cout<<"Time spent float3 "<<test_mgr->gpu_time_/(100000.0)<<" ms"<<vcl_endl;

  bbox_results = test_mgr->get_bbox_output();
  index = 0;
  for(int i=0; i<2; i++) {
    for(int j=0; j<4; j++) {
      vcl_cout<<bbox_results[index++]<<" ";
    }
    vcl_cout<<'\n';
  }

  
}

static void test_scene_info()
{
  bit_tree_test_manager* test_mgr = bit_tree_test_manager::instance();
  if(!test_mgr){
    vcl_cout<<"bit_tree_test instance failed"<<vcl_endl;
    return;
  }
  test_mgr->init_arrays();
  test_mgr->init_manager();
  test_mgr->run_test(6); //test float 4
  cl_float* bbox_results = test_mgr->get_bbox_output();
  int index = 0;
  for(int i=0; i<7; i++) {
    for(int j=0; j<4; j++) {
      vcl_cout<<bbox_results[index++]<<" ";
    }
    vcl_cout<<'\n';
  }
}

static void test_bit_tree()
{
  vcl_cout<<"Testing Bit Tree"<<vcl_endl;

  ////load up the manager for testing
  bit_tree_test_manager* test_mgr = bit_tree_test_manager::instance();
  TEST("bit_tree_test_manager::instance()", !test_mgr, false);
  if (!test_mgr) return;

  ////prepare an octree for testing
  boct_tree<short, float>* boct_tree = open_cl_test_data::four_tree<float>();
  
  ////prepare boct_bit_tree (test_tree)
  boct_bit_tree test_tree = boct_bit_tree(boct_tree);
  
  ////set test tree and initialize manager
  bool init_arrays = test_mgr->init_arrays();
  unsigned char* bits = test_tree.get_bits();
  test_mgr->set_tree(bits);
  bool init_mgr = test_mgr->init_manager();
  //TEST("bit_tree_test_manager::init_manager()", init_mgr, true);

  //Run first test
  //if (init_mgr) test_loc_code(test_mgr);
  
  //run bit_at test
  //if (init_mgr) test_bit_at(test_mgr);
  
  //traverse test...
  //if(init_mgr) test_traverse(test_mgr, boct_tree);
  
  //traverse force test ... 
  //if (init_mgr) test_traverse_force(test_mgr, boct_tree);
  
  //comparison test:
  //compare_traverse();
  
  //compare_float3();
  test_scene_info();
}

TESTMAIN(test_bit_tree);
