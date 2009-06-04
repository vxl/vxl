//:
// \file
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_subgrid_iterator.h>
#include <bvpl/bvpl_voxel_subgrid.h>
#include <bvpl/bvpl_edge2d_kernel.h>
#include <bvpl/bvpl_edge2d_functor.h>
#include <bvpl/bvpl_neighb_operator.h>

#include <vul/vul_file.h>
#include <vnl/vnl_math.h>

//: Test changes
static void test_neighb_oper()
{
  // create the grid
  // we need temporary disk storage for this test.
  vcl_string storage_fname("bvxm_voxel_grid_test_temp.vox");
  vcl_string storage_fname2("bvxm_voxel_grid_test_temp2.vox");
  vcl_string storage_cached_fname("bvxm_voxel_grid_cached_test_temp.vox");
  // remove file if exists from previous test.
  if (vul_file::exists(storage_fname.c_str())) {
    vul_file::delete_file_glob(storage_fname.c_str());
  }
  if (vul_file::exists(storage_fname2.c_str())) {
    vul_file::delete_file_glob(storage_fname2.c_str());
  }
  if (vul_file::exists(storage_cached_fname.c_str())) {
    vul_file::delete_file_glob(storage_cached_fname.c_str());
  }

  int grid_x=50, grid_y=50, grid_z=20;
  vgl_vector_3d<unsigned> grid_size(grid_x, grid_y, grid_z);
  unsigned max_cache_size = grid_size.x()*grid_size.y()*18;

  // try test with all types of underlying storage.
  bvxm_voxel_grid<float>* grid = new bvxm_voxel_grid<float>(storage_fname,grid_size); // disk storage;
  bvxm_voxel_grid<float>* grid_out = new bvxm_voxel_grid<float>(storage_fname2,grid_size);
  vcl_string test_name;

  // check num_observations
  unsigned nobs = grid->num_observations();

  // fill with test data
  float init_val = 0.0f;
  grid->initialize_data(init_val);
  grid_out->initialize_data(init_val);
  bool init_check = true;
  bool write_read_check = true;

  //grid->increment_observations();

  // read in each slice, check that init_val was set, and fill with new value
  unsigned count = 0;
  vcl_cout << "read/write: ";
  bvxm_voxel_slab_iterator<float> slab_it;
  for (slab_it = grid->begin(); slab_it != grid->end(); ++slab_it) {
    vcl_cout << '.';
    bvxm_voxel_slab<float> vit=*slab_it;
    for (unsigned i=0; i<grid_x/2; i++) {
      for (unsigned j=0; j<grid_y; j++) {
        for (unsigned k=0; k<grid_z; k++) {
          float &v = vit(i,j,k);
          v = 1.0f;
        }
      }
    }
  }
  vcl_cout << "done." << vcl_endl;

  // get the subgrid iterator
  int kernel_x=11, kernel_y=11, kernel_z=11;
  bvpl_subgrid_iterator<float> iter(*grid, vgl_vector_3d<int> (kernel_x,kernel_y,kernel_z));
  bvpl_subgrid_iterator<float> out_iter(*grid_out, vgl_vector_3d<int> (kernel_x,kernel_y,kernel_z));
  bvpl_edge2d_kernel<SYMMETRIC> kernel; 
  kernel.create(5, 5, vnl_vector_fixed<double,3>(vnl_math::pi, 0.0, 0.0));
  bvpl_edge2d_functor<float> func;
  bvpl_neighb_operator<float, bvpl_edge2d_functor<float> > oper(func);
  while (!iter.isDone()) {
    bvpl_voxel_subgrid<float> subgrid = *iter;
    oper(iter, kernel.iterator(), out_iter);
    ++out_iter;
    ++iter;
  }
  // go through the subgrid
}

TESTMAIN( test_neighb_oper );
