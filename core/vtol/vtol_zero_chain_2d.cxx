#include "vtol_zero_chain_2d.h"

#include <vcl/vcl_cassert.h>
#include <vtol/vtol_macros_2d.h>
#include <vtol/vtol_list_functions_2d.h>
#include <vtol/vtol_edge_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Name: vtol_zero_chain_2d
// Task: Default constructor. Empty zero-chain
//---------------------------------------------------------------------------
vtol_zero_chain_2d::vtol_zero_chain_2d(void) 
{
}

//---------------------------------------------------------------------------
// Name: vtol_zero_chain_2d
// Task: Constructor from two vertices (to make an edge creation easier)
// Require: v1.ptr()!=0 and v2.ptr()!=0 and v1.ptr()!=v2.ptr()
//---------------------------------------------------------------------------
vtol_zero_chain_2d::vtol_zero_chain_2d(vtol_vertex_2d &v1,
                                       vtol_vertex_2d &v2)
{
  // require
  //  assert(v1.ptr()!=0);
  //assert(v2.ptr()!=0);
  assert(&v1!=&v2);

  link_inferior(v1); 
  link_inferior(v2); 
}

//---------------------------------------------------------------------------
// Name: vtol_zero_chain_2d
// Task: Constructor from an array of vertices
// Require: new_vertices.size()>0
//---------------------------------------------------------------------------
vtol_zero_chain_2d::vtol_zero_chain_2d(const vcl_vector<vtol_vertex_2d_ref> &new_vertices)
{
  // require
  assert(new_vertices.size()>0);

  vcl_vector<vtol_vertex_2d_ref>::const_iterator i;
  for(i=new_vertices.begin();i!=new_vertices.end();++i )
    link_inferior(*(*i)); 
}

//---------------------------------------------------------------------------
// Name: vtol_zero_chain_2d
// Task: Copy constructor. Copy the vertices and the links
//---------------------------------------------------------------------------
vtol_zero_chain_2d::vtol_zero_chain_2d(const vtol_zero_chain_2d &other)
{
  vtol_vertex_2d *new_vertex;
  vcl_vector<vtol_topology_object_2d_ref>::const_iterator i;

  for(i=other.inferiors()->begin();i!=other.inferiors()->end();++i)
    {
      new_vertex=(vtol_vertex_2d *)((*i)->clone().ptr());
      link_inferior(*new_vertex);
    }
}

//---------------------------------------------------------------------------
// Name: ~vtol_zero_chain_2d
// Task: Destructor
//---------------------------------------------------------------------------
vtol_zero_chain_2d::~vtol_zero_chain_2d()
{
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
// Name: clone
// Task: Clone `this': creation of a new object and initialization
//       See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_ref vtol_zero_chain_2d::clone(void) const
{
  return new vtol_zero_chain_2d(*this);
}

//---------------------------------------------------------------------------
// Name: topology_type
// Task: Return the topology type
//---------------------------------------------------------------------------
vtol_zero_chain_2d::vtol_topology_object_2d_type
vtol_zero_chain_2d::topology_type(void) const
{
  return ZEROCHAIN;
}

//---------------------------------------------------------------------------
// Name: v0
// Task: Return the first vertex of `this'. If it does not exist, return 0
//---------------------------------------------------------------------------
vtol_vertex_2d_ref vtol_zero_chain_2d::v0(void) const
{
  if(numinf()>0)
    return (vtol_vertex_2d*)(inferiors()->begin()->ptr());
  else
    return 0;
}

//***************************************************************************
// Replaces dynamic_cast<T>
//***************************************************************************

//---------------------------------------------------------------------------
// Name: cast_to_zero_chain
// Task: Return `this' if `this' is a zero_chain, 0 otherwise
//---------------------------------------------------------------------------
const vtol_zero_chain_2d *
vtol_zero_chain_2d::cast_to_zero_chain(void) const
{
  return this;
}

//---------------------------------------------------------------------------
// Name: cast_to_zero_chain
// Task: Return `this' if `this' is a zero_chain, 0 otherwise
//---------------------------------------------------------------------------
vtol_zero_chain_2d *vtol_zero_chain_2d::cast_to_zero_chain(void)
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
vtol_zero_chain_2d::valid_inferior_type(const vtol_topology_object_2d &inferior) const
{
  return inferior.cast_to_vertex()!=0;
}

//---------------------------------------------------------------------------
// Name: valid_superior_type
// Task: Is `superior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_zero_chain_2d::valid_superior_type(const vtol_topology_object_2d &superior) const
{
  return superior.cast_to_edge()!=0;
}

//---------------------------------------------------------------------------
//: Return the length of the zero-chain
//---------------------------------------------------------------------------
int vtol_zero_chain_2d::length(void) const
{
 return numinf();
}

//:
// get list of vertices
vcl_vector<vtol_vertex_2d*>* vtol_zero_chain_2d::compute_vertices(void)
{
  COPY_INF_2d(vtol_vertex_2d);
}

//:
// get list of zero chains
vcl_vector<vtol_zero_chain_2d*>* vtol_zero_chain_2d::compute_zero_chains(void)
{
 
  LIST_SELF_2d(vtol_zero_chain_2d);
}

//: 
// get list of edges 
vcl_vector<vtol_edge_2d*>* vtol_zero_chain_2d::compute_edges(void)
{
  SEL_SUP_2d(vtol_edge_2d,compute_edges);
}

//:
// get list of one chains

vcl_vector<vtol_one_chain_2d*>* vtol_zero_chain_2d::compute_one_chains(void)
{
  
  SEL_SUP_2d(vtol_one_chain_2d, compute_one_chains);
}

//: 
// get list of faces 

vcl_vector<vtol_face_2d*> *vtol_zero_chain_2d::compute_faces(void)
{
  
  SEL_SUP_2d(vtol_face_2d, compute_faces);
}

//:
// get list of two chain

vcl_vector<vtol_two_chain_2d*>* vtol_zero_chain_2d::compute_two_chains(void)
{
  
  SEL_SUP_2d(vtol_two_chain_2d, compute_two_chains);
}

//:
// get list of blocks 
vcl_vector<vtol_block_2d*>* vtol_zero_chain_2d::compute_blocks(void)
{
   SEL_SUP_2d(vtol_block_2d, compute_blocks);
}


//:
// operators

bool vtol_zero_chain_2d::operator==(const vtol_zero_chain_2d &other) const
{
  bool result;
  
  const vcl_vector<vtol_topology_object_2d_ref> *inf1=inferiors();
  const vcl_vector<vtol_topology_object_2d_ref> *inf2=other.inferiors();
  //vtol_topology_object_2d_ref v1;
  //vtol_topology_object_2d_ref v2;
  vcl_vector<vtol_topology_object_2d_ref>::const_iterator i1;
  vcl_vector<vtol_topology_object_2d_ref>::const_iterator i2;
  
  result=this==&other;
  
  if(!result)
    {
      result=inf1->size()==inf2->size();
      if(result)
        { 
          for(i1=inf1->begin(),i2=inf2->begin();
              i1!=inf1->end()&&result;
              ++i1,++i2)
            {
              //v1=(*i1);
              //v2=(*i2);
              result=*(*i1)==*(*i2);
            }
        }
    }
  return result;
}


//:
// spatial object equality

bool vtol_zero_chain_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::TOPOLOGYOBJECT &&
   ((vtol_topology_object_2d const&)obj).topology_type() == vtol_topology_object_2d::ZEROCHAIN
  ? *this == (vtol_zero_chain_2d const&) (vtol_topology_object_2d const&) obj
  : false;
}


/*******  Print Methods   *************/

//:
// print the object 

void vtol_zero_chain_2d::print(ostream &strm) const
{
  strm << "<vtol_zero_chain_2d " << _inferiors.size() << " " << (void *)this << ">" << endl;
}

void vtol_zero_chain_2d::describe(ostream &strm,
                                  int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  print(strm);
  describe_inferiors(strm, blanking);
  describe_superiors(strm, blanking);
}
