// This is brl/bbas/imesh/imesh_detection.h
#ifndef imesh_detection_h_
#define imesh_detection_h_
//:
// \file
// \brief Detect features on meshes
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date May 29, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <iostream>
#include <vector>
#include "imesh_mesh.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Return the indices of half edges that are on the mesh boundary
//  The results are organized into loops
std::vector<std::vector<unsigned int> >
imesh_detect_boundary_loops(const imesh_half_edge_set& half_edges);


//: Trace half edges that have been selected into loops
//  \return true if all half edges form loops
//  The loops are returned in \param loops as vectors of half edge indices
bool
imesh_trace_half_edge_loops(const imesh_half_edge_set& half_edges,
                            const std::vector<bool>& flags,
                            std::vector<std::vector<unsigned int> >& loops);

//: Return the indices of contour half edges as seen from direction \param dir
//  The results are organized into loops
std::vector<std::vector<unsigned int> >
imesh_detect_contour_generator(const imesh_mesh& mesh, const vgl_vector_3d<double>& dir);

//: Mark contour half edges as seen from direction \param dir
//  For each contour edge the half edge with the face normal opposing dir is marked
std::vector<bool>
imesh_detect_contours(const imesh_mesh& mesh, vgl_vector_3d<double> dir);


//: Return the indices of contour half edges as seen from center of projection \param pt
//  The results are organized into loops
std::vector<std::vector<unsigned int> >
imesh_detect_contour_generator(const imesh_mesh& mesh, const vgl_point_3d<double>& pt);

//: Mark contour half edges as seen from center of projection \param pt
//  For each contour edge the half edge with the face normal opposing dir is marked
std::vector<bool>
imesh_detect_contours(const imesh_mesh& mesh, const vgl_point_3d<double>& pt);


//: Segment the faces into groups of connected components
std::vector<std::set<unsigned int> >
imesh_detect_connected_components(const imesh_half_edge_set& he);

//: Compute the set of all faces in the same connected component as \a face
std::set<unsigned int>
imesh_detect_connected_faces(const imesh_half_edge_set& he, unsigned int face);


#endif // imesh_detection_h_
