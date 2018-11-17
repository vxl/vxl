// This is gel/vtol/vtol_vertex.cxx
#include <iostream>
#include <algorithm>
#include "vtol_vertex.h"
//:
// \file

#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_face.h>
#include <vtol/vtol_macros.h>
#include <vtol/vtol_list_functions.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_vertex::~vtol_vertex()
{
  unlink_all_inferiors();
}

//******************************************************
//*
//*    Accessor Functions
//*

//: Returns a list of Vertices which only contains a pointer to itself.
std::vector<vtol_vertex*> *vtol_vertex::compute_vertices()
{
  LIST_SELF(vtol_vertex);
}

//: Returns a list of ZeroChains that contain the vertex. This is the vertex superiors list.
std::vector<vtol_zero_chain*>* vtol_vertex::compute_zero_chains()
{
  SEL_SUP(vtol_zero_chain,compute_zero_chains);
}


//: Returns a list of Edges which contain the vertex.
 std::vector<vtol_edge*>* vtol_vertex::compute_edges()
{
  SEL_SUP(vtol_edge,compute_edges);
}

//: Returns a list of OneChains which contain the vertex.
std::vector<vtol_one_chain*>* vtol_vertex::compute_one_chains()
{
  SEL_SUP(vtol_one_chain,compute_one_chains);
}

//: Returns a list of Faces which contain the vertex.
 std::vector<vtol_face*>* vtol_vertex::compute_faces()
{
  SEL_SUP(vtol_face,compute_faces);
}

//: Returns a list of TwoChains which contain the vertex.
  std::vector<vtol_two_chain*>* vtol_vertex::compute_two_chains()
{
  SEL_SUP(vtol_two_chain,compute_two_chains);
}

//: Returns a list of Blocks which contain the vertex.
std::vector<vtol_block*>* vtol_vertex::compute_blocks()
{
  SEL_SUP(vtol_block,compute_blocks);
}

//******************************************************
//*
//*    Print Functions
//*

//: This method outputs a simple text representation of the vertex including its address in memory.
void vtol_vertex::print(std::ostream &strm) const
{
  strm<< "<vtol_vertex " << (void const *)this<<"> with id "
      << get_id() << std::endl;
}


//: This method outputs a detailed description of the vertex including the inferiors and superiors.
void vtol_vertex::describe(std::ostream &strm,
                           int blanking) const
{
  for (int i=0; i<blanking; ++i) strm << ' ';
  print(strm);
  describe_inferiors(strm, blanking);
  describe_superiors(strm, blanking);
}

//******************************************************
//*
//*    Implementor Functions
//*

//-----------------------------------------------------------------------------
//: Is `this' connected with `v2' ?
//    ie has a superior of `this' `v2' as inferior ?
//-----------------------------------------------------------------------------
bool vtol_vertex::is_connected(vtol_vertex_sptr const& v2) const
{
  edge_list vertedges; edges(vertedges);
  for (edge_list::const_iterator i=vertedges.begin(); i!=vertedges.end(); ++i)
    if ((*i)->is_endpoint(v2))
      return true;
  return false;
}


//: This method returns true if edg is on the superior list of the vertex.
bool vtol_vertex::is_endpoint(vtol_edge_sptr const& e) const
{
  edge_list e_list; this->edges(e_list);
  return std::find(e_list.begin(),e_list.end(),e)!=e_list.end();
}

//---------------------------------------------------------------------------
//: Assignment of `this' with `other' (copy the point not the links)
//---------------------------------------------------------------------------
vtol_vertex &vtol_vertex::operator=(const vtol_vertex &other)
{
  if (this!=&other)
  {
    this->copy_geometry(other);
    // point_->set_x(other.point_->x());
    // point_->set_y(other.point_->y());

    touch();
  }
  return *this;
}

//: spatial object equality

bool vtol_vertex::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.cast_to_topology_object() &&
   obj.cast_to_topology_object()->cast_to_vertex() &&
   *this == *obj.cast_to_topology_object()->cast_to_vertex();
}


//---------------------------------------------------------------------------
//: Is `this' has the same coordinates for its point than `other' ?
//---------------------------------------------------------------------------
bool vtol_vertex::operator== (const vtol_vertex &other) const
{
  return this==&other || compare_geometry(other);
}

// ******************************************************
//
//    Functions
//

//
//-----------------------------------------------------------------------------
//:
// Determine which other vertices share edges with this. Add any of these which
// are not in the list to it, and recursively call explore_vertex on them. The
// method is intended to recover all of the vertices in a single topological
// structure which is composed of connected edges.
//
void vtol_vertex::explore_vertex(vertex_list &verts)
{
  // Note that "this" is not first put on the list:
  // it will be put as the second element, during the first recursive call.

  edge_list e_list; this->edges(e_list);
  for (const auto& e : e_list)
  {
    vtol_vertex_sptr vv;
    if (e->v1()==this)
      vv=e->v2();
    else if (e->v2()==this)
      vv=e->v1();
    else
    {
      std::cerr << "Explore vtol_vertex: shouldn't get this\n";
      assert(false);
      continue;
    }

    if (std::find(verts.begin(),verts.end(),vv)==verts.end())
    {
      verts.push_back(vv);
      vv->explore_vertex(verts);
    }
  }
}
