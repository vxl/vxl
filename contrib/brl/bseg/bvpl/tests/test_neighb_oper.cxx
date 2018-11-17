//:
// \file
#include <iostream>
#include <limits>
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_subgrid_iterator.h>
#include <bvpl/bvpl_voxel_subgrid.h>
#include <bvpl/kernels/bvpl_edge2d_kernel_factory.h>
#include <bvpl/functors/bvpl_edge2d_functor.h>
#include <bvpl/bvpl_neighb_operator.h>

#include <vul/vul_file.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool save_occupancy_raw(const std::string& filename, bvxm_voxel_grid<float>* grid)
{
  std::fstream ofs(filename.c_str(),std::ios::binary | std::ios::out);
  if (!ofs.is_open()) {
    std::cerr << "error opening file " << filename << " for write!\n";
    return false;
  }

  //get the range of the grid
  bvxm_voxel_grid<float>::iterator grid_it = grid->begin();
  float max = -1.0f * std::numeric_limits<float>::infinity();
  float min = std::numeric_limits<float>::infinity();
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if ((*grid_it)(i,j)> max)
          max = (*grid_it)(i,j);
        if ((*grid_it)(i,j)< min)
          min = (*grid_it)(i,j);
      }
    }
  }

  std::cout << "max: " << max <<std::endl
           << "min: " << min <<std::endl;

  // write header

  unsigned char data_type = 8; // 0 means unsigned byte, 1 signed byte, 8 float

  vxl_uint_32 nx = grid->grid_size().x();
  vxl_uint_32 ny = grid->grid_size().y();
  vxl_uint_32 nz = grid->grid_size().z();

  ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));

  // write data
  // iterate through slabs and fill in memory array
  auto *ocp_array = new float[nx*ny*nz];

  bvxm_voxel_grid<float>::iterator ocp_it = grid->begin();
  for (unsigned k=0; ocp_it != grid->end(); ++ocp_it, ++k) {
    std::cout << '.';
    for (unsigned i=0; i<(*ocp_it).nx(); ++i) {
      for (unsigned j=0; j < (*ocp_it).ny(); ++j) {

        ocp_array[i*ny*nz + j*nz + k] = (float)((*ocp_it)(i,j) );
      }
    }
  }
  std::cout << std::endl;
  ofs.write(reinterpret_cast<char*>(ocp_array),sizeof(float)*nx*ny*nz);

  ofs.close();

  delete[] ocp_array;

  return true;
}

void vertical_grid(bvxm_voxel_grid<float>* grid, unsigned grid_x, unsigned grid_y)
{
  // fill with test data
  float init_val = 0.9f;
  grid->initialize_data(init_val);

  // read in each slice, check that init_val was set, and fill with new value
  std::cout << "read/write: ";
  bvxm_voxel_slab_iterator<float> slab_it;
  for (slab_it = grid->begin(); slab_it != grid->end(); ++slab_it) {
    std::cout << '.';
    bvxm_voxel_slab<float> vit=*slab_it;
    for (unsigned i=0; i<grid_x/2; i++) {
      for (unsigned j=0; j<grid_y; j++) {
        float &v = vit(i,j);
        v = 0.1f;
      }
    }
  }
}

void horizontal_grid(bvxm_voxel_grid<float>* grid, unsigned grid_x, unsigned grid_y)
{
  // fill with test data
  float init_val = 0.9f;
  grid->initialize_data(init_val);

  // read in each slice, check that init_val was set, and fill with new value
  std::cout << "read/write: ";
  bvxm_voxel_slab_iterator<float> slab_it;
  for (slab_it = grid->begin(); slab_it != grid->end(); ++slab_it) {
    std::cout << '.';
    bvxm_voxel_slab<float> vit=*slab_it;
    for (unsigned i=0; i<grid_y/2; i++) {
      for (unsigned j=0; j<grid_x; j++) {
        float &v = vit(j,i);
        v = 0.1f;
      }
    }
  }
}

void diagonal_grid(bvxm_voxel_grid<float>* grid, unsigned grid_x, unsigned grid_y)
{
  // fill with test data
  float init_val = 0.9f;
  grid->initialize_data(init_val);

  // read in each slice, check that init_val was set, and fill with new value
  std::cout << "read/write: ";
  bvxm_voxel_slab_iterator<float> slab_it;
  for (slab_it = grid->begin(); slab_it != grid->end(); ++slab_it) {
    std::cout << '.';
    unsigned x = grid_x;
    bvxm_voxel_slab<float> vit=*slab_it;
    for (unsigned i=0; i<grid_y; i++) {
      for (unsigned j=0; j<x; j++) {
        float &v = vit(j,i);
        v = 0.1f;
      }
      x--;
    }
  }
}

void diagonal_grid_z(bvxm_voxel_grid<float>* grid, unsigned grid_x, unsigned grid_y)
{
  // fill with test data
  float init_val = 0.9f;
  grid->initialize_data(init_val);

  // read in each slice, check that init_val was set, and fill with new value
  std::cout << "read/write: ";
  bvxm_voxel_slab_iterator<float> slab_it;
  unsigned x = 0;
  for (slab_it = grid->begin(); slab_it != grid->end() ; ++slab_it) {
    std::cout << '.';
    bvxm_voxel_slab<float> vit=*slab_it;
    for (unsigned i=0; i<x && i<grid_x; i++) {
      for (unsigned j=0; j<grid_y; j++) {
        float &v = vit(i,j);
        v = 0.1f;
      }
    }
    x++;
  }
}

//: Test changes
static void test_neighb_oper()
{
  // create the grid
  // we need temporary disk storage for this test.
  std::string storage_fname("bvxm_voxel_grid_test_temp1a.vox");
  std::string storage_fname2("bvxm_voxel_grid_test_temp1b.vox");
  // remove file if exists from previous test.
  if (vul_file::exists(storage_fname.c_str())) {
    vul_file::delete_file_glob(storage_fname.c_str());
  }
  if (vul_file::exists(storage_fname2.c_str())) {
    vul_file::delete_file_glob(storage_fname2.c_str());
  }
#if 0
  if (vul_file::exists(storage_cached_fname.c_str())) {
    vul_file::delete_file_glob(storage_cached_fname.c_str());
  }
#endif

  unsigned int grid_x=50, grid_y=50, grid_z=50;

  vgl_vector_3d<unsigned> grid_size(grid_x, grid_y, grid_z);
  //unsigned max_cache_size = grid_size.x()*grid_size.y()*18;

  // try test with all types of underlying storage.
  bvxm_voxel_grid<float>* grid = new bvxm_voxel_grid<float>(storage_fname,grid_size); // disk storage;
  bvxm_voxel_grid<float>* grid_out = new bvxm_voxel_grid<float>(storage_fname2,grid_size);
  grid_out->initialize_data(0.0e-40);
  std::string test_name;

  // check num_observations
  unsigned nobs = grid->num_observations();
  std::cout << "num_observations = " << nobs << std::endl;

  diagonal_grid_z(grid, grid_x, grid_y);

  std::cout << "done." << std::endl;
  save_occupancy_raw("first.raw", grid);

  bvpl_edge2d_kernel_factory edge_factory(5, 5);

  edge_factory.set_rotation_axis(vnl_float_3(1,0,1));
  edge_factory.set_angle(float(vnl_math::pi)); // was float(3*vnl_math::pi_over_4)

  bvpl_kernel_sptr kernel_sptr = new bvpl_kernel(edge_factory.create());
  kernel_sptr->print();
  bvpl_edge2d_functor<float> func;
  bvpl_neighb_operator<float, bvpl_edge2d_functor<float> > oper(func);
  oper.operate(grid, kernel_sptr, grid_out);
  save_occupancy_raw("out.raw", grid_out);
}

TESTMAIN( test_neighb_oper );
