#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "open_cl_test_data.h"
#include <boct/boct_bit_tree.h>
#include "bit_tree_test_manager.h"



//static void test_traverse(octree_test_driver<T> & driver)
//{
  //if (driver.create_kernel("test_traverse")!=SDK_SUCCESS) {
    //TEST("Create Kernel test_traverse", false, true);
    //return;
  //}
  //if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    //TEST("Run Kernel test_traverse", false, true);
    //return;
  //}
  //cl_int* results = driver.tree_results();
  //vcl_size_t size = 16*4;
  //if (size>driver.tree_result_size_bytes())
    //return;
  //if (results)
  //{
    //int test[] = {0,0,0,0,
                  //9,9,9,9,
                  //2,0,0,0,
                  //17,17,17,17,
                  //0,2,0,0,
                  //25,25,25,25,
                  //2,2,0,0,
                  //33,33,33,33,
                  //0,0,2,0,
                  //41,41,41,41,
                  //2,0,2,0,
                  //49,49,49,49,
                  //0,2,2,0,
                  //57,57,57,57,
                  //2,2,2,0,
                  //65,65,65,65};
    //bool good = true;
    //for (vcl_size_t i= 0; i<size; i++)
      //good = good && results[i]==test[i];
    //TEST("test_traverse_data", good, true);
    //if (!good)
      //for (vcl_size_t i= 0; i<size; i+=4)
        //vcl_cout << "test_traverse_result(" << results[i] << ' '
                 //<< results[i+1] << ' '
                 //<< results[i+2] << ' '
                 //<< results[i+3] << ")\n";
    //else
    //{
      //vcl_cout<<"Time spent test_traverse"<<driver.gpu_time()/(10000.0*8)<<" ms"<<vcl_endl;
    //}
  //}
  //driver.release_kernel();
//}

//static void test_traverse_to_level(octree_test_driver<T>& driver)
//{
  //if (driver.create_kernel("test_traverse_to_level")!=SDK_SUCCESS) {
    //TEST("Create Kernel test_traverse_to_level", false, true);
    //return;
  //}
  //if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    //TEST("Run Kernel test_traverse_to_level", false, true);
    //return;
  //}
  //cl_int* results = driver.tree_results();
  //vcl_size_t size = 16*4;
  //if (size>driver.tree_result_size_bytes())
    //return;
  //if (results)
  //{
    //int test[] ={0,0,0,1,
                 //1,1,1,1,
                 //2,0,0,1,
                 //2,2,2,2,
                 //0,2,0,1,
                 //3,3,3,3,
                 //2,2,0,1,
                 //4,4,4,4,
                 //0,0,2,1,
                 //5,5,5,5,
                 //2,0,2,1,
                 //6,6,6,6,
                 //0,2,2,1,
                 //7,7,7,7,
                 //2,2,2,1,
                 //8,8,8,8};
    //bool good = true;
    //for (vcl_size_t i= 0; i<size; i++)
      //good = good && results[i]==test[i];
    //TEST("test_traverse_to_level_data", good, true);
    //if (!good)
      //for (vcl_size_t i= 0; i<size; i+=4)
        //vcl_cout << "traverse_to_level_result(" << results[i] << ' '
                 //<< results[i+1] << ' '
                 //<< results[i+2] << ' '
                 //<< results[i+3] << ")\n";
    //else
    //{
      //vcl_cout<<"Time spent traverse_to_level"<<driver.gpu_time()/(10000.0*8)<<" ms"<<vcl_endl;
    //}
  //}
  //driver.release_kernel();
//}

//static void test_traverse_force(octree_test_driver<T>& driver)
//{
  //if (driver.create_kernel("test_traverse_force")!=SDK_SUCCESS) {
    //TEST("Create Kernel test_traverse_force", false, true);
    //return;
  //}
  //if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    //TEST("Run Kernel test_traverse_force", false, true);
    //return;
  //}

  //cl_int* results = driver.tree_results();
  //vcl_size_t size = 8*4;
  //if (size>driver.tree_result_size_bytes())
    //return;
  //if (results) {
    //int test[]={1,0,0,0,
                //10,10,10,10,
                //0,1,0,0,
                //11,11,11,11,
                //0,0,1,0,
                //13,13,13,13,
                //1,1,1,0,
                //16,16,16,16};
    //bool good = true;
    //for (vcl_size_t i= 0; i<size; i++)
      //good = good && results[i]==test[i];
    //TEST("test_traverse_force_data", good, true);
    //if (!good)
      //for (vcl_size_t i= 0; i<size; i+=4)
        //vcl_cout << "traverse_force_result(" << results[i] << ' '
                 //<< results[i+1] << ' '
                 //<< results[i+2] << ' '
                 //<< results[i+3] << ")\n";
    //else
    //{
      //vcl_cout<<"Time spent traverse_force"<<driver.gpu_time()/(10000.0)<<" ms"<<vcl_endl;
    //}
  //}
  //driver.release_kernel();
//}

//static void test_traverse_force_local(octree_test_driver<T>& driver)
//{
  //if (driver.create_kernel("test_traverse_force_local")!=SDK_SUCCESS) {
    //TEST("Create Kernel test_traverse_force_local", false, true);
    //return;
  //}
  //if (driver.run_tree_with_local_test_kernels()!=SDK_SUCCESS) {
    //TEST("Run Kernel test_traverse_force_local", false, true);
    //return;
  //}

  //cl_int* results = driver.tree_results();
  //vcl_size_t size = 8*4;
  //if (size>driver.tree_result_size_bytes())
    //return;
  //if (results) {
    //int test[]={1,0,0,0,
                //10,10,10,10,
                //0,1,0,0,
                //11,11,11,11,
                //0,0,1,0,
                //13,13,13,13,
                //1,1,1,0,
                //16,16,16,16};
    //bool good = true;
    //for (vcl_size_t i= 0; i<size; i++)
      //good = good && results[i]==test[i];
    //TEST("test_traverse_force_data", good, true);
    //if (!good)
      //for (vcl_size_t i= 0; i<size; i+=4)
        //vcl_cout << "traverse_force_result(" << results[i] << ' '
                 //<< results[i+1] << ' '
                 //<< results[i+2] << ' '
                 //<< results[i+3] << ")\n";
    //else
    //{
      //vcl_cout<<"Time spent traverse_force_local"<<driver.gpu_time()/(10000.0)<<" ms"<<vcl_endl;
    //}
  //}
  //driver.release_kernel();
//}

//static void test_cell_bounding_box(octree_test_driver<T>& driver)
//{
  //if (driver.create_kernel("test_cell_bounding_box")!=SDK_SUCCESS) {
    //TEST("Create Kernel test_cell_bounding_box", false, true);
    //return;
  //}
  //if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    //TEST("Run Kernel test_cell_bounding_box", false, true);
    //return;
  //}

  //cl_int* results = driver.tree_results();
  //vcl_size_t size = 16*4;
  //if (size>driver.tree_result_size_bytes())
    //return;
  //if (results)
  //{
    //int test[] = {0,0,0,0,
                  //250,250,250,0,
                  //500,0,0,0,
                  //750,250,250,0,
                  //0,500,0,0,
                  //250,750,250,0,
                  //500,500,0,0,
                  //750,750,250,0,
                  //0,0,500,0,
                  //250,250,750,0,
                  //500,0,500,0,
                  //750,250,750,0,
                  //0,500,500,0,
                  //250,750,750,0,
                  //500,500,500,0,
                  //750,750,750,0};
    //bool good = true;
    //for (vcl_size_t i= 0; i<size; i++)
      //good = good && results[i]==test[i];
    //TEST("test_cell_bounding_box_data", good, true);
    //if (!good)
      //for (vcl_size_t i= 0; i<size; i+=4)
        //vcl_cout << "cell_bounding_box_result(" << results[i] << ' '
                 //<< results[i+1] << ' '
                 //<< results[i+2] << ' '
                 //<< results[i+3] << ")\n";
    //else
    //{
      //vcl_cout<<"Time spent test_cell_bounding_box"<<driver.gpu_time()/(10000.0*8)<<" ms"<<vcl_endl;
    //}
  //}
  //driver.release_kernel();
//}

//static void test_intersect_cell(octree_test_driver<T>& driver)
//{
  //if (driver.create_kernel("test_intersect_cell")!=SDK_SUCCESS) {
    //TEST("Create Kernel test_intersect_cell", false, true);
    //return;
  //}
  //if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    //TEST("Run Kernel test_intersect_cell", false, true);
    //return;
  //}

  //cl_int* results = driver.tree_results();
  //vcl_size_t size = 16*4;
  //if (size>driver.tree_result_size_bytes())
    //return;
  //if (results)
  //{
//#if 0 // FIXME
    //int test[] = {0,0,0,0,
                  //250,250,250,0,
                  //500,0,0,0,
                  //750,250,250,0,
                  //0,500,0,0,
                  //250,750,250,0,
                  //500,500,0,0,
                  //750,750,250,0,
                  //0,0,500,0,
                  //250,250,750,0,
                  //500,0,500,0,
                  //750,250,750,0,
                  //0,500,500,0,
                  //250,750,750,0,
                  //500,500,500,0,
                  //750,750,750,0};
    //bool good = true;
    //for (vcl_size_t i= 0; i<size; i++)
      //good = good && results[i]==test[i];
    //TEST("test_intersect_cell_data", good, true);
    //if (!good)
      //for (vcl_size_t i= 0; i<size; i+=4)
        //vcl_cout << "intersect_cell_result(" << results[i] << ' '
                 //<< results[i+1] << ' '
                 //<< results[i+2] << ' '
                 //<< results[i+3] << ")\n";
//#endif
    //vcl_cout<<"Time spent test_intersect_cell "<<driver.gpu_time()/(10000.0*8)<<" ms"<<vcl_endl;
  //}
  //driver.release_kernel();
//}

//static void test_common_ancestor(octree_test_driver<T>& driver)
//{
  //if (driver.create_kernel("test_common_ancestor")!=SDK_SUCCESS) {
    //TEST("Create Kernel test_common_ancestor", false, true);
    //return;
  //}
  //if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    //TEST("Run Kernel test_common_ancestor", false, true);
    //return;
  //}

  //cl_int* results = driver.tree_results();
  //vcl_size_t size = 16*4;
  //if (size>driver.tree_result_size_bytes())
    //return;
  //if (results)
  //{
    //int test[] = {0,0,0,1,
                  //1,1,1,1,
                  //2,0,0,1,
                  //2,2,2,2,
                  //0,2,0,1,
                  //3,3,3,3,
                  //2,2,0,1,
                  //4,4,4,4,
                  //0,0,2,1,
                  //5,5,5,5,
                  //2,0,2,1,
                  //6,6,6,6,
                  //0,2,2,1,
                  //7,7,7,7,
                  //2,2,2,1,
                  //8,8,8,8};
    //bool good = true;
    //for (vcl_size_t i= 0; i<size; i++)
      //good = good && results[i]==test[i];
    //TEST("test_common_ancestor_data", good, true);
    //if (!good)
      //for (vcl_size_t i= 0; i<size; i+=4)
        //vcl_cout << "common_ancestor_result(" << results[i] << ' '
                 //<< results[i+1] << ' '
                 //<< results[i+2] << ' '
                 //<< results[i+3] << ")\n";
    //else
    //{
      //vcl_cout<<"Time spent test_common_ancestor"<<driver.gpu_time()/(10000.0*8)<<" ms"<<vcl_endl;
    //}
  //}
  //driver.release_kernel();
//}

//static void test_cell_exit_face(octree_test_driver<T>& driver)
//{
  //if (driver.create_kernel("test_cell_exit_face")!=SDK_SUCCESS) {
    //TEST("Create Kernel test_cell_exit_face", false, true);
    //return;
  //}
  //if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    //TEST("Run Kernel test_cell_exit_face", false, true);
    //return;
  //}

  //cl_int* results = driver.tree_results();
  //vcl_size_t size = 6*4;
  //if (size>driver.tree_result_size_bytes())
    //return;
  //if (results) {
    //int test[] = {1,0,0,0,
                  //1,0,0,1,
                  //0,1,0,0,
                  //0,1,0,1,
                  //0,0,1,0,
                  //0,0,1,1};
    //bool good = true;
    //for (vcl_size_t i= 0; i<size; i++)
      //good = good && results[i]==test[i];
    //TEST("test_cell_exit_face_data", good, true);
    //if (!good)
      //for (vcl_size_t i= 0; i<size; i+=4)
        //vcl_cout << "cell_exit_face_result(" << results[i] << ' '
                 //<< results[i+1] << ' '
                 //<< results[i+2] << ' '
                 //<< results[i+3] << ")\n";
    //else
    //{
      //vcl_cout<<"Time spent test_cell_exit_face"<<driver.gpu_time()/(10000.0)<<" ms"<<vcl_endl;
    //}
  //}
  //driver.release_kernel();
//}

//static void test_neighbor(octree_test_driver<T>& driver)
//{
  //if (driver.create_kernel("test_neighbor")!=SDK_SUCCESS) {
    //TEST("Create Kernel test_neighbor", false, true);
    //return;
  //}
  //if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    //TEST("Run Kernel test_neighbor", false, true);
    //return;
  //}

  //cl_int* results = driver.tree_results();
  //vcl_size_t size = 24*4;
  //if (size>driver.tree_result_size_bytes())
    //return;
  //if (results)
  //{
    //int test[] = {-1,-1,-1,-1,
                  //-1,-1,-1,-1,
                  //1,0,0,0,
                  //10,10,10,10,
                  //-1,-1,-1,-1,
                  //-1,-1,-1,-1,
                  //0,1,0,0,
                  //11,11,11,11,
                  //-1,-1,-1,-1,
                  //-1,-1,-1,-1,
                  //0,0,1,0,
                  //13,13,13,13,
                  //0,1,1,0,
                  //15,15,15,15,
                  //2,1,1,0,
                  //23,23,23,23,
                  //1,0,1,0,
                  //14,14,14,14,
                  //1,2,1,0,
                  //30,30,30,30,
                  //1,1,0,0,
                  //12,12,12,12,
                  //1,1,2,0,
                  //44,44,44,44};
    //bool good = true;
    //for (vcl_size_t i= 0; i<size; i++)
      //good = good && results[i]==test[i];
    //TEST("test_neighbor_data", good, true);
    //if (!good)
      //for (vcl_size_t i= 0; i<size; i+=4)
        //vcl_cout << "neighbor_result(" << results[i] << ' '
                 //<< results[i+1] << ' '
                 //<< results[i+2] << ' '
                 //<< results[i+3] << ")\n";
    //else
    //{
      //vcl_cout<<"Time spent test_neighbor"<<driver.gpu_time()/(10000.0*6)<<" ms"<<vcl_endl;
    //}
  //}
  //driver.release_kernel();
//}

//static void test_ray_trace(octree_test_driver<T>& driver)
//{
  //if (driver.create_kernel("test_ray_trace")!=SDK_SUCCESS) {
    //TEST("Create Kernel test_ray_trace", false, true);
    //return;
  //}
  //if (driver.run_ray_trace_test_kernels()!=SDK_SUCCESS) {
    //TEST("Run Kernel test_ray_trace", false, true);
    //return;
  //}

  //cl_int* results = driver.tree_results();
  //vcl_size_t size = 48*4;
  //if (size>driver.tree_result_size_bytes())
    //return;
  //if (results)
  //{
    //int test[] = {0,0,3,0, 250,250,250,250,
                  //0,0,2,0, 250,250,250,250,
                  //0,0,1,0, 250,250,250,250,
                  //0,0,0,0, 250,250,250,250,
                  //0,0,0,0, 250,250,250,250,
                  //1,0,0,0, 250,250,250,250,
                  //2,0,0,0, 250,250,250,250,
                  //3,0,0,0, 250,250,250,250,
                  //0,0,0,0, 250,250,250,250,
                  //0,1,0,0, 250,250,250,250,
                  //0,2,0,0, 250,250,250,250,
                  //0,3,0,0, 250,250,250,250,
                  //3,3,3,0, 433,433,433,433,
                  //2,2,2,0, 433,433,433,433,
                  //1,1,1,0, 433,433,433,433,
                  //0,0,0,0, 433,433,433,433,
                  //0,3,3,0, 433,433,433,433,
                  //1,2,2,0, 433,433,433,433,
                  //2,1,1,0, 433,433,433,433,
                  //3,0,0,0, 433,433,433,433,
                  //3,0,3,0, 433,433,433,433,
                  //2,1,2,0, 433,433,433,433,
                  //1,2,1,0, 433,433,433,433,
                  //0,3,0,0, 433,433,433,433};
    //bool good = true;
    //for (vcl_size_t i= 0; i<size; i++)
      //good = good && results[i]==test[i];
    //TEST("test_ray_trace_data", good, true);
    //if (!good)
      //for (vcl_size_t i= 0; i<size; i+=4)
        //vcl_cout << "ray_trace_result(" << results[i] << ' '
                 //<< results[i+1] << ' '
                 //<< results[i+2] << ' '
                 //<< results[i+3] << ")\n";
  //}
  //driver.release_kernel();
//}

//static void test_cell_contains_exit_pt(octree_test_driver<T>& driver)
//{
  //if (driver.create_kernel("test_cell_contains_exit_pt")!=SDK_SUCCESS) {
    //TEST("Create Kernel test_cell_contains_exit_pt", false, true);
    //return;
  //}
  //if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    //TEST("Run Kernel test_cell_contains_exit_pt", false, true);
    //return;
  //}
  //cl_int* results = driver.tree_results();
  //vcl_size_t size = 5*4;
  //if (size>driver.tree_result_size_bytes())
    //return;
  //if (results) {
    //int test[]={1,1,1,1,
                //0,0,0,0,
                //1,1,1,1,
                //1,1,1,1,
                //1,1,1,1
    //};
    //bool good = true;
    //for (vcl_size_t i= 0; i<size; i++)
      //good = good && results[i]==test[i];
    //TEST("test_cell_contains_exit_pt_data", good, true);
    //if (true||!good)
      //for (vcl_size_t i= 0; i<size; i+=4)
        //vcl_cout << "test_loc_code_result(" << results[i] << ' '
                 //<< results[i+1] << ' '
                 //<< results[i+2] << ' '
                 //<< results[i+3] << ")\n";
  //}
  //driver.release_kernel();
//}

//void tree_tests(octree_test_driver<T>& test_driver)
//{
  //boxm_ray_trace_manager<T>* ray_mgr = boxm_ray_trace_manager<T>::instance();
  //vcl_string root_dir = testlib_root_dir();
  //test_driver.set_buffers();
  //if (!ray_mgr->load_kernel_source(root_dir + "/contrib/brl/bseg/boxm/ocl/octree_library_functions.cl"))
    //return;
  //if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/tests/octree_test_kernels.cl"))
    //return;
  //if (test_driver.build_program()!=SDK_SUCCESS)
    //return;

  ////START TESTS
  ////================================================================
  //test_loc_code(test_driver);
  //test_traverse(test_driver);
  //test_traverse_to_level(test_driver);
  //test_traverse_force(test_driver);
  //test_traverse_force_local(test_driver);
  //test_cell_bounding_box(test_driver);
  //test_intersect_cell(test_driver);
  //test_common_ancestor(test_driver);
  //test_cell_exit_face(test_driver);
  //test_neighbor(test_driver);
  //test_cell_contains_exit_pt(test_driver);
  //test_ray_trace(test_driver);
  ////==============================================================
  ////END OCTREE TESTS
  //test_driver.cleanup_tree_test();
//}


static void test_loc_code(bit_tree_test_manager* test_mgr)
{

  if(!test_mgr->run_test_loc_code()) {
    TEST("Run Kernel test_loc_code", false, true);
    return;
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
      vcl_cout<<"Time spent test_loc_code"<<test_mgr->gpu_time_/(10000.0)<<" ms"<<vcl_endl;
    }
  }
}

static void test_bit_tree()
{
  vcl_cout<<"Testing Bit Tree"<<vcl_endl;
  vcl_string root_dir = testlib_root_dir();
  
  ////prepare an octree for testing
  //boct_bit_tree test_tree = boct_bit_tree(open_cl_test_data::tree<float>()); 
  //vcl_cout<<"Bit Tree: "<<vcl_endl
          //<<test_tree<<vcl_endl;
  
  ////load up the manager for testing
  //bit_tree_test_manager* test_mgr = bit_tree_test_manager::instance();
  //test_mgr->set_tree(test_tree.get_bits());
  //test_mgr->init_manager();

  ////Run first test
  //test_loc_code(test_mgr);

}

TESTMAIN(test_bit_tree);
