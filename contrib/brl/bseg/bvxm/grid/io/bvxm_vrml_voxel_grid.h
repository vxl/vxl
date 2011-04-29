// This is//projects/vxl/src/contrib/brl/bseg/bvxm/grid/io/bvxm_vrml_voxel_grid.h
#ifndef bvxm_vrml_voxel_grid_h
#define bvxm_vrml_voxel_grid_h
//:
// \file
// \brief A class with vrml utilities for a voxel grid
// \author Isabel Restrepo mir@lems.brown.edu
// \date  September 21, 2009
//
// \verbatim
//  Modifications
//   2009-12-19 Peter Vanroose - added write_vrml_header()
// \endverbatim

#include "../bvxm_voxel_grid.h"
#include <bvrml/bvrml_write.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>
#include <vnl/vnl_float_4.h>
#include <vcl_fstream.h>

class bvxm_vrml_voxel_grid
{
 public:
  static void write_vrml_header(vcl_ofstream& str) { str << "#VRML V2.0 utf8\n"; }
  static void write_vrml_grid(vcl_ofstream& str, bvxm_voxel_grid<float> *grid, float threshold);
  static void write_vrml_grid(vcl_ofstream& str, bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> > *grid, float threshold);
  static void write_vrml_grid_as_spheres(vcl_ofstream& str, bvxm_voxel_grid<float> *grid, float threshold);
  static void write_vrml_grid_as_spheres(vcl_ofstream& str, bvxm_voxel_grid<vnl_float_4> *grid, float threshold, int s=1);
  static void write_vrml_grid_as_pointers(vcl_ofstream& str, bvxm_voxel_grid<vnl_float_4> *grid, float threshold, int s=1);
};

#endif // bvxm_vrml_voxel_grid_h
