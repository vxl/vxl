// This is gel/vtol/vtol_zero_chain.cxx
#include "vtol_zero_chain.h"
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <vtol/vtol_macros.h>
#include <vtol/vtol_list_functions.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_vertex_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

void vtol_zero_chain::link_inferior(const vtol_vertex_sptr& inf)
{
  vtol_topology_object::link_inferior(inf->cast_to_topology_object());
}

void vtol_zero_chain::unlink_inferior(const vtol_vertex_sptr& inf)
{
  vtol_topology_object::unlink_inferior(inf->cast_to_topology_object());
}

void vtol_zero_chain::link_inferior(const vtol_vertex_2d_sptr& inf)
{
  vtol_topology_object::link_inferior(inf->cast_to_topology_object());
}

void vtol_zero_chain::unlink_inferior(const vtol_vertex_2d_sptr& inf)
{
  vtol_topology_object::unlink_inferior(inf->cast_to_topology_object());
}

//---------------------------------------------------------------------------
//: Constructor from two vertices (to make an edge creation easier)
// Require: v1!=v2
//---------------------------------------------------------------------------
vtol_zero_chain::vtol_zero_chain(vtol_vertex_sptr const& v1,
                                 vtol_vertex_sptr const& v2)
{
  // require
#if 0 // temporarily disabled
  assert(*v1!=*v2);
#endif // 0
  link_inferior(v1);
  link_inferior(v2);
}

vtol_zero_chain::vtol_zero_chain(vtol_vertex_2d_sptr const& v1,
                                 vtol_vertex_2d_sptr const& v2)
{
  // require
  assert(v1!=v2);
  link_inferior(v1);
  link_inferior(v2);
}

//---------------------------------------------------------------------------
//: Constructor from an array of vertices
// Require: new_vertices.size()>0
//---------------------------------------------------------------------------
vtol_zero_chain::vtol_zero_chain(const vertex_list &new_vertices)
{
  // require
  assert(new_vertices.size()>0);

  for (const auto & new_vertice : new_vertices)
    link_inferior(new_vertice);
}

//---------------------------------------------------------------------------
//: Pseudo copy constructor. Deep copy.
//---------------------------------------------------------------------------
vtol_zero_chain::vtol_zero_chain(vtol_zero_chain_sptr const& other)
{
  topology_list::const_iterator i;
  for (i=other->inferiors()->begin();i!=other->inferiors()->end();++i)
    link_inferior((*i)->clone()->cast_to_topology_object()->cast_to_vertex());
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_zero_chain::~vtol_zero_chain()
{
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vtol_zero_chain::clone() const
{
  return new vtol_zero_chain(vtol_zero_chain_sptr(const_cast<vtol_zero_chain*>(this)));
}

//---------------------------------------------------------------------------
//: Return the first vertex of `this'. If it does not exist, return 0
//---------------------------------------------------------------------------
vtol_vertex_sptr vtol_zero_chain::v0() const
{
  if (numinf()>0)
    return inferiors()->front()->cast_to_vertex();
  else
    return nullptr;
}

//: get list of vertices
std::vector<vtol_vertex*>* vtol_zero_chain::compute_vertices()
{
  COPY_INF(vertex);
}

//: get list of zero chains
std::vector<vtol_zero_chain*>* vtol_zero_chain::compute_zero_chains()
{
  LIST_SELF(vtol_zero_chain);
}

//: get list of edges
std::vector<vtol_edge*>* vtol_zero_chain::compute_edges()
{
  SEL_SUP(vtol_edge,compute_edges);
}

//: get list of one chains

std::vector<vtol_one_chain*>* vtol_zero_chain::compute_one_chains()
{
  SEL_SUP(vtol_one_chain, compute_one_chains);
}

//: get list of faces

std::vector<vtol_face*> *vtol_zero_chain::compute_faces()
{
  SEL_SUP(vtol_face, compute_faces);
}

//: get list of two chain

std::vector<vtol_two_chain*>* vtol_zero_chain::compute_two_chains()
{
  SEL_SUP(vtol_two_chain, compute_two_chains);
}

//: get list of blocks
std::vector<vtol_block*>* vtol_zero_chain::compute_blocks()
{
   SEL_SUP(vtol_block, compute_blocks);
}


// operators

bool vtol_zero_chain::operator==(const vtol_zero_chain &other) const
{
  if (this==&other) return true;

  // Check to see if the number of vertices is the same
  if (numinf()!=other.numinf())
    return false;

  const topology_list *inf1=inferiors();
  const topology_list *inf2=other.inferiors();
  topology_list::const_iterator i1;
  topology_list::const_iterator i2;

  for (i1=inf1->begin(),i2=inf2->begin(); i1!=inf1->end(); ++i1,++i2)
    if (!(*(*i1)==*(*i2))) return false;

  return true;
}


//: spatial object equality

bool vtol_zero_chain::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.cast_to_topology_object() &&
   obj.cast_to_topology_object()->cast_to_zero_chain() &&
   *this == *obj.cast_to_topology_object()->cast_to_zero_chain();
}

//*******  Print Methods   *************

//: print the object

void vtol_zero_chain::print(std::ostream &strm) const
{
  strm << "<vtol_zero_chain " << inferiors()->size() << ' ' << (void const*)this << ">\n";
}

void vtol_zero_chain::describe(std::ostream &strm,
                               int blanking) const
{
  for (int j=0; j<blanking; ++j)
    strm << ' ';
  print(strm);
  describe_inferiors(strm, blanking);
  describe_superiors(strm, blanking);
}
