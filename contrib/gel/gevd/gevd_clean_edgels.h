#ifndef _clean_h_
#define _clean_h_

// .NAME clean - Run Through the results of edgel segmentation and remove
//               edgel chains that do not satisfy the following criteria:
//               1: No vertices closer than 3 pixels 2: No bridges
//               3:No self-intersection
// .LIBRARY CAD_Detection
// .HEADER CAD package
// .INCLUDE CAD_Detection/clean.h
// .FILE CAD_Detection/clean.h
// .FILE CAD_Detection/clean.C
//
// .SECTION Description
//  In using an edge segmentation for region analysis it is necessary to
//  eliminate edges that do not perimit the extraction of valid region
//  topology. There are a number of conditions that would defeat a region
//  analysis algorithm:
//   1: Too short edges.  If the vertices of an edge closer than 3 pixels,
//   (where distance is measured by the minimum image distance along the
//    image axis) and the edgels are all within a pixel of a staight line
//   joining the vertices then the edge should be eliminated and replaced
//   by a single vertex at the midpoint of the original vertices.
//
//   2: Insufficient edgels.  In some cases the VDEdgeDetector does not
//     suppy enough edgels to form a continuous chain between vertices.
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
// .SECTION Authors
//  J.L. Mundy - Feb 20, 1999
// .SECTION Modifications Feb 2001
//         Effectively the same as Detection/Clean.h but duplicated here
//         to allow for CAD required changes
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vsol/vsol_point_2d.h>

class clean_edgels
{
public:
  clean_edgels();
  ~clean_edgels();
  void DoCleanEdgelChains(vcl_vector<vtol_edge_2d*>& in_edgels, vcl_vector<vtol_edge_2d*>& out_edgels, int steps = 10);
  void JumpGaps();
  void RemoveBridges();
  void DeleteShortEdges();
  void FixDefficientEdgels();
  void RemoveJaggies();
  void RemoveLoops();
protected:
  void print_protection();
  void detect_similar_edges(vcl_vector<vtol_edge_2d*>& common_edges, float tolerance,
                            vcl_vector<vtol_edge_2d*>& deleted_edges);
  void remove_similar_edges(vtol_vertex_2d*& v1, vcl_vector<vtol_edge_2d*>& deleted_edges);
  bool edge_exists(vtol_vertex_2d* v1, vtol_vertex_2d* v2, vcl_vector<vtol_edge_2d*>& intersection);
  void remove_connected_edges(vtol_vertex_2d* v, vcl_vector<vtol_edge_2d*>& edges);
  bool closest_vertex(vtol_edge_2d* e, vsol_point_2d* p, float radius, vtol_vertex_2d*& v);
  bool split_edge(vtol_edge_2d* e, vtol_vertex_2d* v, vtol_edge_2d*& e1, vtol_edge_2d*& e2);
  vcl_vector<vtol_edge_2d*>* _out_edgels;
};
#endif
