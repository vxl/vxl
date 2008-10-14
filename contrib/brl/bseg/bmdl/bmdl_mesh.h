// This is brl/bseg/bmdl/bmdl_mesh.h
#ifndef bmdl_mesh_h_
#define bmdl_mesh_h_
//:
// \file
// \brief Construct a 3-d mesh from classified lidar images
//
// \author Matt Leotta
// \date 10/14/2008
//      

#include <vcl_cstddef.h>

#include <vil/vil_image_view.h>
#include <imesh/imesh_mesh.h>

class bmdl_mesh {
public:  
  //: find the next trace point and direction
  static bool next_trace_point(unsigned int& i, unsigned int& j, int& dir,
                               const unsigned int* &p, unsigned int value,
                               unsigned int ni, unsigned int nj,
                               vcl_ptrdiff_t istep, vcl_ptrdiff_t jstep);
  
  //: trace a single boundary starting and location (i,j)
  static bool trace_boundary(vcl_vector<vgl_point_2d<double> >& pts,
                             unsigned int value,
                             const vil_image_view<unsigned int>& labels,
                             vil_image_view<bool>& visited,
                             unsigned int i, unsigned int j);
  
  //: trace the boundaries of the building labels into polygons
  static vcl_vector<vgl_polygon<double> > 
      trace_boundaries(const vil_image_view<unsigned int>& labels);
  
  //: simplify the boundaries by fitting lines
  static void simplify_boundaries( vcl_vector<vgl_polygon<double> >& boundaries );
  
  //: construct a mesh out of data and labels
  static void mesh_lidar(const vil_image_view<double>& first_return,
                         const vil_image_view<double>& last_return,
                         const vil_image_view<unsigned int>& labels,
                         const vil_image_view<double>& heights,
                               double ground,
                               imesh_mesh& mesh);

};

#endif // bmdl_mesh_h_
