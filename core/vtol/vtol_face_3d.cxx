#include <vtol/vtol_macros_3d.h>
#include <vtol/vtol_two_chain_3d.h>
#include <vtol/vtol_vertex_3d.h>
#include <vtol/vtol_edge_3d.h>
#include <vtol/vtol_face_3d.h>
#include <vtol/vtol_one_chain_3d.h>
#include <vtol/vtol_list_functions_3d.h>
#include <vtol/vtol_face_3d.h>
#include <vsol/vsol_curve_3d.h>
#include <vsol/vsol_surface_3d.h>
#include <vsol/vsol_rectangle_3d.h>
#include <vsol/vsol_point_3d.h>

// #include <vtol/some_stubs.h>
#include <vcl/vcl_rel_ops.h>



vtol_face_3d::vtol_face_3d(void) 
  :_surface(0)
{
  vtol_one_chain_3d *inf = new vtol_one_chain_3d();
  link_inferior(inf);
}

//:
// This is the Copy Constructor for vtol_face_3d. It performs a deep copy of
// all vtol_face_3d inferior one_chains.

vtol_face_3d::vtol_face_3d(const vtol_face_3d &other)
  : _surface(0)
{
  vtol_face_3d* oldf = (vtol_face_3d*)(&other);
  vcl_vector<vtol_edge_3d*>* edges = oldf->edges();
  vcl_vector<vtol_vertex_3d*>* verts = oldf->vertices();
  int vlen = verts->size();
  int elen = edges->size();

  vcl_vector<vtol_topology_object_3d*> newedges(elen);
  vcl_vector<vtol_topology_object_3d*> newverts(vlen);
   

  int i =0;
  vcl_vector<vtol_vertex_3d*>::iterator vi;
  for(vi=verts->begin();vi!= verts->end();++vi, ++i)
    {
      vtol_vertex_3d* v = (*vi);
      newverts[i] = v->copy();
      v->set_id(i);
    }
  int j =0;

  vcl_vector<vtol_edge_3d*>::iterator ei;
  for(ei=edges->begin();ei!= edges->end(); ++ei, ++j)
    {
      vtol_edge_3d* e = (*ei);
      vsol_curve_3d* c = (e->get_curve()) ? (vsol_curve_3d*)(e->get_curve()->clone().ptr()) : 0;

      vtol_topology_object_3d* V1 = newverts[e->get_v1()->get_id()];
      vtol_topology_object_3d* V2 = newverts[e->get_v2()->get_id()];
      if(!(V1&&V2))
        {
 	  cerr << "Inconsistent topology in vtol_face_3d copy constructor\n";
 	  vtol_one_chain_3d *inf = new vtol_one_chain_3d();
          link_inferior(inf);
          return;
 	}
      // make the topology and geometry match
      vtol_edge_3d* newedge = new vtol_edge_3d();
      newedge->set_v1(V1->cast_to_vertex_3d());
      newedge->set_v2(V2->cast_to_vertex_3d());
      c->set_p0(V1->cast_to_vertex_3d()->get_point());
      c->set_p1(V2->cast_to_vertex_3d()->get_point());
      // now set the curve on the new edge;
      newedge->set_curve(c);
      newedges[j] = newedge;
      e->set_id(j);
    }
  // This is a deep copy of the vtol_face_3d.
  vtol_one_chain_3d *onech;

  topology_list_3d::iterator ii;

  for(ii=oldf->_inferiors.begin();ii!= oldf->_inferiors.end();++ii)
    {
      onech = (*ii)->cast_to_one_chain_3d()->copy_with_arrays(newverts, newedges);
      link_inferior(onech);
    }
  delete edges;
  delete verts;
  set_surface(0);
  if(oldf->_surface!=0)
    set_surface((vsol_surface_3d*)(oldf->_surface->clone().ptr()));
}

//: 
// destructor 
vtol_face_3d::~vtol_face_3d()
{
}

//---------------------------------------------------------------------------
// Name: clone
// Task: Clone `this': creation of a new object and initialization
//       See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_ref vtol_face_3d::clone(void) const
{
  return new vtol_face_3d(*this);
}

vsol_surface_3d_ref vtol_face_3d::get_surface(void) const
{
  return _surface;
}

//: 
// copy with an array

vtol_face_3d* vtol_face_3d::copy_with_arrays(vcl_vector<vtol_topology_object_3d*>& verts,
                           vcl_vector<vtol_topology_object_3d*>& edges)
{
  vtol_face_3d* newface = new vtol_face_3d();
  topology_list_3d::iterator i;
  for(i=newface->_inferiors.begin();i!= newface->_inferiors.end();++i )
    {
      vtol_topology_object_3d* obj = (*i);
      newface->unlink_inferior(obj);
      iu_delete(obj);
    }
  newface->_inferiors.clear();
  for(i=_inferiors.begin();i!= _inferiors.end();++i)
    {
      vtol_one_chain_3d* onech = (*i)->cast_to_one_chain_3d()->copy_with_arrays(verts, edges);
      newface->link_inferior(onech);
    }
  if(_surface)
    newface->set_surface((vsol_surface_3d*)(_surface->clone().ptr()));
  return newface;
}

//:
// copy with no links 
vtol_topology_object_3d * vtol_face_3d::shallow_copy_with_no_links( void )
{  
    vtol_face_3d * newface = new vtol_face_3d;
    topology_list_3d * infs = newface->get_inferiors();
    if ( infs->size() != 1 ) 
        cerr << "Error in vtol_face_3d::ShallowCopyWithNoLinks\n"
             << "vtol_face_3d does not have 1 inferior\n";
    vtol_topology_object_3d * inf = (*infs)[0];
    newface->unlink_inferior( inf );
    iu_delete( inf );

    newface->set_surface(NULL);
    if ( _surface!=0 )
      newface->set_surface((vsol_surface_3d*)(_surface->clone().ptr()));

    return newface;
}

//:
//  Constructor for a planar vtol_face_3d from an ordered list of vertices.
// edges are constructed by connecting vtol_vertex_3d[i] to
// vtol_vertex_3d[(i+1)mod L]. L is the length of the vertex list, verts, and
// i goes from 0 to L.

vtol_face_3d::vtol_face_3d(vcl_vector<vtol_vertex_3d *> *verts)
  :   _surface(0)
{
  vsol_point_3d_ref p0,p1,p2;
  double xmin=0;
  double ymin=0;
  double xmax=1;
  double ymax=1;
  double zmin=0;
  double zmax=1;

  
  set_surface(new vsol_rectangle_3d(new vsol_point_3d(xmin,ymin,zmin),
                                    new vsol_point_3d(xmax,ymin,zmin),
                                    new vsol_point_3d(xmax,ymax,zmax)));

  vcl_vector<vtol_edge_3d*> elist;
  vcl_vector<signed char> directions;
  vtol_edge_3d* newedge;

  //generate a list of edges for edge loop

  if(verts->size() > 2)
    {
      bool done = false;
      vcl_vector<vtol_vertex_3d*>::iterator vi = verts->begin();
      vtol_vertex_3d* v1 = (*vi);
      while(!done)
	{
	  // if no next vertex, then use the first vertex by calling
	  // verts->end() again to wrap around  This will close the loop
	  ++vi;
	  if(vi==verts->end()){
	    vi=verts->begin();
	    done=true;
	  }

	  vtol_vertex_3d* v2 = (*vi); // get the next vertex (may be first)
	  newedge = v1->new_edge(v2);
	  elist.push_back(newedge);

	  if(v2 == newedge->get_v2())
	    directions.push_back((signed char)1);
          else
            directions.push_back((signed char)(-1));
	  v1 = v2;		// in the next go around v1 is v2 of the last
	}
      vtol_one_chain_3d *eloop;
      eloop = new vtol_one_chain_3d(elist, directions, true);
      link_inferior(eloop);
      
    }
  else
    {
      cout << "face with not enough verts " << verts << endl;
    }
}


//:
// Constructor for a Planar face from a list of one_chains.  This
// method assumes that the first vtol_one_chain_3d on the list is the outside
// boundary vtol_one_chain_3d.  The remaining one_chains are holes boundaries
// on the face.

vtol_face_3d::vtol_face_3d (vcl_vector<vtol_one_chain_3d *> &onechs)
  :  _surface(0)
{
  // 1)  Add one chains to the inferiors list.
  //     Assume that the first vtol_one_chain_3d on the
  //     list is the outside boundary...the
  //     remaining one_chains are holes.

  if (onechs.size() > 0)
    link_inferior(onechs[0]);

  vtol_one_chain_3d *onech = get_boundary_cycle();

  // 2) This constructor will assume that the
  // surface is an ImplicitPlane().

   vsol_point_3d_ref p0,p1,p2;
  double xmin=0;
  double ymin=0;
  double xmax=1;
  double ymax=1;
  double zmin=0;
  double zmax=1;

  
  set_surface(new vsol_rectangle_3d(new vsol_point_3d(xmin,ymin,zmin),
                                    new vsol_point_3d(xmax,ymin,zmin),
                                    new vsol_point_3d(xmax,ymax,zmax)));


 

  if (onech)
    {
      for (int i = 1; i < onechs.size(); ++i)
        onech->add_inferior_one_chain(onechs[i]);
    }
}

//:
// Constructor of a Planar face from a vtol_one_chain_3d.  This method uses
// the vtol_one_chain_3d, edgeloop, as the outside boundary of the face.

vtol_face_3d::vtol_face_3d(vtol_one_chain_3d *edgeloop)
  : _surface(0)
{
  link_inferior(edgeloop);
  topology_list_3d faces;
  vtol_face_3d* f = (vtol_face_3d*) this;
  faces.push_back(f);

  // big todo
   vsol_point_3d_ref p0,p1,p2;
  double xmin=0;
  double ymin=0;
  double xmax=1;
  double ymax=1;
  double zmin=0;
  double zmax=1;

 
  set_surface(new vsol_rectangle_3d(new vsol_point_3d(xmin,ymin,zmin),
                                    new vsol_point_3d(xmax,ymin,zmin),
                                    new vsol_point_3d(xmax,ymax,zmax)));

 

}

//:
// Constructor requiring only the underlying geometric surface
vtol_face_3d::vtol_face_3d (vsol_surface_3d_ref facesurf)
  : _surface(0)
{
  set_surface(facesurf);

  // not much, but at least its a start...
}

//---------------------------------------------------------------------------
// Name: topology_type
// Task: Return the topology type
//---------------------------------------------------------------------------
vtol_face_3d::vtol_topology_object_3d_type
vtol_face_3d::topology_type(void) const
{
  return FACE;
}

//:
// Set the underlying geometric surface.
void vtol_face_3d::set_surface(vsol_surface_3d_ref newsurf)
{
  _surface = newsurf;
  this->touch();
}

//:
// Returns an ordered list of vertices of the outside boundary of the
// face.  All vertices on any holes of the face are *not* included.
// This vertex list is ordered such that a positive normal is
// computing using the Right Hand rule in the direction of the vertex
// list.

vcl_vector<vtol_vertex_3d*> *vtol_face_3d::outside_boundary_vertices()			
{									
  OUTSIDE_BOUNDARY_3d(vtol_vertex_3d, vtol_one_chain_3d , vertices);		
}									

//:
// Returns a vtol_vertex_3d list of all the vertices on the face.
// If the face does not have any holes, this vertex list is ordered
// in the direction of a positive normal using the Right Hand rule.

vcl_vector<vtol_vertex_3d*> *vtol_face_3d::vertices()
{
  SEL_INF_3d(vtol_vertex_3d,vertices);
}

//:
// Returns a list of the zero_chains on the outside boundary of the face.
// All zero_chains on any hole boundaries of the face are *not* included.

vcl_vector<vtol_zero_chain_3d*> *vtol_face_3d::outside_boundary_zero_chains()
{
  OUTSIDE_BOUNDARY_3d(vtol_zero_chain_3d, vtol_one_chain_3d,zero_chains);
}

//:
// Returns a list of zero_chains of the face.

vcl_vector<vtol_zero_chain_3d*> *vtol_face_3d::zero_chains()
{
  SEL_INF_3d(vtol_zero_chain_3d,zero_chains);
}

//:
// Returns a list of edges that make up the outside boundary of the
// face. All edges on any hole boundaries are *not* included.

vcl_vector<vtol_edge_3d*>* vtol_face_3d::outside_boundary_edges()
{
  OUTSIDE_BOUNDARY_3d(vtol_edge_3d,vtol_one_chain_3d, edges);
}


//:
// Returns a list of edges on the face.


vcl_vector<vtol_edge_3d*>* vtol_face_3d::edges()
{
  SEL_INF_3d(vtol_edge_3d,edges);
}

//:
// Returns a list of one_chains that make up the outside boundary of
// the face.

vcl_vector<vtol_one_chain_3d*>* vtol_face_3d::outside_boundary_one_chains()
{
  COPY_INF_3d(vtol_one_chain_3d);
}


//:
// Returns a list of all Onechains of the face.

vcl_vector<vtol_one_chain_3d*>* vtol_face_3d::one_chains()
{
  SEL_INF_3d(vtol_one_chain_3d,one_chains);
}

//:
// Returns a list of that has itself as the only element.  This method
// is needed for traversing the model hierarchy consistently.

vcl_vector<vtol_face_3d*>* vtol_face_3d::faces()
{
  LIST_SELF_3d(vtol_face_3d);
}

//:
//  Returns a list of all the two_chains which contain the vtol_face_3d.


vcl_vector<vtol_two_chain_3d*>* vtol_face_3d::two_chains()
{
  SEL_SUP_3d(vtol_two_chain_3d,two_chains);
}

//:
// Returns a list of all the blocks that contain the vtol_face_3d.


vcl_vector<vtol_block_3d*>* vtol_face_3d::blocks()
{
  SEL_SUP_3d(vtol_block_3d,blocks);
}


//:
// Links new_vtol_one_chain_3d as an inferior of the vtol_face_3d.  This function will
// be deleted and replaced soon....pav

bool vtol_face_3d::add_edge_loop(vtol_one_chain_3d* new_vtol_one_chain_3d)
{
  this->touch();
  return link_inferior(new_vtol_one_chain_3d);
}

//:
//    This method removes the object from the topological structure
//    by unlinking it.  The removal of the object may recursively cause
//    the removal of some of the object's superiors if these superiors
//    are no longer valid.  In addition, inferiors of the object are
//    recursively checked to see if they should be removed.  They
//    are removed if all their superiors have been removed.
//
//    A record of the changes to the topological structure is returned 
//    through the parameters changes and deleted.  For details on what is
//    stored in these parameters, see vtol_edge_3d::Disconnect( changes, deleted ).
//    (RYF 7-16-98)
//
bool vtol_face_3d::disconnect( vcl_vector< vtol_topology_object_3d * > & changes,
                       vcl_vector< vtol_topology_object_3d * > & deleted )
{
  // Remove this object from its superiors.  A copy is made
  // of the superior list first because the for loop body may
  // make modifications to the list.

  topology_list_3d * tmp = get_superiors();
  vcl_vector< vtol_two_chain_3d * > sup;
  
  topology_list_3d::iterator ti;

  for (ti= tmp->begin(); ti!=tmp->end(); ++ti )
      sup.push_back( (vtol_two_chain_3d *) (*ti) );
  
  vcl_vector< vtol_two_chain_3d * >::iterator si;
  for (si =  sup.begin(); si!=sup.end(); ++si )
      ((*si))->remove( this, changes, deleted );

  unlink_all_superiors_twoway( this );
  deep_remove( deleted );

  return true;
}

//:
//    Removes the one chain from the face by unlinking it.  If 
//    the one chain is not a hole, its removal invalidates the
//    the face and the face is unlinked from the topological
//    structure and appended to the list deleted.  Any holes
//    it has are appended to the list changes.
//    If the one chain is a hole, the face is still valid even with
//    the hole's removal.  In this case, the hole is unlinked from the
//    the face and appended to the list changes.
//    For more details, see vtol_edge_3d::Disconnect( changes, deleted )
//    (RYF 7-16-98)
//

bool vtol_face_3d::remove( vtol_one_chain_3d * one_chain,
                   vcl_vector< vtol_topology_object_3d * > & changes,
                   vcl_vector< vtol_topology_object_3d * > & deleted )
{
  //cout << "            Entering vtol_face_3d::Remove\n";

  // If one_chain is a hole, just unlink hole and return
  vcl_vector< vtol_one_chain_3d * > * holes = get_hole_cycles();

  bool is_hole = (vcl_find(holes->begin(),holes->end(),one_chain) != holes->end());
   delete holes;
  if ( is_hole )
  {
      //cout << "            Removing hole from vtol_face_3d.\n";
      vtol_one_chain_3d * outer = get_boundary_cycle();
      if ( outer )
          outer->remove_inferior_one_chain( (vtol_one_chain_3d *) one_chain );
      //cout << "            Exiting vtol_face_3d::Remove\n";
      return true;
  }

  // Remove this object from its superiors
  topology_list_3d * tmp = get_superiors();
  vcl_vector< vtol_two_chain_3d * > sup;

  topology_list_3d::iterator ti;

  for (ti= tmp->begin();ti !=  tmp->end(); ++ti )
      sup.push_back( (vtol_two_chain_3d *) (*ti) );

  vcl_vector< vtol_two_chain_3d * >::iterator si;
  for (si= sup.begin();si !=  sup.end(); ++si )
      ((*si))->remove( this, changes, deleted );

  // Record changes
  vcl_vector< vtol_one_chain_3d * > * onechains = this->get_hole_cycles();
  
  vcl_vector< vtol_one_chain_3d * >::iterator oi;

  for (oi= onechains->begin(); oi !=onechains->end(); ++oi )
      changes.push_back((*oi));
  delete onechains;
  deleted.push_back( this );

  unlink_all_superiors_twoway( this );
  unlink_all_inferiors_twoway( this );

  //cout << "            Exiting vtol_face_3d::Remove\n";

  return false;
}


//:
//    For each inferior, this method unlinks the inferior
//    from this object.  If the inferior now has zero superiors,
//    the function is called recursively on it.  Finally, this
//    object is pushed onto the list removed. (RYF 7-16-98)
//

void vtol_face_3d::deep_remove( vcl_vector< vtol_topology_object_3d * > & removed )
{
  //cout << "         Entering vtol_face_3d::DeepDeleteInferiors\n";

  // Remove links to holes first
  vcl_vector< vtol_one_chain_3d * > inferiors;
  vtol_one_chain_3d * outer = get_boundary_cycle();
  vcl_vector< vtol_one_chain_3d * > * holes = get_hole_cycles();

  vcl_vector< vtol_one_chain_3d * >::iterator hi;

  for (hi= holes->begin(); hi!=holes->end();++hi )
      inferiors.push_back( (*hi) );

  vcl_vector< vtol_one_chain_3d * >::iterator ii;
  for (ii= inferiors.begin(); ii!=inferiors.end();++ii )
  {
      vtol_one_chain_3d * inferior = (*ii);

      // Unlink inferior from its superior
      if ( outer ) outer->remove_inferior_one_chain( inferior );

      // Test if inferior now has 0 superiors.  If so, 
      // recursively remove its inferiors.
      if ( inferior->get_superiors()->size() 
           + inferior->get_hierarchy_superiors()->size() == 0 )
          inferior->deep_remove( removed );
  }

  // Now remove links to remaining inferiors
  inferiors.clear();
  topology_list_3d * tmp = get_inferiors();

  topology_list_3d::iterator ti;

  for (ti= tmp->begin(); ti!=tmp->end();++ti )
      inferiors.push_back( (vtol_one_chain_3d *) (*ti) );
 
  vcl_vector< vtol_one_chain_3d * >::iterator oci;

  for (oci= inferiors.begin();oci != inferiors.end(); ++oci )
  {
      vtol_one_chain_3d * inferior = (*oci);

      // Unlink inferior from its superior
      inferior->unlink_superior( this );  

      // Test if inferior now has 0 superiors.  If so, 
      // recursively remove its inferiors.
      if ( inferior->numsup() == 0 )  
          inferior->deep_remove( removed );
  }
  removed.push_back( this );

  //cout << "         Exiting vtol_face_3d::DeepDeleteInferiors\n";
}

//:
//  Returns true if the invoking face has an edge in common with
//  vtol_face_3d f.  The method determines if the two faces share an edge
//  by comparing pointer values, not the edge geometry.
//
bool vtol_face_3d::shares_edge_with( vtol_face_3d * f )
{
    vcl_vector< vtol_edge_3d * > * thisedges = this->edges();
    vcl_vector< vtol_edge_3d * > * fedges = f->edges();
    vcl_vector< vtol_edge_3d * >::iterator ei1,ei2;

    for (ei1= thisedges->begin();ei1!= thisedges->end(); ++ei1 )
        for (ei2= fedges->begin();ei2!= fedges->end();++ei2 )
            if ( (*ei1) == (*ei2) )
                return true;
    return false;
}

//:
// Links new_vtol_one_chain_3d as an inferior of the vtol_face_3d and returns True if
// successful. This method will be replacing all calls to add_edge_lop()o.

bool vtol_face_3d::add_one_chain(vtol_one_chain_3d* new_vtol_one_chain_3d)
{
  bool okay = link_inferior(new_vtol_one_chain_3d);

  if (new_vtol_one_chain_3d->contains_sub_chains())
    {
      this->touch();
      return okay;
    }
  else
    return false;
}

//:
// Removes the vtol_one_chain_3d, doomedvtol_one_chain_3d, from the inferiors list of the vtol_face_3d.
// This function will be deleted and replaced soon.

bool vtol_face_3d::remove_edge_loop(vtol_one_chain_3d* doomed_vtol_one_chain_3d)
{
  this->touch();
  return unlink_inferior(doomed_vtol_one_chain_3d);
}

//:
//  Removes the vtol_one_chain_3d, doomedvtol_one_chain_3d, from the inferiors list of the vtol_face_3d.
// This function will be replacing all calls to remove_edge_loop().

bool vtol_face_3d::remove_one_chain(vtol_one_chain_3d* doomed_vtol_one_chain_3d)
{
  this->touch();
  return unlink_inferior(doomed_vtol_one_chain_3d);
}



//:
//  Adds the vtol_two_chain_3d, new_vtol_two_chain_3d, to the superiors list of the vtol_face_3d.
// This function will be deleted and replaced soon.
bool vtol_face_3d::add_face_loop(vtol_two_chain_3d* new_vtol_two_chain_3d)
{
  return link_superior(new_vtol_two_chain_3d);
}


//:
// Adds the vtol_two_chain_3d, new_vtol_two_chain_3d, to the superiors list of the vtol_face_3d.
// This function will replace all calls to add_face_loop().

bool vtol_face_3d::add_two_chain(vtol_two_chain_3d* new_vtol_two_chain_3d)
{
  return link_superior(new_vtol_two_chain_3d);
}

//:
// Removes the vtol_two_chain_3d, doomed_vtol_two_chain_3d, from the superiors list of the vtol_face_3d.
// This function will be deleted and replaced soon.

bool vtol_face_3d::remove_face_loop(vtol_two_chain_3d* doomed_vtol_two_chain_3d)
{
  return unlink_superior(doomed_vtol_two_chain_3d);
}


//:
//  Removes the vtol_two_chain_3d, doomed_vtol_two_chain_3d, from the superiors list of the vtol_face_3d.
// This function will replace all calls to remove_face_loop().

bool vtol_face_3d::remove_two_chain(vtol_two_chain_3d* doomed_vtol_two_chain_3d)
{
  return unlink_superior(doomed_vtol_two_chain_3d);
}

//:
// deep equality check on faces.  uses fuzzy equal on vertices.
//

bool vtol_face_3d::operator==(const vtol_face_3d& otherf) const
{


  if (this == &otherf) return true;

  if(_surface && otherf._surface)
    if(*_surface != *otherf._surface)
      return false;
  if(_surface && !otherf._surface || otherf._surface && !_surface)
    return false;
  if (_inferiors.size() != otherf._inferiors.size())
    return false;
  else
    {

      topology_list_3d::const_iterator ti1,ti2;
      


      for(ti1=_inferiors.begin(), ti2=otherf._inferiors.begin(); ti1 != _inferiors.end();++ti1 , ++ti2)
	{
	  if(!((*ti1) == (*ti2))){
                return false;
            }
        }
    }
  return true;
}


//:
// spatial object equality

bool vtol_face_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::TOPOLOGYOBJECT &&
   ((vtol_topology_object_3d const&)obj).topology_type() == vtol_topology_object_3d::FACE
  ? *this == (vtol_face_3d const&) (vtol_topology_object_3d const&) obj
  : false;
}

//:
// Returns the ith inferior vtol_one_chain_3d of the vtol_face_3d.

vtol_one_chain_3d* vtol_face_3d::get_one_chain(int which)

{
 
  if (which < _inferiors.size())
    return (_inferiors[which])->cast_to_one_chain_3d();
  else
    {
      cerr << "Tried to get bad edge_loop from face" << endl;
      return NULL;
    }
}


//:
// Returns the first inferior vtol_one_chain_3d of the vtol_face_3d (the boundary onechain).

vtol_one_chain_3d* vtol_face_3d::get_boundary_cycle()
{
  // The outside boundary vtol_one_chain_3d will *always*
  // be the first one chain in the inferiors
  // list.
  return get_one_chain(0);
}

//:
// Adds a new hole to the face

bool vtol_face_3d::add_hole_cycle( vtol_one_chain_3d * new_hole ) 
{
  vtol_one_chain_3d *onech = get_boundary_cycle();

  if ( onech )
  {
      onech->add_inferior_one_chain( new_hole );
      return true;
  } else
      return false;
}

//: Returns a list of the one_chains that make up the holes of the vtol_face_3d.

vcl_vector<vtol_one_chain_3d*>* vtol_face_3d::get_hole_cycles()
{
  vcl_vector<vtol_one_chain_3d*>* new_list = new vcl_vector<vtol_one_chain_3d*>();
  
  topology_list_3d::iterator ii;

  for(ii=_inferiors.begin();ii!=  _inferiors.end();++ii)
    {
      vcl_vector<vtol_one_chain_3d*>* templist =
                    (*ii)->cast_to_one_chain_3d()->inferior_one_chains();

      // new_list->insert(new_list->end(),templist->begin(),templist->end());
      vcl_vector<vtol_one_chain_3d*>::iterator oi;
      for(oi=templist->begin();oi!=templist->end();++oi){
        new_list->push_back(*oi);
      }


      delete templist;
    }

  return new_list;
}

//:
// Returns the number of edges on the vtol_face_3d.
//

int vtol_face_3d::get_num_edges()
{
  int sum= 0;
  topology_list_3d::iterator ii;

  for(ii=_inferiors.begin();ii!= _inferiors.end();++ii)
    {
      sum += ((*ii)->cast_to_one_chain_3d())->numinf();
    }
  return sum;
}

//:
// reverse the direction of the face 
void vtol_face_3d::reverse_normal()
{
  for (topology_list_3d::iterator ti=_inferiors.begin(); ti!=_inferiors.end(); ++ti )
    {
      ((vtol_one_chain_3d*)(*ti))->reverse_directions();
    }

  // compute_normal();
}



//:
//-----------------------------------------------------------------
//    Compute bounds from the geometry of _surface. If the surface is
//    not fully bounded, then use the vertices.
//
void vtol_face_3d::compute_bounding_box()
{
  /* todo
  if(_surface && _surface->GetGeometryType() != GeometryObject::IMPLICITPLANE)
     {
      // Get bounds from surface.
      // But are bounds consistent with face vertices? -JLM
      // Anyway, this is what was done in get_min_max on vtol_edge_3d.
       if(_surface->GetExtent() == vsol_surface_3d::FULLY_BOUNDED)
         {
           this->set_minX(_surface->GetMinX());
           this->set_minY(_surface->GetMinY());
           this->set_minZ(_surface->GetMinZ());

           this->set_maxX(_surface->GetMaxX());
           this->set_maxY(_surface->GetMaxY());
           this->set_maxZ(_surface->GetMaxZ());
         }
     }
  else  // Just use the generic method computing bounds from vertices
    this->vtol_topology_object_3d::ComputeBoundingBox();
  */
  this->vtol_topology_object_3d::compute_bounding_box();

}


//:
//  This method describes the data members of the vtol_face_3d including the
// Inferiors.  The blanking argument is used to indent the output in
// a clear fashion.

void vtol_face_3d::describe(ostream& strm,int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  print();
  for(int i=0;i < _inferiors.size();++i)
    {
      if((_inferiors[i])->cast_to_one_chain_3d()){
            (_inferiors[i])->cast_to_one_chain_3d()->describe(strm,blanking);
      }
      else{
        for (int j=0; j<blanking; ++j) strm << ' ';
        cout << "Odd inferior for a face" << endl;
      }
  }
}

//:
// This method prints out a simple text representation for the vtol_face_3d which
// includes its address in memory.


void vtol_face_3d::print(ostream& strm) const
{
  strm << "<vtol_face_3d  ";
  topology_list_3d::const_iterator ii;

  for(ii=_inferiors.begin();ii!= _inferiors.end();++ii)
    {
      strm << " " << (*ii)->get_inferiors()->size();
    }
 strm << "   " << (void *) this << '>' << endl;

}
