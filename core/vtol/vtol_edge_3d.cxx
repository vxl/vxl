#include "vtol_edge_3d.h"

#ifndef GNU_LIBSTDCXX_V3
# include <vcl/vcl_rel_ops.h>
#endif

// #include <vtol/vtol_vertex_3d.h>
#include <vtol/vtol_zero_chain_3d.h>
#include <vtol/vtol_one_chain_3d.h>
// #include <vtol/vtol_two_chain_3d.h>
// #include <Geometry/IUPoint.h>
// #include <Geometry/Curve.h>
#include <vtol/vtol_macros_3d.h>
#include <vtol/vtol_list_functions_3d.h>
// #include <vtol/vtol_face_3d.h>

//#include <vtol/some_stubs.h>
#include <vsol/vsol_curve_3d.h>
#include <vsol/vsol_line_3d.h>

/*
 ******************************************************
 *
 *    Manager Functions
 */


// --
// Empty constructor for the vtol_edge_3d class.  This constructor creates the
// skeleton of an edge with *NO* defaults for vertex endpoints.
// Programmers should use this with caution since many methods that
// operate on an vtol_edge_3d assume the vtol_edge_3d is valid.  This constructor does
// NOT produce a valid vtol_edge_3d.

vtol_edge_3d::vtol_edge_3d(void) 
{
  _curve=0;
  _v1=0;
  _v2=0;
  vtol_zero_chain_3d *inf=new vtol_zero_chain_3d();
  link_inferior(inf);
}

// --
// vtol_edge_3d(vtol_vertex_3d* vert1, vtol_vertex_3d* vert2, Curve* curve)--
// Constructor for an vtol_edge_3d.  If the curve is NULL then
// the vertices, vert1 and vert2, are used as endpoints and
// an ImplicitLine is generated for the vtol_edge_3d, a linear edge.

vtol_edge_3d::vtol_edge_3d(vtol_vertex_3d *vert1,
                           vtol_vertex_3d *vert2,
                           vsol_curve_3d_ref curve) 
{
  if (!curve)
    // _curve =  new ImplicitLine(vert1->get_point(), vert2->get_point());
    // TODO
    _curve=new vsol_line_3d(vert1->get_point(),vert2->get_point());
  else
    _curve=curve;
  _v1=vert1;
  _v2=vert2;
  vtol_zero_chain_3d *inf=new vtol_zero_chain_3d(vert1,vert2);
  link_inferior(inf);
}


// --
// Copy constructor for an vtol_edge_3d. This methods performs a deep copy of
// the elements of the old vtol_edge_3d, olde, and sets the corresponding member
// data of the new vtol_edge_3d.

vtol_edge_3d::vtol_edge_3d(const vtol_edge_3d&olde)
{
  // This is a deep copy of the vtol_edge_3d.
  vtol_edge_3d *old_e=(vtol_edge_3d*)(&olde);
  vtol_zero_chain_3d *zeroch;
  _curve=0;

  for(int i=0;i<old_e->_inferiors.size();++i)
    {
      zeroch=((vtol_zero_chain_3d *)old_e->_inferiors[i])->copy();
      link_inferior(zeroch);
    }
  set_vertices_from_zero_chains();
  if (old_e->_curve)
    {
      vsol_curve_3d_ref curve=(vsol_curve_3d *)(old_e->_curve->clone().ptr());
      // make sure the geometry and Topology are in sync
      if(_v1)
	{
	  // TODO
	  // curve->set_start(_v1->get_point());
	curve->touch();
	}
      if(_v2)
	{
	  // TODO
	  // curve->set_end(_v2->get_point());
	curve->touch();
	}
      set_curve(curve);
    }
  else
    _curve=0;
}


// --
// Constructor for an vtol_edge_3d. If the vtol_zero_chain_3d has two vertices , then the
// first and last vertices of the vtol_zero_chain_3d are used for endpoints and
// an ImplicitLine is assumed to be the curve.  Otherwise, the all data
// (_v1, _v2, _curve) are set to NULL.  The vtol_zero_chain_3d, newchain, becomes
// the Inferior of the vtol_edge_3d.

vtol_edge_3d::vtol_edge_3d(vtol_zero_chain_3d *newchain)
{
  link_inferior(newchain);
  set_vertices_from_zero_chains();
  if (newchain->numinf()==2)
    // Safe to assume that it is an Implicit Line.
    {
      // TODO
      // _curve = new ImplicitLine(_v1->get_point(), _v2->get_point());
    }
  else
    // User must set the type of curve needed.
    // Since guessing could get confusing.
    // So NULL indicates an edge of unknown type.
    _curve=0;
}

// --
// Constructor for an vtol_edge_3d. The list of zero_chains, newchains, is
// assumed to be ordered along an edge. This method assigns the first
// vertex in the chain list to _v1, and assigns the last vertex in the
// chain list to _v2. No assumptions are made as to the curve type. The
// data member, _curve is left to be NULL.

vtol_edge_3d::vtol_edge_3d(vcl_vector<vtol_zero_chain_3d*> &newchains)
{
  // 1) Link the inferiors.
  vcl_vector<vtol_zero_chain_3d*>::iterator i;

  for (i=newchains.begin();i!= newchains.end();++i )
    link_inferior((*i));

  // 2) Set _v1 and _v2;

  set_vertices_from_zero_chains();
  _curve=0;
}


// --
// Constructor for a Linear vtol_edge_3d.  The TopologyObjects, newv1 and newv2,
// are typecast to vtol_vertex_3d* and used as endpoints. If curve == NULL, an
// ImplicitLine is generated for the vtol_edge_3d, a linear edge, otherwise curve is
// used.
vtol_edge_3d::vtol_edge_3d(vtol_topology_object_3d *newv1,
                           vtol_topology_object_3d *newv2,
                           vsol_curve_3d_ref curve)
{
  _v1=newv1->cast_to_vertex_3d();
  _v2=newv2->cast_to_vertex_3d();
  vtol_zero_chain_3d *inf;
  if(_v1&&_v2)
    {
      if (!curve)
	// TODO
        // _curve = new ImplicitLine(_v1->GetPoint(), _v2->GetPoint());
	_curve=new vsol_line_3d(_v1->get_point(),_v2->get_point());
      else
        _curve=(vsol_curve_3d*)(curve->clone().ptr());
      inf=new vtol_zero_chain_3d(_v1,_v2);
    }
  else
    {
      _curve=0;
      inf=new vtol_zero_chain_3d;
    }
  link_inferior(inf);
}


// --
// Constructor for a Linear vtol_edge_3d.  The coordinates, (x1, y1, z1),
// determine vtol_vertex_3d, _v1.  The coordinates, (x2, y2, z2), determine _v2.
// If curve is NULL, an ImplicitLine is generated for the vtol_edge_3d.

// vtol_edge_3d::vtol_edge_3d(double x1, double y1, double z1, double x2, double y2, double z2, vsol_curve_3d* curve)
vtol_edge_3d::vtol_edge_3d(double x1,
                           double y1,
                           double x2,
                           double y2,
                           double z1,
                           double z2,
                           vsol_curve_3d_ref curve)
 
{
  _v1=new vtol_vertex_3d(x1,y1,z1);
  _v2=new vtol_vertex_3d(x2,y2,z1);
  if (!curve)
    // TODO
    //_curve = new ImplicitLine(_v1->GetPoint(), _v2->GetPoint());
    _curve=new vsol_line_3d(_v1->get_point(),_v2->get_point());
  else
    {
      // TODO
      _curve=(vsol_curve_3d*)(curve->clone().ptr());
      // _curve->SetStart(_v1->GetPoint());
      // _curve->SetEnd(_v2->GetPoint());
      // _curve->UpdateGeometry();
    }

  // _curve->Protect();
  vtol_zero_chain_3d *inf=new vtol_zero_chain_3d(_v1,_v2);
  link_inferior(inf);
}

// --
// Constructor for an vtol_edge_3d from a Curve. If edgecurve is of ImplicitLine
// type, vertex locations for endpoints, _v1 and _v2, are computed from
// the ImplicitLine parameters.  If edgecurve is of any other type, _v1
// and _v2 are left as NULL.
// (Actually, this description is wrong. The endpoints are retreived
// from the curve, regardless of its type. -JLM)
vtol_edge_3d::vtol_edge_3d(vsol_curve_3d_ref edgecurve)
{
  vtol_zero_chain_3d *newzc;
  // _curve = (vsol_curve_3d*)(edgecurve->spatial_copy());
  // _curve->Protect();
  if (_curve)
    {
      //  _v1 = new vtol_vertex_3d(&_curve->get_start_point());
      // _v2 = new vtol_vertex_3d(&_curve->get_end_point());
      newzc=new vtol_zero_chain_3d(_v1,_v2);
    } else {
      _v1=0;
      _v2=0;
      _curve=0;
      newzc=new vtol_zero_chain_3d();
    }
  link_inferior(newzc);
}

//---------------------------------------------------------------------------
// -- Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_ref vtol_edge_3d::clone(void) const
{
  return new vtol_edge_3d(*this);
}


vtol_topology_object_3d *vtol_edge_3d::shallow_copy_with_no_links(void)
{
  vtol_edge_3d *newedge=new vtol_edge_3d(*this); 
  topology_list_3d *infs=newedge->get_inferiors();
  if (infs->size()!=1) 
    cerr << "Error in vtol_edge_3d::shallow_copy_with_no_links():\n"
         << "vtol_edge_3d does not have exactly 1 inferior\n";
  vtol_topology_object_3d *inf=(*infs)[0];
  newedge->unlink_inferior(inf);
  iu_delete(inf);
  newedge->_v1=newedge->_v2=0;
  return newedge;
}

//---------------------------------------------------------------------------
// -- Return the topology type
//---------------------------------------------------------------------------
vtol_edge_3d::vtol_topology_object_3d_type
vtol_edge_3d::topology_type(void) const
{
  return EDGE;
}

void vtol_edge_3d::set_curve(vsol_curve_3d_ref newcurve)
{
  _curve=newcurve;
  this->touch(); //Update timestamp
}

// -- Set new curve and return old one for deletion, or unprotect.

void vtol_edge_3d::set_curve(vsol_curve_3d_ref newcurve,
                             vsol_curve_3d_ref &oldcurve)
{
  // if(_curve == newcurve) {
  //  oldcurve = NULL;
  //  return;
  //  }
  oldcurve=_curve;
  _curve=newcurve;
  this->touch(); //Update timestamp
}

// -- Destructor for an vtol_edge_3d.
vtol_edge_3d::~vtol_edge_3d()
{
}


/*
 ******************************************************
 *
 *    Accessor Functions
 */

// --
// SetV1 assigns the first endpoint of the vtol_edge_3d, _v1 to be the argument,
// v.  This function has the side effect of removing the current _v1
// from the inferior zero_chains and adding the new _v1 to the first
// vtol_zero_chain_3d in the inferiors. (Note: Method needs work.)

void vtol_edge_3d::set_v1(vtol_vertex_3d *v)
{
  vtol_zero_chain_3d *zc=get_zero_chain();
  if(_v1&&(_v1!=_v2))
    {
      zc->unlink_inferior(_v1);
      iu_delete(_v1);
    }
  _v1=v;
  zc->link_inferior(_v1);
  this->touch();
}

// --
// SetV2 assigns the second endpoint of the vtol_edge_3d, _v2 to be the argument,
// v.  This function has the side effect of removing the current _v2
// from the inferior zero_chains and adding the new _v2 to the first
// vtol_zero_chain_3d in the inferiors.  (Note: Method needs work.)

void vtol_edge_3d::set_v2 (vtol_vertex_3d *v)
{
 vtol_zero_chain_3d *zc=get_zero_chain();
 if(_v2&&(_v1!=_v2))
   {
     zc->unlink_inferior(_v2);
     iu_delete(_v2);
   }
  _v2=v;
  zc->link_inferior(_v2);
 this->touch();
}


// --
//  This function sets the edge endpoints to endpt1 and endpt2.  
//     Both endp1 and endpt2 must be vertices on the edge.
//
bool vtol_edge_3d::set_end_points(vtol_vertex_3d *endpt1,
                                  vtol_vertex_3d *endpt2)
{
  vtol_zero_chain_3d *zc=get_zero_chain();
  vcl_vector<vtol_vertex_3d*> *verts=zc->vertices();
  
  if((vcl_find(verts->begin(),verts->end(),endpt1)==verts->end())||
     (vcl_find(verts->begin(),verts->end(),endpt2)==verts->end()))
    {
      cerr << "Error in vtol_edge_3d::set_end_points(): both endpoints must be on this vtol_edge_3d.\n";
      delete verts;
      return false;
    }
  _v1=endpt1;
  _v2=endpt2;
  delete verts;
  return true;
}

// --
//  This function removes curendpt from the edge and replaces it with
//     newendpt.  Both curendpt and newendpt must be non-NULL pointers, 
//     curendpt must point to an endpoint of the edge (either _v1 or
//     _v2), and newendpt must be colinear with the edge.  Neither vertex 
//     is destroyed in the process; only links are manipulated.
//     (RYF 7-14-98)
//
bool vtol_edge_3d::replace_end_point(vtol_vertex_3d *curendpt,
                                     vtol_vertex_3d *newendpt)
{
  // Some error checking
  if (!curendpt||!newendpt)
    {
      cerr << "Error in vtol_edge_3d::replace_end_point(): arguments can not be NULL.\n";
      return false;
    }
  
  if((curendpt!=this->get_v1())&&(curendpt!=this->get_v2()))
    {
      cerr << "Error in vtol_edge_3d::replace_end_point(): first argument must be _v1 or _v2.\n";
      return false;
    }
  
  vtol_zero_chain_3d *zc=get_zero_chain();
  zc->unlink_inferior(curendpt);  // remove curendpt
  zc->link_inferior(newendpt);    // add newendpt
  if(curendpt==this->get_v1())  // update the appropriate endpoint
    _v1=newendpt;
  else // curendpt == this->get_v2()
    _v2=newendpt;
  return true;
}

// --
// SetverticesFromzero_chains determines the endpoints of an edge from
// the inferiors list of zero_chains, and assigns the data members _v1
// and _v2. The _Inferiors list of the edge is assumed to be in
// sequential order of zero_chains (point sets) so that _v1 and _v2 will
// have some value as a boundary of the vtol_edge_3d.
void vtol_edge_3d::set_vertices_from_zero_chains(void)
{
  topology_list_3d *zc=get_inferiors();
  
  int numzchains=zc->size();

  if(numzchains==1)
    // This is the normal case.
    {
      // Set _v1 to be the first on the
      // vtol_zero_chain_3d. Set _v2 to be the last on
      // the vtol_zero_chain_3d.
      vtol_zero_chain_3d *zc0=(vtol_zero_chain_3d*)(*zc->begin());
      
      int numverts=zc0->numinf();
      topology_list_3d *verts=zc0->get_inferiors();
      switch(numverts)
        {
        case 0:
          _v1=0;
          _v2=0;
          break;
        case 1:
          _v1=(vtol_vertex_3d *)(*verts)[0];
          _v2=_v1;
          break;
        default:
          _v1=(vtol_vertex_3d *)(*verts)[0];
          _v2=(vtol_vertex_3d *)(*verts)[(numverts-1)];
        }
    }
  else
    // This is a Strange case but we`ll take care of it.
    // Assuming zero_chains have an ordering,
    // Set _v1 to the first vertex and Set _v2
    // to be the last vertex.  They will not
    // be on the same vtol_zero_chain_3d.
    {
      vcl_vector<vtol_vertex_3d*> *verts = vertices();
      
      if (verts != NULL)
        {
          int numverts = verts->size();
          switch(numverts)
            {
            case 0:
              _v1 = NULL;
              _v2 = NULL;
              break;
            case 1:
              _v1 = (vtol_vertex_3d*)((*verts)[0]);
              _v2 = _v1;
              break;
            default:
              _v1 = (vtol_vertex_3d *)(*verts)[0];
              _v2 = (vtol_vertex_3d *)(*verts)[(numverts - 1)];
            }
        }
      delete verts;
    }
  this->touch();
}


// --
// This is a utility method that adds new_edge_loop to the vtol_edge_3d's
// superior list.  It returns a boolean value reflecting the success of
// linking.

bool vtol_edge_3d::add_edge_loop(vtol_one_chain_3d *new_edge_loop)
{
  return link_superior(new_edge_loop);
}

// --
// This is a utility method that removes doomed_edge_loop from the
// vtol_edge_3d's superior list. It returns a boolean value reflecting the
// success of removing.

bool vtol_edge_3d::remove_edge_loop(vtol_one_chain_3d *doomed_edge_loop)
{
  return unlink_superior(doomed_edge_loop);
}


/*
 ******************************************************
 *
 *    Operators
 */

// operators

bool vtol_edge_3d::operator==(const vtol_edge_3d &e) const
{
  vtol_zero_chain_3d *zc1=(vtol_zero_chain_3d *)(this->_inferiors[0]);
  vtol_zero_chain_3d *zc2=(vtol_zero_chain_3d *)(e._inferiors[0]);
  bool equiv;
  equiv=false;

  if(this==&e)
    return true;

  if (_curve&&!e._curve||!_curve&&e._curve)
    return false;
  if(_curve&&e._curve)
    // if(*_curve != *(e._curve))
    if( !(*_curve == *(e._curve)) )
      return false;
  if((_v1==e._v1)&&(_v2==e._v2))    // pointer equivalence.
    {
      if(zc1&&zc2)
        if(*zc1==*zc2)
          equiv=true;
    }
  
  if (_v1&&e._v1&&(*_v1==*(e._v1))&&_v2&&e._v2&&(*_v2==*(e._v2)))
    {
      if(zc1&&zc2)
        if(*zc1==*zc2)
          equiv=true;
    }
  return equiv;
}

// -- spatial object equality

bool vtol_edge_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::TOPOLOGYOBJECT &&
   ((vtol_topology_object_3d const&)obj).topology_type() == vtol_topology_object_3d::EDGE
  ? *this == (vtol_edge_3d const&) (vtol_topology_object_3d const&) obj
  : false;
}


/*
 ******************************************************
 *
 *    Inferior/Superior Accessor Functions
 */

// -- Returns a list of vertices on the vtol_edge_3d.

vcl_vector<vtol_vertex_3d*> *vtol_edge_3d::vertices(void)
{
  SEL_INF_3d(vtol_vertex_3d,vertices);
}

// -- Returns the vtol_zero_chain_3d list of the vtol_edge_3d. This list is the Inferiors of the edge.
vcl_vector<vtol_zero_chain_3d*> *vtol_edge_3d::zero_chains(void)
{
  COPY_INF_3d(vtol_zero_chain_3d);
}

// -- Returns a list with itself as the only element. This utility is used in Inferior/Superior accessing methods.
vcl_vector<vtol_edge_3d*> *vtol_edge_3d::edges(void)
{
  LIST_SELF_3d(vtol_edge_3d);
}

// -- Returns a list of one_chains which contain the vtol_edge_3d. This list is the Superiors of the edge.
vcl_vector<vtol_one_chain_3d*> *vtol_edge_3d::one_chains(void)
{
  SEL_SUP_3d(vtol_one_chain_3d,one_chains);
}


// -- Returns a list of the faces which contain the vtol_edge_3d.
vcl_vector<vtol_face_3d*> *vtol_edge_3d::faces(void)
{
  SEL_SUP_3d(vtol_face_3d,faces);
}

// -- Returns the list of two_chains which contain the vtol_edge_3d.
vcl_vector<vtol_two_chain_3d*> *vtol_edge_3d::two_chains(void)
{
  SEL_SUP_3d(vtol_two_chain_3d,two_chains);
}

// -- Returns the list of blocks which contain the vtol_edge_3d.
vcl_vector<vtol_block_3d*> *vtol_edge_3d::blocks(void)
{
  SEL_SUP_3d(vtol_block_3d,blocks);
}

// -- Returns a list of vertices containing the endpoints of the edge.
// These vertices are _v1 and _v2 in that
// order.
vcl_vector<vtol_vertex_3d*> *vtol_edge_3d::endpoints(void)
{
  vcl_vector<vtol_vertex_3d *> *newl=new vcl_vector<vtol_vertex_3d *>();
  if(_v1)
    newl->push_back(_v1);
  if(_v2)
    newl->push_back(_v2);
  return newl;
}


/*
 ******************************************************
 *
 *    Utility Functions
 */

// --
//    This method removes the object from the topological structure
//    by unlinking it and then recursively checking the object's
//    superiors and inferiors to see if they should also be removed.
//    A record of the changes to the topological structure is returned 
//    through the parameters changes and deleted.  
//    Specifically, upon completion, the list deleted contains all 
//    objects that were 
//    completely unlinked (i.e. removed) from the topological 
//    structure as a result of the removal of this object.  This list
//    includes the invoking object itself.  
//    The list changes contains all
//    objects that (1) are not removed (unlinked) as a result of 
//    this operation,
//    (2) have >= 1 superior prior to calling the method, and (3)
//    have no superiors when this method completes.  In some special
//    cases, the changes list may also contain some objects that were 
//    removed (unlinked).  Therefore, to get the new top level objects 
//    after calling this fuction, use changes - deleted.
//
//    For example, suppose the edge is part of a face which is in turn 
//    part of the outside boundary of a block with a hole.  The superior 
//    topological structures that are removed as a result of the removal 
//    of this edge are: this edge, the two faces it belongs to on the 
//    block, and the block.  These will all put in the deleted list.  
//    The superior topological structures that are placed in the changes 
//    list are: the two one-chains containing the edge (because they 
//    lost their superior faces), the block's outer boundary two chain 
//    (because the block is removed) and the block's hole.  The only 
//    inferior topological structure that is removed and placed in the 
//    deleted list is the zero chain (because it lost its edge).  
//    The edge endpoint vertices are not removed because they belong to
//    other faces of the block that were not removed.  (RYF 7-16-98)
//
//
bool vtol_edge_3d::disconnect(vcl_vector< vtol_topology_object_3d *> &changes,
                              vcl_vector< vtol_topology_object_3d *> &deleted)
{
  // Remove this object from its superiors
  topology_list_3d *tmp=get_superiors();
  vcl_vector<vtol_one_chain_3d *> sup;

  topology_list_3d::iterator t;

  for(t=tmp->begin();t!=tmp->end();++t)
      sup.push_back((vtol_one_chain_3d *)(*t));
  
  vcl_vector<vtol_one_chain_3d *>::iterator s;
  for (s= sup.begin();s!= sup.end();)
    // TODO
    // (*s)->remove( this, changes, deleted );
    ;
  cerr << "Must implement vtol_one_chain_3d::remove()\n";

  unlink_all_superiors_twoway(this);
  deep_remove(deleted);

  return true;
}

// --
//    Removes the zero_chain from the edge.  The removal of
//    an edge's zero chain will invalidate the edge, so the
//    edge is unlinked from the topological structure and
//    recursively removed from its superiors.  
//    See Disconnect(changes,deleted) for detailed comments on
//    contents of parameters changes and deleted. (RYF 7-16-98)
//
bool vtol_edge_3d::remove(vtol_zero_chain_3d *,
                          vcl_vector<vtol_topology_object_3d *> &changes,
                          vcl_vector<vtol_topology_object_3d *> &deleted)
{
  // cerr << "      Entering vtol_edge_3d::remove()\n";

  // Removing the zero chain from the edge will destroy it.
  // Therefore, recursively remove the edge from its superiors.
  topology_list_3d *tmp=get_superiors();

  topology_list_3d::iterator t;

  vcl_vector<vtol_one_chain_3d *> sup;
  for(t= tmp->begin();t!=tmp->end();++t )
    sup.push_back((vtol_one_chain_3d *)(*t));
  
  vcl_vector<vtol_one_chain_3d *>::iterator s;
  for(s= sup.begin();s!= sup.end();++s)
    // TODO
    // ((*s))->remove( this, changes, deleted );
    ;
  cerr << "Must implement vtol_one_chain_3d::remove()\n";

  // Removal of a zerochain from an edge always results in the
  // destruction of the edge.
  deleted.push_back(this);
  unlink_all_superiors_twoway(this);
  unlink_all_inferiors_twoway(this);
  
  // cerr << "      Exiting vtol_edge_3d::remove()\n";
  
  return false; 
}

// --
//    For each inferior, this method unlinks the inferior
//    from this object.  If the inferior now has zero superiors,
//    the function is called recursively on it.  Finally, this
//    object is pushed onto the list removed.  (RYF 7-16-98)
//

void vtol_edge_3d::deep_remove(vcl_vector<vtol_topology_object_3d *> &removed)
{
  // cerr << "               Entering vtol_edge_3d::deep_remove()\n";

  topology_list_3d *tmp=get_inferiors();
  vcl_vector<vtol_zero_chain_3d *>inferiors;

  topology_list_3d::iterator t;

  for(t=tmp->begin();t!=tmp->end();++t)
    inferiors.push_back((vtol_zero_chain_3d *)(*t));
 
  vcl_vector<vtol_zero_chain_3d *>::iterator inf;
  
  for(inf= inferiors.begin();inf!=inferiors.end();++inf)
  {
    vtol_zero_chain_3d *inferior=(*inf);

      // Unlink inferior from its superior
    inferior->unlink_superior(this);
    
    // Test if inferior now has 0 superiors.  If so, 
    // recursively remove its inferiors.
    if(inferior->numsup()==0)
      inferior->deep_remove(removed);
  }
  removed.push_back(this);
  // cerr << "               Exiting vtol_edge_3d::deep_remove()\n";
}

// --
//  Returns true if the invoking edge has a vertex in common with
//  vtol_edge_3d e.  The method determines if the two edges share a vertex
//  by comparing pointer values, not the vertex geometry.
//

bool vtol_edge_3d::share_vertex_with(vtol_edge_3d *e)
{
  vcl_vector< vtol_vertex_3d *> *thisedges=this->vertices();
  vcl_vector< vtol_vertex_3d *> *eedges=e->vertices();
  
  vcl_vector< vtol_vertex_3d *>::iterator i1,i2;
  for(i1=thisedges->begin();i1!=thisedges->end();++i1 )
    for(i2=eedges->begin();i2!=eedges->end(); ++i2 )
      if((*i1)==(*i2))
        return true;
  return false;
}

// --
// This method adds newvert to the vtol_edge_3d by linking it to one of the
// zero_chains of the vtol_edge_3d Inferiors. (Method needs work.)

bool vtol_edge_3d::add_vertex(vtol_vertex_3d *newvert)
{
  vtol_zero_chain_3d *zc=get_zero_chain();

  if(!zc)
    {
      zc=new vtol_zero_chain_3d();
      link_inferior(zc);
    }
  zc->link_inferior(newvert);
  return true;
}

// --
// This method removes uglyvert from the vtol_edge_3d by removing it from the
// inferior zero_chains.  (Method needs work.)

bool vtol_edge_3d::remove_vertex(vtol_vertex_3d *uglyvert)
{
  if(uglyvert==_v1)
    set_v1(0);
  else if(uglyvert==_v2)
    set_v2(0);
  else
    return false;
  this->touch();
  return true;
}

bool vtol_edge_3d::is_endpoint(vtol_vertex_3d *v) const
{
  return (is_endpoint1(v)||is_endpoint2(v));
}


// -- Returns True if v is equal to the first vtol_edge_3d endpoint,_v1.
bool vtol_edge_3d::is_endpoint1(vtol_vertex_3d *v) const
{
  return (*_v1==*v);
}

// -- Returns True if v is equal to the second vtol_edge_3d endpoint, _v2.
bool vtol_edge_3d::is_endpoint2(vtol_vertex_3d* v) const
{
  return (*_v2 == *v);
}


// -- This method works only for ImplicitLine edges.
vtol_vertex_3d* vtol_edge_3d::other_endpoint(vtol_vertex_3d *overt)
{
  vtol_vertex_3d *result=0;
  if(overt==_v1)
    result=_v2;
  else if(overt==_v2)
    result=_v1;
  return result;
}


//-----------------------------------------------------------------
// -- Compute bounds from the geometry of _curve. If the curve is
//    not fully bounded, then use the vertices.
//
void vtol_edge_3d::compute_bounding_box(void)
{
#if 0 // TODO
  if(_curve && _curve->GetGeometryType() != GeometryObject::IMPLICITLINE)
     {
      // Get bounds from curve.
      // But are curve endpoints consistent with vertices? -JLM
      // Anyway, this is what was done in get_min_max
       if(_curve->GetExtent() == vsol_curve_3d::FULLY_BOUNDED)
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
    this->vtol_topology_object_3d::compute_bounding_box();
#endif
}


/*
 ******************************************************
 *
 *    I/O methods
 */

// --
// This method outputs all edge information to the ostream, strm.  It
// indents various levels of output by the number given in blanking.
void vtol_edge_3d::describe(ostream &strm,
                            int blanking) const
{
  for (int i=0; i<blanking; ++i) strm << ' ';
  print(strm);
  for (int i1=0; i1<blanking; ++i1) strm << ' ';
  if(_v1) {
    _v1->print(strm);
  } else {
    strm << "Null vertex 1" << endl;
  }
  for (int i2=0; i2<blanking; ++i2) strm << ' ';
  if(_v2) {
    _v2->print(strm);
  } else {
    strm << "Null vertex 2" << endl;
  }
}

// -- This method outputs a brief vtol_edge_3d info with vtol_edge_3d object address.
void vtol_edge_3d::print(ostream &strm) const
{
   strm<<"<vtol_edge_3d  "<<"  "<<(void *)this <<"> with id "<<get_id()<<endl;
}


//-----------------------------------------------------------------------------
