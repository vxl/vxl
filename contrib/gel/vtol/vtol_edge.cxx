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
  _v1=0;
  _v2=0;
  vtol_zero_chain_sptr c = new vtol_zero_chain();
  // vtol_zero_chain c;
  // link_inferior ( ( (vtol_topology_object*) new vtol_zero_chain) );
  link_inferior ( *c );
}


//---------------------------------------------------------------------------
//: Return the first zero-chain of `this'
//---------------------------------------------------------------------------
vtol_zero_chain_sptr vtol_edge::zero_chain(void) const
{
  return (vtol_zero_chain *)(_inferiors[0].ptr());
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
void vtol_edge::set_v1(vtol_vertex *new_v1)
{
  if (_v1&&_v1!=_v2)
    zero_chain()->unlink_inferior(*_v1);
  _v1=new_v1;
  zero_chain()->link_inferior(*_v1);
  touch();
}


//---------------------------------------------------------------------------
//: Set the first endpoint.
//---------------------------------------------------------------------------
void vtol_edge::set_v1(vtol_vertex_sptr new_v1)
{
  if (_v1&&_v1!=_v2)
    zero_chain()->unlink_inferior(*_v1);
  _v1=new_v1;
  zero_chain()->link_inferior(*_v1);
  touch();
}

//---------------------------------------------------------------------------
//: Set the last endpoint.
//---------------------------------------------------------------------------
void vtol_edge::set_v2(vtol_vertex *new_v2)
{
  if (_v2&&_v2!=_v1)
    zero_chain()->unlink_inferior(*_v2);
  _v2=new_v2;
  zero_chain()->link_inferior(*_v2);
  touch();
}

//---------------------------------------------------------------------------
//: Set the last endpoint.
//---------------------------------------------------------------------------
void vtol_edge::set_v2(vtol_vertex_sptr new_v2)
{
  if (_v2&&_v2!=_v1)
    zero_chain()->unlink_inferior(*_v2);
  _v2=new_v2;
  zero_chain()->link_inferior(*_v2);
  touch();
}

//---------------------------------------------------------------------------
//: Set the first and last endpoints
// Require: vertex_of_edge(new_v1) and vertex_of_edge(new_v2)
//---------------------------------------------------------------------------


//:
//     This function removes curendpt from the edge and replaces it with
//     newendpt.  Both curendpt and newendpt must be non-NULL pointers,
//     curendpt must point to an endpoint of the edge (either _v1 or
//     _v2), and newendpt must be colinear with the edge.  Neither vertex
//     is destroyed in the process; only links are manipulated.
//     (RYF 7-14-98)
//
void vtol_edge::replace_end_point(vtol_vertex &curendpt,
                                     vtol_vertex &newendpt)
{
  // require
  //  assert(curendpt);
  //  assert(newendpt);
  assert(curendpt==*_v1||curendpt==*_v2);

  zero_chain()->unlink_inferior(curendpt);
  zero_chain()->link_inferior(newendpt);
  if(curendpt==*_v1)  // update the appropriate endpoint
    _v1=&newendpt;
  else // curendpt == this->get_v2()
    _v2=&newendpt;
}

//---------------------------------------------------------------------------
//: Determine the endpoints of an edge from its inferiors
//---------------------------------------------------------------------------
void vtol_edge::set_vertices_from_zero_chains(void)
{
  vtol_topology_object_sptr zc0;
  int numverts;

  if(numinf()==1)
    // This is the normal case.
    {
      // Set _v1 to be the first on the
      // vtol_zero_chain. Set _v2 to be the last on
      // the vtol_zero_chain.
      zc0=*(inferiors()->begin());
      numverts=zc0->numinf();
      //topology_list *verts=zc0->inferiors();
      switch(numverts)
        {
        case 0:
          _v1=0;
          _v2=0;
          break;
        case 1:
          _v1=(vtol_vertex *)((*(zc0->inferiors()))[0].ptr());
          _v2=_v1;
          break;
        default:
          _v1=(vtol_vertex *)((*(zc0->inferiors()))[0].ptr());
          _v2=(vtol_vertex *)((*(zc0->inferiors()))[(numverts-1)].ptr());
          break;
        }
    }
  else
    // This is a Strange case but we`ll take care of it.
    // Assuming zero_chains have an ordering,
    // Set _v1 to the first vertex and Set _v2
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
              _v1=0;
              _v2=0;
              break;
            default:
              _v1=(*verts)[0];
              _v2=(*verts)[numverts-1];
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
// Replaces dynamic_cast<T>
//***************************************************************************

//---------------------------------------------------------------------------
//: Return `this' if `this' is an edge, 0 otherwise
//---------------------------------------------------------------------------
const vtol_edge * vtol_edge::cast_to_edge(void) const
{
  return this;
}

//---------------------------------------------------------------------------
//: Return `this' if `this' is an edge, 0 otherwise
//---------------------------------------------------------------------------
vtol_edge * vtol_edge::cast_to_edge(void)
{
  return this;
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


  if(!compare_geometry(other))
    return false;

#if 0
  if ( (_curve && !other._curve) ||
  (!_curve && other._curve) )
    return false;

  if (_curve && (*_curve)!=(*other._curve))
    return false;
#endif

  if (!(*_v1==*(other._v1)) || !(*_v2==*(other._v2))) // ((*_v1!=*(other._v1)) || (*_v2!=*(other._v2)))
    return false;

  vtol_zero_chain_sptr zc1=zero_chain();
  vtol_zero_chain_sptr zc2=other.zero_chain();
  if (!zc1||!zc2)
    return false;
  return *zc1==*zc2;
}

//: spatial object equality

bool vtol_edge::operator==(const vsol_spatial_object_3d& obj) const
{
  return obj.spatial_type() == vsol_spatial_object_3d::TOPOLOGYOBJECT &&
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
// These vertices are _v1 and _v2 in that order.
vertex_list *vtol_edge::endpoints(void)
{
  vertex_list *newl=new vcl_vector<vtol_vertex_sptr>();
  if(_v1)
    newl->push_back(_v1);
  if(_v2)
    newl->push_back(_v2);
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
  for(i1=thisedges->begin();i1!=thisedges->end();++i1 )
    for(i2=eedges->begin();i2!=eedges->end(); ++i2 )
      if((*i1)==(*i2))
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
  if(uglyvert==*_v1)
    set_v1(0);
  else if(uglyvert==*_v2)
    set_v2(0);
  else
    return false;
  touch();
  return true;
}

bool vtol_edge::is_endpoint(const vtol_vertex &v) const
{
  return (is_endpoint1(v)||is_endpoint2(v));
}


//: Returns True if v is equal to the first vtol_edge endpoint,_v1.
bool vtol_edge::is_endpoint1(const vtol_vertex &v) const
{
  return *_v1==v;
}

//: Returns True if v is equal to the second vtol_edge endpoint, _v2.
bool vtol_edge::is_endpoint2(const vtol_vertex &v) const
{
  return *_v2 ==v;
}


//: This method works only for ImplicitLine edges.
vtol_vertex_sptr vtol_edge::other_endpoint(const vtol_vertex &overt) const
{
  if (overt==*_v1)
    return _v2;
  else if (overt==*_v2)
    return _v1;
  else
    return 0;
}

//-----------------------------------------------------------------
//: Compute bounds from the geometry of _curve.
// If the curve is not fully bounded, then use the vertices.
//
void vtol_edge::compute_bounding_box(void)
{
  // TODO
#if 0
  if(_curve && _curve->GetGeometryType() != GeometryObject::IMPLICITLINE)
     {
      // Get bounds from curve.
      // But are curve endpoints consistent with vertices? -JLM
      // Anyway, this is what was done in get_min_max
       if(_curve->GetExtent() == vsol_curve::FULLY_BOUNDED)
         {
           this->set_min_x(_curve->get_min_x());
           this->set_min_y(_curve->get_min_y());
           // this->set_min_z(_curve->get_min_z());

           this->set_max_x(_curve->get_max_x());
           this->set_max_y(_curve->get_max_y());
           //  this->set_max_z(_curve->get_max_z());
         }
     }

  else  // Just use the generic method computing bounds from vertices
    this->vtol_topology_object::compute_bounding_box();
#endif
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
  if(_v1) {
    _v1->print(strm);
  } else {
    strm << "Null vertex 1" << vcl_endl;
  }
  for (int i3=0; i3<blanking; ++i3) strm << ' ';
  if(_v2) {
    _v2->print(strm);
  } else {
    strm << "Null vertex 2" << vcl_endl;
  }
}

//:
// This method outputs a brief vtol_edge info with vtol_edge object address.
void vtol_edge::print(vcl_ostream &strm) const
{
   strm<<"<vtol_edge  "<<"  "<<(void const *)this <<"> with id "<<get_id()<<vcl_endl;
}
