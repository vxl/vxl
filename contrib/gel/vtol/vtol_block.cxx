#include <vtol/vtol_block.h>

//:
//  \file

#include <vtol/vtol_edge.h>
#include <vtol/vtol_macros.h>
#include <vtol/vtol_list_functions.h>
#include <vsol/vsol_box_3d.h>
#include <vcl_algorithm.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vtol/vtol_two_chain.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vtol_block::vtol_block(void)
{
}

//---------------------------------------------------------------------------
//: Constructor from a two-chain (inferior)
//---------------------------------------------------------------------------
vtol_block::vtol_block(vtol_two_chain &faceloop)
{
  link_inferior(faceloop);
}

//---------------------------------------------------------------------------
//: Constructor from a list of two-chains.
// The first two-chain is the outside boundary.
// The remaining two-chains are considered holes inside the the outside volume.
//---------------------------------------------------------------------------
vtol_block::vtol_block(two_chain_list &faceloops)
{
  if (faceloops.size()>0)
  {
    vtol_two_chain_sptr tc = *((faceloops).begin());

    link_inferior(*tc);
  }

  vtol_two_chain *twoch=get_boundary_cycle();

  if (twoch!=0)
    for (unsigned int i=1;i<faceloops.size();++i)
      twoch->link_chain_inferior(*(faceloops[i]));
}

//---------------------------------------------------------------------------
//: Constructor from a list of faces
//---------------------------------------------------------------------------
vtol_block::vtol_block(face_list &new_face_list)
{
  vtol_two_chain_sptr tc = new vtol_two_chain(new_face_list);

  link_inferior(*(tc));
}

//---------------------------------------------------------------------------
//: Copy constructor. Deep copy.
//---------------------------------------------------------------------------
vtol_block::vtol_block(const vtol_block &other)
{
  vtol_block_sptr oldblock=(vtol_block*)(&other); // const violation
  edge_list *edgs=oldblock->edges();
  vertex_list *verts=oldblock->vertices();

  topology_list newedges(edgs->size());
  topology_list newverts(verts->size());

  int i=0;
  vcl_vector<vtol_vertex_sptr>::iterator vi;
  for (vi=verts->begin();vi!=verts->end();++vi,++i)
    {
      vtol_vertex_sptr v= *vi;
      newverts[i]=v->clone()->cast_to_topology_object();
      v->set_id(i);
    }

  int j=0;
  vcl_vector<vtol_edge_sptr>::iterator ei;
  for (ei=edgs->begin();ei!=edgs->end();++ei,++j)
    {
      vtol_edge_sptr e = *ei;

      newedges[j]=newverts[e->v1()->get_id()]->cast_to_vertex()->new_edge(
                                *(newverts[e->v2()->get_id()]->cast_to_vertex()));

      e->set_id(j);
    }

  const topology_list *old2chains = oldblock->inferiors();
  topology_list::const_iterator tci;

  for (tci=old2chains->begin();tci != old2chains->end();tci++)
    {
      vtol_two_chain_sptr new2ch=(*tci)->cast_to_two_chain()->copy_with_arrays(newverts,newedges);

      assert(*new2ch == *(*tci));

      link_inferior(*new2ch);
    }
  delete edgs;
  delete verts;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_block::~vtol_block()
{
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_sptr vtol_block::clone(void) const
{
  return new vtol_block(*this);
}

// ******************************************************
//
//    Accessor Functions
//

//---------------------------------------------------------------------------
//: Return the topology type
//---------------------------------------------------------------------------
vtol_block::vtol_topology_object_type
vtol_block::topology_type(void) const
{
  return BLOCK;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `inferior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_block::valid_inferior_type(const vtol_topology_object &inferior) const
{
  return inferior.cast_to_two_chain()!=0;
}

//---------------------------------------------------------------------------
//: Is `superior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_block::valid_superior_type(const vtol_topology_object &superior)
const
{
  return false;
}

//: outside boundary vertices

vertex_list *vtol_block::outside_boundary_vertices(void)
{
  vertex_list *result;
  vcl_vector<vtol_vertex *> *ptr_list;
  vcl_vector<vtol_vertex *>::iterator i;

  result=new vertex_list();
  ptr_list=outside_boundary_compute_vertices();
  // copy the lists

  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;

  return result;
}

vcl_vector<vtol_vertex *> *vtol_block::outside_boundary_compute_vertices(void)
{
  OUTSIDE_BOUNDARY(vtol_vertex, vtol_two_chain, compute_vertices);
}

//: get vertex list

vcl_vector<vtol_vertex *> *vtol_block::compute_vertices(void)
{
  SEL_INF(vtol_vertex,compute_vertices);
}

//: get outside boundary zero chains

zero_chain_list *vtol_block::outside_boundary_zero_chains(void)
{
 zero_chain_list *result;
 vcl_vector<vtol_zero_chain *> *ptr_list;
 vcl_vector<vtol_zero_chain *>::iterator i;
 result=new zero_chain_list();
 ptr_list=outside_boundary_compute_zero_chains();
  // copy the lists

  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;

  return result;
}


vcl_vector<vtol_zero_chain *> *
vtol_block::outside_boundary_compute_zero_chains(void)
{
  OUTSIDE_BOUNDARY(vtol_zero_chain,vtol_two_chain,
                   compute_zero_chains);
}

//: get zero chains

vcl_vector<vtol_zero_chain *> *vtol_block::compute_zero_chains(void)
{
 SEL_INF(vtol_zero_chain,compute_zero_chains);
}

//: outside boundary edges

edge_list *vtol_block::outside_boundary_edges(void)
{
  edge_list *result;
  vcl_vector<vtol_edge *> *ptr_list;
  vcl_vector<vtol_edge *>::iterator i;

  result=new edge_list();
  ptr_list=outside_boundary_compute_edges();
  // copy the lists

  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;

  return result;
}

//: outside boundary edges

vcl_vector<vtol_edge *> *vtol_block::outside_boundary_compute_edges(void)
{
 OUTSIDE_BOUNDARY(vtol_edge,vtol_two_chain,compute_edges);
}

//: get edges
vcl_vector<vtol_edge *> *vtol_block::compute_edges(void)
{
  SEL_INF(vtol_edge,compute_edges);
}

//: get outside boundary one chains

one_chain_list *vtol_block::outside_boundary_one_chains(void)
{
  one_chain_list *result;
  vcl_vector<vtol_one_chain *> *ptr_list;
  vcl_vector<vtol_one_chain *>::iterator i;

  result=new one_chain_list;
  ptr_list=outside_boundary_compute_one_chains();
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;
  return result;
}

//: get outside boundary one chains

vcl_vector<vtol_one_chain *> *
vtol_block::outside_boundary_compute_one_chains(void)
{
  OUTSIDE_BOUNDARY(vtol_one_chain,vtol_two_chain,compute_one_chains);
}

//: get the one chains
vcl_vector<vtol_one_chain *> *vtol_block::compute_one_chains(void)
{
  SEL_INF(vtol_one_chain,compute_one_chains);
}

//: get the outside boundary faces

face_list *vtol_block::outside_boundary_faces(void)
{
  face_list *result;
  vcl_vector<vtol_face *> *ptr_list;
  vcl_vector<vtol_face*>::iterator i;

  result=new face_list();
  ptr_list=outside_boundary_compute_faces();
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;
  return result;
}

//: get the outside boundary faces

vcl_vector<vtol_face *> *vtol_block::outside_boundary_compute_faces(void)
{
  OUTSIDE_BOUNDARY(vtol_face,vtol_two_chain,compute_faces);
}

//: get the faces
vcl_vector<vtol_face *> *vtol_block::compute_faces(void)
{
  SEL_INF(vtol_face,compute_faces);
}

//: get the outside boundary two chains

two_chain_list *vtol_block::outside_boundary_two_chains(void)
{
  two_chain_list *result;
  vcl_vector<vtol_two_chain *> *ptr_list;
  vcl_vector<vtol_two_chain *>::iterator i;

  result=new two_chain_list();
  ptr_list=outside_boundary_compute_two_chains();
  for (i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;
  return result;
}

//: get the outside boundary two chains

vcl_vector<vtol_two_chain *> *
vtol_block::outside_boundary_compute_two_chains(void)
{
  OUTSIDE_BOUNDARY(vtol_two_chain,vtol_two_chain,compute_two_chains);
}

//: get the two chains

vcl_vector<vtol_two_chain *> *vtol_block::compute_two_chains(void)
{
  SEL_INF(vtol_two_chain,compute_two_chains);
}

//: get blocks
vcl_vector<vtol_block *> *vtol_block::compute_blocks(void)
{
  LIST_SELF(vtol_block);
}


// ******************************************************
//
//    Operators Functions
//

// This is hardly an equality test...but we`ll leave it for now....pav
// June 1992.

bool vtol_block::operator==(const vtol_block &other) const
{
  vtol_two_chain *twoch1;
  vtol_two_chain *twoch2;
  topology_list::const_iterator bi1;
  topology_list::const_iterator bi2;


  if (this==&other)
    return true;


  if (numinf()!=other.numinf())
    return false;
  else
    {
      for (bi1=inferiors()->begin(),bi2=other.inferiors()->begin(); bi1!=inferiors()->end(); ++bi1,++bi2)
        {
          twoch1=(*bi1)->cast_to_two_chain();

          twoch2=(*bi2)->cast_to_two_chain();

          if (!(*twoch1 == *twoch2))
            return false;
        }
    }

  return true;
}

//: spatial object equality

bool vtol_block::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::TOPOLOGYOBJECT &&
   ((vtol_topology_object const&)obj).topology_type() == vtol_topology_object::BLOCK
  ? *this == (vtol_block const&) (vtol_topology_object const&) obj
  : false;
}

//: get a hole cycle
two_chain_list *vtol_block::hole_cycles(void) const
{
  two_chain_list *result;
  topology_list::const_iterator ti;
  two_chain_list *templist;
  two_chain_list::iterator ii;

  result=new vcl_vector<vtol_two_chain_sptr>();

  for (ti=inferiors()->begin();ti!=inferiors()->end();++ti)
    {
      templist=(*ti)->cast_to_two_chain()->inferior_two_chains();
      // new_list->insert_after(*templist);
      // new_list->insert(new_list->end(),templist->begin(),templist->end());
      for (ii=templist->begin();ii!=templist->end();++ii)
        result->push_back(*ii);
      delete templist;
    }
  return result;
}


//: add a hole cycle

bool vtol_block::add_hole_cycle(vtol_two_chain &new_hole)
{
  vtol_two_chain_sptr twoch=get_boundary_cycle();
  if (! twoch.ptr()) return false;
  twoch->link_chain_inferior(new_hole);
  return true;
}

// ******************************************************
//
//    Print Functions
//


//: print data

void vtol_block::print(vcl_ostream &strm) const
{
  strm<<"<vtol_block "<<inferiors()->size()<<"  "<<(void const*)this<<">"<<vcl_endl;
}

void vtol_block::describe(vcl_ostream &strm,
                             int blanking) const
{
  for (int i=0; i<blanking; ++i) strm << ' ';
  print(strm);
  describe_inferiors(strm,blanking);
  describe_superiors(strm,blanking);
}
