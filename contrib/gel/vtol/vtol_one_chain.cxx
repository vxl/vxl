// This is gel/vtol/vtol_one_chain.cxx
#include <iostream>
#include <algorithm>
#include "vtol_one_chain.h"
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_macros.h>
#include <vtol/vtol_list_functions.h>

vtol_edge_sptr vtol_one_chain::edge(int i) const
{
  assert (i>=0);
  assert (i<numinf());
  return inferiors_[i]->cast_to_edge();
}

//***************************************************************************
// Initialization
//***************************************************************************

void vtol_one_chain::link_inferior(const vtol_edge_sptr& inf)
{
  vtol_topology_object::link_inferior(inf->cast_to_topology_object());
}

void vtol_one_chain::unlink_inferior(const vtol_edge_sptr& inf)
{
  vtol_topology_object::unlink_inferior(inf->cast_to_topology_object());
}

void vtol_one_chain::link_chain_inferior(const vtol_one_chain_sptr& chain_inferior)
{
  vtol_chain::link_chain_inferior(chain_inferior->cast_to_chain());
}

void vtol_one_chain::unlink_chain_inferior(const vtol_one_chain_sptr& chain_inferior)
{
  vtol_chain::unlink_chain_inferior(chain_inferior->cast_to_chain());
}

//---------------------------------------------------------------------------
//: Constructor from an array of edges
//---------------------------------------------------------------------------
vtol_one_chain::vtol_one_chain(edge_list const& edgs,
                               bool new_is_cycle)
{
  set_cycle(new_is_cycle);

  for (const auto & edg : edgs)
  {
    link_inferior(edg);
    directions_.push_back((signed char)1);
  }

  if (is_cycle())
    determine_edge_directions();
}

//---------------------------------------------------------------------------
//: Constructor from an array of edges and an array of directions
//---------------------------------------------------------------------------
vtol_one_chain::vtol_one_chain(edge_list const& edgs,
                               std::vector<signed char> const& dirs,
                               bool new_is_cycle)
{
  set_cycle(new_is_cycle);

  for (const auto & edg : edgs)
    link_inferior(edg);

  for (signed char dir : dirs)
    directions_.push_back(dir);
}

//---------------------------------------------------------------------------
//: Pseudo copy constructor.  Deep copy.
//---------------------------------------------------------------------------
vtol_one_chain::vtol_one_chain(vtol_one_chain_sptr const& other)
{
  vertex_list verts; other->vertices(verts);
  topology_list newverts(verts.size());

  int i=0;
  for (auto v=verts.begin();v!=verts.end();++v,++i)
  {
    vtol_vertex_sptr ve=*v;
    newverts[i]=ve->clone()->cast_to_topology_object();
    ve->set_id(i);
  }

  auto dir=other->directions_.begin();
  auto inf=other->inferiors()->begin();
  for (; dir!=other->directions_.end(); ++dir,++inf)
  {
    vtol_edge_sptr e=(*inf)->cast_to_edge();
    vtol_edge_sptr newedge = newverts[e->v1()->get_id()]->cast_to_vertex()->new_edge(
                             newverts[e->v2()->get_id()]->cast_to_vertex());
    link_inferior(newedge);
    directions_.push_back(*dir);
  }
  set_cycle(other->is_cycle());
  const chain_list *hierarchy_infs=other->chain_inferiors();

  for (const auto & hierarchy_inf : *hierarchy_infs)
    link_chain_inferior(hierarchy_inf->clone()->cast_to_topology_object()->cast_to_one_chain());
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_one_chain::~vtol_one_chain()
{
  unlink_all_chain_inferiors();
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization.
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vtol_one_chain::clone() const
{
  return new vtol_one_chain(vtol_one_chain_sptr(const_cast<vtol_one_chain*>(this)));
}

vtol_one_chain *
vtol_one_chain::copy_with_arrays(topology_list &verts,
                                 topology_list &edges) const
{
  auto *result=new vtol_one_chain();

  auto di=directions_.begin();
  auto ti=inferiors()->begin();
  for (; ti!=inferiors()->end(); ++ti,++di)
  {
    vtol_edge *e=(*ti)->cast_to_edge();
    vtol_edge *newedge=edges[e->get_id()]->cast_to_edge();

    assert(*e == *newedge);

    result->link_inferior(newedge);
    result->directions_.push_back((*di));
  }
  result->set_cycle(is_cycle());
  const chain_list *hierarchy_infs=chain_inferiors();

  for (const auto & hierarchy_inf : *hierarchy_infs)
  {
    vtol_one_chain_sptr tch = hierarchy_inf->cast_to_one_chain();
    int n = tch->num_edges();
    vsol_spatial_object_2d_sptr so = hierarchy_inf->clone();
    vtol_one_chain *temp = so->cast_to_topology_object()->cast_to_one_chain();
    // we have to set the ids here because the clone operation does not
    // copy the id field on vsol_spatial_object_2d
    for (int i=0; i<n; ++i)
    {
      vtol_edge_sptr e = temp->edge(i);
      e->set_id(tch->edge(i)->get_id());
    }
    result->link_chain_inferior(temp->copy_with_arrays(verts,edges));
  }

  assert(*result == *this);

  return result;
}

// ***********************************
//         Accessors
//
// **********************************

//: Get the direction of the edge "e" in the onechain.
signed char vtol_one_chain::direction(vtol_edge const &e) const
{
  auto dit=directions_.begin();
  topology_list::const_iterator toit;
  for (toit=inferiors()->begin();toit!=inferiors()->end();++toit)
  {
    vtol_edge *ce=(*toit)->cast_to_edge();
    if (ce==&e)
      return *dit;
    ++dit;
  }
  return (signed char)1;
}

//---------------------------------------------------------------------------
//: Get the outside boundary vertices
//---------------------------------------------------------------------------
std::vector<vtol_vertex*> *
vtol_one_chain::outside_boundary_compute_vertices()
{
  auto *result=new std::vector<vtol_vertex*>;

  auto inf=inferiors()->begin();
  auto dir=directions_.begin();
  for (; inf!=inferiors()->end(); ++inf,++dir)
  {
    vtol_edge_sptr e=(*inf)->cast_to_edge();
    if ((*dir)< 0)
    {
      result->push_back(e->v2()->cast_to_vertex());
      result->push_back(e->v1()->cast_to_vertex());
    }
    else
    {
      result->push_back(e->v1()->cast_to_vertex());
      result->push_back(e->v2()->cast_to_vertex());
    }
  }
  return tagged_union(result);
}

//---------------------------------------------------------------------------
//: Get the outside boundary vertices
//---------------------------------------------------------------------------
vertex_list *vtol_one_chain::outside_boundary_vertices()
{
  std::vector<vtol_vertex*> *tmp_list=outside_boundary_compute_vertices();
  auto *result=new vertex_list;
  result->reserve(tmp_list->size());
  std::vector<vtol_vertex*>::iterator i;
  for (i=tmp_list->begin();i!=tmp_list->end();++i)
    result->push_back(*i);
  delete tmp_list;
  return result;
}

//---------------------------------------------------------------------------
//: Get the vertices of this object
//---------------------------------------------------------------------------
std::vector<vtol_vertex*> *vtol_one_chain::compute_vertices()
{
  // We must collect vertices from subchains as well as
  // from direct Inferiors...so this function only has
  // an ordering if there are no subchains.

  std::vector<vtol_vertex*> *verts=outside_boundary_compute_vertices();

  // This macro adds the subchain vertices to the verts list.

  SUBCHAIN_INF(verts,one_chain,vtol_vertex,compute_vertices);
}


//---------------------------------------------------------------------------
//: Get the outside boundary zero chains
//---------------------------------------------------------------------------
std::vector<vtol_zero_chain *> *
vtol_one_chain::outside_boundary_compute_zero_chains()
{
  SEL_INF(vtol_zero_chain,compute_zero_chains);
}

//---------------------------------------------------------------------------
//: Get the outside boundary zero chains
//---------------------------------------------------------------------------
zero_chain_list *vtol_one_chain::outside_boundary_zero_chains()
{
  auto *result=new zero_chain_list;
  std::vector<vtol_zero_chain *> *ptr_list=outside_boundary_compute_zero_chains();

  // copy the lists
  std::vector<vtol_zero_chain *>::iterator i;
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    if ((*i)->v0()) // PVr- added this condition so no empty chains are returned
      result->push_back(*i);
  delete ptr_list;

  return result;
}


//---------------------------------------------------------------------------
//: Get the zero chains of this object
//---------------------------------------------------------------------------
std::vector<vtol_zero_chain*> *vtol_one_chain::compute_zero_chains()
{
  std::vector<vtol_zero_chain*> *zchs;
  zchs=outside_boundary_compute_zero_chains();

  // This macro adds the subchain zerochains to the zchs list.

  SUBCHAIN_INF(zchs,one_chain,vtol_zero_chain,compute_zero_chains);
}

//---------------------------------------------------------------------------
//: Get the outside boundary edges
//---------------------------------------------------------------------------
std::vector<vtol_edge*> *vtol_one_chain::outside_boundary_compute_edges()
{
  COPY_INF(edge);
}


//---------------------------------------------------------------------------
//: Get the outside boundary edges
//---------------------------------------------------------------------------
edge_list *vtol_one_chain::outside_boundary_edges()
{
  auto *new_ref_list = new edge_list;
  std::vector<vtol_edge*>* ptr_list = this->outside_boundary_compute_edges();
  // copy the lists

  for (auto & ti : *ptr_list)
    new_ref_list->push_back(ti);

  delete ptr_list;
  return new_ref_list;
}

//---------------------------------------------------------------------------
//: Get the edges
//---------------------------------------------------------------------------
std::vector<vtol_edge*> *vtol_one_chain::compute_edges()
{
  std::vector<vtol_edge*> *edges;
  edges=outside_boundary_compute_edges();

  // This macro adds the subchain zerochains to the zchs list.

  SUBCHAIN_INF(edges,one_chain,vtol_edge,compute_edges);
}

//---------------------------------------------------------------------------
//: Get the one chains
//---------------------------------------------------------------------------
std::vector<vtol_one_chain*> *vtol_one_chain::compute_one_chains()
{
  std::vector<vtol_one_chain*> *result=outside_boundary_compute_one_chains();

  for (auto & chain_inferior : chain_inferiors_)
  {
    vsol_spatial_object_2d_sptr so = chain_inferior->clone();
    //==============================================================
    //compensate for leaving scope.  The clone method returns a smart
    //pointer, but the output in this method is a bare pointer.  Thus
    //so will be deleted when the routine leaves scope.  The calling
    //routine for compute_one_chains will push these one_chains on a list
    //of smart pointers so there should be no leak.  This routine never
    //worked -- JLM
    so->ref();
    //===============================================================
    vtol_one_chain* sub_chain = so->cast_to_topology_object()->
      cast_to_one_chain();
    result->push_back(sub_chain);
  }
  return result;
}

//---------------------------------------------------------------------------
//: Get the inferior one chains
//---------------------------------------------------------------------------
one_chain_list *vtol_one_chain::inferior_one_chains()
{
  auto *result=new one_chain_list;

  for (auto & chain_inferior : chain_inferiors_)
    result->push_back(chain_inferior->clone()->cast_to_topology_object()->cast_to_one_chain());

  return result;
}

//---------------------------------------------------------------------------
//: Get the superior one chains
//---------------------------------------------------------------------------
one_chain_list *vtol_one_chain::superior_one_chains()
{
  auto *result=new one_chain_list;

  std::list<vtol_chain*>::iterator i;
  for (i=chain_superiors_.begin();i!=chain_superiors_.end();++i)
    result->push_back((*i)->clone()->cast_to_topology_object()->cast_to_one_chain());

  return result;
}

//---------------------------------------------------------------------------
//: Get the outside boundary one chains
//---------------------------------------------------------------------------
one_chain_list *vtol_one_chain::outside_boundary_one_chains()
{
  std::vector<vtol_one_chain*>* ptr_list= outside_boundary_compute_one_chains();
  auto *ref_list= new one_chain_list;

  std::vector<vtol_one_chain*>::iterator i;
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
  {
    ref_list->push_back(*i);
  }
  delete ptr_list;
  return ref_list;
}


//---------------------------------------------------------------------------
//: Get the outside boundary one chains
//---------------------------------------------------------------------------
std::vector<vtol_one_chain*> *vtol_one_chain::outside_boundary_compute_one_chains()
{
  LIST_SELF(vtol_one_chain);
}

//---------------------------------------------------------------------------
//: Get the faces
//---------------------------------------------------------------------------
std::vector<vtol_face*> *vtol_one_chain::compute_faces()
{
  if (is_sub_chain())
  {
    auto *result=new std::vector<vtol_face*>();
    one_chain_list *onech=superior_one_chains();

    for (auto & i : *onech)
    {
      std::vector<vtol_face*> *sublist=i->compute_faces();
      std::vector<vtol_face*>::iterator ii;
      for (ii=sublist->begin();ii!=sublist->end();++ii)
        result->push_back(*ii);
      delete sublist;
    }
    delete onech;
    return tagged_union(result);
  }
  else
  {
    SEL_SUP(vtol_face, compute_faces);
  }
}

//---------------------------------------------------------------------------
//: Get the two chains
//---------------------------------------------------------------------------
std::vector<vtol_two_chain*> *vtol_one_chain::compute_two_chains()
{
  if (is_sub_chain())
  {
    auto *result=new std::vector<vtol_two_chain*>;
    one_chain_list *onech=superior_one_chains();

    for (auto & i : *onech)
    {
      std::vector<vtol_two_chain*> *sublist=i->compute_two_chains();
      std::vector<vtol_two_chain*>::iterator ii;
      for (ii=sublist->begin();ii!=sublist->end();++ii)
        result->push_back(*ii);
      delete sublist;
    }
    delete onech;
    return tagged_union(result);
  }
  else
  {
    SEL_SUP(vtol_two_chain, compute_two_chains);
  }
}

//---------------------------------------------------------------------------
//: Get the blocks
//---------------------------------------------------------------------------
std::vector<vtol_block*> *vtol_one_chain::compute_blocks()
{
  if (is_sub_chain())
  {
    auto *result=new std::vector<vtol_block*>;
    one_chain_list *onech=superior_one_chains();
    for (auto & i : *onech)
    {
      std::vector<vtol_block*> *sublist=i->compute_blocks();
      std::vector<vtol_block*>::iterator ii;
      for (ii=sublist->begin();ii!=sublist->end();++ii)
        result->push_back(*ii);
      delete sublist;
    }
    delete onech;
    return tagged_union(result);
  }
  else
  {
    SEL_SUP(vtol_block, compute_blocks);
  }
}

//---------------------------------------------------------------------------
//: Computes the bounding box of a vtol_one_chain from the edges.
//  Just get the bounding box for each edge and update this's box accordingly.
//  Note that the computation is done independently of dimension.
//---------------------------------------------------------------------------

void vtol_one_chain::compute_bounding_box() const
{
  // we need to clear the bounds of the box to correctly reflect edge bounds
  this->empty_bounding_box();

  edge_list edgs; this->edges(edgs);
  for (auto & edg : edgs)
  {
    if (!edg->get_bounding_box())
    {
      std::cout << "In vtol_one_chain::compute_bounding_box() -"
               << " edge has null bounding box\n";
      continue;
    }
    this->add_to_bounding_box(edg->get_bounding_box());
  }
}

//---------------------------------------------------------------------------
//: Redetermining the directions of all edges in the onechain.
// Require: is_cycle()
//---------------------------------------------------------------------------
void vtol_one_chain::determine_edge_directions()
{
  // require
  assert(is_cycle());

  int num_edges;
  vtol_edge_sptr first_edge;
  vtol_edge_sptr second_edge;
  vtol_vertex_sptr tweeney;

  // Clear out any old info...
  directions_.clear();
  num_edges=numinf();
  if (num_edges>=2)
  {
    // Determining the cycle direction
    // with reference to the first edge.

    auto i=inferiors()->begin();
    first_edge=(*i)->cast_to_edge();
    ++i;

    second_edge=(*i)->cast_to_edge();

    if (second_edge->is_endpoint1(first_edge->v1()))
    {
      directions_.push_back((signed char)(-1));
      tweeney=second_edge->v2();
      directions_.push_back((signed char)1);
    }
    else if (second_edge->is_endpoint2(first_edge->v1()))
    {
      directions_.push_back((signed char)(-1));
      directions_.push_back((signed char)(-1));
      tweeney=second_edge->v1();
    }
    else
    {
      directions_.push_back((signed char)1);
      if (second_edge->is_endpoint1(first_edge->v2()))
      {
        tweeney=second_edge->v2();
        directions_.push_back((signed char)1);
      }
      else
      {
        tweeney=second_edge->v1();
        directions_.push_back((signed char)(-1));
      }
    }
    if (num_edges>2)
    {
      vtol_edge *cur_edge;
      ++i;
      while (i!=inferiors()->end())
      {
        cur_edge=(*i)->cast_to_edge();
        if (cur_edge->is_endpoint1(tweeney))
        {
          tweeney=cur_edge->v2();
          directions_.push_back((signed char)1);
        }
        else
        {
          tweeney=cur_edge->v1();
          directions_.push_back((signed char)(-1));
        }
        ++i;
      }
    }
  }
  else if (num_edges==1)
    directions_.push_back((signed char)1);
}

//---------------------------------------------------------------------------
//: Add an edge
//---------------------------------------------------------------------------
void vtol_one_chain::add_edge(vtol_edge_sptr const& new_edge,
                              bool dir)
{
  if (dir)
    directions_.push_back((signed char)1);
  else
    directions_.push_back((signed char)(-1));
  link_inferior(new_edge);
}

void vtol_one_chain::add_edge(vtol_edge_2d_sptr const& new_edge,
                              bool dir)
{
  if (dir)
    directions_.push_back((signed char)1);
  else
    directions_.push_back((signed char)(-1));
  link_inferior(new_edge->cast_to_edge());
}

#if 1 // deprecated
void vtol_one_chain::add_edge(vtol_edge &new_edge,
                              bool dir)
{
  std::cerr << "Warning: deprecated form of vtol_one_chain::add_edge()\n";
  if (dir)
    directions_.push_back((signed char)1);
  else
    directions_.push_back((signed char)(-1));
  link_inferior(&new_edge);
}
#endif // 1

//---------------------------------------------------------------------------
//: Remove an edge
//---------------------------------------------------------------------------
void vtol_one_chain::remove_edge(vtol_edge_sptr const& doomed_edge,
                                 bool force_it)
{
  // require
  assert(force_it||!is_cycle());

  vtol_topology_object_sptr t=doomed_edge->cast_to_topology_object();
  topology_list::const_iterator i=std::find(inferiors()->begin(),inferiors()->end(),t);
  topology_list::difference_type index=i-inferiors()->begin();

  if (index>=0 && i!= inferiors()->end())
  {
    auto j = directions_.begin() + index;
    directions_.erase(j);// get rid of the direction associated with the edge
    touch();
    unlink_inferior(doomed_edge);
  }
}

void vtol_one_chain::remove_edge(vtol_edge_2d_sptr const& doomed_edge,
                                 bool force_it)
{
  // require
  assert(force_it||!is_cycle());

  vtol_topology_object_sptr t=doomed_edge->cast_to_topology_object();
  topology_list::const_iterator i=std::find(inferiors()->begin(),inferiors()->end(),t);
  topology_list::difference_type index=i-inferiors()->begin();

  if (index>=0 && i!= inferiors()->end())
  {
    auto j = directions_.begin() + index;
    directions_.erase(j);// get rid of the direction associated with the edge
    touch();
    unlink_inferior(doomed_edge->cast_to_edge());
  }
}

#if 1 // deprecated
void vtol_one_chain::remove_edge(vtol_edge &doomed_edge,
                                 bool force_it)
{
  std::cerr << "Warning: deprecated form of vtol_one_chain::remove_edge()\n";
  assert(force_it||!is_cycle());

  vtol_topology_object_sptr t=&doomed_edge;
  topology_list::const_iterator i=std::find(inferiors()->begin(),inferiors()->end(),t);
  topology_list::difference_type index=i-inferiors()->begin();

  if (index>=0 && i!= inferiors()->end())
  {
    auto j = directions_.begin() + index;
    directions_.erase(j);// get rid of the direction associated with the edge
    touch();
    unlink_inferior(&doomed_edge);
  }
}
#endif // 1

//---------------------------------------------------------------------------
//: Comparison operator
//---------------------------------------------------------------------------
bool vtol_one_chain::operator==(vtol_one_chain const &other) const
{
  if (this==&other) return true;

  // Check to see if the number of edges is the same
  if (numinf()!=other.numinf())
    return false;

  const topology_list *inf1=inferiors();
  const topology_list *inf2=other.inferiors();

  topology_list::const_iterator i1;
  topology_list::const_iterator i2;
  for (i1=inf1->begin() , i2 = inf2->begin(); i1 != inf1->end(); ++i1 , ++i2)
  {
    if (!( *(*i1) == *(*i2) ))
      return false;

    // Comparing the directions_
    const std::vector<signed char> *dir1=directions();
    const std::vector<signed char> *dir2=other.directions();

    if ((dir1->size()!=dir2->size())||(is_cycle()!=other.is_cycle()))
      return false;

    std::vector<signed char>::const_iterator d1;
    std::vector<signed char>::const_iterator d2;
    for (d1=dir1->begin(), d2=dir2->begin(); d1 != dir1->end(); ++d1, ++d2)
      if (!(*d1 == *d2))
    return false;

    // compare onechains that make up any holes
    const chain_list &righth=chain_inferiors_;
    const chain_list &lefth=other.chain_inferiors_;
    if (righth.size() != lefth.size())
      return false;

    chain_list::const_iterator r;
    chain_list::const_iterator l;
    for (r=righth.begin(), l=lefth.begin(); r!=righth.end(); ++r, ++l)
      if ( !(*(*r) == *(*l)) ) // ( *(*r) != *(*l))
        return false;
  }
  return true;
}

//---------------------------------------------------------------------------
//: Reverse the direction of the one chain
//---------------------------------------------------------------------------
void vtol_one_chain::reverse_directions()
{
  // This function reverses the direction
  // array in the list.

  for (signed char & direction : directions_)
    direction = - direction;

  // reverse the inferiors

  topology_list inf_tmp(inferiors()->size());

  // std::reverse_copy(inferiors()->begin(),inferiors()->end(),inf_tmp.begin());
  // reverse copy does not seem to work do this the hard way
  int s= inferiors()->size();
  for (int i=0;i<s;++i)
    inf_tmp[i]=inferiors_[s-1-i];

  inferiors()->clear();
  //  std::copy(inf_tmp.begin(),inf_tmp.end(),inferiors()->begin());

  for (auto & ti : inf_tmp)
    inferiors()->push_back(ti);

  std::vector<signed char> dir_tmp(directions_.size());

  // std::reverse_copy(directions_.begin(),directions_.end(),dir_tmp.begin());
  s=directions_.size();

  for (int i=0;i<s;++i)
    dir_tmp[i]=directions_[s-1-i];

  directions_.clear();
  // std::copy(dir_tmp.begin(),dir_tmp.end(),directions_.begin());

  for (signed char & di : dir_tmp)
    directions_.push_back(di);

  for (auto & chain_inferior : chain_inferiors_)
    chain_inferior->cast_to_one_chain()->reverse_directions();
}

//---------------------------------------------------------------------------
//: Spatial object equality
//---------------------------------------------------------------------------
bool vtol_one_chain::operator==(vsol_spatial_object_2d const& obj) const
{
  return
   obj.cast_to_topology_object() &&
   obj.cast_to_topology_object()->cast_to_one_chain() &&
   *this == *obj.cast_to_topology_object()->cast_to_one_chain();
}

//---------------------------------------------------------------------------
//: Print Methods
//---------------------------------------------------------------------------
void vtol_one_chain::print(std::ostream &strm) const
{
  strm << "<one_chain " << inferiors()->size() << "  "
       << (void const *) this << ">\n";
}

//---------------------------------------------------------------------------
//: Describe the directions
//---------------------------------------------------------------------------
void vtol_one_chain::describe_directions(std::ostream &strm, int blanking) const
{
  for (int j=0; j<blanking; ++j) { strm << ' '; }
  strm << "<Dirs [" << directions_.size() << "]:";

  std::vector<signed char>::const_iterator d1;
  for (d1=directions_.begin();d1!=directions_.end();++d1)
  {
    strm << ' ' << (int)(*d1);
  }
  strm << ">\n";
}

//---------------------------------------------------------------------------
//: Describe the one chain
//---------------------------------------------------------------------------
void vtol_one_chain::describe(std::ostream &strm, int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  print(strm);
  describe_inferiors(strm, blanking);
  describe_directions(strm, blanking);
  describe_superiors(strm, blanking);
}
