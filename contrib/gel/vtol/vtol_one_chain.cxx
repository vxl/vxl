// This is gel/vtol/vtol_one_chain.cxx
#include "vtol_one_chain.h"
//:
// \file

#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_macros.h>
#include <vtol/vtol_list_functions.h>


vtol_edge_sptr vtol_one_chain::edge(int i) const
{
  return inferiors_[i]->cast_to_edge();
}

//***************************************************************************
// Initialization
//***************************************************************************

void vtol_one_chain::link_inferior(vtol_edge_sptr inf)
{
  vtol_topology_object::link_inferior(inf->cast_to_topology_object());
}

void vtol_one_chain::unlink_inferior(vtol_edge_sptr inf)
{
  vtol_topology_object::unlink_inferior(inf->cast_to_topology_object());
}

void vtol_one_chain::link_chain_inferior(vtol_one_chain_sptr chain_inferior)
{
  vtol_chain::link_chain_inferior(chain_inferior->cast_to_chain());
}

void vtol_one_chain::unlink_chain_inferior(vtol_one_chain_sptr chain_inferior)
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

  for (edge_list::const_iterator i=edgs.begin(); i!=edgs.end(); ++i)
    {
      link_inferior(*i);
      directions_.push_back((signed char)1);
    }

  if (is_cycle())
    determine_edge_directions();
}

//---------------------------------------------------------------------------
//: Constructor from an array of edges and an array of directions
//---------------------------------------------------------------------------
vtol_one_chain::vtol_one_chain(edge_list const& edgs,
                               vcl_vector<signed char> const& dirs,
                               bool new_is_cycle)
{
  set_cycle(new_is_cycle);

  for (edge_list::const_iterator i=edgs.begin(); i!=edgs.end(); ++i)
    link_inferior(*i);

  for (vcl_vector<signed char>::const_iterator j=dirs.begin();j!=dirs.end();++j)
    directions_.push_back(*j);
}

//---------------------------------------------------------------------------
//: Pseudo copy constructor.  Deep copy.
//---------------------------------------------------------------------------
vtol_one_chain::vtol_one_chain(vtol_one_chain_sptr const& other)
{
  vertex_list *verts=other->vertices();
  topology_list newverts(verts->size());

  int i=0;
  for (vertex_list::iterator v=verts->begin();v!=verts->end();++v,++i)
  {
    vtol_vertex_sptr ve=*v;
    newverts[i]=ve->clone()->cast_to_topology_object();
    ve->set_id(i);
  }

  vcl_vector<signed char>::iterator dir=other->directions_.begin();
  topology_list::iterator inf=other->inferiors()->begin();
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

  for (chain_list::const_iterator h=hierarchy_infs->begin();h!=hierarchy_infs->end();++h)
    link_chain_inferior((*h)->clone()->cast_to_topology_object()->cast_to_one_chain());
  delete verts;
}

#if 0 // deprecated
//---------------------------------------------------------------------------
//: Copy constructor.  Deep copy.  Deprecated.
//---------------------------------------------------------------------------
vtol_one_chain::vtol_one_chain(vtol_one_chain const& other)
{
  vcl_cerr << "vtol_one_chain copy constructor is deprecated; use vtol_one_chain_sptr constructor instead\n";
  vtol_one_chain *el=const_cast<vtol_one_chain*>(&other); // const violation
  vertex_list *verts=el->vertices();
  topology_list newverts(verts->size());

  int i=0;
  for (vertex_list::iterator v=verts->begin();v!=verts->end();++v,++i)
    {
      vtol_vertex_sptr ve=*v;
      newverts[i]=ve->clone()->cast_to_topology_object();
      ve->set_id(i);
    }

  vcl_vector<signed char>::iterator dir=el->directions_.begin();
  topology_list::iterator inf=el->inferiors()->begin();
  for (; dir!=el->directions_.end(); ++dir,++inf)
    {
      vtol_edge_sptr e=(*inf)->cast_to_edge();

      vtol_edge_sptr newedge = newverts[e->v1()->get_id()]->cast_to_vertex()->new_edge(
                               newverts[e->v2()->get_id()]->cast_to_vertex());
      link_inferior(newedge);
      directions_.push_back(*dir);
    }
  set_cycle(el->is_cycle());
  const chain_list *hierarchy_infs=el->chain_inferiors();

  for (chain_list::const_iterator h=hierarchy_infs->begin();h!=hierarchy_infs->end();++h)
    link_chain_inferior((*h)->clone()->cast_to_topology_object()->cast_to_one_chain());
  delete verts;
}
#endif

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
vsol_spatial_object_2d_sptr vtol_one_chain::clone(void) const
{
  return new vtol_one_chain(vtol_one_chain_sptr(const_cast<vtol_one_chain*>(this)));
}

vtol_one_chain *
vtol_one_chain::copy_with_arrays(topology_list &verts,
                                 topology_list &edges) const
{
  vtol_one_chain *result=new vtol_one_chain();

  vcl_vector<signed char>::const_iterator di=directions_.begin();
  topology_list::const_iterator ti=inferiors()->begin();
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

  for (chain_list::const_iterator hi=hierarchy_infs->begin();hi!=hierarchy_infs->end();++hi)
    {
      vtol_one_chain *oldone=(*hi)->clone()->cast_to_topology_object()->cast_to_one_chain();
      result->link_chain_inferior(oldone->copy_with_arrays(verts,edges));
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
  vcl_vector<signed char>::const_iterator dit=directions_.begin();
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
vcl_vector<vtol_vertex*> *
vtol_one_chain::outside_boundary_compute_vertices(void)
{
  vcl_vector<vtol_vertex*> *result=new vcl_vector<vtol_vertex*>;

  topology_list::iterator inf=inferiors()->begin();
  vcl_vector<signed char>::iterator dir=directions_.begin();
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
vertex_list *vtol_one_chain::outside_boundary_vertices(void)
{
  vcl_vector<vtol_vertex*> *tmp_list=outside_boundary_compute_vertices();
  vertex_list *result=new vertex_list;
  result->reserve(tmp_list->size());
  vcl_vector<vtol_vertex*>::iterator i;
  for (i=tmp_list->begin();i!=tmp_list->end();++i)
    result->push_back(*i);
  delete tmp_list;
  return result;
}

//---------------------------------------------------------------------------
//: Get the vertices of this object
//---------------------------------------------------------------------------
vcl_vector<vtol_vertex*> *vtol_one_chain::compute_vertices(void)
{
  // We must collect vertices from subchains as well as
  // from direct Inferiors...so this function only has
  // an ordering if there are no subchains.

  vcl_vector<vtol_vertex*> *verts=outside_boundary_compute_vertices();

  // This macro adds the subchain vertices to the verts list.

  SUBCHAIN_INF(verts,one_chain,vtol_vertex,compute_vertices);
}


//---------------------------------------------------------------------------
//: Get the outside boundary zero chains
//---------------------------------------------------------------------------
vcl_vector<vtol_zero_chain *> *
vtol_one_chain::outside_boundary_compute_zero_chains(void)
{
  SEL_INF(vtol_zero_chain,compute_zero_chains);
}

//---------------------------------------------------------------------------
//: Get the outside boundary zero chains
//---------------------------------------------------------------------------
zero_chain_list *vtol_one_chain::outside_boundary_zero_chains(void)
{
  zero_chain_list *result=new zero_chain_list;
  vcl_vector<vtol_zero_chain *> *ptr_list=outside_boundary_compute_zero_chains();

  // copy the lists
  vcl_vector<vtol_zero_chain *>::iterator i;
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    if ((*i)->v0()) // PVr- added this condition so no empty chains are returned
      result->push_back(*i);
  delete ptr_list;

  return result;
}


//---------------------------------------------------------------------------
//: Get the zero chains of this object
//---------------------------------------------------------------------------
vcl_vector<vtol_zero_chain*> *vtol_one_chain::compute_zero_chains(void)
{
  vcl_vector<vtol_zero_chain*> *zchs;
  zchs=outside_boundary_compute_zero_chains();

  // This macro adds the subchain zerochains to the zchs list.

  SUBCHAIN_INF(zchs,one_chain,vtol_zero_chain,compute_zero_chains);
}

//---------------------------------------------------------------------------
//: Get the outside boundary edges
//---------------------------------------------------------------------------
vcl_vector<vtol_edge*> *vtol_one_chain::outside_boundary_compute_edges(void)
{
  COPY_INF(edge);
}


//---------------------------------------------------------------------------
//: Get the outside boundary edges
//---------------------------------------------------------------------------
edge_list *vtol_one_chain::outside_boundary_edges(void)
{
  edge_list *new_ref_list = new edge_list;
  vcl_vector<vtol_edge*>* ptr_list = this->outside_boundary_compute_edges();
  // copy the lists

  for (vcl_vector<vtol_edge*>::iterator ti = ptr_list->begin(); ti != ptr_list->end(); ++ti)
    new_ref_list->push_back(*ti);

  delete ptr_list;
  return new_ref_list;
}

//---------------------------------------------------------------------------
//: Get the edges
//---------------------------------------------------------------------------
vcl_vector<vtol_edge*> *vtol_one_chain::compute_edges(void)
{
  vcl_vector<vtol_edge*> *edges;
  edges=outside_boundary_compute_edges();

  // This macro adds the subchain zerochains to the zchs list.

  SUBCHAIN_INF(edges,one_chain,vtol_edge,compute_edges);
}

//---------------------------------------------------------------------------
//: Get the one chains
//---------------------------------------------------------------------------
vcl_vector<vtol_one_chain*> *vtol_one_chain::compute_one_chains(void)
{
  vcl_vector<vtol_one_chain*> *result=outside_boundary_compute_one_chains();
  for (chain_list::iterator i=chain_inferiors_.begin();i!=chain_inferiors_.end();++i)
    result->push_back((*i)->clone()->cast_to_topology_object()->cast_to_one_chain());
  return result;
}

//---------------------------------------------------------------------------
//: Get the inferior one chains
//---------------------------------------------------------------------------
one_chain_list *vtol_one_chain::inferior_one_chains(void)
{
  one_chain_list *result=new one_chain_list;

  for (chain_list::iterator i=chain_inferiors_.begin();i!=chain_inferiors_.end();++i)
    result->push_back((*i)->clone()->cast_to_topology_object()->cast_to_one_chain());

  return result;
}

//---------------------------------------------------------------------------
//: Get the superior one chains
//---------------------------------------------------------------------------
one_chain_list *vtol_one_chain::superior_one_chains(void)
{
  one_chain_list *result=new one_chain_list;

  vcl_list<vtol_chain*>::iterator i;
  for (i=chain_superiors_.begin();i!=chain_superiors_.end();++i)
    result->push_back((*i)->clone()->cast_to_topology_object()->cast_to_one_chain());

  return result;
}

//---------------------------------------------------------------------------
//: Get the outside boundary one chains
//---------------------------------------------------------------------------
one_chain_list *vtol_one_chain::outside_boundary_one_chains(void)
{
  vcl_vector<vtol_one_chain*>* ptr_list= outside_boundary_compute_one_chains();
  one_chain_list *ref_list= new one_chain_list;

  vcl_vector<vtol_one_chain*>::iterator i;
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
vcl_vector<vtol_one_chain*> *vtol_one_chain::outside_boundary_compute_one_chains(void)
{
  LIST_SELF(vtol_one_chain);
}

//---------------------------------------------------------------------------
//: Get the faces
//---------------------------------------------------------------------------
vcl_vector<vtol_face*> *vtol_one_chain::compute_faces(void)
{
  if (is_sub_chain())
    {
      vcl_vector<vtol_face*> *result=new vcl_vector<vtol_face*>();
      one_chain_list *onech=superior_one_chains();

      for (one_chain_list::iterator i=onech->begin();i!=onech->end();++i)
        {
          vcl_vector<vtol_face*> *sublist=(*i)->compute_faces();
          vcl_vector<vtol_face*>::iterator ii;
          for (ii=sublist->begin();ii!=sublist->end();++ii)
            result->push_back(*ii);
          delete sublist;
        }
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
vcl_vector<vtol_two_chain*> *vtol_one_chain::compute_two_chains(void)
{
  if (is_sub_chain())
    {
      vcl_vector<vtol_two_chain*> *result=new vcl_vector<vtol_two_chain*>;
      one_chain_list *onech=superior_one_chains();

      for (one_chain_list::iterator i=onech->begin();i!=onech->end();++i)
        {
          vcl_vector<vtol_two_chain*> *sublist=(*i)->compute_two_chains();
          vcl_vector<vtol_two_chain*>::iterator ii;
          for (ii=sublist->begin();ii!=sublist->end();++ii)
            result->push_back(*ii);
          delete sublist;
        }
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
vcl_vector<vtol_block*> *vtol_one_chain::compute_blocks(void)
{
  if (is_sub_chain())
    {
      vcl_vector<vtol_block*> *result=new vcl_vector<vtol_block*>;
      one_chain_list *onech=superior_one_chains();
      for (one_chain_list::iterator i=onech->begin();i!=onech->end();++i)
        {
          vcl_vector<vtol_block*> *sublist=(*i)->compute_blocks();
          vcl_vector<vtol_block*>::iterator ii;
          for (ii=sublist->begin();ii!=sublist->end();++ii)
            result->push_back(*ii);
          delete sublist;
        }
      return tagged_union(result);
    }
  else
    {
      SEL_SUP(vtol_block, compute_blocks);
    }
}

//---------------------------------------------------------------------------
//: Computes the bounding box of a vtol_one_chain from the edges.
//    Just get the bounding box for each edge and update this's
//    box accordingly. Note that the computation can be done independently of
//    dimension.
//---------------------------------------------------------------------------

void vtol_one_chain::compute_bounding_box(void) const
{
  if (!this->bounding_box_)
    {
      vcl_cout << "In void vtol_one_chain::compute_bounding_box() - shouldn't happen\n";
      return;
    }

  edge_list *edgs= const_cast<vtol_one_chain*>(this)->edges();

  if (edgs->size()==0)//default method, things are screwed up anyway
    {
      vtol_topology_object::compute_bounding_box();
      return;
    }
  //at this point we need to clear the bounds of the box
  //to correctly reflect edge bounds
  bounding_box_->reset_bounds();

  for (edge_list::iterator eit = edgs->begin(); eit != edgs->end(); eit++)
  {
    if (!(*eit)->get_bounding_box())
    {
      vcl_cout << "In vtol_one_chain::compute_bounding_box() -"
               << " edge has null bounding box\n";
      continue;
    }
    bounding_box_->grow_minmax_bounds(*(*eit)->get_bounding_box());
  }
  delete edgs;
}

//---------------------------------------------------------------------------
//: Redetermining the directions of all edges in the onechain.
// Require: is_cycle()
//---------------------------------------------------------------------------
void vtol_one_chain::determine_edge_directions(void)
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

      topology_list::iterator i=inferiors()->begin();
      first_edge=(*i)->cast_to_edge();
      ++i;

      second_edge=(*i)->cast_to_edge();

      if (second_edge->is_endpoint1(*(first_edge->v1())))
        {
          directions_.push_back((signed char)(-1));
          tweeney=second_edge->v2();
          directions_.push_back((signed char)1);
        }
      else if (second_edge->is_endpoint2(*(first_edge->v1())))
        {
          directions_.push_back((signed char)(-1));
          directions_.push_back((signed char)(-1));
          tweeney=second_edge->v1();
        }
      else
        {
          directions_.push_back((signed char)1);
          if (second_edge->is_endpoint1(*(first_edge->v2())))
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
              if (cur_edge->is_endpoint1(*tweeney))
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
  vcl_cerr << "Warning: deprecated form of vtol_one_chain::add_edge()\n";
  if (dir)
    directions_.push_back((signed char)1);
  else
    directions_.push_back((signed char)(-1));
  link_inferior(&new_edge);
}
#endif

//---------------------------------------------------------------------------
//: Remove an edge
//---------------------------------------------------------------------------
void vtol_one_chain::remove_edge(vtol_edge_sptr const& doomed_edge,
                                 bool force_it)
{
  // require
  assert(force_it||!is_cycle());

  vtol_topology_object_sptr t=doomed_edge->cast_to_topology_object();
  topology_list::const_iterator i=vcl_find(inferiors()->begin(),inferiors()->end(),t);
  topology_list::difference_type index=i-inferiors()->begin();

  if (index>=0 && i!= inferiors()->end())
    {
      vcl_vector<signed char>::iterator j = directions_.begin() + index;
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
  topology_list::const_iterator i=vcl_find(inferiors()->begin(),inferiors()->end(),t);
  topology_list::difference_type index=i-inferiors()->begin();

  if (index>=0 && i!= inferiors()->end())
    {
      vcl_vector<signed char>::iterator j = directions_.begin() + index;
      directions_.erase(j);// get rid of the direction associated with the edge
      touch();
      unlink_inferior(doomed_edge->cast_to_edge());
    }
}

#if 1 // deprecated
void vtol_one_chain::remove_edge(vtol_edge &doomed_edge,
                                 bool force_it)
{
  vcl_cerr << "Warning: deprecated form of vtol_one_chain::remove_edge()\n";
  assert(force_it||!is_cycle());

  vtol_topology_object_sptr t=&doomed_edge;
  topology_list::const_iterator i=vcl_find(inferiors()->begin(),inferiors()->end(),t);
  topology_list::difference_type index=i-inferiors()->begin();

  if (index>=0 && i!= inferiors()->end())
    {
      vcl_vector<signed char>::iterator j = directions_.begin() + index;
      directions_.erase(j);// get rid of the direction associated with the edge
      touch();
      unlink_inferior(&doomed_edge);
    }
}
#endif

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
    const vcl_vector<signed char> *dir1=directions();
    const vcl_vector<signed char> *dir2=other.directions();

    if ((dir1->size()!=dir2->size())||(is_cycle()!=other.is_cycle()))
      return false;

    vcl_vector<signed char>::const_iterator d1;
    vcl_vector<signed char>::const_iterator d2;
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
void vtol_one_chain::reverse_directions(void)
{
  // This function reverses the direction
  // array in the list.

  for (vcl_vector<signed char>::iterator di=directions_.begin();
       di !=directions_.end();++di)
    (*di) = - (*di);

  // reverse the inferiors

  topology_list inf_tmp(inferiors()->size());

  // vcl_reverse_copy(inferiors()->begin(),inferiors()->end(),inf_tmp.begin());
  // reverse copy does not seem to work do this the hard way
  int s= inferiors()->size();
  for (int i=0;i<s;++i)
    inf_tmp[i]=inferiors_[s-1-i];

  inferiors()->clear();
  //  vcl_copy(inf_tmp.begin(),inf_tmp.end(),inferiors()->begin());

  for (topology_list::iterator ti=inf_tmp.begin();ti!=inf_tmp.end();ti++)
    inferiors()->push_back(*ti);

  vcl_vector<signed char> dir_tmp(directions_.size());

  // vcl_reverse_copy(directions_.begin(),directions_.end(),dir_tmp.begin());
  s=directions_.size();

  for (int i=0;i<s;++i)
    dir_tmp[i]=directions_[s-1-i];

  directions_.clear();
  // vcl_copy(dir_tmp.begin(),dir_tmp.end(),directions_.begin());

  for (vcl_vector<signed char>::iterator di=dir_tmp.begin();di!=dir_tmp.end();++di)
    directions_.push_back(*di);

  for (chain_list::iterator hi=chain_inferiors_.begin();
       hi!=chain_inferiors_.end();++hi)
    (*hi)->cast_to_one_chain()->reverse_directions();
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
void vtol_one_chain::print(vcl_ostream &strm) const
{
  strm << "<one_chain " << inferiors()->size() << "  "
       << (void const *) this << ">\n";
}

//---------------------------------------------------------------------------
//: Describe the directions
//---------------------------------------------------------------------------
void vtol_one_chain::describe_directions(vcl_ostream &strm, int blanking) const
{
  for (int j=0; j<blanking; ++j) { strm << ' '; }
  strm << "<Dirs [" << directions_.size() << "]:";

  vcl_vector<signed char>::const_iterator d1;
  for (d1=directions_.begin();d1!=directions_.end();++d1)
  {
    strm << ' ' << (int)(*d1);
  }
  strm << ">\n";
}

//---------------------------------------------------------------------------
//: Describe the one chain
//---------------------------------------------------------------------------
void vtol_one_chain::describe(vcl_ostream &strm, int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  print(strm);
  describe_inferiors(strm, blanking);
  describe_directions(strm, blanking);
  describe_superiors(strm, blanking);
}
