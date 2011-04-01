// This is brl/bbas/imesh/algo/imesh_generate_mesh.h
#ifndef imesh_generate_mesh_h_
#define imesh_generate_mesh_h_
//:
// \file
// \brief mesh generation functions
// \author J.L. Mundy
// \date March 16, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <imesh/imesh_mesh.h>
#include <vcl_vector.h>
#include <vgl/vgl_fwd.h>

//: Fill the convex hull with triangles, constrained by the line segments
void
imesh_generate_mesh_2d(vcl_vector<vgl_point_2d<double> > const& convex_hull,
                       vcl_vector< vgl_line_segment_2d<double> > const & segs,
                       imesh_mesh& mesh);
void 
imesh_generate_mesh_2d_2(vcl_vector<vgl_point_2d<double> > const& convex_hull,
                       vcl_vector<vgl_line_segment_2d<double> > const& segs,
                       vcl_vector<vgl_point_2d<double> > const & points,
                       imesh_mesh& mesh);

#endif // imesh_generate_mesh_h_
