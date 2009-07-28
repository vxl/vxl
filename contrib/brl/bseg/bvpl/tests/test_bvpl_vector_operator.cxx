//:
// \file
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_edge2d_kernel_factory.h>
#include <bvpl/bvpl_edge3d_kernel_factory.h>
#include <bvpl/bvpl_edge_algebraic_mean_functor.h>
#include <bvpl/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/bvpl_opinion_functor.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_random.h>

#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>

#include <vbl/vbl_array_3d.h>
#include <bvxm/grid/bvxm_opinion.h>
#include <bvxm/grid/bvxm_voxel_grid_basic_ops.h>
#include <bvxm/grid/bvxm_voxel_grid_opinion_basic_ops.h>
#include <bvxm/grid/io/bvxm_io_voxel_grid.h>
#include <bvpl/bvpl_opinion_functor.h>
#include <bvpl/bvpl_neighb_operator.h>
#include <bvpl/bvpl_vector_operator.h>
#include <vul/vul_file.h>

void create_grid(vcl_string grid_filename)
{
	if(vul_file_exists(grid_filename))
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
MAIN(test_bvpl_vector_operator)
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
	bvpl_kernel_vector_sptr kernel_vec=kernels_3d.create_kernel_vector();

	vcl_string out_grid_path="out_grid.vox";
	vcl_string orientation_grid_path="orientation_grid.vox";
	vcl_string out_grid_expectation_path="out_grid_expectation.vox";

	bvxm_voxel_grid<bvxm_opinion> *grid_out=new bvxm_voxel_grid<bvxm_opinion>(out_grid_path, grid->grid_size());
	bvxm_voxel_grid<vnl_vector_fixed<float,3> > *orientation_grid
		=new bvxm_voxel_grid<vnl_vector_fixed<float,3> >(orientation_grid_path, grid->grid_size());
	bvpl_opinion_functor func;
	bvpl_neighb_operator<bvxm_opinion, bvpl_opinion_functor> oper(func);
	bvpl_vector_operator<bvxm_opinion,  bvpl_opinion_functor> vector_oper;
	vector_oper.apply_and_suppress(grid,kernel_vec,&oper,grid_out, orientation_grid);

	bvxm_voxel_grid<float> *out_grid_expectation=new bvxm_voxel_grid<float>(out_grid_expectation_path,grid->grid_size());
	bvxm_expectation_opinion_voxel_grid(grid_out,out_grid_expectation);
	bvxm_grid_save_raw<float>(out_grid_expectation,"grid_out_expectation.raw");



	return 0;
}
