
//:
//  \file

#include "vtol_face_2d.h"

#include <vcl_cassert.h>
#include <vtol/vtol_macros.h>
#include <vtol/vtol_two_chain.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_list_functions.h>
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

//---------------------------------------------------------------------------
//: Copy constructor
//---------------------------------------------------------------------------
// This is the Copy Constructor for vtol_face_2d. It performs a deep copy of
// all vtol_face_2d inferior one_chains.
vtol_face_2d::vtol_face_2d(const vtol_face_2d &other)
  : _surface(0)
{
  vtol_face_2d *oldf=(vtol_face_2d *)(&other); // const violation
  edge_list *edgs=oldf->edges();
  vertex_list *verts=oldf->vertices();

  vcl_vector<vtol_topology_object_sptr> newedges(edgs->size());
  vcl_vector<vtol_topology_object_sptr> newverts(verts->size());

  int i=0;
  vcl_vector<vtol_vertex_sptr>::iterator vi;
  for(vi=verts->begin();vi!=verts->end();++vi,++i)
    {
      vtol_vertex_sptr v=(*vi);
      newverts[i]=v->clone().ptr()->cast_to_topology_object();
      v->set_id(i);
    }
  int j=0;
  vcl_vector<vtol_edge_sptr>::iterator ei;
  for(ei=edgs->begin();ei!= edgs->end();++ei,++j)
    {
      vtol_edge_sptr e=(*ei);

      vtol_topology_object_sptr V1 = newverts[e->v1()->get_id()];
      vtol_topology_object_sptr V2 = newverts[e->v2()->get_id()];
      if(!V1 || !V2)
        {
           vcl_cerr << "Inconsistent topology in vtol_face_2d copy constructor\n";
           vtol_one_chain_sptr inf = new vtol_one_chain();
          link_inferior(*inf);
          return;
         }
      // make the topology and geometry match
      vtol_edge_sptr newedge=new vtol_edge_2d(*(V1->cast_to_vertex()->cast_to_vertex_2d()),
                                            *(V2->cast_to_vertex()->cast_to_vertex_2d()));


      //newedges[j]=(vtol_topology_object_2d*)(newedge.ptr());
      newedges[j]=newedge.ptr();
      e->set_id(j);
    }
  // This is a deep copy of the vtol_face_2d.

  topology_list::iterator ii;
  for(ii=oldf->_inferiors.begin();ii!= oldf->_inferiors.end();++ii)
    {
      vtol_one_chain_sptr onech=(*ii)->cast_to_one_chain()->copy_with_arrays(newverts,newedges);
      link_inferior(*onech);
    }
  delete edgs;
  delete verts;
  set_surface(0);
  if(oldf->_surface)
    set_surface((vsol_region_2d*)(oldf->_surface->clone().ptr()));
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_face_2d::~vtol_face_2d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization.
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_sptr vtol_face_2d::clone(void) const
{
  return new vtol_face_2d(*this);
}

vsol_region_2d_sptr vtol_face_2d::surface(void) const
{
  return _surface;
}

//: copy with an array

vtol_face *
vtol_face_2d::copy_with_arrays(vcl_vector<vtol_topology_object_sptr> &verts,
                               vcl_vector<vtol_topology_object_sptr> &edges) const
{
  vtol_face_2d *newface=new vtol_face_2d();
  topology_list::const_iterator i;
  for(i=newface->_inferiors.begin();i!= newface->_inferiors.end();++i )
    {
      vtol_topology_object_sptr obj=(*i);
      newface->unlink_inferior(*obj);
    }
  newface->_inferiors.clear();
  for(i=_inferiors.begin();i!=_inferiors.end();++i)
    {
      vtol_one_chain *onech=(*i)->cast_to_one_chain()->copy_with_arrays(verts,edges);
      assert(*onech == *(*i));
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
//: Return `this' if `this' is a face, 0 otherwise
//---------------------------------------------------------------------------
const vtol_face_2d *vtol_face_2d::cast_to_face_2d(void) const
{
  return this;
}

//---------------------------------------------------------------------------
//: Return `this' if `this' is a face, 0 otherwise
//---------------------------------------------------------------------------
vtol_face_2d *vtol_face_2d::cast_to_face_2d(void)
{
  return this;
}

//---------------------------------------------------------------------------
//: Copy with no links. Only copy the surface if it exists
//---------------------------------------------------------------------------
vtol_face *vtol_face_2d::shallow_copy_with_no_links(void) const
{
  vtol_face_2d *result;
  result=new vtol_face_2d;
  result->set_surface(0);
  if(_surface)
    result->set_surface((vsol_region_2d*)(_surface->clone().ptr()));
  return result;
}

//: Constructor for a planar vtol_face_2d from an ordered list of vertices.
// Edges are constructed by connecting vtol_vertex_2d[i] to vtol_vertex_2d[(i+1)mod L].
// L is the length of the vertex list, verts, and i goes from 0 to L.
// Require: verts.size()>2

vtol_face_2d::vtol_face_2d(vertex_list &verts)
  :_surface(0)
{
  // require
  assert(verts.size()>2);

  vsol_point_2d_sptr p0;
  vsol_point_2d_sptr p1;
  vsol_point_2d_sptr p2;
  double xmin=0;
  double ymin=0;
  double xmax=1;
  double ymax=1;
  edge_list elist;
  vcl_vector<signed char> directions;
  vtol_edge_sptr newedge;
  bool done;
  vertex_list::iterator vi;
  vtol_vertex_sptr v01;
  vtol_vertex_sptr v02;
  vtol_one_chain_sptr eloop;

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
      v01=v02; // in the next go around v1 is v2 of the last
    }

  eloop=new vtol_one_chain(elist,directions,true);

  link_inferior(*eloop);
}

//: Constructor for a planar face from a list of one_chains.
// This method assumes that the first vtol_one_chain on the list is the outside
// boundary vtol_one_chain.  The remaining one_chains are holes boundaries
// on the face.

vtol_face_2d::vtol_face_2d(one_chain_list &onechs)
  :_surface(0)
{
  // 1)  Add one chains to the inferiors list.
  //     Assume that the first vtol_one_chain on the
  //     list is the outside boundary...the
  //     remaining one_chains are holes.

  if(onechs.size()>0)
    link_inferior(*(onechs[0]));

  vtol_one_chain *onech=get_boundary_cycle();

  // 2) This constructor will assume that the
  // surface is an ImplicitPlane().

   vsol_point_2d_sptr p0,p1,p2;
  double xmin=0;
  double ymin=0;
  double xmax=1;
  double ymax=1;

  set_surface(new vsol_rectangle_2d(new vsol_point_2d(xmin,ymin),
                                    new vsol_point_2d(xmax,ymin),
                                    new vsol_point_2d(xmax,ymax)));

  if(onech!=0)
    {
      for(unsigned int i = 1; i < onechs.size(); ++i)
        onech->link_chain_inferior(*(onechs[i]));
    }
}

//: Constructor of a Planar face from a vtol_one_chain.
//  This method uses the vtol_one_chain, edgeloop, as the outside boundary of the face.

vtol_face_2d::vtol_face_2d(vtol_one_chain &edgeloop)
  : _surface(0)
{
  link_inferior(edgeloop);
  topology_list faces;
  vtol_face_2d* f = (vtol_face_2d*) this;
  faces.push_back(f);

  // big todo
   vsol_point_2d_sptr p0,p1,p2;
  double xmin=0;
  double ymin=0;
  double xmax=1;
  double ymax=1;

  set_surface(new vsol_rectangle_2d(new vsol_point_2d(xmin,ymin),
                                    new vsol_point_2d(xmax,ymin),
                                    new vsol_point_2d(xmax,ymax)));
}

//: Constructor requiring only the underlying geometric surface
vtol_face_2d::vtol_face_2d (vsol_region_2d &facesurf)
  : _surface(0)
{
  set_surface(&facesurf);

  // not much, but at least it's a start...
}


//: Set the underlying geometric surface.
void vtol_face_2d::set_surface(vsol_region_2d *const newsurf)
{
  _surface=newsurf;
  touch();
}

bool vtol_face_2d::operator==(const vtol_face_2d &other) const
{
  if (this==&other) return true;


  if ( (_surface.ptr() && other._surface.ptr()==0)
     ||(other._surface.ptr() && _surface.ptr()==0))
    return false;


  if(_surface.ptr() && *_surface!=*(other._surface))
    return false;


  if (numinf()!=other.numinf())
    return false;

  topology_list::const_iterator ti1;
  topology_list::const_iterator ti2;

  for(ti1=_inferiors.begin(),ti2=other._inferiors.begin();
      ti1!=_inferiors.end();
      ++ti1,++ti2)
    {
      if (!(*(*ti1)== *(*ti2)))
        return false;
    }

  return true;
}

bool vtol_face_2d::operator==(const vtol_face &other) const
{
  if (! other.cast_to_face_2d() )
    return false;
  return *this == (vtol_face_2d const&) other;
}

//: spatial object equality

bool vtol_face_2d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::TOPOLOGYOBJECT &&
   ((vtol_topology_object const&)obj).topology_type() == vtol_topology_object::FACE
  ? *this == (vtol_face_2d const&) (vtol_topology_object const&) obj
  : false;
}


//:
//  This method describes the data members of the vtol_face_2d including the
// Inferiors.  The blanking argument is used to indent the output in
// a clear fashion.

void vtol_face_2d::describe(vcl_ostream &strm,
                            int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  print();
  for(int i=0;i<_inferiors.size();++i)
    {
      if((_inferiors[i])->cast_to_one_chain()!=0)
        (_inferiors[i])->cast_to_one_chain()->describe(strm,blanking);
      else
        vcl_cout << "*** Odd inferior for a face" << vcl_endl;
    }
}

//:
// This method prints out a simple text representation for the vtol_face_2d which
// includes its address in memory.
void vtol_face_2d::print(vcl_ostream &strm) const
{
  strm << "<vtol_face_2d  ";
  topology_list::const_iterator ii;

  for(ii=_inferiors.begin();ii!= _inferiors.end();++ii)
    strm << " " << (*ii)->inferiors()->size();
  strm << "   " << (void const *) this << '>' << vcl_endl;
}

//: provide a mechanism to compare geometry

bool vtol_face_2d::compare_geometry(const vtol_face &other) const
{
  if(other.cast_to_face_2d()){
    return (*_surface)== *(other.cast_to_face_2d()->surface());
  }
  return false;
}
