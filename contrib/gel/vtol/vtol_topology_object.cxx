// This is gel/vtol/vtol_topology_object.cxx
#include "vtol_topology_object.h"
//:
// \file

#include <vsol/vsol_box_2d.h>
#include <vtol/vtol_topology_cache.h>
#include <vtol/vtol_vertex.h>
#include <vcl_cassert.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vtol_topology_object::vtol_topology_object(void)
  :superiors_(0),
   inferiors_(0)
{
  inf_sup_cache_= new vtol_topology_cache(this);
  touch();
}

//---------------------------------------------------------------------------
//: Constructor with given sizes for arrays of inferiors and superiors
//---------------------------------------------------------------------------
vtol_topology_object::vtol_topology_object(const int num_inferiors,
                                           const int num_superiors)
  :superiors_(num_superiors),
   inferiors_(num_inferiors)
{
  inf_sup_cache_=new vtol_topology_cache(this);
  touch();
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_topology_object::~vtol_topology_object()
{
  delete inf_sup_cache_;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `inferior' already an inferior of `this' ?
//---------------------------------------------------------------------------
bool
vtol_topology_object::is_inferior(vtol_topology_object_sptr inferior) const
{
  topology_list::const_iterator i;
  for (i=inferiors_.begin(); i!=inferiors_.end(); ++i)
    if ((*i) == inferior)
      return true;

  return false;
}

//---------------------------------------------------------------------------
//: Is `superior' already a superior of `this' ?
//---------------------------------------------------------------------------
bool
vtol_topology_object::is_superior(vtol_topology_object* const& superior) const
{
  vcl_list<vtol_topology_object*>::const_iterator i;
  for (i=superiors_.begin(); i!=superiors_.end(); ++i)
    if (*i == superior)
      return true;

  return false;
}

#if 0
//---------------------------------------------------------------------------
//: Return the superiors list (must be deallocated after use)
//---------------------------------------------------------------------------
const topology_list * vtol_topology_object::superiors(void) const
{
  vcl_cerr << "*** Warning: superiors() is deprecated\n";
  topology_list *result=new topology_list;
  result->reserve(superiors_.size());

  vcl_list<vtol_topology_object*>::const_iterator i;
  for (i=superiors_.begin();i!=superiors_.end();++i)
    result->push_back(*i);

  return result;
}
#endif

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Link `this' with an inferior `inferior'
// Require: valid_inferior_type(inferior) and !is_inferior(inferior)
//---------------------------------------------------------------------------
void vtol_topology_object::link_inferior(vtol_topology_object_sptr inferior)
{
  // require
  assert(valid_inferior_type(inferior->cast_to_topology_object()));

  // Do nothing if already an inferior
  if ( is_inferior(inferior) )  return;

  assert(!is_inferior(inferior));
  assert(!inferior->is_superior(this));

  inferiors_.push_back(inferior);
  inferior->superiors_.push_back(this);
  inferior->touch();//The inferior's topology cache is now stale as well JLM
  touch();
}

//---------------------------------------------------------------------------
//: Unlink `this' with the inferior `inferior'
// Require: valid_inferior_type(inferior) and is_inferior(inferior)
//---------------------------------------------------------------------------
void vtol_topology_object::unlink_inferior(vtol_topology_object_sptr inferior)
{
  // require
  assert(valid_inferior_type(inferior->cast_to_topology_object()));
  assert(is_inferior(inferior));
  assert(inferior->is_superior(this));

  vcl_list<vtol_topology_object*>::iterator i=inferior->superiors_.begin();
  while ( i!=inferior->superiors_.end() && *i!=this ) ++i;
  // check presence in "superiors_" list of inferior:
  assert(*i==this);

  inferior->superiors_.erase(i); // unlink this from superiors_ list of inferior

  topology_list::iterator j=inferiors_.begin();
  while ( j!=inferiors_.end() && (*j)!=inferior) ++j;
  // check presence in "inferiors_" list:
  assert((*j)==inferior);

  inferiors()->erase(j);
  touch();
}

//---------------------------------------------------------------------------
//: Unlink `this' from all its inferiors
//---------------------------------------------------------------------------
void vtol_topology_object::unlink_all_inferiors(void)
{
  // remove superior-inferior link, running through inferiors list back-to-front
  while (inferiors_.size()>0)
    unlink_inferior(inferiors_.back());
}

//---------------------------------------------------------------------------
//: Unlink `this' of the network
//---------------------------------------------------------------------------
void vtol_topology_object::unlink(void)
{
  while (superiors_.size()>0)
    superiors_.front()->unlink_inferior(this);
  unlink_all_inferiors();
}

//: get list of vertices

vertex_list* vtol_topology_object::vertices(void)
{
  vertex_list* new_list=new vertex_list;
  inf_sup_cache_->vertices(*new_list);
  return new_list;
}

//: get list of vertices

void vtol_topology_object::vertices(vertex_list& verts)
{
  inf_sup_cache_->vertices(verts);
}

//: get list of zero_chains
zero_chain_list* vtol_topology_object::zero_chains(void)
{
  zero_chain_list* new_list=new zero_chain_list;
  inf_sup_cache_->zero_chains(*new_list);
  return new_list;
}

//: get list of zero chains
void vtol_topology_object::zero_chains(zero_chain_list &zerochains)
{
  inf_sup_cache_->zero_chains(zerochains);
}

//: get list of edges

edge_list* vtol_topology_object::edges(void)
{
  edge_list* new_list=new edge_list;
  inf_sup_cache_->edges(*new_list);
  return new_list;
}

//: get list of edges

void vtol_topology_object::edges(edge_list &edges)
{
  inf_sup_cache_->edges(edges);
}

//: get list of one chains

one_chain_list* vtol_topology_object::one_chains(void)
{
  one_chain_list* new_list=new one_chain_list;
  inf_sup_cache_->one_chains(*new_list);
  return new_list;
}

//: get list of one chains

void vtol_topology_object::one_chains(one_chain_list &onechains)
{
  inf_sup_cache_->one_chains(onechains);
}

//: get list of faces

face_list *vtol_topology_object::faces(void)
{
  face_list *new_list=new face_list;
  inf_sup_cache_->faces(*new_list);
  return new_list;
}

//: get list of faces

void vtol_topology_object::faces(face_list &face_list)
{
  inf_sup_cache_->faces(face_list);
}

//: get list of two chains

two_chain_list *vtol_topology_object::two_chains(void)
{
  two_chain_list *new_list=new two_chain_list;
  inf_sup_cache_->two_chains(*new_list);
  return new_list;
}

//: get list of two chains

void vtol_topology_object::two_chains(two_chain_list &new_list)
{
  inf_sup_cache_->two_chains(new_list);
}

//: get list of blocks

block_list *vtol_topology_object::blocks(void)
{
  block_list *new_list=new block_list;
  inf_sup_cache_->blocks(*new_list);
  return new_list;
}

//: get list of blocks

void vtol_topology_object::blocks(block_list &new_list)
{
  inf_sup_cache_->blocks(new_list);
}

//: print the object
void vtol_topology_object::print(vcl_ostream &strm) const
{
  strm<<"<vtol_topology_object "<<(void const *)this<<">\n"
      <<"number of inferiors "<<numinf()<<vcl_endl
      <<"number of superiors "<<numsup()<<vcl_endl;
}

void vtol_topology_object::describe_inferiors(vcl_ostream &strm,
                                              int blanking) const
{
  for (int n=0; n<blanking; ++n) strm << ' ';
  if (inferiors()->size()==0)
    strm<<"**INFERIORS:  Empty\n";
  else
    strm<<"**INFERIORS:\n";

  topology_list::const_iterator i;
  for (i=inferiors()->begin();i!=inferiors()->end();++i)
  {
    for (int n=0; n<blanking+2; ++n) strm << ' ';
    (*i)->print();
  }
}

void vtol_topology_object::describe_superiors(vcl_ostream &strm,
                                              int blanking) const
{
  for (int n=0; n<blanking; ++n) strm << ' ';
  if (superiors_.size()==0)
  {
    strm<<"**SUPERIORS:  Empty\n";
    return;
  }
  else
    strm<<"**SUPERIORS:\n";

  vcl_list<vtol_topology_object*>::const_iterator i;
  for (i=superiors_.begin();i!= superiors_.end();++i)
  {
    for (int n=0; n<blanking+2; ++n) strm << ' ';
    (*i)->print();
  }
}

void vtol_topology_object::describe(vcl_ostream &strm,
                                    int blanking) const
{
  describe_inferiors(strm,blanking);
  describe_superiors(strm,blanking);
}


// temperary methods used for testing


//---------------------------------------------------------------------------
//: Compute lists of vertices
//---------------------------------------------------------------------------
vcl_vector<vtol_vertex *> *vtol_topology_object::compute_vertices(void)
{
  vcl_cout << "Compute vertices\n";
  return 0;
}


//---------------------------------------------------------------------------
//: Compute lists of zero chains
//---------------------------------------------------------------------------
vcl_vector<vtol_zero_chain *> *
vtol_topology_object::compute_zero_chains(void)
{
  vcl_cout << "Compute zero_chains\n";
  return 0;
}

//---------------------------------------------------------------------------
//: compute lists of edges
//---------------------------------------------------------------------------

vcl_vector<vtol_edge *> *vtol_topology_object::compute_edges(void)
{
  vcl_cout << "Compute edges\n";
  return 0;
}

//---------------------------------------------------------------------------
//: compute lists of one chains
//---------------------------------------------------------------------------
vcl_vector<vtol_one_chain *> *
vtol_topology_object::compute_one_chains(void)
{
  vcl_cout << "Compute one chains\n";
  return 0;
}

//---------------------------------------------------------------------------
//: compute lists of faces
//---------------------------------------------------------------------------
vcl_vector<vtol_face *> *vtol_topology_object::compute_faces(void)
{
  vcl_cout << "Compute faces\n";
  return 0;
}

//---------------------------------------------------------------------------
//: compute lists of two chains
//---------------------------------------------------------------------------
vcl_vector<vtol_two_chain *> *
vtol_topology_object::compute_two_chains(void)
{
  vcl_cout << "Compute two chains\n";
  return 0;
}

//---------------------------------------------------------------------------
//: compute lists of blocks
//---------------------------------------------------------------------------
vcl_vector<vtol_block *> *vtol_topology_object::compute_blocks(void)
{
  vcl_cout << "Compute blocks\n";
  return 0;
}

//---------------------------------------------------------------------------
//: compute the bounding box from the set of vertices.
//  A generic method that applies to all topology_object(s)
//---------------------------------------------------------------------------
void vtol_topology_object::compute_bounding_box() const
{
  if (!this->bounding_box_)
  {
    vcl_cout << "In void vtol_topology_object::compute_bounding_box() -"
             << " shouldn't happen\n";
    return;
  }
  vertex_list *verts= const_cast<vtol_topology_object*>(this)->vertices();
  if (!verts->size())
  {
    vcl_cout << "In void vtol_topology_object::compute_bounding_box() -"
             << " no vertices\n";
    return;
  }
  this->bounding_box_->reset_bounds();
  for (vertex_list::iterator vit = verts->begin(); vit != verts->end(); vit++)
    this->bounding_box_->grow_minmax_bounds(*(*vit)->get_bounding_box());
  delete verts;
}
