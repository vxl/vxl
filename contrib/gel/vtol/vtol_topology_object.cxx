// This is gel/vtol/vtol_topology_object.cxx
#include "vtol_topology_object.h"
//:
// \file

#include <vtol/vtol_topology_cache.h>
#include <vtol/vtol_vertex.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  std::list<vtol_topology_object*>::const_iterator i;
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
  std::cerr << "*** Warning: superiors() is deprecated\n";
  topology_list *result=new topology_list;
  result->reserve(superiors_.size());

  std::list<vtol_topology_object*>::const_iterator i;
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

  auto i=inferior->superiors_.begin();
  while ( i!=inferior->superiors_.end() && *i!=this ) ++i;
  // check presence in "superiors_" list of inferior:
  assert(*i==this);

  inferior->superiors_.erase(i); // unlink this from superiors_ list of inferior
  inferior->touch();
  auto j=inferiors_.begin();
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

vertex_list* vtol_topology_object::vertices(void) const
{
  auto* new_list=new vertex_list;
  inf_sup_cache_->vertices(*new_list);
  return new_list;
}

//: get list of vertices

void vtol_topology_object::vertices(vertex_list& verts) const
{
  inf_sup_cache_->vertices(verts);
}

//: get list of zero_chains
zero_chain_list* vtol_topology_object::zero_chains(void) const
{
  auto* new_list=new zero_chain_list;
  inf_sup_cache_->zero_chains(*new_list);
  return new_list;
}

//: get list of zero chains
void vtol_topology_object::zero_chains(zero_chain_list &zerochains) const
{
  inf_sup_cache_->zero_chains(zerochains);
}

//: get list of edges

edge_list* vtol_topology_object::edges(void) const
{
  auto* new_list=new edge_list;
  inf_sup_cache_->edges(*new_list);
  return new_list;
}

//: get list of edges

void vtol_topology_object::edges(edge_list &edges) const
{
  inf_sup_cache_->edges(edges);
}

//: get list of one chains

one_chain_list* vtol_topology_object::one_chains(void) const
{
  auto* new_list=new one_chain_list;
  inf_sup_cache_->one_chains(*new_list);
  return new_list;
}

//: get list of one chains

void vtol_topology_object::one_chains(one_chain_list &onechains) const
{
  inf_sup_cache_->one_chains(onechains);
}

//: get list of faces

face_list *vtol_topology_object::faces(void) const
{
  auto *new_list=new face_list;
  inf_sup_cache_->faces(*new_list);
  return new_list;
}

//: get list of faces

void vtol_topology_object::faces(face_list &face_list) const
{
  inf_sup_cache_->faces(face_list);
}

//: get list of two chains

two_chain_list *vtol_topology_object::two_chains(void) const
{
  auto *new_list=new two_chain_list;
  inf_sup_cache_->two_chains(*new_list);
  return new_list;
}

//: get list of two chains

void vtol_topology_object::two_chains(two_chain_list &new_list) const
{
  inf_sup_cache_->two_chains(new_list);
}

//: get list of blocks

block_list *vtol_topology_object::blocks(void) const
{
  auto *new_list=new block_list;
  inf_sup_cache_->blocks(*new_list);
  return new_list;
}

//: get list of blocks

void vtol_topology_object::blocks(block_list &new_list) const
{
  inf_sup_cache_->blocks(new_list);
}

//: print the object
void vtol_topology_object::print(std::ostream &strm) const
{
  strm<<"<vtol_topology_object "<<(void const *)this<<">\n"
      <<"number of inferiors "<<numinf()<<std::endl
      <<"number of superiors "<<numsup()<<std::endl;
}

void vtol_topology_object::describe_inferiors(std::ostream &strm,
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

void vtol_topology_object::describe_superiors(std::ostream &strm,
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

  std::list<vtol_topology_object*>::const_iterator i;
  for (i=superiors_.begin();i!= superiors_.end();++i)
  {
    for (int n=0; n<blanking+2; ++n) strm << ' ';
    (*i)->print();
  }
}

void vtol_topology_object::describe(std::ostream &strm,
                                    int blanking) const
{
  describe_inferiors(strm,blanking);
  describe_superiors(strm,blanking);
}


// temperary methods used for testing


//---------------------------------------------------------------------------
//: Compute lists of vertices
//---------------------------------------------------------------------------
std::vector<vtol_vertex *> *vtol_topology_object::compute_vertices(void)
{
  std::cout << "Compute vertices\n";
  return nullptr;
}


//---------------------------------------------------------------------------
//: Compute lists of zero chains
//---------------------------------------------------------------------------
std::vector<vtol_zero_chain *> *
vtol_topology_object::compute_zero_chains(void)
{
  std::cout << "Compute zero_chains\n";
  return nullptr;
}

//---------------------------------------------------------------------------
//: compute lists of edges
//---------------------------------------------------------------------------

std::vector<vtol_edge *> *vtol_topology_object::compute_edges(void)
{
  std::cout << "Compute edges\n";
  return nullptr;
}

//---------------------------------------------------------------------------
//: compute lists of one chains
//---------------------------------------------------------------------------
std::vector<vtol_one_chain *> *
vtol_topology_object::compute_one_chains(void)
{
  std::cout << "Compute one chains\n";
  return nullptr;
}

//---------------------------------------------------------------------------
//: compute lists of faces
//---------------------------------------------------------------------------
std::vector<vtol_face *> *vtol_topology_object::compute_faces(void)
{
  std::cout << "Compute faces\n";
  return nullptr;
}

//---------------------------------------------------------------------------
//: compute lists of two chains
//---------------------------------------------------------------------------
std::vector<vtol_two_chain *> *
vtol_topology_object::compute_two_chains(void)
{
  std::cout << "Compute two chains\n";
  return nullptr;
}

//---------------------------------------------------------------------------
//: compute lists of blocks
//---------------------------------------------------------------------------
std::vector<vtol_block *> *vtol_topology_object::compute_blocks(void)
{
  std::cout << "Compute blocks\n";
  return nullptr;
}

//---------------------------------------------------------------------------
//: compute the bounding box from the set of vertices.
//  A generic method that applies to all topology_object(s)
//---------------------------------------------------------------------------
void vtol_topology_object::compute_bounding_box() const
{
  this->empty_bounding_box();
  vertex_list verts; this->vertices(verts);
  for (auto & vert : verts)
    this->add_to_bounding_box(vert->get_bounding_box());
}
