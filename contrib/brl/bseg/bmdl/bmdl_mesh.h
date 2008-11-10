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
  // if \a dropped_clipped is true then buildings clipped by the image boundaries are not traced
  static vcl_vector<vgl_polygon<double> > 
      trace_boundaries(const vil_image_view<unsigned int>& labels, bool drop_clipped = true);
  
  //: test if a boundary is clipped by the image of size \a ni by \a nj
  static bool is_clipped(const vcl_vector<vgl_point_2d<double> >& poly, 
                         unsigned ni, unsigned nj);
  
  //: simplify a polygon by fitting lines
  // \a tol is the tolerence for line fitting
  static void simplify_polygon( vcl_vector<vgl_point_2d<double> >& polygon, double tol);
  
  //: simplify the boundaries by fitting lines
  static void simplify_boundaries( vcl_vector<vgl_polygon<double> >& boundaries );
  
  //: construct a mesh out of data and labels
  // The coordinate system is flipped over the x-axis to make it right handed
  // i.e. (x,y) -> (x,-y)
  static void mesh_lidar(const vcl_vector<vgl_polygon<double> >& boundaries,
                         const vil_image_view<unsigned int>& labels,
                         const vil_image_view<double>& heights,
                         const vil_image_view<double>& ground,
                         imesh_mesh& mesh);

};

#endif // bmdl_mesh_h_
