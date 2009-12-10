// This is brl/bseg/bvxm/grid/io/bvxm_vrml_voxel_grid.h
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
//   <none yet>
// \endverbatim

#include "../bvxm_voxel_grid.h"
#include <vcl_fstream.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vnl/vnl_float_4.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_f1.h>

class bvxm_vrml_voxel_grid
{
 public:
  // Default Constructor
  bvxm_vrml_voxel_grid() {}

  static void write_vrml_grid(vcl_ofstream& str, bvxm_voxel_grid<float> *grid, float threshold);

  static void write_vrml_grid(vcl_ofstream& str, bvxm_voxel_grid<bsta_num_obs<bsta_gauss_f1> > *grid, float threshold);

  static void write_vrml_header(vcl_ofstream& str);

  static void write_vrml_grid_as_spheres(vcl_ofstream& str, bvxm_voxel_grid<float> *grid, float threshold);

  static void write_vrml_grid_as_spheres(vcl_ofstream& str, bvxm_voxel_grid<vnl_float_4> *grid, float threshold, int s=1);

#if 0 // not yet implemented
  static void write_vrml_line_segment(vcl_ofstream& str, const vgl_line_segment_3d<int>& line,
                                      const float r, const float g, const float b,
                                      const float transparency);
#endif
  static void write_vrml_line(vcl_ofstream& str,vgl_point_3d<double> pt,vgl_vector_3d<double> dir,
                              float response,
                              const float r,
                              const float g,
                              const float b);

 static void write_vrml_grid_as_pointers(vcl_ofstream& str, bvxm_voxel_grid<vnl_float_4> *grid, float threshold, int s=1);

#if 0 // not yet implemented
  static void write_vrml_trailer(vcl_ofstream& str);

  static void write_vrml_points(vcl_ofstream& str,
                                vcl_vector<vsol_point_3d_sptr> const& pts3d);

  static void write_vrml_points(vcl_ofstream& str,
                                vcl_vector<vgl_point_3d<double> > const& pts3d,
                                vcl_vector<vgl_point_3d<float> > const& color);

  static void write_vrml_polyline(vcl_ofstream& str,
                                  vcl_vector<vgl_point_3d<double> > const& vts,
                                  const float r,
                                  const float g,
                                  const float b);
#endif

  static void write_vrml_box(vcl_ofstream& str, vgl_box_3d<int> const& box,
                             const float r = 1.0, const float g = 1.0,
                             const float b = 1.0,
                             const float transparency = 0.0);

  static void write_vrml_sphere(vcl_ofstream& str,
                                vgl_sphere_3d<float> const& sphere,
                                const float r = 1.0, const float g =1.0,
                                const float b=1.0,
                                const float transparency = 0);

  static void write_vrml_disk(vcl_ofstream& str,
                              vgl_point_3d<double> const& pt,
                              vgl_vector_3d<double> const &dir,
                              float radius,
                              const float r, const float g, const float b);
};

#endif
