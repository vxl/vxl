#include <vtol/vtol_edge_2d.h>

#include <vtol/vtol_zero_chain_2d.h>
#include <vtol/vtol_one_chain_2d.h>
#include <vtol/vtol_macros_2d.h>
#include <vtol/vtol_list_functions_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Name: vtol_edge_2d
// Task: Default constructor. Empty edge. Not a valid edge.
//---------------------------------------------------------------------------
vtol_edge_2d::vtol_edge_2d(void) 
{
  _curve=0;
  _v1=0;
  _v2=0;
}

//---------------------------------------------------------------------------
// Name: vtol_edge_2d
// Task: Constructor from the two endpoints `new_v1', `new_v2' and from a
//       curve `new_curve'. If `new_curve' is 0, a line is created from
//       `new_v1' and `new_v2'.
//---------------------------------------------------------------------------
vtol_edge_2d::vtol_edge_2d(vtol_vertex_2d &new_v1,
                           vtol_vertex_2d &new_v2,
                           const vsol_curve_2d_ref &new_curve)
{
  vtol_topology_object_2d *zc;
  
  if(new_curve.ptr()==0)
    _curve=new vsol_line_2d(new_v1.point(),new_v2.point());
  else
    _curve=new_curve;
  _v1=&new_v1;
  _v2=&new_v2;
  zc=new vtol_zero_chain_2d(new_v1,new_v2);
  link_inferior(*zc);
}

//---------------------------------------------------------------------------
// Name: vtol_edge_2d
// Task: Copy constructor
//---------------------------------------------------------------------------

//:
// Copy constructor for an vtol_edge_2d. This methods performs a deep copy of
// the elements of the old vtol_edge_2d, olde, and sets the corresponding member
// data of the new vtol_edge_2d.

vtol_edge_2d::vtol_edge_2d(const vtol_edge_2d &other)
{
  _curve=0;
  vsol_curve_2d_ref tmp_curve;

  vcl_vector<vtol_topology_object_2d_ref>::const_iterator i;

  for(i=other.inferiors()->begin();i!=other.inferiors()->end();++i)
    link_inferior(*(vtol_zero_chain_2d *)((*i)->clone().ptr()));

  set_vertices_from_zero_chains();
  if(other._curve.ptr()!=0)
    {
      _curve=(vsol_curve_2d *)(other._curve->clone().ptr());
      // make sure the geometry and Topology are in sync
      if(_v1.ptr()!=0)
	{
          _curve->set_p0(_v1->point());
          _curve->touch();
	}
      if(_v2.ptr()!=0)
	{
          _curve->set_p1(_v2->point());
          _curve->touch();
	}
    }
  touch();
}

//---------------------------------------------------------------------------
// Name: vtol_edge_2d
// Task: Constructor from a zero-chain.
//---------------------------------------------------------------------------

//:
// Constructor for an vtol_edge_2d. If the vtol_zero_chain_2d has two vertices , then the
// first and last vertices of the vtol_zero_chain_2d are used for endpoints and
// an ImplicitLine is assumed to be the curve.  Otherwise, the all data
// (_v1, _v2, _curve) are set to NULL.  The vtol_zero_chain_2d, newchain, becomes
// the Inferior of the vtol_edge_2d.

vtol_edge_2d::vtol_edge_2d(vtol_zero_chain_2d &new_zero_chain)
{
  link_inferior(new_zero_chain);
  set_vertices_from_zero_chains();
  if(new_zero_chain.numinf()==2)
    // Safe to assume that it is an Implicit Line.
    _curve=new vsol_line_2d(_v1->point(),_v2->point());
  else
    // User must set the type of curve needed.
    // Since guessing could get confusing.
    // So NULL indicates an edge of unknown type.
    _curve=0;
  touch();
}

//:
// Constructor for an vtol_edge_2d. The list of zero_chains, newchains, is
// assumed to be ordered along an edge. This method assigns the first
// vertex in the chain list to _v1, and assigns the last vertex in the
// chain list to _v2. No assumptions are made as to the curve type. The
// data member, _curve is left to be NULL.

vtol_edge_2d::vtol_edge_2d(zero_chain_list_2d &newchains)
{
  // 1) Link the inferiors.
  zero_chain_list_2d::iterator i;

  for (i=newchains.begin();i!= newchains.end();++i )
    link_inferior(*(*i));

  // 2) Set _v1 and _v2;

  set_vertices_from_zero_chains();
  _curve=0;
}

//:
// Constructor for a Linear vtol_edge_2d.  The coordinates, (x1, y1, z1),
// determine vtol_vertex_2d, _v1.  The coordinates, (x2, y2, z2), determine _v2.
// If curve is NULL, an ImplicitLine is generated for the vtol_edge_2d.

// vtol_edge_2d::vtol_edge_2d(double x1, double y1, double z1, double x2, double y2, double z2, vsol_curve_2d* curve)
vtol_edge_2d::vtol_edge_2d(double x1,
                           double y1,
                           double x2,
                           double y2,
                           vsol_curve_2d_ref curve)
 
{
  _v1=new vtol_vertex_2d(x1,y1);
  _v2=new vtol_vertex_2d(x2,y2);
  if(curve==0)
    _curve=new vsol_line_2d(_v1->point(),_v2->point());
  else
    _curve=(vsol_curve_2d*)(curve->clone().ptr());

  vtol_zero_chain_2d *inf=new vtol_zero_chain_2d(*_v1,*_v2);
  link_inferior(*inf);
}

//:
// Constructor for an vtol_edge_2d from a Curve. If edgecurve is of ImplicitLine
// type, vertex locations for endpoints, _v1 and _v2, are computed from
// the ImplicitLine parameters.  If edgecurve is of any other type, _v1
// and _v2 are left as NULL.
// (Actually, this description is wrong. The endpoints are retreived
// from the curve, regardless of its type. -JLM)
vtol_edge_2d::vtol_edge_2d(vsol_curve_2d &edgecurve)
{
  vtol_zero_chain_2d *newzc;
  if(_curve!=0)
    {
      //  _v1 = new vtol_vertex_2d(&_curve->get_start_point());
      // _v2 = new vtol_vertex_2d(&_curve->get_end_point());
      newzc=new vtol_zero_chain_2d(*_v1,*_v2);
    } else {
      _v1=0;
      _v2=0;
      _curve=0;
      newzc=new vtol_zero_chain_2d();
    }
  link_inferior(*newzc);
}

//---------------------------------------------------------------------------
// Name: clone
// Task: Clone `this': creation of a new object and initialization
//       See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_ref vtol_edge_2d::clone(void) const
{
  return new vtol_edge_2d(*this);
}

//---------------------------------------------------------------------------
// Name: curve
// Task: Return the curve associated to `this'
//---------------------------------------------------------------------------
vsol_curve_2d_ref vtol_edge_2d::curve(void) const
{
  return _curve;
}


//---------------------------------------------------------------------------
// Name: zero_chain
// Task: Return the first zero-chain of `this'
//---------------------------------------------------------------------------
vtol_zero_chain_2d_ref vtol_edge_2d::zero_chain(void) const
{
  return (vtol_zero_chain_2d *)(_inferiors[0].ptr());
}
 
//---------------------------------------------------------------------------
// Name: set_curve
// Task: Set the curve with `new_curve'
//---------------------------------------------------------------------------
void vtol_edge_2d::set_curve(vsol_curve_2d &new_curve)
{
  _curve=&new_curve;
  touch(); //Update timestamp
}

//---------------------------------------------------------------------------
// Name: ~vtol_edge_2d
// Task: Destructor
//---------------------------------------------------------------------------
vtol_edge_2d::~vtol_edge_2d()
{
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
// Name: set_v1
// Task: Set the first endpoint.
//---------------------------------------------------------------------------
void vtol_edge_2d::set_v1(vtol_vertex_2d *new_v1)
{
  if((_v1!=0)&&(_v1!=_v2))
    zero_chain()->unlink_inferior(*_v1);
  _v1=new_v1;
  zero_chain()->link_inferior(*_v1);
  touch();
}

//---------------------------------------------------------------------------
// Name: set_v2
// Task: Set the last endpoint.
//---------------------------------------------------------------------------
void vtol_edge_2d::set_v2(vtol_vertex_2d *new_v2)
{
  if((_v2!=0)&&(_v2!=_v1))
    zero_chain()->unlink_inferior(*_v2);
  _v2=new_v2;
  zero_chain()->link_inferior(*_v2);
  touch();
}

//---------------------------------------------------------------------------
// Name: set_end_points
// Task: Set the first and last endpoints
// Require: vertex_of_edge(new_v1) and vertex_of_edge(new_v2)
//---------------------------------------------------------------------------

//:
//  This function sets the edge endpoints to endpt1 and endpt2.  
//  Both endp1 and endpt2 must be vertices on the edge.
//
void vtol_edge_2d::set_end_points(vtol_vertex_2d &endpt1,
                                  vtol_vertex_2d &endpt2)
{
#if 0
  vtol_zero_chain_2d *zc=get_zero_chain();
  vcl_vector<vtol_vertex_2d*> *verts=zc->vertices();
  
  if((vcl_find(verts->begin(),verts->end(),endpt1)==verts->end())||
     (vcl_find(verts->begin(),verts->end(),endpt2)==verts->end()))
    {
      cerr << "Error in vtol_edge_2d::set_end_points(): both endpoints must be on this vtol_edge_2d.\n";
      delete verts;
    }
  _v1=endpt1;
  _v2=endpt2;
  delete verts;

#endif
}

//:
//     This function removes curendpt from the edge and replaces it with
//     newendpt.  Both curendpt and newendpt must be non-NULL pointers, 
//     curendpt must point to an endpoint of the edge (either _v1 or
//     _v2), and newendpt must be colinear with the edge.  Neither vertex 
//     is destroyed in the process; only links are manipulated.
//     (RYF 7-14-98)
//
void vtol_edge_2d::replace_end_point(vtol_vertex_2d &curendpt,
                                     vtol_vertex_2d &newendpt)
{
  // require
  //  assert(curendpt.ptr()!=0);
  //  assert(newendpt.ptr()!=0);
  assert(curendpt==*_v1||curendpt==*_v2);

  zero_chain()->unlink_inferior(curendpt);
  zero_chain()->link_inferior(newendpt);
  if(curendpt==*_v1)  // update the appropriate endpoint
    _v1=&newendpt;
  else // curendpt == this->get_v2()
    _v2=&newendpt;
}

//---------------------------------------------------------------------------
// Name: set_vertices_from_zero_chains
// Task: Determine the endpoints of an edge from its inferiors
//---------------------------------------------------------------------------
void vtol_edge_2d::set_vertices_from_zero_chains(void)
{
  vtol_topology_object_2d_ref zc0;
  int numverts;

  if(numinf()==1)
    // This is the normal case.
    {
      // Set _v1 to be the first on the
      // vtol_zero_chain_2d. Set _v2 to be the last on
      // the vtol_zero_chain_2d.
      zc0=*(inferiors()->begin());
      numverts=zc0->numinf();
      //topology_list_2d *verts=zc0->inferiors();
      switch(numverts)
        {
        case 0:
          _v1=0;
          _v2=0;
          break;
        case 1:
          _v1=(vtol_vertex_2d *)((*(zc0->inferiors()))[0].ptr());
          _v2=_v1;
          break;
        default:
          _v1=(vtol_vertex_2d *)((*(zc0->inferiors()))[0].ptr());
          _v2=(vtol_vertex_2d *)((*(zc0->inferiors()))[(numverts-1)].ptr());
          break;
        }
    }
  else
    // This is a Strange case but we`ll take care of it.
    // Assuming zero_chains have an ordering,
    // Set _v1 to the first vertex and Set _v2
    // to be the last vertex.  They will not
    // be on the same vtol_zero_chain_2d.
    {
      vertex_list_2d *verts=vertices();
      
      if(verts!=0)
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
// This is a utility method that adds new_edge_loop to the vtol_edge_2d's
// superior list.  It returns a boolean value reflecting the success of
// linking.

void vtol_edge_2d::add_edge_loop(vtol_one_chain_2d &new_edge_loop)
{
  new_edge_loop.link_inferior(*this);
}

//:
// This is a utility method that removes doomed_edge_loop from the
// vtol_edge_2d's superior list. It returns a boolean value reflecting the
// success of removing.

void vtol_edge_2d::remove_edge_loop(vtol_one_chain_2d &doomed_edge_loop)
{
  doomed_edge_loop.unlink_inferior(*this);
}

//***************************************************************************
// Replaces dynamic_cast<T>
//***************************************************************************

//---------------------------------------------------------------------------
// Name: cast_to_edge
// Task: Return `this' if `this' is an edge, 0 otherwise
//---------------------------------------------------------------------------
const vtol_edge_2d * vtol_edge_2d::cast_to_edge(void) const
{
  return this;
}

//---------------------------------------------------------------------------
// Name: cast_to_edge
// Task: Return `this' if `this' is an edge, 0 otherwise
//---------------------------------------------------------------------------
vtol_edge_2d * vtol_edge_2d::cast_to_edge(void)
{
  return this;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Name: valid_inferior_type
// Task: Is `inferior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_edge_2d::valid_inferior_type(const vtol_topology_object_2d &inferior) const
{
  return inferior.cast_to_zero_chain()!=0;
}

//---------------------------------------------------------------------------
// Name: valid_superior_type
// Task: Is `superior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_edge_2d::valid_superior_type(const vtol_topology_object_2d &superior) const
{
  return superior.cast_to_one_chain()!=0;
}

/*
 ******************************************************
 *
 *    Operators
 */

//:
// operators

bool vtol_edge_2d::operator==(const vtol_edge_2d &other) const
{
  if (this==&other) return true;

  if ( (_curve.ptr() && other._curve.ptr()==0) ||
       (_curve.ptr()==0 && other._curve.ptr()) )
    return false;

  if (_curve.ptr() && (*_curve)!=(*other._curve))
    return false;

  if ((*_v1!=*(other._v1)) || (*_v2!=*(other._v2)))
    return false;

  vtol_zero_chain_2d_ref zc1=zero_chain();
  vtol_zero_chain_2d_ref zc2=other.zero_chain();
  if ((zc1.ptr()==0)||(zc2.ptr()==0))
    return false;
  return *zc1==*zc2;
}

//:
// spatial object equality

bool vtol_edge_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::TOPOLOGYOBJECT &&
   ((vtol_topology_object_2d const&)obj).topology_type() == vtol_topology_object_2d::EDGE
  ? *this == (vtol_edge_2d const&) (vtol_topology_object_2d const&) obj
  : false;
}

/*
 ******************************************************
 *
 *    Inferior/Superior Accessor Functions
 */

//:
// Returns a list of vertices on the vtol_edge_2d.

vcl_vector<vtol_vertex_2d *> *vtol_edge_2d::compute_vertices(void)
{
  SEL_INF_2d(vtol_vertex_2d,compute_vertices);
}

//:
// Returns the vtol_zero_chain_2d list of the vtol_edge_2d. This list is the Inferiors
// of the edge.
vcl_vector<vtol_zero_chain_2d *> *vtol_edge_2d::compute_zero_chains(void)
{
  COPY_INF_2d(vtol_zero_chain_2d);
}

//:
// Returns a list with itself as the only element. This utility is used
// in Inferior/Superior accessing methods.
vcl_vector<vtol_edge_2d *> *vtol_edge_2d::compute_edges(void)
{ 
  LIST_SELF_2d(vtol_edge_2d);
}

//:
// Returns a list of one_chains which contain the vtol_edge_2d. This list is the
// Superiors of the edge.
vcl_vector<vtol_one_chain_2d *> *vtol_edge_2d::compute_one_chains(void)
{
  SEL_SUP_2d(vtol_one_chain_2d,compute_one_chains);
}


//:
// Returns a list of the faces which contain the vtol_edge_2d.
vcl_vector<vtol_face_2d *> *vtol_edge_2d::compute_faces(void)
{ 
  SEL_SUP_2d(vtol_face_2d,compute_faces);
}

//:
//  Returns the list of two_chains which contain the vtol_edge_2d.
vcl_vector<vtol_two_chain_2d *> *vtol_edge_2d::compute_two_chains(void)
{
  SEL_SUP_2d(vtol_two_chain_2d,compute_two_chains);
}

//:
//  Returns the list of blocks which contain the vtol_edge_2d.
vcl_vector<vtol_block_2d *> *vtol_edge_2d::compute_blocks(void)
{
  SEL_SUP_2d(vtol_block_2d,compute_blocks);
}

//:
// Returns a list of vertices containing the endpoints of the edge.
// These vertices are _v1 and _v2 in that
// order.
vertex_list_2d *vtol_edge_2d::endpoints(void)
{
  vertex_list_2d *newl=new vcl_vector<vtol_vertex_2d_ref>();
  if(_v1.ptr()!=0)
    newl->push_back(_v1);
  if(_v2.ptr()!=0)
    newl->push_back(_v2);
  return newl;
}

//:
//  Returns true if the invoking edge has a vertex in common with
//  vtol_edge_2d `other'.  The method determines if the two edges share a vertex
//  by comparing pointer values, not the vertex geometry.
//

bool vtol_edge_2d::share_vertex_with(vtol_edge_2d &other)
{
  vertex_list_2d *thisedges=vertices();
  vertex_list_2d *eedges=other.vertices();
  
  vcl_vector<vtol_vertex_2d_ref>::iterator i1;
  vcl_vector<vtol_vertex_2d_ref>::iterator i2;
  for(i1=thisedges->begin();i1!=thisedges->end();++i1 )
    for(i2=eedges->begin();i2!=eedges->end(); ++i2 )
      if((*i1)==(*i2))
        return true;
  return false;
}

//:
// This method adds newvert to the vtol_edge_2d by linking it to one of the
// zero_chains of the vtol_edge_2d Inferiors. (Method needs work.)

bool vtol_edge_2d::add_vertex(vtol_vertex_2d &newvert)
{
  vtol_zero_chain_2d_ref zc;

  zc=zero_chain();
  if(zc==0)
    {
      zc=new vtol_zero_chain_2d;
      link_inferior(*zc);
    }

  zc->link_inferior(newvert);
  return true;
}

//:
// This method removes uglyvert from the vtol_edge_2d by removing it from the
// inferior zero_chains.  (Method needs work.)

bool vtol_edge_2d::remove_vertex(vtol_vertex_2d &uglyvert)
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

bool vtol_edge_2d::is_endpoint(const vtol_vertex_2d &v) const
{
  return (is_endpoint1(v)||is_endpoint2(v));
}


//:
// Returns True if v is equal to the first vtol_edge_2d endpoint,_v1.
bool vtol_edge_2d::is_endpoint1(const vtol_vertex_2d &v) const
{
  return *_v1==v;
}

//:
// Returns True if v is equal to the second vtol_edge_2d endpoint, _v2.
bool vtol_edge_2d::is_endpoint2(const vtol_vertex_2d &v) const
{
  return *_v2 ==v;
}


//:
// This method works only for ImplicitLine edges.
vtol_vertex_2d_ref vtol_edge_2d::other_endpoint(const vtol_vertex_2d &overt) const
{
  if (overt==*_v1)
    return _v2;
  else if (overt==*_v2)
    return _v1;
  else
    return 0;
}

//:
//-----------------------------------------------------------------
// :  Compute bounds from the geometry of _curve. If the curve is
//    not fully bounded, then use the vertices.
//
void vtol_edge_2d::compute_bounding_box(void)
{
  /* TODO
  if(_curve && _curve->GetGeometryType() != GeometryObject::IMPLICITLINE)
     {
      // Get bounds from curve.
      // But are curve endpoints consistent with vertices? -JLM
      // Anyway, this is what was done in get_min_max
       if(_curve->GetExtent() == vsol_curve_2d::FULLY_BOUNDED)
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
    this->vtol_topology_object_2d::compute_bounding_box();
  */
}


/*
 ******************************************************
 *
 *    I/O methods
 */

//:
// This method outputs all edge information to the ostream, strm.  It
// indents various levels of output by the number given in blanking.
void vtol_edge_2d::describe(ostream &strm,
                            int blanking) const
{
  for (int i1=0; i1<blanking; ++i1) strm << ' ';
  print(strm);
  for (int i2=0; i2<blanking; ++i2) strm << ' ';
  if(_v1) {
    _v1->print(strm);
  } else {
    strm << "Null vertex 1" << endl;
  }
  for (int i3=0; i3<blanking; ++i3) strm << ' ';
  if(_v2) {
    _v2->print(strm);
  } else {
    strm << "Null vertex 2" << endl;
  }
}

//:
// This method outputs a brief vtol_edge_2d info with vtol_edge_2d object address.
void vtol_edge_2d::print(ostream &strm) const
{
   strm<<"<vtol_edge_2d  "<<"  "<<(void *)this <<"> with id "<<get_id()<<endl;
}
