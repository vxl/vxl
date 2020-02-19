// This is gel/vtol/vtol_face.cxx
#include "vtol_face.h"
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <vtol/vtol_macros.h>
#include <vtol/vtol_two_chain.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_list_functions.h>

void vtol_face::link_inferior(const vtol_one_chain_sptr& inf)
{
  vtol_topology_object::link_inferior(inf->cast_to_topology_object());
}

void vtol_face::unlink_inferior(const vtol_one_chain_sptr& inf)
{
  vtol_topology_object::unlink_inferior(inf->cast_to_topology_object());
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_face::~vtol_face()
{
  unlink_all_inferiors();
}

//:
// Returns an ordered list of vertices of the outside boundary of the
// face.  All vertices on any holes of the face are \e not included.
// This vertex list is ordered such that a positive normal is
// computing using the Right Hand rule in the direction of the vertex
// list.

vertex_list *vtol_face::outside_boundary_vertices() {
  auto *new_ref_list = new vertex_list;
  std::vector<vtol_vertex*>* ptr_list = this->outside_boundary_compute_vertices();

  // copy the lists
  for (std::vector<vtol_vertex*>::const_iterator ti = ptr_list->begin();
       ti != ptr_list->end(); ++ti)
    new_ref_list->push_back(*ti);

  delete ptr_list;
  return new_ref_list;
}

std::vector<vtol_vertex *> *vtol_face::outside_boundary_compute_vertices() {
  OUTSIDE_BOUNDARY(vtol_vertex,one_chain,compute_vertices);
}

//:
// Returns a vtol_vertex list of all the vertices on the face.
// If the face does not have any holes, this vertex list is ordered
// in the direction of a positive normal using the Right Hand rule.

std::vector<vtol_vertex *> *vtol_face::compute_vertices() {
  SEL_INF(vtol_vertex,compute_vertices);
}

//:
// Returns a list of the zero_chains on the outside boundary of the face.
// All zero_chains on any hole boundaries of the face are \e not included.

std::vector<vtol_zero_chain *> *
vtol_face::outside_boundary_compute_zero_chains() {
  OUTSIDE_BOUNDARY(vtol_zero_chain,one_chain,compute_zero_chains);
}

//:
// Returns a list of the zero_chains on the outside boundary of the face.
// All zero_chains on any hole boundaries of the face are \e not included.

zero_chain_list *vtol_face::outside_boundary_zero_chains() {
  auto *new_ref_list = new zero_chain_list;
  std::vector<vtol_zero_chain*>* ptr_list = this->outside_boundary_compute_zero_chains();

  // copy the lists
  for (std::vector<vtol_zero_chain*>::const_iterator ti = ptr_list->begin();
       ti != ptr_list->end(); ++ti)
    new_ref_list->push_back(*ti);

  delete ptr_list;
  return new_ref_list;
}

//: Returns a list of zero_chains of the face.

std::vector<vtol_zero_chain *> *vtol_face::compute_zero_chains() {
  SEL_INF(vtol_zero_chain,compute_zero_chains);
}

//:
// Returns a list of edges that make up the outside boundary of the
// face. All edges on any hole boundaries are \e not included.

std::vector<vtol_edge *> *vtol_face::outside_boundary_compute_edges() {
  OUTSIDE_BOUNDARY(vtol_edge,one_chain,compute_edges);
}

//---------------------------------------------------------------------------
//: Get the outside boundary edges
//---------------------------------------------------------------------------
edge_list *vtol_face::outside_boundary_edges() {
  auto *new_ref_list = new edge_list;
  std::vector<vtol_edge*>* ptr_list = this->outside_boundary_compute_edges();

  // copy the lists
  for (std::vector<vtol_edge*>::const_iterator ti = ptr_list->begin();
       ti != ptr_list->end(); ++ti)
    new_ref_list->push_back(*ti);

  delete ptr_list;
  return new_ref_list;
}

//: Returns a list of edges on the face.
std::vector<vtol_edge *> *vtol_face::compute_edges() {
  SEL_INF(vtol_edge,compute_edges);
}

//: Returns a list of one_chains that make up the outside boundary of the face.

one_chain_list *vtol_face::outside_boundary_one_chains() {
  std::vector<vtol_one_chain*>* ptr_list= outside_boundary_compute_one_chains();
  auto *ref_list= new one_chain_list;

  for (std::vector<vtol_one_chain*>::const_iterator i=ptr_list->begin();
       i!=ptr_list->end(); ++i)
    ref_list->push_back(*i);

  delete ptr_list;
  return ref_list;
}

std::vector<vtol_one_chain *> *
vtol_face::outside_boundary_compute_one_chains() {
  COPY_INF(one_chain);
}

//: Returns a list of all Onechains of the face.

std::vector<vtol_one_chain *> *vtol_face::compute_one_chains() {
  SEL_INF(vtol_one_chain,compute_one_chains);
}

//:
// Returns a list of that has itself as the only element.  This method
// is needed for traversing the model hierarchy consistently.

std::vector<vtol_face *> *vtol_face::compute_faces() { LIST_SELF(vtol_face); }

//: Returns a list of all the two_chains which contain the vtol_face.
std::vector<vtol_two_chain *> *vtol_face::compute_two_chains() {
  SEL_SUP(vtol_two_chain,compute_two_chains);
}

//: Returns a list of all the blocks that contain the vtol_face.

std::vector<vtol_block *> *vtol_face::compute_blocks() {
  SEL_SUP(vtol_block,compute_blocks);
}

//---------------------------------------------------------------------------
//: Does `this' share an edge with `f' ?
//  Comparison of edge pointers, not geometric values
//---------------------------------------------------------------------------
bool vtol_face::shares_edge_with(vtol_face_sptr const& f)
{
  edge_list thised; this->edges(thised);
  edge_list fedges; f->edges(fedges);
  for (edge_list::const_iterator ei1=thised.begin(); ei1!=thised.end(); ++ei1)
    for (edge_list::const_iterator ei2=fedges.begin(); ei2!=fedges.end(); ++ei2)
      if ((*ei1)==(*ei2)) return true;
  return false;
}

//:
// Links new_vtol_one_chain as an inferior of the vtol_face and returns True if
// successful. This method will be replacing all calls to add_edge_loop().

void vtol_face::add_one_chain(vtol_one_chain_sptr const& new_vtol_one_chain)
{
  // require
  assert(new_vtol_one_chain->contains_sub_chains());

  link_inferior(new_vtol_one_chain);
}

#if 1 // deprecated
void vtol_face::add_one_chain(vtol_one_chain &new_vtol_one_chain)
{
  std::cerr << "Warning: deprecated form of vtol_face::add_one_chain()\n";
  assert(new_vtol_one_chain.contains_sub_chains());

  link_inferior(&new_vtol_one_chain);
}
#endif

//: deep equality check on faces.  uses fuzzy equal on vertices.
//

bool vtol_face::operator==(const vtol_face &other) const
{
  if (this==&other) return true;

  if (numinf()!=other.numinf())
    return false;

  if (!compare_geometry(other))
    return false;

  topology_list::const_iterator ti1;
  topology_list::const_iterator ti2;

  for (ti1=inferiors()->begin(),ti2=other.inferiors()->begin(); ti1!=inferiors()->end(); ++ti1,++ti2)
      if (!(*(*ti1)== *(*ti2)))
        return false;

  return true;
}

//---------------------------------------------------------------------------
//: Spatial object equality
//---------------------------------------------------------------------------
bool vtol_face::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.cast_to_topology_object() &&
   obj.cast_to_topology_object()->cast_to_face() &&
   *this == *obj.cast_to_topology_object()->cast_to_face();
}

//: Returns the ith inferior vtol_one_chain of the vtol_face.

vtol_one_chain_sptr vtol_face::get_one_chain(int which)
{
  assert((unsigned int)which < inferiors()->size());
  if ((unsigned int)which < inferiors()->size())
    return (inferiors_[which])->cast_to_one_chain();
  else
  {
    std::cerr << "Tried to get bad edge_loop from face\n";
    return nullptr;
  }
}

//: Returns the first inferior vtol_one_chain of the vtol_face (the boundary onechain).

vtol_one_chain_sptr vtol_face::get_boundary_cycle() {
  // The outside boundary vtol_one_chain will *always*
  // be the first one chain in the inferiors
  // list.
  return get_one_chain(0);
}

//---------------------------------------------------------------------------
//: Adds a new hole to the face
//---------------------------------------------------------------------------
bool vtol_face::add_hole_cycle(vtol_one_chain_sptr new_hole)
{
  vtol_one_chain_sptr onech=get_boundary_cycle();

  if (onech)
  {
    onech->link_chain_inferior(new_hole);
    return true;
  }
  else
    return false;
}

// Returns a list of the one_chains that make up the holes of the vtol_face.

one_chain_list *vtol_face::get_hole_cycles() {
  auto * result=new one_chain_list;

  topology_list::const_iterator ii;
  for (ii=inferiors()->begin();ii!=inferiors()->end();++ii)
  {
    one_chain_list* templist=(*ii)->cast_to_one_chain()->inferior_one_chains();

    for (one_chain_list::const_iterator oi=templist->begin();oi!=templist->end();++oi)
      result->push_back(*oi);
    delete templist;
  }

  return result;
}

//: Returns the number of edges on the vtol_face.
//

int vtol_face::get_num_edges() const {
  int result=0;
  topology_list::const_iterator ii;
  for (ii=inferiors()->begin();ii!=inferiors()->end();++ii)
    result+=((*ii)->cast_to_one_chain())->numinf();
  return result;
}

//---------------------------------------------------------------------------
//: Reverse the direction of the face
//---------------------------------------------------------------------------
void vtol_face::reverse_normal() {
  topology_list::const_iterator ti;
  for (ti=inferiors()->begin();ti!=inferiors()->end();++ti)
    (*ti)->cast_to_one_chain()->reverse_directions();
  // compute_normal();
}

//:
//  This method describes the data members of the vtol_face including the
// Inferiors.  The blanking argument is used to indent the output in
// a clear fashion.

void vtol_face::describe(std::ostream &strm,
                         int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  print();
  for (unsigned int i=0;i<inferiors()->size();++i)
  {
    if ((inferiors_[i])->cast_to_one_chain()!=nullptr)
      (inferiors_[i])->cast_to_one_chain()->describe(strm,blanking);
    else
      std::cout << "*** Odd inferior for a face\n";
  }
}

//:
// This method prints out a simple text representation for the vtol_face which
// includes its address in memory.
void vtol_face::print(std::ostream &strm) const
{
  strm << "<vtol_face ";

  topology_list::const_iterator ii;
  for (ii=inferiors()->begin();ii!= inferiors()->end();++ii)
  {
    strm << ' ' << (*ii)->inferiors()->size();
  }
  strm << "   " << (void const *) this << ">\n";
}

//-------------------------------------------------------------
//: Update the bounding box, a member of vsol_spatial_object_2d.
//  The algorithm uses the bounding boxes of the vtol_edge(s) forming
//  the boundary of the face.
void vtol_face::compute_bounding_box() const
{
  this->empty_bounding_box();
  edge_list edges; this->edges(edges);
  for (auto & edge : edges)
    this->add_to_bounding_box(edge->get_bounding_box());
}

//: This method determines if a vtol_face is a hole of another vtol_face.
bool vtol_face::IsHoleP() const
{
  edge_list* edges = const_cast<vtol_face*>(this)->outside_boundary_edges();
  if (edges->size() == 0) { delete edges; return false; }
  vtol_edge_sptr e = edges->front();
  delete edges;
  std::list<vtol_topology_object*> const* chains = e->superiors_list();
  for (auto chain : *chains)
    if (chain->cast_to_one_chain()->numsup() > 0)
      return true;
  return false;
}
