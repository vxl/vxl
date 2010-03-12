#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "octree_test_driver.h"
#include "open_cl_test_data.h"
#include <boxm/opt/open_cl/boxm_ray_trace_manager.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_vector_3d.h>

template <class T>
static void test_create_a_ray(octree_test_driver<T> & driver)
{
  if (driver.create_kernel("test_ray_create")!=SDK_SUCCESS) {
    TEST("Create Kernel test_ray_trace", false, true);
    return;
  }
  if (driver.run_ray_creation_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_ray_trace", false, true);
    return;
  }
}

template <class T>
static void ray_creation_tests(octree_test_driver<T> & test_driver)
{
  vcl_string root_dir = testlib_root_dir();
  boxm_ray_trace_manager<T>* ray_mgr = boxm_ray_trace_manager<T>::instance();
  ray_mgr->setup_camera();
  ray_mgr->setup_ray_origin();
  ray_mgr->setup_ray_results();

  if (ray_mgr->setup_camera_input_buffer()!=SDK_SUCCESS)
    return;

  if (ray_mgr->setup_imgdims_buffer()!=SDK_SUCCESS)
    return;

  if (ray_mgr->setup_ray_origin_buffer()!=SDK_SUCCESS)
    return;

  test_driver.set_buffers();

  if (!ray_mgr->load_kernel_source(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/backproject.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/test_create_ray.cl"))
    return;
  if (test_driver.build_program()!=SDK_SUCCESS)
    return;
  //START TESTS
  //================================================================
  test_create_a_ray(test_driver);
}

#if 0
static void backproject_rays(vcl_string filename)
{
  vpgl_perspective_camera<double> pcam;
  vcl_ifstream ifs(filename.c_str());
  if (!ifs)
    return ;
  else
    ifs >> pcam;

  vgl_line_3d_2_points<double> line=pcam.backproject(vgl_point_2d<double>(1,1));
}
#endif

static void test_backproject_ray()
{
  vcl_string root_dir = testlib_root_dir();
  boxm_ray_trace_manager<float >* ray_mgr = boxm_ray_trace_manager<float >::instance();
  ray_mgr->setup_img_dims(64,64);
  vpgl_perspective_camera<double> *pcam=new vpgl_perspective_camera<double> ();
  vcl_string cam_filename=root_dir+"/contrib/brl/bseg/boxm/opt/open_cl/tests/cam_0.txt";
  vcl_ifstream ifs(cam_filename.c_str());
  if (!ifs)
    return;
  else
    ifs >> (*pcam);

  ray_mgr->set_perspective_camera(pcam);
  octree_test_driver<float > test_driver;
  bool flag=true;
  if (test_driver.init())
  {
    ray_creation_tests(test_driver);
    float * ray_results=test_driver.ray_results();
    unsigned int inc=0;
    for (unsigned i=0;i<64;i++)
    {
      for (unsigned j=0;j<64;j++)
      {
        vgl_line_3d_2_points<double> cam_ray = pcam->backproject(vgl_homg_point_2d<double>((double)i,(double)j));
        vgl_vector_3d<double> norm_direction = cam_ray.direction();
        normalize(norm_direction);
        float dist=(float)vcl_sqrt((ray_results[inc]  -norm_direction.x())*(ray_results[inc]-norm_direction.x())+
                            (ray_results[inc+1]-norm_direction.y())*(ray_results[inc+1]-norm_direction.y())+
                            (ray_results[inc+2]-norm_direction.z())*(ray_results[inc+2]-norm_direction.z()));

        inc+=4;
        if (dist>1e-4)
          flag=false;
      }
    }
    TEST("backproject_ray_test_driver", true, flag);
  }
  else
  {
    TEST("backproject_ray_test_driver", true, false);
  }
}

TESTMAIN(test_backproject_ray);
