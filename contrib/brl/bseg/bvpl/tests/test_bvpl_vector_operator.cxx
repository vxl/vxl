//:
// \file
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_edge2d_kernel_factory.h>
#include <bvpl/bvpl_edge3d_kernel_factory.h>
#include <bvpl/bvpl_edge_algebraic_mean_functor.h>
#include <bvpl/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/bvpl_opinion_functor.h>
#include <bvpl/bvpl_neighb_operator.h>
#include <bvpl/bvpl_vector_operator.h>
#include <bvpl/bvpl_create_directions.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>

#include <bvxm/grid/bvxm_opinion.h>
#include <bvxm/grid/bvxm_voxel_grid_basic_ops.h>
#include <bvxm/grid/bvxm_voxel_grid_opinion_basic_ops.h>
#include <bvxm/grid/io/bvxm_io_voxel_grid.h>

#include <vnl/vnl_vector_fixed.h>
#include <vul/vul_file.h>

void create_grid(vcl_string grid_filename)
{
  if (vul_file_exists(grid_filename))
    vul_file::delete_file_glob(grid_filename);

  bvxm_voxel_grid<bvxm_opinion> surface_grid(grid_filename,vgl_vector_3d<unsigned int>(32,32,32));
  bvxm_opinion bnonsurf(0.9,0.1);
  surface_grid.initialize_data(bnonsurf);

  vcl_vector<vgl_point_3d<double> > poly_points;
  poly_points.push_back(vgl_point_3d<double>(10.0,10.0,10.0));
  poly_points.push_back(vgl_point_3d<double>(10.0,20.0,10.0));
  poly_points.push_back(vgl_point_3d<double>(20.0,20.0,10.0));
  poly_points.push_back(vgl_point_3d<double>(20.0,10.0,10.0));

  bvxm_opinion bsurf(0.1,0.9);
  bvxm_load_polygon_into_grid<bvxm_opinion>(&surface_grid,poly_points,bsurf);
}

void fill_in_data(bvxm_voxel_grid<float> *grid, float min_p, float max_p, vnl_float_3 axis)
{
  vgl_vector_3d<unsigned> grid_dim = grid->grid_size();
  unsigned ni=grid_dim.x();
  unsigned nj=grid_dim.y();
  unsigned nk=grid_dim.z();

  float ci=ni*0.5f;
  float cj=nj*0.5f;
  float ck=nk*0.5f;

  unsigned slab_idx = 0;
  bvxm_voxel_grid<float>::iterator grid_it = grid->slab_iterator(slab_idx,nk);
  for (unsigned i=0;i<ni;i++)
  {
    for (unsigned j=0;j<nj;j++)
    {
      for (unsigned k=0;k<nk;k++)
      {
        if ((i-ci)*axis[0]+(j-cj)*axis[1]+(k-ck)*axis[2]>=0)
          (*grid_it)(i,j,k)=max_p;
        else
          (*grid_it)(i,j,k)=min_p;
      }
    }
  }
}

bool check_data(bvxm_voxel_grid<unsigned> *grid, vnl_float_3 axis, unsigned id, int margin)
{
  vgl_vector_3d<unsigned> grid_dim = grid->grid_size();
  unsigned ni=grid_dim.x();
  unsigned nj=grid_dim.y();
  unsigned nk=grid_dim.z();

  float ci=ni*0.5f;
  float cj=nj*0.5f;
  float ck=nk*0.5f;

  bool result = true;

  unsigned slab_idx = 0;
  bvxm_voxel_grid<unsigned>::iterator grid_it = grid->slab_iterator(slab_idx,nk);
  for (unsigned i=margin;i<ni-margin;i++)
  {
    for (unsigned j=margin;j<nj-margin;j++)
    {
      for (unsigned k=margin;k<nk-margin;k++)
      {
        if (((i-ci)*axis[0]+(j-cj)*axis[1]+(k-ck)*axis[2]>=-1) && ((i-ci)*axis[0]+(j-cj)*axis[1]+(k-ck)*axis[2] <=1))
        {
          result = result && (*grid_it)(i,j,k)==id;
          //vcl_cout << "id at center " << i << j << k << " is " <<(*grid_it)(i,j,k) << vcl_endl;
        }
      }
    }
  }
  return result;
}

bool check_non_max(bvxm_voxel_grid<float> *grid)
{
  vgl_vector_3d<unsigned> grid_dim = grid->grid_size();
  unsigned ni=grid_dim.x();
  unsigned nj=grid_dim.y();
  unsigned nk=grid_dim.z();

  float ci=ni*0.5f ;
  float cj=nj*0.5f ;
  float ck=nk*0.5f ;

  bool result = true;

  unsigned slab_idx = 0;
  bvxm_voxel_grid<float>::iterator grid_it = grid->slab_iterator(slab_idx,nk);
  for (unsigned i=0;i<ni;i++)
  {
    for (unsigned j=0;j<nj;j++)
    {
      for (unsigned k=0;k<nk;k++)
      {
        if ( i == (unsigned int)ci && j == (unsigned int)cj && k == (unsigned int)ck)
        {
          //vcl_cout << "Response at center " << i << j << k << "is " << (*grid_it)(i,j,k) << vcl_endl;
          result = result && ((*grid_it)(i,j,k) > 1e-2);
        }
        else if (!((*grid_it)(i,j,k) < 1e-2) )
        {
          result = false;
          //vcl_cout <<  "Response at " << i << j << k << "is " << (*grid_it)(i,j,k) << vcl_endl;
        }
      }
    }
  }
  return result;
}

void test_vector_operator()
{
  vcl_string grid_filename="grid_plane.vox";
  create_grid(grid_filename);
  bvxm_voxel_grid<bvxm_opinion>* grid = new bvxm_voxel_grid<bvxm_opinion>(grid_filename);
  vcl_string grid_expectation_filename="grid_plane_expectation.vox";
  //: output to verify if the digitization of the plane is correct.
  bvxm_voxel_grid<float> * surface_grid_expectation
  =new bvxm_voxel_grid<float>(grid_expectation_filename,vgl_vector_3d<unsigned int>(32,32,32));

  bvxm_expectation_opinion_voxel_grid(grid,surface_grid_expectation);

  bvxm_grid_save_raw<float>(surface_grid_expectation,"grid_plane_expectation.raw");
  //: get vector of kernel
  bvpl_edge3d_kernel_factory kernels_3d(5,5,5);
  bvpl_create_directions_a dir;
  bvpl_kernel_vector_sptr kernel_vec = kernels_3d.create_kernel_vector(dir);

  vcl_string out_grid_path="out_grid.vox";
  vcl_string id_grid_path="orientation_grid.vox";
  vcl_string out_grid_expectation_path="out_grid_expectation.vox";

  bvxm_voxel_grid<bvxm_opinion> *grid_out=new bvxm_voxel_grid<bvxm_opinion>(out_grid_path, grid->grid_size());
  bvxm_voxel_grid<unsigned > *id_grid
  =new bvxm_voxel_grid<unsigned >(id_grid_path, grid->grid_size());
  bvpl_opinion_functor func;
  bvpl_neighb_operator<bvxm_opinion, bvpl_opinion_functor> oper(func);
  bvpl_vector_operator vector_oper;
  vector_oper.apply_and_suppress(grid,kernel_vec,&oper,grid_out, id_grid);

  bvxm_voxel_grid<float> *out_grid_expectation=new bvxm_voxel_grid<float>(out_grid_expectation_path,grid->grid_size());
  bvxm_expectation_opinion_voxel_grid(grid_out,out_grid_expectation);
  bvxm_grid_save_raw<float>(out_grid_expectation,"grid_out_expectation.raw");
}


bool test_non_max_suppression()
{
  //Create vector of kernels
  bvpl_edge3d_kernel_factory kernels_3d(3,3,3);
  bvpl_create_directions_a dir;
  bvpl_kernel_vector_sptr kernel_vec = kernels_3d.create_kernel_vector(dir);


  //Create an in memory grid and filled it with a plane
  bvxm_voxel_grid<float> *grid = new bvxm_voxel_grid<float> (vgl_vector_3d<unsigned>(5,5,5));
  unsigned target_id = 9;
  vnl_float_3 target_axis = kernel_vec->kernels_[target_id]->axis();
  vcl_cout << "taget axis " << target_axis << vcl_endl;
  fill_in_data(grid, 0.01f, 0.99f, target_axis);

  //Run all the kernels
  bvxm_voxel_grid<float> *grid_out=new bvxm_voxel_grid<float>(grid->grid_size());
  bvxm_voxel_grid<unsigned > *id_grid=new bvxm_voxel_grid<unsigned >(grid->grid_size());
  id_grid->initialize_data(10);
  bvpl_edge_algebraic_mean_functor<float> func;
  bvpl_neighb_operator<float, bvpl_edge_algebraic_mean_functor<float> > oper(func);
  bvpl_vector_operator vector_oper;
  vector_oper.apply_and_suppress(grid,kernel_vec,&oper,grid_out, id_grid);

  //along the plane the winner should be the target axis
  TEST("Directions", true,  check_data(id_grid, target_axis, target_id, 1));


  bvxm_voxel_grid<float> *grid_non_max=new bvxm_voxel_grid<float>(grid->grid_size());
  grid_non_max->initialize_data(0.0f);
  vector_oper.non_maxima_suppression(grid_out, id_grid, kernel_vec, grid_non_max);

  //after non-maxima suppression the center voxel should be the winner
  TEST("Non-max suppressions", true, check_non_max(grid_non_max));
}

MAIN(test_bvpl_vector_operator)
{
  //test_vector_operator();
  test_non_max_suppression();
  return 0;
}
