#include <vtol/vtol_block_2d.h>

#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_macros_2d.h>
#include <vtol/vtol_list_functions_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_vector.h>
#include <vtol/vtol_two_chain_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Name: vtol_block_2d
// Task: Default constructor
//---------------------------------------------------------------------------
vtol_block_2d::vtol_block_2d(void)
{
}

//---------------------------------------------------------------------------
// Name: vtol_block_2d
// Task: Constructor from a two-chain (inferior)
//---------------------------------------------------------------------------
vtol_block_2d::vtol_block_2d(vtol_two_chain_2d &faceloop)
{	
  link_inferior(faceloop);
}

//---------------------------------------------------------------------------
// Name: vtol_block_2d
// Task: Constructor from a list of two-chains. The first two-chain is the
//       outside boundary. The remaining two-chains are considered holes
//       inside the the outside volume.
//---------------------------------------------------------------------------
vtol_block_2d::vtol_block_2d(two_chain_list_2d &faceloops)
{
  int i;
  if(faceloops.size()>0)
    link_inferior(*((vtol_topology_object_2d *)faceloops[0].ptr()));

  vtol_two_chain_2d *twoch=get_boundary_cycle();
  
  if(twoch!=0)
    for(i=1;i<faceloops.size();++i)
      twoch->link_chain_inferior(*(faceloops[i]));
}

//---------------------------------------------------------------------------
// Name: vtol_block_2d
// Task: Constructor from a list of faces
//---------------------------------------------------------------------------
vtol_block_2d::vtol_block_2d(face_list_2d &new_face_list) 
{
  // tagged_union((vcl_vector<vsol_spatial_object_2d *> *)&new_face_list);
  link_inferior(*(new vtol_two_chain_2d(new_face_list)));
}

//---------------------------------------------------------------------------
// Name: vtol_block_2d
// Task: Copy constructor. Deep copy.
//---------------------------------------------------------------------------
vtol_block_2d::vtol_block_2d(const vtol_block_2d &other)
{
  vtol_block_2d_ref oldblock=(vtol_block_2d*)(&other);
  edge_list_2d *edgs=oldblock->edges();
  vertex_list_2d *verts=oldblock->vertices();

  topology_list_2d newedges(edgs->size());
  topology_list_2d newverts(verts->size());

  int i=0;
  vcl_vector<vtol_vertex_2d_ref>::iterator vi;
  for(vi=verts->begin();vi!=verts->end();++vi,++i)
    {
      vtol_vertex_2d_ref v= *vi;
      newverts[i]=v->clone().ptr()->cast_to_topology_object_2d();
      v->set_id(i);
    }
  int j=0;
  vcl_vector<vtol_edge_2d_ref>::iterator ei;
  for(ei=edgs->begin();ei!=edgs->end();++ei,++j)
    {
      vtol_edge_2d_ref e = *ei;
      newedges[j]=new vtol_edge_2d(*(newverts[e->v1()->get_id()]->cast_to_vertex()),
                                   *(newverts[e->v2()->get_id()]->cast_to_vertex()));
      e->set_id(j);
    }

  vcl_vector<vtol_two_chain_2d_ref> *old2chains=oldblock->two_chains();

  for(int k=0;k<old2chains->size();++k)
    {
      vtol_two_chain_2d_ref new2ch=((*old2chains)[k])->copy_with_arrays(newverts,newedges);
      link_inferior(*new2ch);
    }
  delete edgs;
  delete verts;
  delete old2chains;
}

//---------------------------------------------------------------------------
// Name: ~vtol_block_2d
// Task: Destructor
//---------------------------------------------------------------------------
vtol_block_2d::~vtol_block_2d()
{
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
// Name: clone
// Task: Clone `this': creation of a new object and initialization
//       See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_ref vtol_block_2d::clone(void) const
{
  return new vtol_block_2d(*this);
}

/*
 ******************************************************
 *
 *    Accessor Functions
 */

//---------------------------------------------------------------------------
// Name: topology_type
// Task: Return the topology type
//---------------------------------------------------------------------------
vtol_block_2d::vtol_topology_object_2d_type
vtol_block_2d::topology_type(void) const
{
  return BLOCK;
}

//***************************************************************************
// Replaces dynamic_cast<T>
//***************************************************************************

//---------------------------------------------------------------------------
// Name: cast_to_block
// Task: Return `this' if `this' is a block, 0 otherwise
//---------------------------------------------------------------------------
const vtol_block_2d *vtol_block_2d::cast_to_block(void) const
{
  return this;
}

//---------------------------------------------------------------------------
// Name: cast_to_block
// Task: Return `this' if `this' is a block, 0 otherwise
//---------------------------------------------------------------------------
vtol_block_2d *vtol_block_2d::cast_to_block(void)
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
vtol_block_2d::valid_inferior_type(const vtol_topology_object_2d &inferior) const
{
  return inferior.cast_to_two_chain()!=0;
}

//---------------------------------------------------------------------------
// Name: valid_superior_type
// Task: Is `superior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_block_2d::valid_superior_type(const vtol_topology_object_2d &superior)
const
{
  return false;
}

//:
// outside boundary vertices 

vertex_list_2d *vtol_block_2d::outside_boundary_vertices(void)
{
  vertex_list_2d *result;
  vcl_vector<vtol_vertex_2d *> *ptr_list;
  vcl_vector<vtol_vertex_2d *>::iterator i;

  result=new vertex_list_2d();
  ptr_list=outside_boundary_compute_vertices();
  // copy the lists
  
  for(i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;
  
  return result;
}

vcl_vector<vtol_vertex_2d *> *vtol_block_2d::outside_boundary_compute_vertices(void)
{
  OUTSIDE_BOUNDARY_2d(vtol_vertex_2d, vtol_two_chain_2d, compute_vertices);
}

//:
// get vertex list 

vcl_vector<vtol_vertex_2d *> *vtol_block_2d::compute_vertices(void)
{
  SEL_INF_2d(vtol_vertex_2d,compute_vertices);
}

//:
// get outside boundary zero chains

zero_chain_list_2d *vtol_block_2d::outside_boundary_zero_chains(void)
{
 zero_chain_list_2d *result;
 vcl_vector<vtol_zero_chain_2d *> *ptr_list;
 vcl_vector<vtol_zero_chain_2d *>::iterator i;
 result=new zero_chain_list_2d();
 ptr_list=outside_boundary_compute_zero_chains();
  // copy the lists
  
  for(i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;
  
  return result;
}


vcl_vector<vtol_zero_chain_2d *> *
vtol_block_2d::outside_boundary_compute_zero_chains(void)
{
  OUTSIDE_BOUNDARY_2d(vtol_zero_chain_2d,vtol_two_chain_2d,
                      compute_zero_chains);
}

//:
// get zero chains

vcl_vector<vtol_zero_chain_2d *> *vtol_block_2d::compute_zero_chains(void)
{
 SEL_INF_2d(vtol_zero_chain_2d,compute_zero_chains);
}

//:
// outside boundary edges

edge_list_2d *vtol_block_2d::outside_boundary_edges(void)
{
  edge_list_2d *result;
  vcl_vector<vtol_edge_2d *> *ptr_list;
  vcl_vector<vtol_edge_2d *>::iterator i;

  result=new edge_list_2d();
  ptr_list=outside_boundary_compute_edges();
  // copy the lists
  
  for(i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;
  
  return result;
}

//:
// outside boundary edges

vcl_vector<vtol_edge_2d *> *vtol_block_2d::outside_boundary_compute_edges(void)
{
 OUTSIDE_BOUNDARY_2d(vtol_edge_2d,vtol_two_chain_2d,compute_edges);
}

//:
// get edges 
vcl_vector<vtol_edge_2d *> *vtol_block_2d::compute_edges(void)
{
  SEL_INF_2d(vtol_edge_2d,compute_edges);
}

//:
// get outside boundary one chains

one_chain_list_2d *vtol_block_2d::outside_boundary_one_chains(void)
{ 
  one_chain_list_2d *result;
  vcl_vector<vtol_one_chain_2d *> *ptr_list;
  vcl_vector<vtol_one_chain_2d *>::iterator i;

  result=new one_chain_list_2d;
  ptr_list=outside_boundary_compute_one_chains();
  for(i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;
  return result;
}

//:
// get outside boundary one chains

vcl_vector<vtol_one_chain_2d *> *
vtol_block_2d::outside_boundary_compute_one_chains(void)
{
  OUTSIDE_BOUNDARY_2d(vtol_one_chain_2d,vtol_two_chain_2d,compute_one_chains);
}

//: 
// get the one chains
vcl_vector<vtol_one_chain_2d *> *vtol_block_2d::compute_one_chains(void)
{
  SEL_INF_2d(vtol_one_chain_2d,compute_one_chains);
}

//:
// get the outside boundary faces

face_list_2d *vtol_block_2d::outside_boundary_faces(void)
{
  face_list_2d *result;
  vcl_vector<vtol_face_2d *> *ptr_list;
  vcl_vector<vtol_face_2d*>::iterator i;

  result=new face_list_2d();
  ptr_list=outside_boundary_compute_faces();
  for(i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;
  return result;
}

//:
// get the outside boundary faces

vcl_vector<vtol_face_2d *> *vtol_block_2d::outside_boundary_compute_faces(void)
{
  OUTSIDE_BOUNDARY_2d(vtol_face_2d,vtol_two_chain_2d,compute_faces);
}

//:
// get the faces 
vcl_vector<vtol_face_2d *> *vtol_block_2d::compute_faces(void)
{
  SEL_INF_2d(vtol_face_2d,compute_faces);
}

//:
// get the outside boundary two chains

two_chain_list_2d *vtol_block_2d::outside_boundary_two_chains(void)
{
  two_chain_list_2d *result;
  vcl_vector<vtol_two_chain_2d *> *ptr_list;
  vcl_vector<vtol_two_chain_2d *>::iterator i;

  result=new two_chain_list_2d();
  ptr_list=outside_boundary_compute_two_chains();
  for(i=ptr_list->begin();i!=ptr_list->end();++i)
    result->push_back(*i);
  delete ptr_list;
  return result;
}

//:
// get the outside boundary two chains

vcl_vector<vtol_two_chain_2d *> *
vtol_block_2d::outside_boundary_compute_two_chains(void)
{
  OUTSIDE_BOUNDARY_2d(vtol_two_chain_2d,vtol_two_chain_2d,compute_two_chains);
}

//:
// get the two chains

vcl_vector<vtol_two_chain_2d *> *vtol_block_2d::compute_two_chains(void)
{
  SEL_INF_2d(vtol_two_chain_2d,compute_two_chains);
}

//:
// get blocks
vcl_vector<vtol_block_2d *> *vtol_block_2d::compute_blocks(void)
{
  LIST_SELF_2d(vtol_block_2d);
}


/*
 ******************************************************
 *
 *    Operators Functions
 */

//:
// This is hardly an equality test...but we`ll leave it for now....pav
// June 1992.

bool vtol_block_2d::operator==(const vtol_block_2d &other) const
{
  vtol_two_chain_2d *twoch1;
  vtol_two_chain_2d *twoch2;
  topology_list_2d::const_iterator bi1;
  topology_list_2d::const_iterator bi2;
  
  if(this==&other)
    return true;
  
  if(numinf()!=other.numinf())
    return false;
  else
    {
      other._inferiors.begin();
      for(bi1=_inferiors.begin(),bi2=other._inferiors.begin();
          bi1!=_inferiors.end();
          ++bi1,++bi2)
        {
          twoch1=(*bi1)->cast_to_two_chain();
          
	  twoch2=(*bi2)->cast_to_two_chain();
	  if(!(*twoch1==*twoch2))
	    return false;
	}
    }
  return true;
}

//:
// get a hole cycle
two_chain_list_2d *vtol_block_2d::hole_cycles(void) const
{
  two_chain_list_2d *result;
  topology_list_2d::const_iterator ti;
  two_chain_list_2d *templist;
  two_chain_list_2d::iterator ii;

  result=new vcl_vector<vtol_two_chain_2d_ref>();

  for(ti=_inferiors.begin();ti!=_inferiors.end();++ti)
    {
      templist=(*ti)->cast_to_two_chain()->inferior_two_chains();
      // new_list->insert_after(*templist);
      // new_list->insert(new_list->end(),templist->begin(),templist->end());
      for(ii=templist->begin();ii!=templist->end();++ii)
        result->push_back(*ii);
      delete templist;
    }
  return result;
}


//:
//add a hole cycle

bool vtol_block_2d::add_hole_cycle(vtol_two_chain_2d &new_hole) 
{
  vtol_two_chain_2d_ref twoch=get_boundary_cycle();
  if (! twoch.ptr()) return false;
  twoch->link_chain_inferior(new_hole);
  return true;
}

/*
 ******************************************************
 *
 *    Print Functions
 */
  

//:
// print data 
 
void vtol_block_2d::print(ostream &strm) const
{
  strm<<"<vtol_block_2d "<<_inferiors.size()<<"  "<<(void *)this<<">"<<endl;
}

void vtol_block_2d::describe(ostream &strm,
                             int blanking) const
{
  for (int i=0; i<blanking; ++i) strm << ' ';
  print(strm);
  describe_inferiors(strm,blanking);
  describe_superiors(strm,blanking);
}
