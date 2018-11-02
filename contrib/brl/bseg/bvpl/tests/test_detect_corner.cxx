#include <sstream>
#include <iostream>
#include <iomanip>
#include <testlib/testlib_test.h>

#include <bvpl/kernels/bvpl_corner2d_kernel_factory.h>
#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>
#include <bvpl/functors/bvpl_edge_algebraic_mean_functor.h>
#include <bvpl/functors/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/functors/bvpl_gauss_convolution_functor.h>
#include <bvpl/bvpl_neighb_operator.h>
#include <bvpl/bvpl_vector_operator.h>
#include <bvpl/kernels/bvpl_create_directions.h>

#include <bvxm/grid/bvxm_opinion.h>
#include <bvxm/grid/bvxm_voxel_grid_basic_ops.h>
#include <bvxm/grid/bvxm_voxel_grid_opinion_basic_ops.h>
#include <bvxm/grid/io/bvxm_io_voxel_grid.h>

#include <vnl/vnl_float_3.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

typedef bsta_num_obs<bsta_gauss_sf1> gauss_sf1;

void create_window(bvxm_voxel_grid<gauss_sf1> *grid)
{
  //fill in window grid
  vgl_vector_3d<unsigned> grid_dim = grid->grid_size();
  grid->initialize_data(bsta_gauss_sf1(0.99f, 1.0f));
  unsigned ni=grid_dim.x();
  unsigned nj=grid_dim.y();
  unsigned nk=grid_dim.z();

  unsigned slab_idx = 0;
  bvxm_voxel_grid<gauss_sf1>::iterator grid_it = grid->slab_iterator(slab_idx,nk);

  for (unsigned x= 0; x<ni; x++)
    for (unsigned z = nk/3 + 1; z < 2*nk/3 + 1; z++)
      for (unsigned y = nj/3 + 1; y < 2*nj/3 + 1; y++)
        (*grid_it)(x,y,z)=bsta_gauss_sf1(0.01f, 1.0f);
}

bool test_result(bvxm_voxel_grid<gauss_sf1> *grid, unsigned x, unsigned y, unsigned z)
{
  // iterate through the grid and get the max

  bvxm_voxel_grid<gauss_sf1>::iterator grid_it = grid->begin();
  float max =  std::abs(((*grid_it)(0,0)).mean());
  unsigned max_x =0;
  unsigned max_y =0;
  unsigned max_z =0;
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny() -3 ; ++j) {
        if (std::abs(((*grid_it)(i,j)).mean())> max){
          max = std::abs(((*grid_it)(i,j)).mean());
          max_x = i; max_y = j; max_z = k;
        }
      }
    }
  }
  std::cout << "Location of max = " << max_x << max_y << max_z << std::endl;
  if ((x!=max_x)||(y!=max_y) || (z!=max_z))
    return false;

  bvxm_voxel_grid<gauss_sf1>::iterator grid_it2 = grid->slab_iterator(0,grid->grid_size().z());
  std::cout << "Max response= " << std::abs(((*grid_it2)(max_x,max_y, max_z)).mean()) << std::endl;
  return true;
}

bool test_id_grid(bvxm_voxel_grid<int> *grid,unsigned x, unsigned y, unsigned z, int id)
{
  vgl_vector_3d<unsigned> grid_dim = grid->grid_size();
  unsigned nk=grid_dim.z();
  unsigned slab_idx = 0;
  bvxm_voxel_grid<int>::iterator grid_it = grid->slab_iterator(slab_idx,nk);

  return (*grid_it)(x,y,z)==id;
}

bool test_non_max_grid(bvxm_voxel_grid<gauss_sf1> *grid)
{
  std::cout << std::endl;
  bvxm_voxel_grid<gauss_sf1>::iterator grid_it = grid->begin();
  unsigned count = 0;
  for (unsigned k=0; k <grid->grid_size().z()-2; ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny() -2; ++j) {
        if (std::abs(((*grid_it)(i,j)).mean())> 1.0e-7){
          std::cout << "Response at " << i << ',' << j << ',' << k << " is " <<std::abs(((*grid_it)(i,j)).mean())
                   << std::endl;
          ++count;
        }
      }
    }
  }
  return count==4;
}


static void test_detect_corner()
{
  //Create vector of kernels
  unsigned length = 2;
  unsigned width = 2;
  unsigned thickness = 2;

  bvpl_kernel_vector_sptr kernel_vector= new bvpl_kernel_vector();
  bvpl_corner2d_kernel_factory factory(length, width, thickness);
  factory.set_rotation_axis( vnl_float_3(1.0f, 0.0f, 0.0f));
  factory.set_angle(0.0f);
  bvpl_kernel_sptr kernel = new bvpl_kernel(factory.create());
  kernel_vector->kernels_.push_back(kernel);

  //Create a synthetic world, with a window
  bvxm_voxel_grid<gauss_sf1> *grid = new bvxm_voxel_grid<gauss_sf1> (vgl_vector_3d<unsigned>(5,12,12));
  create_window(grid);

  bvxm_voxel_grid<gauss_sf1> *grid_out = new bvxm_voxel_grid<gauss_sf1> (vgl_vector_3d<unsigned>(5,12,12));
  grid_out->initialize_data(bsta_gauss_sf1(0.0f,1.0f));

  //Run apply kernel to world
  bvpl_gauss_convolution_functor func(kernel->iterator());
  bvpl_neighb_operator<gauss_sf1, bvpl_gauss_convolution_functor> oper(func);
  oper.operate(grid, kernel, grid_out);

  //run several kernels and determine which window is being found
  TEST("Position Corner1", true, test_result(grid_out,2,4,9));

  factory.set_angle(float(vnl_math::pi_over_2));
  kernel = new bvpl_kernel(factory.create());
  kernel_vector->kernels_.push_back(kernel);
  oper.operate(grid, kernel, grid_out);
  TEST("Position Corner2", true, test_result(grid_out,2,8,9));

  factory.set_angle(float(vnl_math::pi));
  kernel = new bvpl_kernel(factory.create());
  kernel_vector->kernels_.push_back(kernel);
  oper.operate(grid, kernel, grid_out);
  TEST("Position Corner3", true, test_result(grid_out,2,8,4));

  factory.set_angle(float(3.0*vnl_math::pi_over_2));
  kernel = new bvpl_kernel(factory.create());
  kernel_vector->kernels_.push_back(kernel);
  oper.operate(grid, kernel, grid_out);
  TEST("Position Corner 4", true, test_result(grid_out,2,4,4));

  //create a functor
  bvpl_gauss_convolution_functor gauss_func(kernel->iterator());
  bvxm_voxel_grid<int > *id_grid=new bvxm_voxel_grid<int >(grid->grid_size());
  bvpl_vector_operator vector_oper;
  vector_oper.apply_and_suppress(grid,kernel_vector,&oper,grid_out,id_grid);
  std::cout << std::endl;
  TEST("Id at Corner 1", true, test_id_grid(id_grid,2,4,9,0));
  TEST("Id at Corner 2", true, test_id_grid(id_grid,2,9,9,1));
  TEST("Id at Corner 3", true, test_id_grid(id_grid,2,9,4,2));
  TEST("Id at Corner 4", true, test_id_grid(id_grid,2,4,4,3));

  //test non-maxima suppression
  //bvxm_voxel_grid<gauss_sf1> *non_max_grid= new bvxm_voxel_grid<gauss_sf1>(grid->grid_size());
  //bvxm_voxel_grid_copy<gauss_sf1> (grid_out, non_max_grid);
  vector_oper.non_maxima_suppression(grid_out,id_grid,kernel_vector);
  std::cout << std::endl;
  TEST("Number of corners after non_max", true, test_non_max_grid(grid_out));
}

TESTMAIN(test_detect_corner);
