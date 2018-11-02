// This is brl/bbas/imesh/imesh_detection.cxx
#include "imesh_detection.h"
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Return the indices of half edges that are on the mesh boundary
// The results are organized into loops
std::vector<std::vector<unsigned int> >
imesh_detect_boundary_loops(const imesh_half_edge_set& half_edges)
{
  std::vector<bool> visited(half_edges.size(),false);
  std::vector<std::vector<unsigned int> > loops;
  for (unsigned int i=0; i<half_edges.size(); ++i) {
    if (visited[i])
      continue;
    if (half_edges[i].is_boundary()) {
      visited[i] = true;
      imesh_half_edge_set::f_const_iterator end(i,half_edges);
      imesh_half_edge_set::f_const_iterator itr(end);
      std::vector<unsigned int> loop(1,end->half_edge_index());
      ++itr;
      for (; itr != end; ++itr) {
        visited[itr->half_edge_index()] = true;
        loop.push_back(itr->half_edge_index());
      }
      loops.push_back(loop);
    }
  }
  return loops;
}


//: Trace half edges that have been selected into loops
//  \returns true if all half edges form loops
//  The loops are returned in \param loops as vectors of half edge indices
bool
imesh_trace_half_edge_loops(const imesh_half_edge_set& half_edges,
                            const std::vector<bool>& flags,
                            std::vector<std::vector<unsigned int> >& loops)
{
  loops.clear();
  std::vector<bool> visited(half_edges.size(),false);
  for (unsigned int i=0; i<half_edges.size(); ++i) {
    if (visited[i] || !flags[i])
      continue;

    std::vector<unsigned int> loop;
    loop.push_back(i);
    visited[i] = true;
    imesh_half_edge_set::f_const_iterator end(i,half_edges);
    imesh_half_edge_set::f_const_iterator itr(end);
    ++itr;
    for (; itr!=end; ++itr) {
      unsigned int j = itr->half_edge_index();
      if (!flags[j]) {
        imesh_half_edge_set::v_const_iterator vend(itr);
        imesh_half_edge_set::v_const_iterator vitr(vend);
        ++vitr;
        for (; vitr!=vend && !flags[vitr->half_edge_index()]; ++vitr) /*nothing*/;
        if (vitr == vend)
          return false; // loop reached a dead end

        itr = imesh_half_edge_set::f_const_iterator(vitr);
        j = itr->half_edge_index();
      }
      if (visited[j]) {
        if (j == end->half_edge_index())
          break; // loop completed
        return false; // loop joins a previous loop
      }

      visited[j] = true;
      loop.push_back(j);
    }
    loops.push_back(loop);
  }
  return true;
}


//: Return the indices of contour half edges as seen from direction \param dir
//  The results are organized into loops
std::vector<std::vector<unsigned int> >
imesh_detect_contour_generator(const imesh_mesh& mesh, const vgl_vector_3d<double>& dir)
{
  std::vector<bool> contours = imesh_detect_contours(mesh,dir);

  std::vector<std::vector<unsigned int> > loops;
  bool valid = imesh_trace_half_edge_loops(mesh.half_edges(),contours,loops);
  assert(valid);

  return loops;
}


//: Mark contour half edges as seen from direction \param dir
//  For each contour edge the half edge with the face normal opposing dir is marked
std::vector<bool>
imesh_detect_contours(const imesh_mesh& mesh, vgl_vector_3d<double> dir)
{
  assert(mesh.has_half_edges());
  assert(mesh.faces().has_normals());

  normalize(dir);

  const imesh_half_edge_set& half_edges = mesh.half_edges();
  const std::vector<vgl_vector_3d<double> >& normals = mesh.faces().normals();

  std::vector<double> fdotd(normals.size(),0.0);
  for (unsigned int i=0; i<normals.size(); ++i)
  {
    fdotd[i] = dot_product(normalized(normals[i]),dir);
  }

  const unsigned int num_edges = half_edges.size()/2;
  std::vector<bool> is_contour(half_edges.size(),false);
  for (unsigned int i=0; i<num_edges; ++i)
  {
    const imesh_half_edge& he = half_edges[i*2];
    const imesh_half_edge& ohe = half_edges[he.pair_index()];
    double v1=0.0, v2=0.0;
    if (!he.is_boundary())
      v1 = fdotd[he.face_index()];
    if (!ohe.is_boundary())
      v2 = fdotd[ohe.face_index()];
    double prod = v1*v2;
    if (prod<=0.0) {
      if (v1<0.0) is_contour[i*2] = true;
      if (v2<0.0) is_contour[i*2+1] = true;
    }
  }
  return is_contour;
}


//: Return the indices of contour half edges as seen from center of projection \param pt
//  The results are organized into loops
std::vector<std::vector<unsigned int> >
imesh_detect_contour_generator(const imesh_mesh& mesh, const vgl_point_3d<double>& pt)
{
  std::vector<bool> contours = imesh_detect_contours(mesh,pt);

  std::vector<std::vector<unsigned int> > loops;
  bool valid = imesh_trace_half_edge_loops(mesh.half_edges(),contours,loops);
  assert(valid);

  return loops;
}


//: Mark contour half edges as seen from center of projection \param pt
//  For each contour edge the half edge with the face normal opposing dir is marked
std::vector<bool>
imesh_detect_contours(const imesh_mesh& mesh, const vgl_point_3d<double>& pt)
{
  assert(mesh.has_half_edges());
  assert(mesh.faces().has_normals());

  const imesh_half_edge_set& half_edges = mesh.half_edges();
  const std::vector<vgl_vector_3d<double> >& normals = mesh.faces().normals();
  const imesh_vertex_array<3>& verts = mesh.vertices<3>();


  const unsigned int num_edges = half_edges.size()/2;
  std::vector<bool> is_contour(half_edges.size(),false);
  for (unsigned int i=0; i<num_edges; ++i)
  {
    const imesh_half_edge& he = half_edges[i*2];
    const imesh_half_edge& ohe = half_edges[he.pair_index()];
    vgl_vector_3d<double> dir1 = vgl_point_3d<double>(verts[he.vert_index()]) - pt;
    vgl_vector_3d<double> dir2 = vgl_point_3d<double>(verts[ohe.vert_index()]) - pt;
    vgl_vector_3d<double> dir = normalized(dir1+dir2);

    double v1=0.0, v2=0.0;
    if (!he.is_boundary())
      v1 = dot_product(normalized(normals[he.face_index()]),dir);
    if (!ohe.is_boundary())
      v2 = dot_product(normalized(normals[ohe.face_index()]),dir);

    double prod = v1*v2;
    if (prod<=0.0) {
      if (v1<0.0) is_contour[i*2] = true;
      if (v2<0.0) is_contour[i*2+1] = true;
    }
  }
  return is_contour;
}


//: Segment the faces into groups of connected components
std::vector<std::set<unsigned int> >
imesh_detect_connected_components(const imesh_half_edge_set& he)
{
  std::vector<std::set<unsigned int> > components;
  std::vector<bool> visited(he.num_faces(),false);
  for (unsigned int i=0; i<visited.size(); ++i)
  {
    if (visited[i]) continue;
    components.push_back(imesh_detect_connected_faces(he,i));
    for (auto &itr : components.back())
      visited[itr] = true;
  }
  return components;
}


//: Compute the set of all faces in the same connected component as \a face
std::set<unsigned int>
imesh_detect_connected_faces(const imesh_half_edge_set& he, unsigned int face)
{
  std::set<unsigned int> component;
  std::vector<unsigned int > stack(1,face);

  while (!stack.empty())
  {
    unsigned int f = stack.back();
    stack.pop_back();
    if (component.find(f) != component.end())
      continue;

    component.insert(f);
    imesh_half_edge_set::f_const_iterator fi = he.face_begin(f);
    imesh_half_edge_set::f_const_iterator end = fi;
    do {
      unsigned int nf = he[fi->pair_index()].face_index();
      if (nf != imesh_invalid_idx)
        stack.push_back(nf);
      ++fi;
    } while (fi != end);
  }

  return component;
}
