// This is brl/bseg/bmdl/bmdl_mesh.h
#ifndef bmdl_mesh_h_
#define bmdl_mesh_h_
//:
// \file
// \brief Construct a 3-d mesh from classified lidar images
//
// \author Matt Leotta
// \date Oct 14, 2008

#include <iostream>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_image_view.h>
#include <imesh/imesh_mesh.h>

struct bmdl_edge
{
  bmdl_edge(unsigned int b1, unsigned int b2)
  : building1(b1), building2(b2) {}

  unsigned int building1;
  unsigned int building2;
  unsigned int joint1;
  unsigned int joint2;
  std::vector<vgl_point_2d<double> > pts;
};


struct bmdl_region
{
  std::vector<unsigned int> edge_idxs;
  std::vector<std::vector<unsigned int> > hole_edge_idxs;
};


class bmdl_mesh
{
 public:
  //: find the next trace point and direction
  static bool next_trace_point(unsigned int& i, unsigned int& j, int& dir,
                               const unsigned int* &p, unsigned int value,
                               unsigned int ni, unsigned int nj,
                               std::ptrdiff_t istep, std::ptrdiff_t jstep);

  //: trace a single boundary starting and location (i,j)
  static bool trace_boundary(std::vector<vgl_point_2d<double> >& pts,
                             unsigned int value,
                             const vil_image_view<unsigned int>& labels,
                             vil_image_view<bool>& visited,
                             unsigned int i, unsigned int j);

  //: trace the boundaries of the building labels into polygons
  // If \a dropped_clipped is true then buildings clipped by the image boundaries are not traced
  static std::vector<vgl_polygon<double> >
      trace_boundaries(const vil_image_view<unsigned int>& labels, bool drop_clipped = true);

  //: extract shared boundary edges from the polygon boundaries
  // \returns the number of joints linking the edges
  static unsigned int link_boundary_edges(const vil_image_view<unsigned int>& labels,
                                          const std::vector<vgl_polygon<double> >& polygons,
                                          std::vector<bmdl_edge>& edges,
                                          std::vector<bmdl_region>& regions);


  //: test if a boundary is clipped by the image of size \a ni by \a nj
  static bool is_clipped(const std::vector<vgl_point_2d<double> >& poly,
                         unsigned ni, unsigned nj);

  //: simplify a polygon by fitting lines
  // \a tol is the tolerance for line fitting
  static void simplify_polygon( std::vector<vgl_point_2d<double> >& polygon, double tol);

  //: simplify the boundaries by fitting lines
  static void simplify_boundaries( std::vector<vgl_polygon<double> >& boundaries );

  //: simplify an edge by fitting lines
  // \a tol is the tolerance for line fitting
  static void simplify_edge( std::vector<vgl_point_2d<double> >& pts, double tol );

  //: simplify the linked edges by fitting lines
  static void simplify_edges( std::vector<bmdl_edge>& edges );

  //: Subtract a hole from an existing face in a mesh
  static void roof_subtract_hole(const imesh_vertex_array<3>& verts,
                                 std::vector<unsigned int>& face,
                                 const std::vector<unsigned int>& hole);

  //: construct a mesh out of data and labels
  // The coordinate system is flipped over the x-axis to make it right handed
  // i.e. (x,y) -> (x,-y)
  static void mesh_lidar(const std::vector<vgl_polygon<double> >& boundaries,
                         const vil_image_view<unsigned int>& labels,
                         const vil_image_view<double>& heights,
                         const vil_image_view<double>& ground,
                         imesh_mesh& mesh);

  //: construct a mesh out of data and labels using linked edges
  // The coordinate system is flipped over the x-axis to make it right handed
  // i.e. (x,y) -> (x,-y)
  static void mesh_lidar(const std::vector<bmdl_edge>& edges,
                         const std::vector<bmdl_region>& regions,
                         unsigned int num_joints,
                         const vil_image_view<unsigned int>& labels,
                         const vil_image_view<double>& heights,
                         const vil_image_view<double>& ground,
                         imesh_mesh& mesh);
};

#endif // bmdl_mesh_h_
