#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "ray_bundle_test_driver.h"
#include "open_cl_test_data.h"
#include <boxm/ocl/boxm_ray_trace_manager.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vul/vul_timer.h>
#include <vbl/vbl_array_2d.h>


template <class T>
static void test_load_data_using_loc_codes(ray_bundle_test_driver<T>& driver)
{
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.set_work_space_ni(8);
  driver.set_work_space_nj(8);
  if (driver.create_kernel("test_load_data_using_loc_codes")!=SDK_SUCCESS) {
    TEST("Create Kernel test_load_data_using_loc_codes", false, true);
    return;
  }
  if (driver.set_basic_test_args()!=SDK_SUCCESS)
    return;
  vul_timer t;
  t.mark();
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_load_data_using_loc_codes", false, true);
    return;
  }
  bool good = true;
  cl_int* results = driver.tree_results();
#if 0
  int count = results[0];
  vcl_cout<<t.all()<<' '
          << "Transfer rate = " << 64.0f*((float)count*count/(static_cast<float>(t.real())))/1000.0f << " Mbytes/second\n";
#endif
  vcl_size_t size = 10*4;
  if (size<driver.tree_result_size_bytes() && results) {
    int test[]={1,1,1,1,
                0,0,0,0,
                1,1,1,1,
                2,2,2,2,
                3,3,3,3,
                1,1,1,1,
                0,0,0,0,
                0,0,0,0,
                0,0,0,0,
                0,0,0,0};
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_load_data_into_cache", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_load_data_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.release_kernel();
}

template <class T>
static void test_load_data_mutable_using_loc_codes(ray_bundle_test_driver<T>& driver)
{
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.set_work_space_ni(8);
  driver.set_work_space_nj(8);
  if (driver.create_kernel("test_load_data_mutable_using_loc_codes")!=SDK_SUCCESS) {
    TEST("Create Kernel test_load_data_mutable_using_loc_codes", false, true);
    return;
  }
  if (driver.set_basic_test_args("use_char4")!=SDK_SUCCESS)
    return;
  vul_timer t;
  t.mark();
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_load_data_mutable_using_loc_codes", false, true);
    return;
  }
  bool good = true;
  cl_int* results = driver.tree_results();

  vcl_size_t size = 10*4;
  if (size<driver.tree_result_size_bytes() && results) {
    int test[]={1,1,1,1,
                0,0,0,1,
                1,0,1,1,
                2,0,2,1,
                3,0,3,1,
                1,1,1,1,
                0,1,3,3,
                0,2,0,3,
                0,3,0,3,
                0,0,0,1};
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_load_data_mutable_using_loc_codes_into_cache", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_load_data_mutable_using_loc_codes_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.release_kernel();
}

template <class T>
static void test_seg_len_obs(ray_bundle_test_driver<T>& driver)
{
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.set_work_space_ni(8);
  driver.set_work_space_nj(8);
  if (driver.create_kernel("test_seg_len_obs")!=SDK_SUCCESS) {
    TEST("Create Kernel test_seg_len_obs", false, true);
    return;
  }
  // the string argument below means use uchar4 instead of char
  // for the ray_bundle_array and add an additional argument
  // which is an image bundle array
  if (driver.set_basic_test_args("include_image_array")!=SDK_SUCCESS)
    return;
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run kernel test_seg_len_obs", false, true);
    return;
  }
  bool good = true;
  cl_int* results = driver.tree_results();

  vcl_size_t size = 10*4;
  if (size<driver.tree_result_size_bytes() && results) {
    int test[]={1,1,1,1,
                1,10,0,0,
                2,40,0,0,
                3,90,0,0,
                4,160,0,0,
                1,1,1,1,
                11,310,0,0,
                2,0,0,0,
                3,0,0,0,
                4,0,0,0};
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_seg_len_obs", good, true);

    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_seg_len_obs_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.release_kernel();
}

template <class T>
static void test_pre_infinity(ray_bundle_test_driver<T>& driver)
{
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.set_work_space_ni(8);
  driver.set_work_space_nj(8);
  if (driver.create_kernel("test_pre_infinity")!=SDK_SUCCESS) {
    TEST("Create Kernel test_pre_infinity", false, true);
    return;
  }
  // the string argument below means use uchar4 instead of char
  // for the ray_bundle_array and add an additional argument
  // which is an image bundle array
  if (driver.set_basic_test_args("include_image_array")!=SDK_SUCCESS)
    return;
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run kernel test_pre_infinity", false, true);
    return;
  }
  bool good = true;
  cl_int* results = driver.tree_results();

  vcl_size_t size = 18*4;
  if (size<driver.tree_result_size_bytes() && results) {
    int test[]={1,1,1,1,
                1000,100,0,0,
                1000,200,0,0,
                1000,500,0,0,
                1000,700,0,0,
                100,693,500,3989,
                200,1386,250,5984,
                500,223,799,1595,
                700,2302,99,7180,
                1,1,1,1,
                2500,1275,0,0,
                500,0,0,0,
                750,0,0,0,
                1000,0,0,0,
                500,173,840,467,
                600,346,707,859,
                400,519,594,1190,
                550,693,500,1468};
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_pre_infinity", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_pre_infinity_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.release_kernel();
}

template <class T>
static void test_bayes_ratio(ray_bundle_test_driver<T>& driver)
{
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.set_work_space_ni(8);
  driver.set_work_space_nj(8);
  if (driver.create_kernel("test_bayes_ratio")!=SDK_SUCCESS) {
    TEST("Create Kernel test_bayes_ratio", false, true);
    return;
  }
  // the string argument below means use uchar4 instead of char
  // for the ray_bundle_array and add an additional argument
  // which is an image bundle array
  if (driver.set_basic_test_args("include_image_array")!=SDK_SUCCESS)
    return;
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run kernel test_bayes_ratio", false, true);
    return;
  }
  bool good = true;
  cl_int* results = driver.tree_results();

  vcl_size_t size = 18*4;
  if (size<driver.tree_result_size_bytes() && results) {

    int test[]={1,1,1,1,
                1000,100,7978,500,
                1000,200,7978,250,
                1000,500,7978,799,
                1000,700,7978,99,
                1000,693,500,3989,
                1000,1386,250,5984,
                1000,223,799,1595,
                1000,2302,99,7180,
                1,1,1,1,
                2500,1275,24466,2000,
                500,0,0,0,
                750,0,0,0,
                1000,0,0,0,
                250,693,500,1468,
                500,693,500,1468,
                750,693,500,1468,
                1000,693,500,1468};
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_bayes_ratio", good, true);

    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_bayes_ratio_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.release_kernel();
}

template <class T>
static void test_map_work_space(ray_bundle_test_driver<T>& driver)
{
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.set_work_space_ni(8);
  driver.set_work_space_nj(8);
  if (driver.create_kernel("test_map_work_space")!=SDK_SUCCESS) {
    TEST("Create Kernel test_map_work_space", false, true);
    return;
  }
  if (driver.set_basic_test_args()!=SDK_SUCCESS)
    return;
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_map_work_space", false, true);
    return;
  }
  bool good = true;
  cl_int* results = driver.tree_results();

  vcl_size_t size = 64*4;
  if (size<driver.tree_result_size_bytes() && results) {
    //int test[]={64,0,0,0};
    //for (vcl_size_t i= 0; i<4; i++)
    //  good = good && results[i]==test[i];
    vbl_array_2d<int> mi(8,8), mj(8,8);

    mi[0][0]=0; mi[1][0]=1; mi[2][0]=4; mi[3][0]=5;mi[4][0]=0; mi[5][0]=1; mi[6][0]=4; mi[7][0]=5;
    mi[0][1]=0; mi[1][1]=1; mi[2][1]=4; mi[3][1]=5;mi[4][1]=0; mi[5][1]=1; mi[6][1]=4; mi[7][1]=5;
    mi[0][2]=2; mi[1][2]=3; mi[2][2]=6; mi[3][2]=7;mi[4][2]=2; mi[5][2]=3; mi[6][2]=6; mi[7][2]=7;
    mi[0][3]=2; mi[1][3]=3; mi[2][3]=6; mi[3][3]=7;mi[4][3]=2; mi[5][3]=3; mi[6][3]=6; mi[7][3]=7;
    mi[0][4]=0; mi[1][4]=1; mi[2][4]=4; mi[3][4]=5;mi[4][4]=0; mi[5][4]=1; mi[6][4]=4; mi[7][4]=5;
    mi[0][5]=0; mi[1][5]=1; mi[2][5]=4; mi[3][5]=5;mi[4][5]=0; mi[5][5]=1; mi[6][5]=4; mi[7][5]=5;
    mi[0][6]=2; mi[1][6]=3; mi[2][6]=6; mi[3][6]=7;mi[4][6]=2; mi[5][6]=3; mi[6][6]=6; mi[7][6]=7;
    mi[0][7]=2; mi[1][7]=3; mi[2][7]=6; mi[3][7]=7;mi[4][7]=2; mi[5][7]=3; mi[6][7]=6; mi[7][7]=7;


    mj[0][0]=0; mj[1][0]=0; mj[2][0]=0; mj[3][0]=0;mj[4][0]=4; mj[5][0]=4; mj[6][0]=4; mj[7][0]=4;
    mj[0][1]=1; mj[1][1]=1; mj[2][1]=1; mj[3][1]=1;mj[4][1]=5; mj[5][1]=5; mj[6][1]=5; mj[7][1]=5;
    mj[0][2]=0; mj[1][2]=0; mj[2][2]=0; mj[3][2]=0;mj[4][2]=4; mj[5][2]=4; mj[6][2]=4; mj[7][2]=4;
    mj[0][3]=1; mj[1][3]=1; mj[2][3]=1; mj[3][3]=1;mj[4][3]=5; mj[5][3]=5; mj[6][3]=5; mj[7][3]=5;
    mj[0][4]=2; mj[1][4]=2; mj[2][4]=2; mj[3][4]=2;mj[4][4]=6; mj[5][4]=6; mj[6][4]=6; mj[7][4]=6;
    mj[0][5]=3; mj[1][5]=3; mj[2][5]=3; mj[3][5]=3;mj[4][5]=7; mj[5][5]=7; mj[6][5]=7; mj[7][5]=7;
    mj[0][6]=2; mj[1][6]=2; mj[2][6]=2; mj[3][6]=2;mj[4][6]=6; mj[5][6]=6; mj[6][6]=6; mj[7][6]=6;
    mj[0][7]=3; mj[1][7]=3; mj[2][7]=3; mj[3][7]=3;mj[4][7]=7; mj[5][7]=7; mj[6][7]=7; mj[7][7]=7;

    for (vcl_size_t i= 4; i<size; i+=4) {
      int gidi = results[i], gidj = results[i+1];
      int mpi = results[i+2], mpj = results[i+3];
      //int grpi = gidi/2, grpj = gidj/2;
      //int mgrpi = mpi/2, mgrpj = mpj/2;
      good = good && mpi == mi[gidi][gidj];
      good = good && mpj == mj[gidi][gidj];
    }
    TEST("test_map_work_space", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_map_work_space_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.release_kernel();
}

template <class T>
static void test_ray_entry_point(ray_bundle_test_driver<T>& driver)
{
  // set up camera
  vnl_matrix_fixed<double, 3, 3> mk(0.0);
  mk[0][0]=990.0; mk[0][2]=4.0;
  mk[1][1]=990.0; mk[1][2]=4.0; mk[2][2]=8.0/7.0;
  vpgl_calibration_matrix<double> K(mk);
  vnl_matrix_fixed<double, 3, 3> mr(0.0);
  mr[0][0]=1.0; mr[1][1]=-1.0; mr[2][2]=-1.0;
  vgl_rotation_3d<double> R(mr);
  vgl_vector_3d<double> t(-0.5,0.5,100);
  vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(K,R,t);
  boxm_ray_trace_manager<float >::instance()->set_camera(cam);
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.set_work_space_ni(8);
  driver.set_work_space_nj(8);
  boxm_ray_trace_manager<float >::instance()->setup_roi_dims(0,7,0,7);
  if (driver.create_kernel("test_ray_entry_point")!=SDK_SUCCESS) {
    TEST("Create Kernel test_ray_entry_point", false, true);
    return;
  }
  if (!driver.setup_image_cam_data())
    return;
  if (driver.set_image_cam_args()!=SDK_SUCCESS)
    return;
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_ray_entry_point", false, true);
    return;
  }
  bool good = true;
  cl_int* results = driver.tree_results();

  vcl_size_t size = 64*4;
  if (size<driver.tree_result_size_bytes() && results) {
    int test[]={99,900,999,0,214,900,999,1000,
                328,900,999,0,442,900,999,1000,
                557,900,1000,0,671,900,999,1000,
                785,900,1000,0,900,900,999,1000,
                99,785,999,2000,214,785,1000,3000,
                328,785,999,2000,442,785,1000,3000,
                557,785,1000,2000,671,785,999,3000,
                785,785,999,2000,900,785,1000,3000,
                100,671,1000,0,214,671,1000,1000,
                328,671,1000,0,442,671,1000,1000,
                557,671,999,0,671,671,1000,1000,
                785,671,999,0,900,671,999,1000,
                99,557,1000,2000,214,557,999,3000,
                328,557,1000,2000,442,557,999,3000,
                557,557,1000,2000,671,557,999,3000,
                785,557,1000,2000,900,557,1000,3000,
                99,442,999,0,214,442,999,1000,
                328,442,1000,0,442,442,999,1000,
                557,442,999,0,671,442,1000,1000,
                785,442,1000,0,900,442,999,1000,
                99,328,1000,2000,214,328,999,3000,
                328,328,1000,2000,442,328,1000,3000,
                557,328,1000,2000,671,328,1000,3000,
                785,328,999,2000,900,328,999,3000,
                99,214,999,0,214,214,1000,1000,
                328,214,999,0,442,214,999,1000,
                557,214,999,0,671,214,1000,1000,
                785,214,1000,0,899,214,999,1000,
                99,99,999,2000,214,99,999,3000,
                328,100,1000,2000,442,99,999,3000,
                557,99,1000,2000,671,99,1000,3000,
                785,99,999,2000,900,99,999,3000};
    for (vcl_size_t i= 0; i<size; i++)
      good = good && vcl_fabs((float)results[i]-(float)test[i])<=1.0f;
    TEST("test_entry_points", good, true);

    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_ray_entry_point_result("
                 << results[i]   << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.clean_image_cam_data();
  driver.release_kernel();
}

template <class T>
static void test_norm_uniform(ray_bundle_test_driver<T>& driver)
{
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.setup_norm_data("4x4_uniform");/*uniform distribution*/
  if (driver.create_kernel("proc_norm_image")!=SDK_SUCCESS) {
    TEST("Create Kernel proc_norm_image", false, true);
    return;
  }
  // the string argument below means use uchar4 instead of char
  // for the ray_bundle_array and add an additional argument
  // which is an image bundle array
  if (driver.set_norm_args()!=SDK_SUCCESS)
    return;
  if (driver.run_norm_kernel()!=SDK_SUCCESS) {
    TEST("Run kernel proc_norm_image", false, true);
    return;
  }
  bool good = true;
  cl_float* image = driver.work_image();
  vcl_size_t size = 16*4;
  if (size<driver.tree_result_size_bytes())
    if (image) {
      int test[]={1,0,1,0,
                  2,0,1,0,
                  3,0,1,0,
                  4,0,1,0,
                  2,0,1,0,
                  3,0,1,0,
                  4,0,1,0,
                  5,0,1,0,
                  3,0,1,0,
                  4,0,1,0,
                  5,0,1,0,
                  6,0,1,0,
                  4,0,1,0,
                  5,0,1,0,
                  6,0,1,0,
                  7,0,1,0};
      for (vcl_size_t i= 0; i<size; i++)
        good = good && image[i]==test[i];
      TEST("test_4x4_uniform", good, true);
      if (!good)
        for (vcl_size_t i= 0; i<size; i+=4)
          vcl_cout << "test_4x4_uniform(" << image[i] << ' '
                   << image[i+1] << ' '
                   << image[i+2] << ' '
                   << image[i+3] << ")\n";
    }
  driver.clean_norm_data();
  driver.release_kernel();
}

static bool near_eq(float x, float y, float tol)
{ return vcl_fabs(x - y) <= tol;}

template <class T>
static void test_norm_gauss(ray_bundle_test_driver<T>& driver)
{
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  /*gaussian distribution*/
  driver.setup_norm_data("4x4_gauss",false, 0.0f, 0.1f);
  if (driver.create_kernel("proc_norm_image")!=SDK_SUCCESS) {
    TEST("Create Kernel proc_norm_image", false, true);
    return;
  }
  // the string argument below means use uchar4 instead of char
  // for the ray_bundle_array and add an additional argument
  // which is an image bundle array
  if (driver.set_norm_args()!=SDK_SUCCESS)
    return;
  if (driver.run_norm_kernel()!=SDK_SUCCESS) {
    TEST("Run kernel proc_norm_image", false, true);
    return;
  }
  bool good = true;
  cl_float* image = driver.work_image();
  vcl_size_t size = 16*4;
  if (size<driver.tree_result_size_bytes())
    if (image) {
      float test[]={3.98942f,0.0f,1.0f,0.0f,
                    1.99472f,0.0f,1.0f,0.0f,
                    2.01542f,0.0f,1.0f,0.0f,
                    3.00001f,0.0f,1.0f,0.0f,
                    1.99472f,0.0f,1.0f,0.0f,
                    2.01542f,0.0f,1.0f,0.0f,
                    3.00001f,0.0f,1.0f,0.0f,
                    4.0f,0.0f,1.0f,0.0f,
                    2.01542f,0.0f,1.0f,0.0f,
                    3.00001f,0.0f,1.0f,0.0f,
                    4.0f,0.0f,1.0f,0.0f,
                    5.0f,0.0f,1.0f,0.0f,
                    3.00001f,0.0f,1.0f,0.0f,
                    4.0f,0.0f,1.0f,0.0f,
                    5.0f,0.0f,1.0f,0.0f,
                    6.0f,0.0f,1.0f,0.0f};
      for (vcl_size_t i= 0; i<size; i++)
        good = good && near_eq(image[i], test[i], 0.001f);
      TEST("test_4x4_gauss", good, true);
      if (!good)
        for (vcl_size_t i= 0; i<size; i+=4)
          vcl_cout << "test_4x4_gauss(" << image[i] << ' '
                   << image[i+1] << ' '
                   << image[i+2] << ' '
                   << image[i+3] << ")\n";
    }
  driver.clean_norm_data();
  driver.release_kernel();
}

template <class T>
void ray_bundle_tests(ray_bundle_test_driver<T>& test_driver)
{
  boxm_ray_trace_manager<T>* ray_mgr = boxm_ray_trace_manager<T>::instance();
  vcl_string root_dir = testlib_root_dir();
  if (!ray_mgr->load_kernel_source(root_dir + "/contrib/brl/bseg/boxm/ocl/octree_library_functions.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/statistics_library_functions.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/backproject.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/ray_bundle_library_functions.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/tests/ray_bundle_test_kernels.cl"))
    return;
  if (test_driver.build_program()!=SDK_SUCCESS)
    return;

  //START TESTS
  //================================================================

  //test_load_data(test_driver);
  //  test_load_data_mutable(test_driver);
  test_load_data_using_loc_codes(test_driver);
  test_load_data_mutable_using_loc_codes(test_driver);
  test_map_work_space(test_driver);
  test_ray_entry_point(test_driver);

  test_seg_len_obs(test_driver);
  test_pre_infinity(test_driver);
  test_bayes_ratio(test_driver);
  test_norm_uniform(test_driver);
  test_norm_gauss(test_driver);
  //==============================================================
  //END RAY BUNDLE TESTS
  test_driver.cleanup_bundle_test();
}

static void test_ray_bundle()
{
  bool good = true;
  vcl_string root_dir = testlib_root_dir();
  ray_bundle_test_driver<float > test_driver;
  boxm_ray_trace_manager<float >* ray_mgr = boxm_ray_trace_manager<float >::instance();
  ray_mgr->set_tree(open_cl_test_data::tree<float>());
  ray_mgr->setup_tree();
  ray_mgr->setup_tree_results();
  ray_mgr->setup_tree_input_buffers();
  good = test_driver.init();
  if (good)ray_bundle_tests(test_driver);
  else { TEST("ray_bundle_test_driver", true, false); }
  ray_mgr->clean_tree_input_buffers();
}

TESTMAIN(test_ray_bundle);
