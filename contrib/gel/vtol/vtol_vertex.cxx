//:
//  \file

#include <vtol/vtol_vertex.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_face.h>
#include <vtol/vtol_macros.h>
#include <vtol/vtol_list_functions.h>

#include <vcl_cassert.h>


//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vtol_vertex::vtol_vertex(void)
{
}

#if 0
//---------------------------------------------------------------------------
//: Copy constructor. Copy the point but not the links
//---------------------------------------------------------------------------
vtol_vertex::vtol_vertex(const vtol_vertex &other)
{
  copy_geometry(other);

  //point_=new vsol_point(*other.point_);
}
#endif

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_vertex::~vtol_vertex()
{
  unlink_all_inferiors();
}

#if 0
//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
 vsol_spatial_object_2d_sptr vtol_vertex::clone(void) const
{
  return new vtol_vertex(*this);
}
#endif

//******************************************************
//*
//*    Accessor Functions
//*

//---------------------------------------------------------------------------
//: Return the topology type
//---------------------------------------------------------------------------
vtol_vertex::vtol_topology_object_type
vtol_vertex::topology_type(void) const
{
  return VERTEX;
}

//: Returns a list of Vertices which only contains a pointer to itself.
vcl_vector<vtol_vertex*> *vtol_vertex::compute_vertices(void)
{
  LIST_SELF(vtol_vertex);
}

//: Returns a list of ZeroChains that contain the vertex. This is the vertex superiors list.
vcl_vector<vtol_zero_chain*>* vtol_vertex::compute_zero_chains(void)
{
  SEL_SUP(vtol_zero_chain,compute_zero_chains);
}


//: Returns a list of Edges which contain the vertex.
 vcl_vector<vtol_edge*>* vtol_vertex::compute_edges(void)
{
  SEL_SUP(vtol_edge,compute_edges);
}

//: Returns a list of OneChains which contain the vertex.
vcl_vector<vtol_one_chain*>* vtol_vertex::compute_one_chains(void)
{
  SEL_SUP(vtol_one_chain,compute_one_chains);
}

//: Returns a list of Faces which contain the vertex.
 vcl_vector<vtol_face*>* vtol_vertex::compute_faces(void)
{
  SEL_SUP(vtol_face,compute_faces);
}

//: Returns a list of TwoChains which contain the vertex.
  vcl_vector<vtol_two_chain*>* vtol_vertex::compute_two_chains(void)
{
  SEL_SUP(vtol_two_chain,compute_two_chains);
}

//: Returns a list of Blocks which contain the vertex.
vcl_vector<vtol_block*>* vtol_vertex::compute_blocks(void)
{
  SEL_SUP(vtol_block,compute_blocks);
}


//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `inferior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_vertex::valid_inferior_type(const vtol_topology_object &) const
{
  return false; // a vertex can never have an inferior
}

//---------------------------------------------------------------------------
//: Is `superior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_vertex::valid_superior_type(const vtol_topology_object &superior) const
{
  return superior.cast_to_zero_chain()!=0;
}

//******************************************************
//*
//*    Print Functions
//*

//: This method outputs a simple text representation of the vertex including its address in memory.
void vtol_vertex::print(vcl_ostream &strm) const
{
  strm<< "<vtol_vertex " << (void const *)this<<"> with id ";
  strm<< get_id() << vcl_endl;
}


//: This method outputs a detailed description of the vertex including the inferiors and superiors.
void vtol_vertex::describe(vcl_ostream &strm,
                              int blanking) const
{
  for (int i=0; i<blanking; ++i) strm << ' ';
  print(strm);
  describe_inferiors(strm, blanking);
  describe_superiors(strm, blanking);
}


//******************************************************
//*
//*    Implementor Functions
//*

//-----------------------------------------------------------------------------
//: Is `this' connected with `v2' ?
//    ie has a superior of `this' `v2' as inferior ?
//-----------------------------------------------------------------------------
bool vtol_vertex::is_connected(const vtol_vertex &v2)
{
  edge_list *vertedges=edges();
  edge_list::const_iterator i;
  for (i=vertedges->begin();i!=vertedges->end();++i)
    if ((*i)->is_endpoint(v2))
      {
        delete vertedges;
        return true;
      }
  delete vertedges;
  return false;
}


//: This method returns true if the Edge, edg, is on the superior list of the vertex.
bool vtol_vertex::is_endpoint(const vtol_edge &edg)
{
  vtol_edge_sptr e=(vtol_edge*)(&edg); // const violation
  const edge_list *edge_list=edges();
  bool result=vcl_find(edge_list->begin(),edge_list->end(),e)!=edge_list->end();
  delete edge_list;
  return result;
}

//---------------------------------------------------------------------------
//: Assignment of `this' with `other' (copy the point not the links)
//---------------------------------------------------------------------------
vtol_vertex &vtol_vertex::operator=(const vtol_vertex &other)
{
  if (this!=&other)
    {
      this->copy_geometry(other);
      // point_->set_x(other.point_->x());
      // point_->set_y(other.point_->y());

      touch();
    }
  return *this;
}

//: spatial object equality

bool vtol_vertex::operator==(const vsol_spatial_object_2d& obj) const
{
  return
  obj.spatial_type() == vsol_spatial_object_2d::TOPOLOGYOBJECT &&
   ((vtol_topology_object const&)obj).topology_type() == vtol_topology_object::VERTEX
  ? *this == (vtol_vertex const&) (vtol_topology_object const&) obj
  : false;
}


//---------------------------------------------------------------------------
//: Is `this' has the same coordinates for its point than `other' ?
//---------------------------------------------------------------------------
bool vtol_vertex::operator== (const vtol_vertex &other) const
{
  bool result;

  result=this==&other;
  if (!result)
    result= compare_geometry(other);
    // (*point_)==(*(other.point_));
  return result;
}

// ******************************************************
//
//    Functions
//

//
//-----------------------------------------------------------------------------
//:
// Determine which other vertices share edges with this. Add any of these which
// are not in the list to it, and recursively call explore_vertex on them. The
// method is intended to recover all of the vertices in a single topological
// structure which is composed of connected edges.
//
void vtol_vertex::explore_vertex(vertex_list &verts)
{
  // Note that "this" is not first put on the list:
  // it will be put as the second element, during the first recursive call.

  edge_list *e_list=this->edges();
  for (edge_list::iterator i=e_list->begin();i!=e_list->end();++i)
    {
      vtol_edge_sptr e=*i;
      vtol_vertex_sptr vv;
      if (e->v1()==this)
        vv=e->v2();
      else if (e->v2()==this)
        vv=e->v1();
      else
        {
          vcl_cerr << "Explore vtol_vertex: shouldn't get this\n";
          assert(false);
          continue;
        }

      if (vcl_find(verts.begin(),verts.end(),vv)==verts.end())
        {
          verts.push_back(vv);
          vv->explore_vertex(verts);
        }
    }
  delete e_list;
}
