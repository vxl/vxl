// This is gel/vtol/vtol_block.cxx
#include "vtol_block.h"
//:
// \file

#include <vtol/vtol_edge.h>
#include <vtol/vtol_two_chain.h>
#include <vtol/vtol_macros.h>
#include <vtol/vtol_list_functions.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>

//***************************************************************************
// Initialization
//***************************************************************************

void vtol_block::link_inferior(vtol_two_chain_sptr inf)
{
  vtol_topology_object::link_inferior(inf->cast_to_topology_object());
}

void vtol_block::unlink_inferior(vtol_two_chain_sptr inf)
{
  vtol_topology_object::unlink_inferior(inf->cast_to_topology_object());
}

//---------------------------------------------------------------------------
//: Constructor from a two-chain (inferior)
//---------------------------------------------------------------------------
vtol_block::vtol_block(vtol_two_chain_sptr const& faceloop)
{
  link_inferior(faceloop);
}

//---------------------------------------------------------------------------
//: Constructor from a list of two-chains.
// The first two-chain is the outside boundary.
// The remaining two-chains are considered holes inside the outside volume.
//---------------------------------------------------------------------------
vtol_block::vtol_block(two_chain_list const& faceloops)
{
  if (faceloops.size()>0)
  {
    link_inferior(faceloops.front());
  }

  vtol_two_chain_sptr twoch=get_boundary_cycle();

  if (twoch)
    for (unsigned int i=1;i<faceloops.size();++i)
      twoch->link_chain_inferior(faceloops[i]);
}

//---------------------------------------------------------------------------
//: Constructor from a list of faces
//---------------------------------------------------------------------------
vtol_block::vtol_block(face_list const& new_face_list)
{
  link_inferior(new vtol_two_chain(new_face_list));
}

//---------------------------------------------------------------------------
//: Pseudo-copy constructor. Deep copy.
//---------------------------------------------------------------------------
vtol_block::vtol_block(vtol_block_sptr const& other)
{
  edge_list edgs;    other->edges(edgs);
  vertex_list verts; other->vertices(verts);

  topology_list newedges(edgs.size());
  topology_list newverts(verts.size());

  int i=0;
  for (vertex_list::iterator vi=verts.begin();vi!=verts.end();++vi,++i)
  {
    vtol_vertex_sptr v= *vi;
    newverts[i]=v->clone()->cast_to_topology_object();
    v->set_id(i);
  }

  int j=0;
  for (edge_list::iterator ei=edgs.begin();ei!=edgs.end();++ei,++j)
  {
    vtol_edge_sptr e = *ei;

    newedges[j]=newverts[e->v1()->get_id()]->cast_to_vertex()->new_edge(
                              newverts[e->v2()->get_id()]->cast_to_vertex())->cast_to_topology_object();

    e->set_id(j);
  }

  const topology_list *old2chains = other->inferiors();

  topology_list::const_iterator tci;
  for (tci=old2chains->begin();tci != old2chains->end();tci++)
  {
    vtol_two_chain_sptr new2ch=(*tci)->cast_to_two_chain()->copy_with_arrays(newverts,newedges);
    assert(*new2ch == *(*tci));
    link_inferior(new2ch);
  }
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_block::~vtol_block()
{
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vtol_block::clone() const
{
  return new vtol_block(vtol_block_sptr(const_cast<vtol_block*>(this)));
}

//: outside boundary vertices

vertex_list *vtol_block::outside_boundary_vertices()
{
  vertex_list *result=new vertex_list();
  vcl_vector<vtol_vertex *> *ptr_list=outside_boundary_compute_vertices();

  // copy the lists
  vcl_vector<vtol_vertex*>::iterator i;
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;

  return result;
}

vcl_vector<vtol_vertex *> *vtol_block::outside_boundary_compute_vertices()
{
  OUTSIDE_BOUNDARY(vtol_vertex,two_chain,compute_vertices);
}

//: get vertex list

vcl_vector<vtol_vertex *> *vtol_block::compute_vertices()
{
  SEL_INF(vtol_vertex,compute_vertices);
}

//: get outside boundary zero chains

zero_chain_list *vtol_block::outside_boundary_zero_chains()
{
  zero_chain_list *result=new zero_chain_list();
  vcl_vector<vtol_zero_chain *> *ptr_list=outside_boundary_compute_zero_chains();

  // copy the lists
  vcl_vector<vtol_zero_chain*>::iterator i;
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;

  return result;
}


vcl_vector<vtol_zero_chain *> *
vtol_block::outside_boundary_compute_zero_chains()
{
  OUTSIDE_BOUNDARY(vtol_zero_chain,two_chain,compute_zero_chains);
}

//: get zero chains

vcl_vector<vtol_zero_chain *> *vtol_block::compute_zero_chains()
{
 SEL_INF(vtol_zero_chain,compute_zero_chains);
}

//: outside boundary edges

edge_list *vtol_block::outside_boundary_edges()
{
  edge_list *result=new edge_list();
  vcl_vector<vtol_edge *> *ptr_list=outside_boundary_compute_edges();

  // copy the lists
  vcl_vector<vtol_edge*>::iterator i;
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;

  return result;
}

//: outside boundary edges

vcl_vector<vtol_edge *> *vtol_block::outside_boundary_compute_edges()
{
  OUTSIDE_BOUNDARY(vtol_edge,two_chain,compute_edges);
}

//: get edges
vcl_vector<vtol_edge *> *vtol_block::compute_edges()
{
  SEL_INF(vtol_edge,compute_edges);
}

//: get outside boundary one chains

one_chain_list *vtol_block::outside_boundary_one_chains()
{
  one_chain_list *result=new one_chain_list;
  vcl_vector<vtol_one_chain *> *ptr_list=outside_boundary_compute_one_chains();

  vcl_vector<vtol_one_chain*>::iterator i;
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;

  return result;
}

//: get outside boundary one chains

vcl_vector<vtol_one_chain *> *
vtol_block::outside_boundary_compute_one_chains()
{
  OUTSIDE_BOUNDARY(vtol_one_chain,two_chain,compute_one_chains);
}

//: get the one chains
vcl_vector<vtol_one_chain *> *vtol_block::compute_one_chains()
{
  SEL_INF(vtol_one_chain,compute_one_chains);
}

//: get the outside boundary faces

face_list *vtol_block::outside_boundary_faces()
{
  face_list *result=new face_list();
  vcl_vector<vtol_face *> *ptr_list=outside_boundary_compute_faces();

  vcl_vector<vtol_face*>::iterator i;
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;

  return result;
}

//: get the outside boundary faces

vcl_vector<vtol_face *> *vtol_block::outside_boundary_compute_faces()
{
  OUTSIDE_BOUNDARY(vtol_face,two_chain,compute_faces);
}

//: get the faces
vcl_vector<vtol_face *> *vtol_block::compute_faces()
{
  SEL_INF(vtol_face,compute_faces);
}

//: get the outside boundary two chains

two_chain_list *vtol_block::outside_boundary_two_chains()
{
  two_chain_list *result=new two_chain_list();
  vcl_vector<vtol_two_chain *> *ptr_list=outside_boundary_compute_two_chains();

  vcl_vector<vtol_two_chain*>::iterator i;
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;

  return result;
}

//: get the outside boundary two chains

vcl_vector<vtol_two_chain *> *
vtol_block::outside_boundary_compute_two_chains()
{
  OUTSIDE_BOUNDARY(vtol_two_chain,two_chain,compute_two_chains);
}

//: get the two chains

vcl_vector<vtol_two_chain *> *vtol_block::compute_two_chains()
{
  SEL_INF(vtol_two_chain,compute_two_chains);
}

//: get blocks
vcl_vector<vtol_block *> *vtol_block::compute_blocks()
{
  LIST_SELF(vtol_block);
}


// ******************************************************
//
//    Operators Functions
//

// This is hardly an equality test...but we`ll leave it for now....pav
// June 1992.

bool vtol_block::operator==(const vtol_block &other) const
{
  if (this==&other)
    return true;

  if (numinf()!=other.numinf())
    return false;

  topology_list::const_iterator bi1=inferiors()->begin();
  topology_list::const_iterator bi2=other.inferiors()->begin();
  for (; bi1!=inferiors()->end(); ++bi1,++bi2)
  {
    vtol_two_chain_sptr twoch1=(*bi1)->cast_to_two_chain();
    vtol_two_chain_sptr twoch2=(*bi2)->cast_to_two_chain();

    if (!(*twoch1 == *twoch2))
      return false;
  }

  return true;
}

//: spatial object equality

bool vtol_block::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.cast_to_topology_object() &&
   obj.cast_to_topology_object()->cast_to_block() &&
   *this == *obj.cast_to_topology_object()->cast_to_block();
}

//: get a hole cycle
two_chain_list *vtol_block::hole_cycles() const
{
  two_chain_list *result=new two_chain_list;

  topology_list::const_iterator ti;
  for (ti=inferiors_.begin();ti!=inferiors_.end();++ti)
  {
    two_chain_list *templist=(*ti)->cast_to_two_chain()->inferior_two_chains();
    for (two_chain_list::iterator ii=templist->begin();ii!=templist->end();++ii)
      result->push_back(*ii);
    delete templist;
  }
  return result;
}

//: get the boundary cycle
vtol_two_chain_sptr vtol_block::get_boundary_cycle()
{
  return
    (inferiors_.size() > 0)
    ? inferiors_[0]->cast_to_two_chain()
    : 0;
}

//: add a hole cycle

bool vtol_block::add_hole_cycle(vtol_two_chain_sptr new_hole)
{
  vtol_two_chain_sptr twoch=get_boundary_cycle();
  if (! twoch) return false;
  twoch->link_chain_inferior(new_hole);
  return true;
}

// ******************************************************
//
//    Print Functions
//


//: print data

void vtol_block::print(vcl_ostream &strm) const
{
  strm<<"<vtol_block "<<inferiors()->size()<<"  "<<(void const*)this<<">\n";
}

void vtol_block::describe(vcl_ostream &strm,
                          int blanking) const
{
  for (int i=0; i<blanking; ++i) strm << ' ';
  print(strm);
  describe_inferiors(strm,blanking);
  describe_superiors(strm,blanking);
}
