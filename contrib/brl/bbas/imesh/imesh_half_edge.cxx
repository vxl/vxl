// This is brl/bbas/imesh/imesh_half_edge.cxx
#include <iostream>
#include <map>
#include <utility>
#include "imesh_half_edge.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


//: Construct from a face index list
imesh_half_edge_set::imesh_half_edge_set(const std::vector<std::vector<unsigned int> >& face_list)
{
  build_from_ifs(face_list);
}


//: Build the half edges from an indexed face set
void
imesh_half_edge_set::build_from_ifs(const std::vector<std::vector<unsigned int> >& face_list)
{
  half_edges_.clear();
  typedef std::pair<unsigned int, unsigned int> vert_pair;
  std::map<vert_pair, unsigned int> edge_map;

  face_to_he_.resize(face_list.size(), imesh_invalid_idx);

  unsigned int max_v = 0;

  const unsigned int num_faces = face_list.size();
  for (unsigned int f=0; f<num_faces; ++f) {
    const std::vector<unsigned int>& verts = face_list[f];
    const unsigned int num_verts = verts.size();
    unsigned int first_e = imesh_invalid_idx; // first edge index
    unsigned int prev_e = imesh_invalid_idx; // previous edge index
    for (unsigned int i=0; i<num_verts; ++i) {
      const unsigned int& v = verts[i];
      if (v > max_v) max_v = v;
      unsigned int ni = (i+1)%num_verts;
      const unsigned int& nv = verts[ni];

      vert_pair vp(v,nv);
      if (v > nv) vp = vert_pair(nv,v);
      auto m = edge_map.find(vp);
      unsigned int curr_e;
      if (m == edge_map.end()) {
        curr_e = half_edges_.size();
        edge_map.insert(std::pair<vert_pair,unsigned int>(vp,curr_e));
        half_edges_.emplace_back(curr_e,imesh_invalid_idx,v,f);
        half_edges_.emplace_back(curr_e+1,imesh_invalid_idx,nv,imesh_invalid_idx);
      }
      else {
        curr_e = m->second+1;
        assert(half_edges_[curr_e].next_index() == imesh_invalid_idx);
        assert(half_edges_[curr_e].vert_index() == v);
        half_edges_[curr_e].face_ = f;
      }
      if (first_e == imesh_invalid_idx)
        first_e = curr_e;
      if (prev_e != imesh_invalid_idx)
        half_edges_[prev_e].next_ = curr_e;
      prev_e = curr_e;
    }
    if (prev_e != imesh_invalid_idx)
      half_edges_[prev_e].next_ = first_e;
    face_to_he_[f] = first_e;
  }

  vert_to_he_.resize(max_v+1, imesh_invalid_idx);

  // create half edges for boundaries
  for (unsigned int i=0; i<half_edges_.size(); ++i) {
    imesh_half_edge& he = half_edges_[i];
    if (i < vert_to_he_[he.vert_index()])
      vert_to_he_[he.vert_index()] = i;
    if (he.next_index() != imesh_invalid_idx)
      continue;
    unsigned int next_b = half_edges_[i].pair_index();
    while(half_edges_[next_b].face_index() != imesh_invalid_idx)
    {
      f_iterator fi(next_b,*this);
      while (fi->next_index() != next_b)
        ++fi;
      next_b = fi->pair_index();
    }
    he.next_ = next_b;
  }
}


//: Count the number of vertices pointed to by these edges
unsigned int
imesh_half_edge_set::num_verts() const
{
  unsigned int count = 0;
  for (unsigned int i : vert_to_he_)
    if (i != imesh_invalid_idx)
      ++count;
  return count;
}


//: Count the number of faces pointed to by these edges
unsigned int
imesh_half_edge_set::num_faces() const
{
  unsigned int count = 0;
  for (unsigned int i : face_to_he_)
    if (i != imesh_invalid_idx)
      ++count;
  return count;
}
