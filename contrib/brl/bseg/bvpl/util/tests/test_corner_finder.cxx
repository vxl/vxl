//
//:
//\file
//\author Isabel Restrepo
//\date 9/24/09.

#include <testlib/testlib_test.h>
#include <bvpl/util/bvpl_corner_pair_finder.h>
#include <bvpl/kernels/bvpl_weighted_cube_kernel_factory.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/io/bvxm_vrml_voxel_grid.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_line_segment_3d.h>
#include <bvpl/bvpl_direction_to_color_map.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h>

void test_corner_pair_finder()
{
  //create a grid
  vgl_vector_3d<unsigned> grid_size(20,20,20);
  bvxm_voxel_grid<int> *id_grid = new bvxm_voxel_grid<int>(grid_size);

  int idx = 0;
  bvxm_voxel_slab_iterator<int> slab_iterator = id_grid->slab_iterator(idx, grid_size.z());
  id_grid->initialize_data(-1);

  (*slab_iterator)(3,3,17) = 0;
  (*slab_iterator)(3,4,17) = 0;
  (*slab_iterator)(4,3,17) = 0;
  (*slab_iterator)(3,3,16) = 0;
  (*slab_iterator)(3,7,17) = 1;
  (*slab_iterator)(3,8,13) = 1;
  (*slab_iterator)(3,7,12) = 2;

  //create directions and vector of kernels
  bvpl_kernel_vector_sptr vec_kernel=new bvpl_kernel_vector();
  bvpl_weighted_cube_kernel_factory factory(2,4,2);
  vnl_float_3 axis(1.0f, 0.0f, 0.0f);
  factory.set_rotation_axis(axis);
  factory.set_angle(0.0f);
  vec_kernel->kernels_.push_back(new bvpl_kernel(factory.create()));
  factory.set_rotation_axis(axis);
  factory.set_angle(float(vnl_math::pi_over_2));
  vec_kernel->kernels_.push_back(new bvpl_kernel(factory.create()));
  factory.set_rotation_axis(axis);
  factory.set_angle(float(vnl_math::pi));
  vec_kernel->kernels_.push_back(new bvpl_kernel(factory.create()));

  vcl_vector<vcl_vector<vgl_line_segment_3d<int> > > all_lines= bvpl_corner_pair_finder::find_pairs(id_grid, vec_kernel, vec_kernel)->pairs_;

  //visualize
  bvxm_voxel_grid<float> *response_grid= new bvxm_voxel_grid<float>(grid_size);
  response_grid->initialize_data(1.0f);
  bvxm_voxel_grid<vnl_float_4> *out_grid = new bvxm_voxel_grid<vnl_float_4>(grid_size);
   vcl_vector<float> colors;
  float hue = 0.0f;
  for ( int i = 0; i < vec_kernel->size(); ++i) {
    colors.push_back(hue);
    hue+=1.0f/float(vec_kernel->size());
  }
  bvpl_convert_id_grid_to_hsv_grid(id_grid, response_grid, out_grid, colors);

  if (vul_file::exists("spheres.wrl"))
    vpl_unlink("spheres.wrl");

  vcl_ofstream os2("spheres.wrl");
  bvxm_vrml_voxel_grid::write_vrml_header(os2);
  bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(os2, out_grid, 0.0f);

  //vcl_ofstream os2("lines.wrl");
  //bvxm_vrml_voxel_grid::write_vrml_header(os2);

  vcl_cout << all_lines.size() << vcl_endl;
  for (unsigned i=0; i<all_lines.size(); ++i){
      vcl_cout << all_lines[i].size() << vcl_endl;
    for (unsigned j=0; j<all_lines[i].size(); ++j);
       //bvxm_vrml_voxel_grid::write_vrml_line_segment(os2, all_lines[i][j], 0.5f, 0.5f, 0.5f, 0.f);
      }
}


MAIN(test_corner_finder)
{
  test_corner_pair_finder();
  return 0;
}


