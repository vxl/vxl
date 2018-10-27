//  This is brl/bseg/bvxm/grid/tests/test_bvxm_vrml.cxx
#include <testlib/testlib_test.h>
//:
// \file
//  \brief  Tests from vrml functions defined for voxel grid
//  \author Isabel Restrepo
//  \date   September 22, 2009

#include <bvxm/grid/io/bvxm_vrml_voxel_grid.h>

bool grid_to_vrml()
{
  //Fill in the grid
  vgl_vector_3d<unsigned> grid_size(4, 4, 4);
  auto *grid = new bvxm_voxel_grid<float>(grid_size);
  grid->initialize_data(0.5f);

  bvxm_voxel_grid<float>::iterator grid_it = grid->begin();

  for (unsigned k=0; grid_it != grid->end(); ++grid_it, k++)
    for (unsigned i=0; i<(*grid_it).nx(); i=i+2)
      for (unsigned j=0; j < (*grid_it).ny(); j=j+2)
        (*grid_it)(i,j)=1.0f;

  //Write to VRML file
  std::ofstream os("point_set.wrl");
  bvrml_write::write_vrml_header(os);
  bvxm_vrml_voxel_grid::write_vrml_grid(os, grid, 0.2f);

  std::ofstream os2("spheres.wrl");
  bvrml_write::write_vrml_header(os2);
  bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(os2, grid, 0.2f);

  return true;
}

bool color_grid_to_vrml()
{
  //Fill in the grid
  vgl_vector_3d<unsigned> grid_size(8, 8, 8);
  auto *grid = new bvxm_voxel_grid<vnl_float_4>(grid_size);
  grid->initialize_data(vnl_float_4(255.0f, 255.0f, 252.0f, 225.0f));

  bvxm_voxel_grid<vnl_float_4>::iterator grid_it = grid->begin();

  for (unsigned k=0; grid_it != grid->end(); ++grid_it, k++)
    for (unsigned i=0; i<(*grid_it).nx(); i=i+2)
      for (unsigned j=0; j < (*grid_it).ny(); j=j+2)
        (*grid_it)(i,j)=vnl_float_4(255.0f, 0.0f, 0.0f, 255.0f);

  std::ofstream os("color_spheres.wrl");
  bvrml_write::write_vrml_header(os);
  bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(os, grid, 0.2f);

  return true;
}

static void test_bvxm_vrml()
{
  std::cout << "Grid to vrml\n";
  grid_to_vrml();
  std::cout << "Color Grid to vrml\n";
  color_grid_to_vrml();
}

TESTMAIN(test_bvxm_vrml);
