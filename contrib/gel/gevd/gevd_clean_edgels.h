#ifndef gevd_clean_edgels_h_
#define gevd_clean_edgels_h_
//:
// \file
// \brief Remove edgel chains with (1) end vertices closer than 3 pixels, (2) bridges, (3) self-intersection
//
//  In using an edge segmentation for region analysis it is necessary to
//  eliminate edges that do not permit the extraction of valid region
//  topology. There are a number of conditions that would defeat a region
//  analysis algorithm:
//
//   1: Too short edges.  If the vertices of an edge closer than 3 pixels,
//   (where distance is measured by the minimum image distance along the
//    image axis) and the edgels are all within a pixel of a straight line
//   joining the vertices then the edge should be eliminated and replaced
//   by a single vertex at the midpoint of the original vertices.
//
//   2: Insufficient edgels.  In some cases the VDEdgeDetector does not
//     supply enough edgels to form a continuous chain between vertices.
//     This problem appears to happen for n_edgels<=2.
//
//   3: Bridges.  There is no point in passing edges which have a vertex
//   of order 1. That is, there is only one edge incident on the vertex.
//   Such edges will never lead to a correct region topology.
//
//   4: Self intersection. An edge should never intersect itself in any case,
//   but it might be possible that an edge-detection algorithm could produce
//   such edges. For example, the VanDuc edge-detector does currently
//   form self-intersecting edges.(not yet implemented)
//
// \author
//  J.L. Mundy - Feb 20, 1999
//
// \verbatim
//  Modifications
//   Feb 2001
//         Effectively the same as Detection/Clean.h but duplicated here
//         to allow for CAD required changes
// \endverbatim
//-------------------------------------------------------------------------
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>

class gevd_clean_edgels
{
 public:
  gevd_clean_edgels();
  ~gevd_clean_edgels();
  void DoCleanEdgelChains(std::vector<vtol_edge_2d_sptr>& in_edgels, std::vector<vtol_edge_2d_sptr>& out_edgels, int steps = 10);
  void JumpGaps();
  void RemoveBridges();
  void DeleteShortEdges();
  void FixDefficientEdgels();
  void RemoveJaggies();
  void RemoveLoops();
 protected:
  void print_protection();
  void detect_similar_edges(std::vector<vtol_edge_2d_sptr>& common_edges, float tolerance,
                            std::vector<vtol_edge_2d_sptr>& deleted_edges);
  void remove_similar_edges(vtol_vertex_2d*& v1, std::vector<vtol_edge_2d_sptr>& deleted_edges);
  bool edge_exists(const vtol_vertex_2d_sptr& v1, const vtol_vertex_2d_sptr& v2, std::vector<vtol_edge_2d_sptr>& intersection);
  void remove_connected_edges(vtol_vertex_2d* v, std::vector<vtol_edge_2d_sptr>& edges);
  bool closest_vertex(const vtol_edge_2d_sptr& e, const vsol_point_2d_sptr& p, float radius, vtol_vertex_2d_sptr& v);
  bool split_edge(const vtol_edge_2d_sptr& e, const vtol_vertex_2d_sptr& v, vtol_edge_2d_sptr& e1, vtol_edge_2d_sptr& e2);
  std::vector<vtol_edge_2d_sptr>* out_edgels_;
};

#endif // gevd_clean_edgels_h_
