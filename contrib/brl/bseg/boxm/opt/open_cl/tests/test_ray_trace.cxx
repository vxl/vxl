#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "octree_test_driver.h"
#include "open_cl_test_data.h"
#include <boxm/opt/open_cl/boxm_ray_trace_manager.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vnl/vnl_vector_fixed.h>
#include <vul/vul_timer.h>
static void test_ray_trace(octree_test_driver& driver, unsigned repeats = 1)
{
  if (driver.create_kernel("test_ray_trace")!=SDK_SUCCESS) {
    TEST("Create Kernel test_ray_trace", false, true);
    return;
  }
  vul_timer t;
  for (unsigned i = 0; i<repeats; ++i)
    if (driver.run_ray_trace_test_kernels()!=SDK_SUCCESS) {
      TEST("Run Kernel test_ray_trace", false, true);
      return;
    }
  double time = t.real();
  double nrays = driver.n_rays();
  vcl_cout << "Average ray trace of octree: " << time/(repeats*nrays) << " msecs/ray\n";
}

static void ray_trace_tests(octree_test_driver& test_driver)
{
  vcl_string root_dir = testlib_root_dir();
  boxm_ray_trace_manager* ray_mgr = boxm_ray_trace_manager::instance();
#if 0
  vbl_array_2d<vnl_vector_fixed<float, 3> > ray_origin;
  vbl_array_2d<vnl_vector_fixed<float, 3> > ray_dir;
  open_cl_test_data::test_rays(ray_origin, ray_dir);
  ray_mgr->set_rays(ray_origin, ray_dir);
#endif
  ray_mgr->setup_tree();
  ray_mgr->setup_rays();
  //  ray_mgr->print_tree_input();
  //ray_mgr->print_ray_input();
  ray_mgr->setup_ray_processing();
  test_driver.set_buffers();
  if (!ray_mgr->load_kernel_source(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/octree_library_functions.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/ray_trace_test_kernels.cl"))
    return;
  //ray_mgr->write_program(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/junk.cl");
  if (test_driver.build_program()!=SDK_SUCCESS)
    return;
  //START TESTS
  //================================================================
  test_ray_trace(test_driver, 100);
}

static  void create_test_tree(vcl_string const& image_path,
                              vcl_string const& tree_path,
                              vcl_string const& ray_path)
{
  boxm_ray_trace_manager* ray_mgr = boxm_ray_trace_manager::instance();
  boct_tree<short, vnl_vector_fixed<float, 2> >* tree =0;
  vbl_array_2d<vnl_vector_fixed<float, 3> > ray_origin;
  vbl_array_2d<vnl_vector_fixed<float, 3> > ray_dir;
  vcl_size_t gsize = ray_mgr->group_size();
  open_cl_test_data::tree_and_rays_from_image(image_path, gsize, tree,
                                              ray_origin, ray_dir);
  ray_mgr->set_tree(tree);
  ray_mgr->write_tree(tree_path);
  ray_mgr->set_rays(ray_origin, ray_dir);
  ray_mgr->write_rays(ray_path);
}

MAIN( test_ray_trace )
{
  vcl_string root_dir = testlib_root_dir();
  boxm_ray_trace_manager* ray_mgr = boxm_ray_trace_manager::instance();
    create_test_tree(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/dalmation.tif", root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/dalmation_tree.vsl", root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/dalmation_rays.vsl");

#if 0
  create_test_tree(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/lena-std-gray.tif", root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/lena_tree.vsl",root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/lena_rays.vsl");

    create_test_tree(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/lena-std-gray-small.tif",
                     root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/lena_small_tree.vsl");

    create_test_tree(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/dalmation.tif",
                     root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/dalmation_tree.vsl");

  ray_mgr->load_tree(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/dalmation_tree.vsl");
#endif
#if 0
  ray_mgr->load_tree(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/lena_tree.vsl");
  ray_mgr->load_rays(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/lena_rays.vsl");
#endif
  octree_test_driver test_driver;
  if (!test_driver.init())
    return 0;
  ray_trace_tests(test_driver);
  test_driver.print_kernel_usage_info();
   open_cl_test_data::save_expected_image(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/expected_dalmation.tiff", ray_mgr->ray_cols(), ray_mgr->ray_rows(), ray_mgr->ray_results());
#if 0
   open_cl_test_data::save_expected_image(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/expected_lena.tiff", ray_mgr->ray_cols(), ray_mgr->ray_rows(), ray_mgr->ray_results());

   open_cl_test_data::save_expected_image(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/expected_lena_small.tiff", ray_mgr->ray_cols(), ray_mgr->ray_rows(), ray_mgr->ray_results());

   open_cl_test_data::save_expected_image(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/expected_dalmation.tiff", ray_mgr->ray_cols(), ray_mgr->ray_rows(), ray_mgr->ray_results());

#endif
  SUMMARY();
}
