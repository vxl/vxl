#include "vtol_edge.h"

//:
//  \file

#include <vcl_cassert.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_macros.h>
#include <vtol/vtol_list_functions.h>


//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default constructor. Empty edge. Not a valid edge.
//---------------------------------------------------------------------------
vtol_edge::vtol_edge(void)
{
  v1_=0;
  v2_=0;
  vtol_zero_chain_sptr c = new vtol_zero_chain();
  // vtol_zero_chain c;
  // link_inferior ( ( (vtol_topology_object*) new vtol_zero_chain) );
  link_inferior ( *c );
}


//---------------------------------------------------------------------------
//: Return the first non-empty zero-chain of `this'
//---------------------------------------------------------------------------
vtol_zero_chain_sptr vtol_edge::zero_chain(void) const
{
  vcl_vector<vtol_topology_object_sptr> const& inf = *(inferiors());
  for (unsigned int i=0; i<inf.size(); ++i)
    if (inf[i]->cast_to_zero_chain()->v0()) // PVr- avoid returning empty chain
      return inf[i]->cast_to_zero_chain();
  for (unsigned int i=0; i<inf.size(); ++i)
    if (inf[i]->cast_to_zero_chain()) // return empty chain if nothing else present
      return inf[i]->cast_to_zero_chain();
  return 0;
}


//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_edge::~vtol_edge()
{
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
//: Set the first endpoint.
//---------------------------------------------------------------------------
void vtol_edge::set_v1(vtol_vertex_sptr new_v1)
{
  if (v1_&&v1_!=v2_)
    zero_chain()->unlink_inferior(*v1_);
  v1_=new_v1;
  zero_chain()->link_inferior(*v1_);
  touch();
}

//---------------------------------------------------------------------------
//: Set the last endpoint.
//---------------------------------------------------------------------------
void vtol_edge::set_v2(vtol_vertex_sptr new_v2)
{
  if (v2_&&v2_!=v1_)
    zero_chain()->unlink_inferior(*v2_);
  v2_=new_v2;
  zero_chain()->link_inferior(*v2_);
  touch();
}

//---------------------------------------------------------------------------
//: Set the first and last endpoints
// Require: vertex_of_edge(new_v1) and vertex_of_edge(new_v2)
//---------------------------------------------------------------------------


//:
//     This function removes curendpt from the edge and replaces it with
//     newendpt.  Both curendpt and newendpt must be non-NULL pointers,
//     curendpt must point to an endpoint of the edge (either v1_ or
//     v2_), and newendpt must be colinear with the edge.  Neither vertex
//     is destroyed in the process; only links are manipulated.
//     (RYF 7-14-98)
//
void vtol_edge::replace_end_point(vtol_vertex &curendpt,
                                  vtol_vertex &newendpt)
{
  // require
  //  assert(curendpt);
  //  assert(newendpt);
  assert(curendpt==*v1_||curendpt==*v2_);

  zero_chain()->unlink_inferior(curendpt);
  zero_chain()->link_inferior(newendpt);
  if (curendpt==*v1_)  // update the appropriate endpoint
    v1_=&newendpt;
  else // curendpt == this->get_v2()
    v2_=&newendpt;
}

//---------------------------------------------------------------------------
//: Determine the endpoints of an edge from its inferiors
//---------------------------------------------------------------------------
void vtol_edge::set_vertices_from_zero_chains(void)
{
  if (numinf()==1) // This is the normal case.
    {
      // Set v1_ to be the first on the
      // vtol_zero_chain. Set v2_ to be the last on
      // the vtol_zero_chain.
      vtol_topology_object_sptr zc0=inferiors()->front();
      int numverts=zc0->numinf();
      //topology_list *verts=zc0->inferiors();
      switch(numverts)
        {
        case 0:
          v1_=0;
          v2_=0;
          break;
        case 1:
          v1_=zc0->inferiors()->front()->cast_to_vertex();
          v2_=v1_;
          break;
        default:
          vcl_vector<vtol_topology_object_sptr> const * v = zc0->inferiors();
          v1_=v->front()->cast_to_vertex();
          v2_=v->back()->cast_to_vertex();
          break;
        }
    }
  else // This is a Strange case but we'll take care of it.
    // Assuming zero_chains have an ordering,
    // Set v1_ to the first vertex and Set v2_
    // to be the last vertex.  They will not
    // be on the same vtol_zero_chain.
    {
      vertex_list *verts=vertices();

      if (verts)
        {
          int numverts=verts->size();
          switch(numverts)
            {
            case 0:
              v1_=0;
              v2_=0;
              break;
            default:
              v1_=(*verts)[0];
              v2_=(*verts)[numverts-1];
            }
        }
      delete verts;
    }
  touch();
}

//:
// This is a utility method that adds new_edge_loop to the vtol_edge's
// superior list.  It returns a boolean value reflecting the success of
// linking.

void vtol_edge::add_edge_loop(vtol_one_chain &new_edge_loop)
{
  new_edge_loop.link_inferior(*this);
}

//:
// This is a utility method that removes doomed_edge_loop from the
// vtol_edge's superior list. It returns a boolean value reflecting the
// success of removing.

void vtol_edge::remove_edge_loop(vtol_one_chain &doomed_edge_loop)
{
  doomed_edge_loop.unlink_inferior(*this);
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `inferior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_edge::valid_inferior_type(const vtol_topology_object &inferior) const
{
  return bool(inferior.cast_to_zero_chain());
}

//---------------------------------------------------------------------------
//: Is `superior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_edge::valid_superior_type(const vtol_topology_object &superior) const
{
  return bool(superior.cast_to_one_chain());
}

// ******************************************************
//
//    Operators
//

// operators

bool vtol_edge::operator==(const vtol_edge &other) const
{
  if (this==&other) return true;


  if (!compare_geometry(other))
    return false;

  if (!(*v1_==*(other.v1_)) || !(*v2_==*(other.v2_))) // ((*v1_!=*(other.v1_)) || (*v2_!=*(other.v2_)))
    return false;

  vtol_zero_chain_sptr zc1=zero_chain();
  vtol_zero_chain_sptr zc2=other.zero_chain();
  if (!zc1||!zc2)
    return false;
  return *zc1==*zc2;
}

//: spatial object equality

bool vtol_edge::operator==(const vsol_spatial_object_2d& obj) const
{
  return obj.spatial_type() == vsol_spatial_object_2d::TOPOLOGYOBJECT &&
   ((vtol_topology_object const&)obj).topology_type() == vtol_topology_object::EDGE
  ? *this == (vtol_edge const&) (vtol_topology_object const&) obj
  : false;
}

// ******************************************************
//
//    Inferior/Superior Accessor Functions
//

//: Returns a list of vertices on the vtol_edge.

vcl_vector<vtol_vertex *> *vtol_edge::compute_vertices(void)
{
  SEL_INF(vtol_vertex,compute_vertices);
}

//: Returns the vtol_zero_chain list of the vtol_edge. This list is the Inferiors of the edge.
vcl_vector<vtol_zero_chain *> *vtol_edge::compute_zero_chains(void)
{
  COPY_INF(vtol_zero_chain);
}

//: Returns a list with itself as the only element. This utility is used in Inferior/Superior accessing methods.
vcl_vector<vtol_edge *> *vtol_edge::compute_edges(void)
{
  LIST_SELF(vtol_edge);
}

//: Returns a list of one_chains which contain the vtol_edge. This list is the Superiors of the edge.
vcl_vector<vtol_one_chain *> *vtol_edge::compute_one_chains(void)
{
  SEL_SUP(vtol_one_chain,compute_one_chains);
}


//: Returns a list of the faces which contain the vtol_edge.
vcl_vector<vtol_face *> *vtol_edge::compute_faces(void)
{
  SEL_SUP(vtol_face,compute_faces);
}

//: Returns the list of two_chains which contain the vtol_edge.
vcl_vector<vtol_two_chain *> *vtol_edge::compute_two_chains(void)
{
  SEL_SUP(vtol_two_chain,compute_two_chains);
}

//: Returns the list of blocks which contain the vtol_edge.
vcl_vector<vtol_block *> *vtol_edge::compute_blocks(void)
{
  SEL_SUP(vtol_block,compute_blocks);
}

//: Returns a list of vertices containing the endpoints of the edge.
// These vertices are v1_ and v2_ in that order.
vertex_list *vtol_edge::endpoints(void)
{
  vertex_list *newl=new vcl_vector<vtol_vertex_sptr>();
  if (v1_)
    newl->push_back(v1_);
  if (v2_)
    newl->push_back(v2_);
  return newl;
}

//:
//  Returns true if the invoking edge has a vertex in common with
//  vtol_edge `other'.  The method determines if the two edges share a vertex
//  by comparing pointer values, not the vertex geometry.
//

bool vtol_edge::share_vertex_with(vtol_edge &other)
{
  vertex_list *thisedges=vertices();
  vertex_list *eedges=other.vertices();

  vcl_vector<vtol_vertex_sptr>::iterator i1;
  vcl_vector<vtol_vertex_sptr>::iterator i2;
  for (i1=thisedges->begin();i1!=thisedges->end();++i1 )
    for (i2=eedges->begin();i2!=eedges->end(); ++i2 )
      if ((*i1)==(*i2))
        return true;
  return false;
}

//:
// This method adds newvert to the vtol_edge by linking it to one of the
// zero_chains of the vtol_edge Inferiors. (Method needs work.)

bool vtol_edge::add_vertex(vtol_vertex &newvert)
{
  vtol_zero_chain_sptr zc;

  zc=zero_chain();
  if (!zc)
  {
    zc=new vtol_zero_chain;
    link_inferior(*zc);
  }

  zc->link_inferior(newvert);
  return true;
}

//:
// This method removes uglyvert from the vtol_edge by removing it from the
// inferior zero_chains.  (Method needs work.)

bool vtol_edge::remove_vertex(vtol_vertex &uglyvert)
{
  if (uglyvert==*v1_)
    set_v1(0);
  else if (uglyvert==*v2_)
    set_v2(0);
  else
    return false;
  touch();
  return true;
}

bool vtol_edge::is_endpoint(const vtol_vertex &v) const
{
  return is_endpoint1(v)||is_endpoint2(v);
}


//: Returns True if v is equal to the first vtol_edge endpoint,v1_.
bool vtol_edge::is_endpoint1(const vtol_vertex &v) const
{
  return *v1_==v;
}

//: Returns True if v is equal to the second vtol_edge endpoint, v2_.
bool vtol_edge::is_endpoint2(const vtol_vertex &v) const
{
  return *v2_ ==v;
}


//: This method works only for ImplicitLine edges.
vtol_vertex_sptr vtol_edge::other_endpoint(const vtol_vertex &overt) const
{
  if (overt==*v1_)
    return v2_;
  else if (overt==*v2_)
    return v1_;
  else
    return 0;
}

//-----------------------------------------------------------------
//: Compute bounds from the geometry of curve().
// If the curve is not fully bounded, then use the vertices.
//
void vtol_edge::compute_bounding_box(void)
{
#if 0 // TODO
  if (curve() && curve()->GetGeometryType() != GeometryObject::IMPLICITLINE)
  {
    // Get bounds from curve.
    // But are curve endpoints consistent with vertices? -JLM
    // Anyway, this is what was done in get_min_max
    if (curve()->GetExtent() == vsol_curve::FULLY_BOUNDED)
    {
      this->set_min_x(curve()->get_min_x());
      this->set_min_y(curve()->get_min_y());
      this->set_max_x(curve()->get_max_x());
      this->set_max_y(curve()->get_max_y());
    }
  }
  else  // Just use the generic method computing bounds from vertices
#endif
    this->vtol_topology_object::compute_bounding_box();
}


// ******************************************************
//
//    I/O methods
//

//:
// This method outputs all edge information to the vcl_ostream, strm.  It
// indents various levels of output by the number given in blanking.
void vtol_edge::describe(vcl_ostream &strm,
                         int blanking) const
{
  for (int i1=0; i1<blanking; ++i1) strm << ' ';
  print(strm);
  for (int i2=0; i2<blanking; ++i2) strm << ' ';
  if (v1_) v1_->print(strm);
  else     strm << "Null vertex 1" << vcl_endl;

  for (int i3=0; i3<blanking; ++i3) strm << ' ';
  if (v2_) v2_->print(strm);
  else     strm << "Null vertex 2" << vcl_endl;
}

//:
// This method outputs a brief vtol_edge info with vtol_edge object address.
void vtol_edge::print(vcl_ostream &strm) const
{
   strm<<"<vtol_edge  "<<"  "<<(void const *)this <<"> with id "<<get_id()<<vcl_endl;
}
