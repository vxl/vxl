#include "vtol_face_2d.h"

#include <vcl_cassert.h>
#include <vtol/vtol_macros_2d.h>
#include <vtol/vtol_two_chain_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_one_chain_2d.h>
#include <vtol/vtol_list_functions_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_point_2d.h>

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vtol_face_2d::vtol_face_2d(void) 
  :_surface(0)
{
}

// --
// This is the Copy Constructor for vtol_face_2d. It performs a deep copy of
// all vtol_face_2d inferior one_chains.

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vtol_face_2d::vtol_face_2d(const vtol_face_2d &other)
  : _surface(0)
{
  vtol_face_2d *oldf=(vtol_face_2d *)(&other);
  edge_list_2d *edgs=oldf->edges();
  vertex_list_2d *verts=oldf->vertices();

  vcl_vector<vtol_topology_object_2d_ref> newedges(edgs->size());
  vcl_vector<vtol_topology_object_2d_ref> newverts(verts->size());
   
  int i=0;
  vcl_vector<vtol_vertex_2d_ref>::iterator vi;
  for(vi=verts->begin();vi!=verts->end();++vi,++i)
    {
      vtol_vertex_2d_ref v=(*vi);
      newverts[i]=v->clone().ptr()->cast_to_topology_object_2d();
      v->set_id(i);
    }
  int j=0;
  vcl_vector<vtol_edge_2d_ref>::iterator ei;
  for(ei=edgs->begin();ei!= edgs->end();++ei,++j)
    {
      vtol_edge_2d_ref e=(*ei);
      vsol_curve_2d_ref c=(e->curve()) ? (vsol_curve_2d*)(e->curve()->clone().ptr()) : 0;


      vtol_topology_object_2d_ref V1 = newverts[e->v1()->get_id()];
      vtol_topology_object_2d_ref V2 = newverts[e->v2()->get_id()];
      if(!V1 || !V2)
        {
 	  cerr << "Inconsistent topology in vtol_face_2d copy constructor\n";
 	  vtol_one_chain_2d_ref inf = new vtol_one_chain_2d();
          link_inferior(*inf);
          return;
 	}
      // make the topology and geometry match
      vtol_edge_2d_ref newedge=new vtol_edge_2d();
      newedge->set_v1(V1->cast_to_vertex());
      newedge->set_v2(V2->cast_to_vertex());
      c->set_p0(V1->cast_to_vertex()->point());
      c->set_p1(V2->cast_to_vertex()->point());
      // now set the curve on the new edge;
      newedge->set_curve(*c);
      //newedges[j]=(vtol_topology_object_2d*)(newedge.ptr());
      newedges[j]=newedge.ptr();
      e->set_id(j);
    }
  // This is a deep copy of the vtol_face_2d.

  topology_list_2d::iterator ii;
  for(ii=oldf->_inferiors.begin();ii!= oldf->_inferiors.end();++ii)
    {
      vtol_one_chain_2d_ref onech=(*ii)->cast_to_one_chain()->copy_with_arrays(newverts,newedges);
      link_inferior(*onech);
    }
  delete edgs;
  delete verts;
  set_surface(0);
  if(oldf->_surface!=0)
    set_surface((vsol_region_2d*)(oldf->_surface->clone().ptr()));
}

//---------------------------------------------------------------------------
// Destructor 
//---------------------------------------------------------------------------
vtol_face_2d::~vtol_face_2d()
{
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
// -- Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_ref vtol_face_2d::clone(void) const
{
  return new vtol_face_2d(*this);
}

vsol_region_2d_ref vtol_face_2d::surface(void) const
{
  return _surface;
}

// -- copy with an array

vtol_face_2d *
vtol_face_2d::copy_with_arrays(vcl_vector<vtol_topology_object_2d_ref> &verts,
                               vcl_vector<vtol_topology_object_2d_ref> &edges) const
{
  vtol_face_2d *newface=new vtol_face_2d();
  topology_list_2d::const_iterator i;
  for(i=newface->_inferiors.begin();i!= newface->_inferiors.end();++i )
    {
      vtol_topology_object_2d_ref obj=(*i);
      newface->unlink_inferior(*obj);
    }
  newface->_inferiors.clear();
  for(i=_inferiors.begin();i!=_inferiors.end();++i)
    {
      vtol_one_chain_2d *onech=(*i)->cast_to_one_chain()->copy_with_arrays(verts,edges);
      newface->link_inferior(*onech);
    }
  if(_surface)
    newface->set_surface((vsol_region_2d*)(_surface->clone().ptr()));
  return newface;
}

//***************************************************************************
// Replaces dynamic_cast<T>
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a face, 0 otherwise
//---------------------------------------------------------------------------
const vtol_face_2d *vtol_face_2d::cast_to_face(void) const
{
  return this;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a face, 0 otherwise
//---------------------------------------------------------------------------
vtol_face_2d *vtol_face_2d::cast_to_face(void)
{
  return this;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// -- Is `inferior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_face_2d::valid_inferior_type(const vtol_topology_object_2d &inferior) const
{
  return inferior.cast_to_one_chain()!=0;
}

//---------------------------------------------------------------------------
// -- Is `superior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_face_2d::valid_superior_type(const vtol_topology_object_2d &superior) const
{
  return superior.cast_to_two_chain()!=0;
}

//---------------------------------------------------------------------------
// -- Copy with no links. Only copy the surface if it exists
//---------------------------------------------------------------------------
vtol_face_2d *vtol_face_2d::shallow_copy_with_no_links(void) const
{
  vtol_face_2d *result;
  result=new vtol_face_2d;
  result->set_surface(0);
  if(_surface!=0)
    result->set_surface((vsol_region_2d*)(_surface->clone().ptr()));
  return result;
}

// --
//  Constructor for a planar vtol_face_2d from an ordered list of vertices.
// edges are constructed by connecting vtol_vertex_2d[i] to
// vtol_vertex_2d[(i+1)mod L]. L is the length of the vertex list, verts, and
// i goes from 0 to L.
// Require: verts.size()>2

vtol_face_2d::vtol_face_2d(vertex_list_2d &verts)
  :_surface(0)
{
  // require
  assert(verts.size()>2);

  vsol_point_2d_ref p0;
  vsol_point_2d_ref p1;
  vsol_point_2d_ref p2;
  double xmin=0;
  double ymin=0;
  double xmax=1;
  double ymax=1;
  edge_list_2d elist;
  vcl_vector<signed char> directions;
  vtol_edge_2d_ref newedge;
  bool done;
  vertex_list_2d::iterator vi;
  vtol_vertex_2d_ref v01;
  vtol_vertex_2d_ref v02;
  vtol_one_chain_2d_ref eloop;

  set_surface(new vsol_rectangle_2d(new vsol_point_2d(xmin,ymin),
                                    new vsol_point_2d(xmax,ymin),
                                    new vsol_point_2d(xmax,ymax)));
  

  //generate a list of edges for edge loop
  done=false;
  vi=verts.begin();
  v01=(*vi);
  while(!done)
    {
      // if no next vertex, then use the first vertex by calling
      // verts->end() again to wrap around  This will close the loop
      ++vi;
      if(vi==verts.end())
        {
          vi=verts.begin();
          done=true;
        }
      
      v02=(*vi); // get the next vertex (may be first)
      newedge=v01->new_edge(*v02);
      elist.push_back(newedge);
      
	  if(*v02 == *(newedge->v2()))
	    directions.push_back((signed char)1);
          else
            directions.push_back((signed char)(-1));
	  v01=v02;		// in the next go around v1 is v2 of the last
    }
  eloop=new vtol_one_chain_2d(elist,directions,true);
  link_inferior(*eloop);
}

// --
// Constructor for a Planar face from a list of one_chains.  This
// method assumes that the first vtol_one_chain_2d on the list is the outside
// boundary vtol_one_chain_2d.  The remaining one_chains are holes boundaries
// on the face.

vtol_face_2d::vtol_face_2d(one_chain_list_2d &onechs)
  :_surface(0)
{
  // 1)  Add one chains to the inferiors list.
  //     Assume that the first vtol_one_chain_2d on the
  //     list is the outside boundary...the
  //     remaining one_chains are holes.

  if(onechs.size()>0)
    link_inferior(*(onechs[0]));

  vtol_one_chain_2d *onech=get_boundary_cycle();

  // 2) This constructor will assume that the
  // surface is an ImplicitPlane().

   vsol_point_2d_ref p0,p1,p2;
  double xmin=0;
  double ymin=0;
  double xmax=1;
  double ymax=1;
  
  set_surface(new vsol_rectangle_2d(new vsol_point_2d(xmin,ymin),
                                    new vsol_point_2d(xmax,ymin),
                                    new vsol_point_2d(xmax,ymax)));

  if(onech!=0)
    {
      for(int i = 1; i < onechs.size(); ++i)
        onech->link_chain_inferior(*(onechs[i]));
    }
}

// -- Constructor of a Planar face from a vtol_one_chain_2d.  This method uses
// the vtol_one_chain_2d, edgeloop, as the outside boundary of the face.

vtol_face_2d::vtol_face_2d(vtol_one_chain_2d &edgeloop)
  : _surface(0)
{
  link_inferior(edgeloop);
  topology_list_2d faces;
  vtol_face_2d* f = (vtol_face_2d*) this;
  faces.push_back(f);

  // big todo
   vsol_point_2d_ref p0,p1,p2;
  double xmin=0;
  double ymin=0;
  double xmax=1;
  double ymax=1;
  
  set_surface(new vsol_rectangle_2d(new vsol_point_2d(xmin,ymin),
                                    new vsol_point_2d(xmax,ymin),
                                    new vsol_point_2d(xmax,ymax)));
}

// -- Constructor requiring only the underlying geometric surface
vtol_face_2d::vtol_face_2d (vsol_region_2d &facesurf)
  : _surface(0)
{
  set_surface(&facesurf);

  // not much, but at least its a start...
}

//---------------------------------------------------------------------------
// -- Return the topology type
//---------------------------------------------------------------------------
vtol_face_2d::vtol_topology_object_2d_type
vtol_face_2d::topology_type(void) const
{
  return FACE;
}

// -- Set the underlying geometric surface.
void vtol_face_2d::set_surface(vsol_region_2d *const newsurf)
{
  _surface=newsurf;
  touch();
}

// --
// Returns an ordered list of vertices of the outside boundary of the
// face.  All vertices on any holes of the face are *not* included.
// This vertex list is ordered such that a positive normal is
// computing using the Right Hand rule in the direction of the vertex
// list.

vertex_list_2d *vtol_face_2d::outside_boundary_vertices(void)
{
  vertex_list_2d *new_ref_list = new vertex_list_2d();
  vcl_vector<vtol_vertex_2d*>* ptr_list = this->outside_boundary_compute_vertices();
  // copy the lists
  
  for(vcl_vector<vtol_vertex_2d*>::iterator ti = ptr_list->begin();
      ti != ptr_list->end(); ++ti){
    new_ref_list->push_back(*ti);
  }
  delete ptr_list;
  
  return new_ref_list;
 
}							

vcl_vector<vtol_vertex_2d*> *vtol_face_2d::outside_boundary_compute_vertices(void)
{
  
  OUTSIDE_BOUNDARY_2d(vtol_vertex_2d,vtol_one_chain_2d,compute_vertices);
}							

// --
// Returns a vtol_vertex_2d list of all the vertices on the face.
// If the face does not have any holes, this vertex list is ordered
// in the direction of a positive normal using the Right Hand rule.

vcl_vector<vtol_vertex_2d*> *vtol_face_2d::compute_vertices(void)
{
  
  SEL_INF_2d(vtol_vertex_2d,compute_vertices);
}

// --
// Returns a list of the zero_chains on the outside boundary of the face.
// All zero_chains on any hole boundaries of the face are *not* included.

vcl_vector<vtol_zero_chain_2d*> *vtol_face_2d::outside_boundary_compute_zero_chains(void)
{
  
  OUTSIDE_BOUNDARY_2d(vtol_zero_chain_2d, vtol_one_chain_2d,compute_zero_chains);
}


// --
// Returns a list of the zero_chains on the outside boundary of the face.
// All zero_chains on any hole boundaries of the face are *not* included.

zero_chain_list_2d *vtol_face_2d::outside_boundary_zero_chains(void)
{
  zero_chain_list_2d *new_ref_list = new zero_chain_list_2d();
  vcl_vector<vtol_zero_chain_2d*>* ptr_list = this->outside_boundary_compute_zero_chains();
  // copy the lists
  
  for(vcl_vector<vtol_zero_chain_2d*>::iterator ti = ptr_list->begin();
      ti != ptr_list->end(); ++ti){
    new_ref_list->push_back(*ti);
  }
  delete ptr_list;
  
  return new_ref_list;
}


// -- Returns a list of zero_chains of the face.

vcl_vector<vtol_zero_chain_2d*> *vtol_face_2d::compute_zero_chains(void)
{
  
  SEL_INF_2d(vtol_zero_chain_2d,compute_zero_chains);
}

// --
// Returns a list of edges that make up the outside boundary of the
// face. All edges on any hole boundaries are *not* included.

vcl_vector<vtol_edge_2d*> *vtol_face_2d::outside_boundary_compute_edges(void)
{
  
  OUTSIDE_BOUNDARY_2d(vtol_edge_2d,vtol_one_chain_2d, compute_edges);
}

// -- get the outside boundary edges

edge_list_2d *vtol_face_2d::outside_boundary_edges(void)
{
 
  edge_list_2d *new_ref_list = new edge_list_2d();
  vcl_vector<vtol_edge_2d*>* ptr_list = this->outside_boundary_compute_edges();
  // copy the lists
  
  for(vcl_vector<vtol_edge_2d*>::iterator ti = ptr_list->begin();
      ti != ptr_list->end(); ++ti){
    new_ref_list->push_back(*ti);
  }
  delete ptr_list;
  
  return new_ref_list;
}

// -- Returns a list of edges on the face.
vcl_vector<vtol_edge_2d*> *vtol_face_2d::compute_edges(void)
{
  
  SEL_INF_2d(vtol_edge_2d,compute_edges);
}

// -- Returns a list of one_chains that make up the outside boundary of the face.

one_chain_list_2d *vtol_face_2d::outside_boundary_one_chains(void)
{
  vcl_vector<vtol_one_chain_2d*>* ptr_list= outside_boundary_compute_one_chains();
  one_chain_list_2d *ref_list= new one_chain_list_2d();
  
  vcl_vector<vtol_one_chain_2d*>::iterator i;
  for(i=ptr_list->begin();i!=ptr_list->end();++i){
    ref_list->push_back(*i);
  }
  delete ptr_list;
  return ref_list;
}


vcl_vector<vtol_one_chain_2d*> *vtol_face_2d::outside_boundary_compute_one_chains(void)
{

  COPY_INF_2d(vtol_one_chain_2d);
}


// -- Returns a list of all Onechains of the face.

vcl_vector<vtol_one_chain_2d*> *vtol_face_2d::compute_one_chains(void)
{
 
  SEL_INF_2d(vtol_one_chain_2d,compute_one_chains);
}

// --
// Returns a list of that has itself as the only element.  This method
// is needed for traversing the model hierarchy consistently.

vcl_vector<vtol_face_2d*>  *vtol_face_2d::compute_faces(void)
{
 
  LIST_SELF_2d(vtol_face_2d);
}

// -- Returns a list of all the two_chains which contain the vtol_face_2d.
vcl_vector<vtol_two_chain_2d*>  *vtol_face_2d::compute_two_chains(void)
{
 
  SEL_SUP_2d(vtol_two_chain_2d,compute_two_chains);
}

// -- Returns a list of all the blocks that contain the vtol_face_2d.

vcl_vector<vtol_block_2d*> *vtol_face_2d::compute_blocks(void)
{

  SEL_SUP_2d(vtol_block_2d,compute_blocks);
}

//---------------------------------------------------------------------------
// -- Does `this' share an edge with `f' ? Comparison of edge pointers,
//       not geometric values
//---------------------------------------------------------------------------
bool vtol_face_2d::shares_edge_with(vtol_face_2d &f)
{
  bool result;
  edge_list_2d *thisedges;
  edge_list_2d *fedges;
  vcl_vector<vtol_edge_2d_ref>::const_iterator ei1;
  vcl_vector<vtol_edge_2d_ref>::const_iterator ei2;

  result=this==&f;
  if(!result)
    {
      thisedges=edges();
      fedges=f.edges();
      for(ei1=thisedges->begin();!result&&ei1!=thisedges->end();++ei1)
        for(ei2= fedges->begin();!result&&ei2!=fedges->end();++ei2)
          result=(*ei1)==(*ei2);
    }
  return result;
}

// --
// Links new_vtol_one_chain_2d as an inferior of the vtol_face_2d and returns True if
// successful. This method will be replacing all calls to add_edge_lop()o.

void vtol_face_2d::add_one_chain(vtol_one_chain_2d &new_vtol_one_chain_2d)
{
  // require
  assert(new_vtol_one_chain_2d.contains_sub_chains());

  link_inferior(new_vtol_one_chain_2d);
}

// -- deep equality check on faces.  uses fuzzy equal on vertices.
//

bool vtol_face_2d::operator==(const vtol_face_2d &other) const
{
  if (this==&other) return true;

  if ( (_surface.ptr()&&other._surface.ptr()==0)
     ||(other._surface.ptr()&&_surface.ptr()!=0))
    return false;

  if(_surface.ptr() && *_surface!=*(other._surface))
    return false;

  if (numinf()!=other.numinf())
    return false;

  topology_list_2d::const_iterator ti1;
  topology_list_2d::const_iterator ti2;

  for(ti1=_inferiors.begin(),ti2=other._inferiors.begin();
      ti1!=_inferiors.end();
      ++ti1,++ti2)
    if ((*ti1)!=(*ti2))
      return false;

  return true;
}

// -- spatial object equality

bool vtol_face_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::TOPOLOGYOBJECT &&
   ((vtol_topology_object_2d const&)obj).topology_type() == vtol_topology_object_2d::FACE
  ? *this == (vtol_face_2d const&) (vtol_topology_object_2d const&) obj
  : false;
}

// -- Returns the ith inferior vtol_one_chain_2d of the vtol_face_2d.

vtol_one_chain_2d *vtol_face_2d::get_one_chain(int which)
{
  if (which < _inferiors.size())
    return (_inferiors[which])->cast_to_one_chain();
  else
    {
      cerr << "Tried to get bad edge_loop from face" << endl;
      return NULL;
    }
}

// -- Returns the first inferior vtol_one_chain_2d of the vtol_face_2d (the boundary onechain).

vtol_one_chain_2d *vtol_face_2d::get_boundary_cycle(void)
{
  // The outside boundary vtol_one_chain_2d will *always*
  // be the first one chain in the inferiors
  // list.
  return get_one_chain(0);
}

// -- Adds a new hole to the face

bool vtol_face_2d::add_hole_cycle(vtol_one_chain_2d &new_hole)
{
  vtol_one_chain_2d *onech=get_boundary_cycle();

  if(onech!=0)
    {
      onech->link_chain_inferior(new_hole);
      return true;
    } else
      return false;
}

// Returns a list of the one_chains that make up the holes of the vtol_face_2d.

vcl_vector<vtol_one_chain_2d_ref> *vtol_face_2d::get_hole_cycles(void)
{
  vcl_vector<vtol_one_chain_2d_ref> *result;
  topology_list_2d::iterator ii;
  vcl_vector<vtol_one_chain_2d_ref> *templist;
  vcl_vector<vtol_one_chain_2d_ref>::iterator oi;

  result=new vcl_vector<vtol_one_chain_2d_ref>();

  for(ii=_inferiors.begin();ii!=_inferiors.end();++ii)
    {
      templist=(*ii)->cast_to_one_chain()->inferior_one_chains();

      // new_list->insert(new_list->end(),templist->begin(),templist->end());
      for(oi=templist->begin();oi!=templist->end();++oi)
        result->push_back(*oi);
      delete templist;
    }

  return result;
}

// -- Returns the number of edges on the vtol_face_2d.
//

int vtol_face_2d::get_num_edges(void) const
{
  int result=0;
  topology_list_2d::const_iterator ii;

  for(ii=_inferiors.begin();ii!=_inferiors.end();++ii)
    result+=((*ii)->cast_to_one_chain())->numinf();
  return result;
}

// -- reverse the direction of the face 
void vtol_face_2d::reverse_normal(void)
{
  topology_list_2d::iterator ti;
  for(ti=_inferiors.begin();ti!=_inferiors.end();++ti)
    ((vtol_one_chain_2d *)(ti->ptr()))->reverse_directions();
  // compute_normal();
}

//-----------------------------------------------------------------
// -- Compute bounds from the geometry of _surface. If the surface is
//    not fully bounded, then use the vertices.
//
void vtol_face_2d::compute_bounding_box()
{
  /* todo
  if(_surface && _surface->GetGeometryType() != GeometryObject::IMPLICITPLANE)
     {
      // Get bounds from surface.
      // But are bounds consistent with face vertices? -JLM
      // Anyway, this is what was done in get_min_max on vtol_edge_2d.
       if(_surface->GetExtent() == vsol_region_2d::FULLY_BOUNDED)
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
    this->vtol_topology_object_2d::ComputeBoundingBox();
  */
  vtol_topology_object_2d::compute_bounding_box();
}


// --
//  This method describes the data members of the vtol_face_2d including the
// Inferiors.  The blanking argument is used to indent the output in
// a clear fashion.

void vtol_face_2d::describe(ostream &strm,
                            int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  print();
  for(int i=0;i<_inferiors.size();++i)
    {
      if((_inferiors[i])->cast_to_one_chain()!=0)
        (_inferiors[i])->cast_to_one_chain()->describe(strm,blanking);
      else
        cout << "*** Odd inferior for a face" << endl;
    }
}

// --
// This method prints out a simple text representation for the vtol_face_2d which
// includes its address in memory.
void vtol_face_2d::print(ostream &strm) const
{
  strm << "<vtol_face_2d  ";
  topology_list_2d::const_iterator ii;
  
  for(ii=_inferiors.begin();ii!= _inferiors.end();++ii)
    strm << " " << (*ii)->inferiors()->size();
  strm << "   " << (void *) this << '>' << endl;
}
