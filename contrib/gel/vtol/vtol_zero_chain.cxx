#include "vtol_zero_chain.h"

//:
//  \file

#include <vcl_cassert.h>
#include <vtol/vtol_macros.h>
#include <vtol/vtol_list_functions.h>
#include <vtol/vtol_edge.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default constructor. Empty zero-chain
//---------------------------------------------------------------------------
vtol_zero_chain::vtol_zero_chain(void)
{
}

//---------------------------------------------------------------------------
//: Constructor from two vertices (to make an edge creation easier)
// Require: v1.ptr()!=0 and v2.ptr()!=0 and v1.ptr()!=v2.ptr()
//---------------------------------------------------------------------------
vtol_zero_chain::vtol_zero_chain(vtol_vertex &v1,
                                 vtol_vertex &v2)
{
  // require
  //  assert(v1.ptr()!=0);
  //assert(v2.ptr()!=0);
  assert(&v1!=&v2);

  link_inferior(v1);

  link_inferior(v2);
}

//---------------------------------------------------------------------------
//: Constructor from an array of vertices
// Require: new_vertices.size()>0
//---------------------------------------------------------------------------
vtol_zero_chain::vtol_zero_chain(const vcl_vector<vtol_vertex_sptr> &new_vertices)
{
  // require
  assert(new_vertices.size()>0);

  vcl_vector<vtol_vertex_sptr>::const_iterator i;
  for (i=new_vertices.begin();i!=new_vertices.end();++i )
    link_inferior(*(*i));
}

//---------------------------------------------------------------------------
//: Copy constructor. Copy the vertices and the links
//---------------------------------------------------------------------------
vtol_zero_chain::vtol_zero_chain(const vtol_zero_chain &other)
{
  vtol_vertex_sptr new_vertex;
  vcl_vector<vtol_topology_object_sptr>::const_iterator i;

  for (i=other.inferiors()->begin();i!=other.inferiors()->end();++i)
    {
      new_vertex = (*i)->clone()->cast_to_topology_object()->cast_to_vertex();

      // new_vertex=(vtol_vertex *)((*i)->clone().ptr());
      link_inferior(*new_vertex);
    }
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_zero_chain::~vtol_zero_chain()
{
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_sptr vtol_zero_chain::clone(void) const
{
  return new vtol_zero_chain(*this);
}

//---------------------------------------------------------------------------
//: Return the topology type
//---------------------------------------------------------------------------
vtol_zero_chain::vtol_topology_object_type
vtol_zero_chain::topology_type(void) const
{
  return ZEROCHAIN;
}

//---------------------------------------------------------------------------
//: Return the first vertex of `this'. If it does not exist, return 0
//---------------------------------------------------------------------------
vtol_vertex_sptr vtol_zero_chain::v0(void) const
{
  if (numinf()>0)
    return (vtol_vertex*)(inferiors()->begin()->ptr());
  else
    return 0;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `inferior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_zero_chain::valid_inferior_type(const vtol_topology_object &inferior) const
{
   return inferior.cast_to_vertex()!=0;
}

//---------------------------------------------------------------------------
//: Is `superior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_zero_chain::valid_superior_type(const vtol_topology_object &superior) const
{
  return superior.cast_to_edge()!=0;
}

//---------------------------------------------------------------------------
//: Return the length of the zero-chain
//---------------------------------------------------------------------------
int vtol_zero_chain::length(void) const
{
 return numinf();
}

//: get list of vertices
vcl_vector<vtol_vertex*>* vtol_zero_chain::compute_vertices(void)
{
  COPY_INF(vtol_vertex);
}

//: get list of zero chains
vcl_vector<vtol_zero_chain*>* vtol_zero_chain::compute_zero_chains(void)
{
  LIST_SELF(vtol_zero_chain);
}

//: get list of edges
vcl_vector<vtol_edge*>* vtol_zero_chain::compute_edges(void)
{
  SEL_SUP(vtol_edge,compute_edges);
}

//: get list of one chains

vcl_vector<vtol_one_chain*>* vtol_zero_chain::compute_one_chains(void)
{
  SEL_SUP(vtol_one_chain, compute_one_chains);
}

//: get list of faces

vcl_vector<vtol_face*> *vtol_zero_chain::compute_faces(void)
{
  SEL_SUP(vtol_face, compute_faces);
}

//: get list of two chain

vcl_vector<vtol_two_chain*>* vtol_zero_chain::compute_two_chains(void)
{
  SEL_SUP(vtol_two_chain, compute_two_chains);
}

//: get list of blocks
vcl_vector<vtol_block*>* vtol_zero_chain::compute_blocks(void)
{
   SEL_SUP(vtol_block, compute_blocks);
}


// operators

bool vtol_zero_chain::operator==(const vtol_zero_chain &other) const
{
  const vcl_vector<vtol_topology_object_sptr> *inf1=inferiors();
  const vcl_vector<vtol_topology_object_sptr> *inf2=other.inferiors();
  //vtol_topology_object_sptr v1;
  //vtol_topology_object_sptr v2;
  vcl_vector<vtol_topology_object_sptr>::const_iterator i1;
  vcl_vector<vtol_topology_object_sptr>::const_iterator i2;

  bool result=this==&other;

  if (!result)
    {
      result=inf1->size()==inf2->size();
      if (result)
        for (i1=inf1->begin(),i2=inf2->begin(); i1!=inf1->end()&&result; ++i1,++i2)
          result=*(*i1)==*(*i2);
    }
  return result;
}


//: spatial object equality

bool vtol_zero_chain::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::TOPOLOGYOBJECT &&
   ((vtol_topology_object const&)obj).topology_type() == vtol_topology_object::ZEROCHAIN
  ? *this == (vtol_zero_chain const&) (vtol_topology_object const&) obj
  : false;
}


/*******  Print Methods   *************/

//: print the object

void vtol_zero_chain::print(vcl_ostream &strm) const
{
  strm << "<vtol_zero_chain " << inferiors()->size() << " " << (void const*)this << ">\n";
}

void vtol_zero_chain::describe(vcl_ostream &strm,
                               int blanking) const
{
  for (int j=0; j<blanking; ++j)
    strm << ' ';
  print(strm);
  describe_inferiors(strm, blanking);
  describe_superiors(strm, blanking);
}
