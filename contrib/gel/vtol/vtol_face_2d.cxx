// This is gel/vtol/vtol_face_2d.cxx
#include "vtol_face_2d.h"
//:
// \file

#include <vcl_cassert.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_one_chain.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_point_2d.h>

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vtol_face_2d::vtol_face_2d(void)
  :surface_(0)
{
}

//---------------------------------------------------------------------------
//: Pseudo copy constructor.  Deep copy.
//---------------------------------------------------------------------------
vtol_face_2d::vtol_face_2d(vtol_face_2d_sptr const& other)
  : surface_(0)
{
  edge_list *edgs=other->edges();
  vertex_list *verts=other->vertices();

  topology_list newedges(edgs->size());
  topology_list newverts(verts->size());

  int i=0;
  for (vertex_list::iterator vi=verts->begin();vi!=verts->end();++vi,++i)
  {
    vtol_vertex_sptr v=(*vi);
    newverts[i]=v->clone()->cast_to_topology_object();
    v->set_id(i);
  }
  int j=0;
  for (edge_list::iterator ei=edgs->begin();ei!= edgs->end();++ei,++j)
  {
    vtol_edge_sptr e=(*ei);

    vtol_topology_object_sptr V1 = newverts[e->v1()->get_id()];
    vtol_topology_object_sptr V2 = newverts[e->v2()->get_id()];
    if (!V1 || !V2)
    {
      vcl_cerr << "Inconsistent topology in vtol_face_2d pseudo copy constructor\n";
      link_inferior(new vtol_one_chain);
      return;
     }
    // make the topology and geometry match
    vtol_edge_sptr newedge=new vtol_edge_2d(V1->cast_to_vertex(),
                                            V2->cast_to_vertex());

    newedges[j]=newedge->cast_to_topology_object();
    e->set_id(j);
  }

  // This is a deep copy of the vtol_face_2d.
  topology_list::const_iterator ii;
  for (ii=other->inferiors()->begin();ii!= other->inferiors()->end();++ii)
    link_inferior((*ii)->cast_to_one_chain()->copy_with_arrays(newverts,newedges));
  delete edgs;
  delete verts;
  set_surface(0);
  if (other->surface_)
    set_surface(other->surface_->clone()->cast_to_region());
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
vsol_spatial_object_2d* vtol_face_2d::clone(void) const
{
  return new vtol_face_2d(vtol_face_2d_sptr(const_cast<vtol_face_2d*>(this)));
}

//: Return a platform independent string identifying the class
vcl_string vtol_face_2d::is_a() const
{
  return vcl_string("vtol_face_2d");
}

vsol_region_2d_sptr vtol_face_2d::surface(void) const
{
  return surface_;
}

//: copy with an array

vtol_face *
vtol_face_2d::copy_with_arrays(topology_list &verts,
                               topology_list &edges) const
{
  vtol_face_2d *newface=new vtol_face_2d();
  topology_list::const_iterator i;
  for (i=newface->inferiors()->begin();i!= newface->inferiors()->end();++i )
    newface->unlink_inferior((*i)->cast_to_one_chain());
  newface->inferiors()->clear();
  for (i=inferiors()->begin();i!=inferiors()->end();++i)
  {
    vtol_one_chain *onech=(*i)->cast_to_one_chain()->copy_with_arrays(verts,edges);
    assert(*onech == *(*i));
    newface->link_inferior(onech);
  }
  if (surface_)
    newface->set_surface(surface_->clone()->cast_to_region());
  return newface;
}

//---------------------------------------------------------------------------
//: Copy with no links. Only copy the surface if it exists
//---------------------------------------------------------------------------
vtol_face *vtol_face_2d::shallow_copy_with_no_links(void) const
{
  vtol_face_2d *result;
  result=new vtol_face_2d;
  result->set_surface(0);
  if (surface_)
    result->set_surface(surface_->clone()->cast_to_region());
  return result;
}

//: Constructor for a planar vtol_face_2d from an ordered list of vertices.
// Edges are constructed by connecting vtol_vertex_2d[i] to vtol_vertex_2d[(i+1)mod L].
// L is the length of the vertex list, verts, and i goes from 0 to L.
// Require: verts.size()>2

vtol_face_2d::vtol_face_2d(vertex_list const& verts)
  :surface_(0)
{
  // require
  assert(verts.size()>2);

  double xmin=0;
  double ymin=0;
  double xmax=1;
  double ymax=1;

  set_surface(new vsol_rectangle_2d(new vsol_point_2d(xmin,ymin),
                                    new vsol_point_2d(xmax,ymin),
                                    new vsol_point_2d(xmax,ymax)));

  //generate a list of edges for edge loop
  bool done=false;
  vertex_list::const_iterator vi=verts.begin();
  vtol_vertex_sptr v01=(*vi);
  edge_list elist;
  vcl_vector<signed char> directions;

  while (!done)
  {
    // if no next vertex, then use the first vertex by calling
    // verts->end() again to wrap around  This will close the loop
    ++vi;

    if (vi==verts.end())
    {
      vi=verts.begin();
      done=true;
    }

    vtol_vertex_sptr v02=(*vi); // get the next vertex (may be first)

    vtol_edge_sptr newedge=v01->new_edge(v02);
    elist.push_back(newedge);

    if (*v02 == *(newedge->v2()))
      directions.push_back((signed char)1);
    else
      directions.push_back((signed char)(-1));
    v01=v02; // in the next go around v1 is v2 of the last
  }

  link_inferior(new vtol_one_chain(elist,directions,true));
}

//: Constructor for a planar face from a list of one_chains.
// This method assumes that the first vtol_one_chain on the list is the outside
// boundary vtol_one_chain.  The remaining one_chains are holes boundaries
// on the face.

vtol_face_2d::vtol_face_2d(one_chain_list const& onechs)
  :surface_(0)
{
  // 1)  Add one chains to the inferiors list.
  //     Assume that the first vtol_one_chain on the
  //     list is the outside boundary...the
  //     remaining one_chains are holes.

  if (onechs.size()>0)
    link_inferior(onechs[0]);

  vtol_one_chain_sptr onech=get_boundary_cycle();

  // 2) This constructor will assume that the
  // surface is an ImplicitPlane().

  double xmin=0;
  double ymin=0;
  double xmax=1;
  double ymax=1;

  set_surface(new vsol_rectangle_2d(new vsol_point_2d(xmin,ymin),
                                    new vsol_point_2d(xmax,ymin),
                                    new vsol_point_2d(xmax,ymax)));

  if (onech)
    for (unsigned int i = 1; i < onechs.size(); ++i)
      onech->link_chain_inferior(onechs[i]);
}

//: Constructor of a Planar face from a vtol_one_chain.
//  This method uses the vtol_one_chain, edgeloop, as the outside boundary of the face.

vtol_face_2d::vtol_face_2d(vtol_one_chain_sptr const& edgeloop)
  : surface_(0)
{
  link_inferior(edgeloop);

  // TODO - surface is set to bounding box rectangle, which is often too large
  set_surface(new vsol_rectangle_2d(new vsol_point_2d(get_min_x(),get_min_y()),
                                    new vsol_point_2d(get_max_x(),get_min_y()),
                                    new vsol_point_2d(get_max_x(),get_max_y())));
}

#if 0 // deprecated
vtol_face_2d::vtol_face_2d(vtol_one_chain &edgeloop)
  : surface_(0)
{
  vcl_cerr << "Warning: deprecated vtol_face_2d constructor\n";
  link_inferior(&edgeloop);

  // TODO - surface is set to bounding box rectangle, which is often too large
  set_surface(new vsol_rectangle_2d(new vsol_point_2d(get_min_x(),get_min_y()),
                                    new vsol_point_2d(get_max_x(),get_min_y()),
                                    new vsol_point_2d(get_max_x(),get_max_y())));
}
#endif

//: Constructor requiring only the underlying geometric surface
vtol_face_2d::vtol_face_2d (vsol_region_2d &facesurf)
  : surface_(0)
{
  set_surface(&facesurf);

  // not much, but at least it's a start...
}

//: Set the underlying geometric surface.
void vtol_face_2d::set_surface(vsol_region_2d_sptr const& newsurf)
{
  surface_=newsurf;
  touch();
}

bool vtol_face_2d::operator==(const vtol_face_2d &other) const
{
  if (this==&other) return true;

  if (numinf()!=other.numinf())
    return false;

  if ( (surface_ && ! other.surface_) ||
       (other.surface_ && ! surface_))
    return false;

  if (surface_ && *surface_!=*(other.surface_))
    return false;

  topology_list::const_iterator ti1;
  topology_list::const_iterator ti2;

  for (ti1=inferiors()->begin(),ti2=other.inferiors()->begin();
       ti1!=inferiors()->end(); ++ti1,++ti2)
    if (!(*(*ti1)== *(*ti2)))
      return false;

  return true;
}

bool vtol_face_2d::operator==(const vtol_face &other) const
{
  if (! other.cast_to_face_2d() )
    return false;
  else
    return *this == (vtol_face_2d const&) other;
}

//: spatial object equality

bool vtol_face_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.cast_to_topology_object() &&
   obj.cast_to_topology_object()->cast_to_face() &&
   obj.cast_to_topology_object()->cast_to_face()->cast_to_face_2d() &&
   *this == *obj.cast_to_topology_object()->cast_to_face()->cast_to_face_2d();
}

//:
//  This method describes the data members of the vtol_face_2d including the
// Inferiors.  The blanking argument is used to indent the output in
// a clear fashion.

void vtol_face_2d::describe(vcl_ostream &strm,
                            int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  print(strm);
  for (unsigned int i=0;i<inferiors()->size();++i)
  {
    if ((inferiors_[i])->cast_to_one_chain()!=0)
      inferiors_[i]->cast_to_one_chain()->describe(strm,blanking);
    else
      strm << "*** Odd inferior for a face\n";
  }
}

//:
// This method prints out a simple text representation for the vtol_face_2d which
// includes its address in memory.
void vtol_face_2d::print(vcl_ostream &strm) const
{
  strm << "<vtol_face_2d ";

  topology_list::const_iterator ii;
  for (ii=inferiors()->begin();ii!= inferiors()->end();++ii)
  {
    strm << ' ' << (*ii)->inferiors()->size();
  }
  strm << "   " << (void const *) this << ">\n";
}

//: copy the geometry

void vtol_face_2d::copy_geometry(const vtol_face &other)
{
  if (other.cast_to_face_2d())
    surface_ = other.cast_to_face_2d()->surface();
}

//: provide a mechanism to compare geometry

bool vtol_face_2d::compare_geometry(const vtol_face &other) const
{
  if (other.cast_to_face_2d())
    return (*surface_)== *(other.cast_to_face_2d()->surface());
  else
    return false;
}
