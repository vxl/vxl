#include <vtol/vtol_topology_object_2d.h>
#include <vtol/vtol_topology_cache_2d.h>
#include <vcl_cassert.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vtol_topology_object_2d::vtol_topology_object_2d(void)
  :_superiors(0),
   _inferiors(0)
{
  inf_sup_cache= new vtol_topology_cache_2d(this);
  touch();
}

//---------------------------------------------------------------------------
// -- Constructor with given sizes for arrays of inferiors and superiors
//---------------------------------------------------------------------------
vtol_topology_object_2d::vtol_topology_object_2d(const int num_inferiors,
                                                 const int num_superiors)
  :_superiors(num_superiors),
   _inferiors(num_inferiors)
{
  inf_sup_cache=new vtol_topology_cache_2d(this);
  touch();
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_topology_object_2d::~vtol_topology_object_2d()
{
  delete inf_sup_cache;
}

//***************************************************************************
// Replaces dynamic_cast<T>
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a vertex, 0 otherwise
//---------------------------------------------------------------------------
const vtol_vertex_2d *vtol_topology_object_2d::cast_to_vertex(void) const
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a vertex, 0 otherwise
//---------------------------------------------------------------------------
vtol_vertex_2d *vtol_topology_object_2d::cast_to_vertex(void)
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a zero_chain, 0 otherwise
//---------------------------------------------------------------------------
const vtol_zero_chain_2d *
vtol_topology_object_2d::cast_to_zero_chain(void) const
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a zero_chain, 0 otherwise
//---------------------------------------------------------------------------
vtol_zero_chain_2d *vtol_topology_object_2d::cast_to_zero_chain(void)
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is an edge, 0 otherwise
//---------------------------------------------------------------------------
const vtol_edge_2d *vtol_topology_object_2d::cast_to_edge(void) const
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is an edge, 0 otherwise
//---------------------------------------------------------------------------
vtol_edge_2d *vtol_topology_object_2d::cast_to_edge(void)
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is an one_chain, 0 otherwise
//---------------------------------------------------------------------------
const vtol_one_chain_2d *vtol_topology_object_2d::cast_to_one_chain(void) const
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is an one_chain, 0 otherwise
//---------------------------------------------------------------------------
vtol_one_chain_2d *vtol_topology_object_2d::cast_to_one_chain(void)
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a face, 0 otherwise
//---------------------------------------------------------------------------
const vtol_face_2d *vtol_topology_object_2d::cast_to_face(void) const
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a face, 0 otherwise
//---------------------------------------------------------------------------
vtol_face_2d *vtol_topology_object_2d::cast_to_face(void)
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a two_chain, 0 otherwise
//---------------------------------------------------------------------------
const vtol_two_chain_2d *
vtol_topology_object_2d::cast_to_two_chain(void) const
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a two_chain, 0 otherwise
//---------------------------------------------------------------------------
vtol_two_chain_2d *vtol_topology_object_2d::cast_to_two_chain(void)
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a block, 0 otherwise
//---------------------------------------------------------------------------
const vtol_block_2d *vtol_topology_object_2d::cast_to_block(void) const
{
  return 0;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a block, 0 otherwise
//---------------------------------------------------------------------------
vtol_block_2d *vtol_topology_object_2d::cast_to_block(void)
{
  return 0;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// -- Is `inferior' already an inferior of `this' ?
//---------------------------------------------------------------------------
bool
vtol_topology_object_2d::is_inferior(const vtol_topology_object_2d &inferior) const
{
  vcl_vector<vtol_topology_object_2d_ref>::const_iterator i;
  for (i=_inferiors.begin(); i!=_inferiors.end(); ++i)
    if (*(*i) == inferior)
      return true;
  return false;
}

//---------------------------------------------------------------------------
// -- Is `superior' already an superior of `this' ?
//---------------------------------------------------------------------------
bool
vtol_topology_object_2d::is_superior(const vtol_topology_object_2d &superior) const
{
  vcl_list<vtol_topology_object_2d_ref>::const_iterator i;
  for (i=_superiors.begin(); i!=_superiors.end(); ++i)
    if ( *(*i) == superior)
      return true;
  return false;
}

//---------------------------------------------------------------------------
// -- Number of inferiors
//---------------------------------------------------------------------------
int vtol_topology_object_2d::numinf(void) const
{
  return _inferiors.size();
}

//---------------------------------------------------------------------------
// -- Number of superiors
//---------------------------------------------------------------------------
int vtol_topology_object_2d::numsup(void) const
{
  return _superiors.size();
}

//---------------------------------------------------------------------------
// -- Return the superiors list
//---------------------------------------------------------------------------
const vcl_vector<vtol_topology_object_2d_ref> *
vtol_topology_object_2d::superiors(void) const
{
  vcl_vector<vtol_topology_object_2d_ref> *result;
  vcl_list<vtol_topology_object_2d_ref>::const_iterator i;

  result=new vcl_vector<vtol_topology_object_2d_ref>();
  result->reserve(_superiors.size());
  for(i=_superiors.begin();i!=_superiors.end();++i)
    result->push_back(*i);

  // check
  assert(_superiors.size()==result->size());

  return result;
}

//---------------------------------------------------------------------------
// -- Return the inferiors list
//---------------------------------------------------------------------------
const vcl_vector<vtol_topology_object_2d_ref> *
vtol_topology_object_2d::inferiors(void) const
{
  return &_inferiors;
}

//---------------------------------------------------------------------------
// Name: spatial_type
// Name: Return the spatial type
//---------------------------------------------------------------------------
vtol_topology_object_2d::vsol_spatial_object_2d_type
vtol_topology_object_2d::spatial_type(void) const
{
  return TOPOLOGYOBJECT;
}

//---------------------------------------------------------------------------
// -- Return the topology type
//---------------------------------------------------------------------------
vtol_topology_object_2d::vtol_topology_object_2d_type
vtol_topology_object_2d::topology_type(void) const
{
  return TOPOLOGY_NO_TYPE;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// -- Link `this' with an inferior `inferior'
// Require: valid_inferior_type(inferior) and !is_inferior(inferior)
//---------------------------------------------------------------------------
void vtol_topology_object_2d::link_inferior(vtol_topology_object_2d &inferior)
{
  // require
  assert(valid_inferior_type(inferior));
  assert(!is_inferior(inferior));

  _inferiors.push_back(&inferior);
  ref();
  inferior.link_superior(*this);
  --ref_count;
  touch();
}

//---------------------------------------------------------------------------
// -- Unlink `this' with the inferior `inferior'
// Require: valid_inferior_type(inferior) and is_inferior(inferior)
//---------------------------------------------------------------------------
void vtol_topology_object_2d::unlink_inferior(vtol_topology_object_2d &inferior)
{
  // require
  assert(valid_inferior_type(inferior));
  assert(is_inferior(inferior));
  
  vcl_vector<vtol_topology_object_2d_ref>::iterator i;
  
  for(i=_inferiors.begin();(i!=_inferiors.end())&&(*(*i)!=inferior); ++i)
    ;
  inferior.unlink_superior(*this);
  _inferiors.erase(i);
  touch();
}

//---------------------------------------------------------------------------
// -- Unlink `this' with all its inferiors
//---------------------------------------------------------------------------
void vtol_topology_object_2d::unlink_all_inferiors(void)
{
  while(_inferiors.size()>0)
    {
      (*_inferiors.begin())->unlink_superior(*this);
      _inferiors.erase(_inferiors.begin());
    }
  touch();
}

//---------------------------------------------------------------------------
// -- Unlink `this' of the network
//---------------------------------------------------------------------------
void vtol_topology_object_2d::unlink(void)
{
  while(_superiors.size()>0)
    (*_superiors.begin())->unlink_inferior(*this);
  unlink_all_inferiors();
}

//***************************************************************************
// WARNING: the 2 following methods are directly called only by the superior
// class. It is FORBIDDEN to use them directly
// If you want to link and unlink superior use sup.link_inferior(*this)
// of sup.unlink_inferior(*this) 
//***************************************************************************

//---------------------------------------------------------------------------
// -- Link `this' with a superior `superior'
// Require: valid_superior_type(superior) and !is_superior(superior)
//---------------------------------------------------------------------------
void vtol_topology_object_2d::link_superior(vtol_topology_object_2d &superior)
{
  // require
  assert(valid_superior_type(superior));
  assert(!is_superior(superior));
  vcl_list<vtol_topology_object_2d_ref>::iterator i;

  _superiors.push_back(&superior);
  i=_superiors.end();
  --i;
  (*i).unprotect();
  touch();
}

//---------------------------------------------------------------------------
// -- Unlink `this' with its superior `superior'
// Require: valid_superior_type(superior) and is_superior(superior)
//---------------------------------------------------------------------------
void vtol_topology_object_2d::unlink_superior(vtol_topology_object_2d &superior)
{
  // require
  assert(valid_superior_type(superior));
  assert(is_superior(superior));

  vcl_list<vtol_topology_object_2d_ref>::iterator i;

  for(i=_superiors.begin();(i!=_superiors.end())&&(*(*i)!=superior); ++i)
    ;

  // check
  assert(*(*i)==superior);

  _superiors.erase(i); // unlink
  touch();
}


// -- get list of vertices 

vertex_list_2d* vtol_topology_object_2d::vertices(void) 
{
  vertex_list_2d* new_list=new vertex_list_2d;
  inf_sup_cache->vertices(*new_list);
  return new_list;
  
}

// -- get list of vertices 

void vtol_topology_object_2d::vertices(vertex_list_2d& verts) 
{
  inf_sup_cache->vertices(verts); 
}

// -- get list of zero_chains 
zero_chain_list_2d* vtol_topology_object_2d::zero_chains(void) 
{
  zero_chain_list_2d* new_list=new zero_chain_list_2d;
  inf_sup_cache->zero_chains(*new_list);
  return new_list;
}


// -- get list of zero chains
void vtol_topology_object_2d::zero_chains(zero_chain_list_2d &zerochains) 
{
  inf_sup_cache->zero_chains(zerochains);
}

// -- get list of edges

edge_list_2d* vtol_topology_object_2d::edges(void) 
{
  edge_list_2d* new_list=new edge_list_2d;
  inf_sup_cache->edges(*new_list);
  return new_list;
}

// -- get list of edges

void vtol_topology_object_2d::edges(edge_list_2d &edges) 
{
  inf_sup_cache->edges(edges);
}

// -- get list of one chains

one_chain_list_2d* vtol_topology_object_2d::one_chains(void) 
{
  one_chain_list_2d* new_list=new one_chain_list_2d;
  inf_sup_cache->one_chains(*new_list);
  return new_list;

}

// -- get list of one chains

void vtol_topology_object_2d::one_chains(one_chain_list_2d &onechains) 
{
  inf_sup_cache->one_chains(onechains);
}

// -- get list of faces

face_list_2d *vtol_topology_object_2d::faces(void) 
{
  face_list_2d *new_list=new face_list_2d;
  inf_sup_cache->faces(*new_list);
  return new_list;
}

// -- get list of faces

void vtol_topology_object_2d::faces(face_list_2d &face_list)
{
  inf_sup_cache->faces(face_list);
}

// -- get list of two chains 

two_chain_list_2d *vtol_topology_object_2d::two_chains(void) 
{
  two_chain_list_2d *new_list=new two_chain_list_2d;
  inf_sup_cache->two_chains(*new_list);
  return new_list;

}

// -- get list of two chains 

void vtol_topology_object_2d::two_chains(two_chain_list_2d &new_list) 
{
  inf_sup_cache->two_chains(new_list);
}


// -- get list of blocks

block_list_2d *vtol_topology_object_2d::blocks(void) 
{
  block_list_2d *new_list=new block_list_2d;
  inf_sup_cache->blocks(*new_list);
  return new_list;
}


// -- get list of blocks

void vtol_topology_object_2d::blocks(block_list_2d &new_list) 
{
  inf_sup_cache->blocks(new_list);
}


// -- print the object
void vtol_topology_object_2d::print(ostream &strm) const
{
  strm<<"<vtol_topology_object_2d "<<(void *)this<<">"<<endl;
  strm<<"number of inferiors "<<numinf()<<endl;
  strm<<"number of superiors "<<numsup()<<endl;
}

void vtol_topology_object_2d::describe_inferiors(ostream &strm,
                                                 int blanking) const
{
  vcl_vector<vtol_topology_object_2d_ref>::const_iterator i;

  for (int n=0; n<blanking; ++n) strm << ' ';
  if(_inferiors.size()==0)
    strm<<"**INFERIORS:  Empty"<<endl;
  else
    strm<<"**INFERIORS:"<<endl;
  
  for(i=_inferiors.begin();i!=_inferiors.end();++i) {
    for (int n=0; n<blanking; ++n) strm << ' ';
    (*i)->print();
  }
}

void vtol_topology_object_2d::describe_superiors(ostream &strm,
                                                 int blanking) const
{
  vcl_list<vtol_topology_object_2d_ref>::const_iterator i;

  for (int n=0; n<blanking; ++n) strm << ' ';
  if(_superiors.size()==0)
    strm<<"**SUPERIORS:  Empty"<<endl;
  else
    strm<<"**SUPERIORS:"<<endl;
  
  for(i=_superiors.begin();i!= _superiors.end();++i) {
    for (int n=0; n<blanking; ++n) strm << ' ';
    (*i)->print();
  }
}

void vtol_topology_object_2d::describe(ostream &strm,
                                       int blanking) const
{
  describe_inferiors(strm,blanking);
  describe_superiors(strm,blanking);
}


// temperary methods used for testing


//---------------------------------------------------------------------------
// -- Compute lists of vertices 
//---------------------------------------------------------------------------
vcl_vector<vtol_vertex_2d *> *vtol_topology_object_2d::compute_vertices(void)
{       
  cout << "Compute vertices" << endl;
  return 0;
}


//---------------------------------------------------------------------------
// -- Compute lists of zero chains 
//---------------------------------------------------------------------------
vcl_vector<vtol_zero_chain_2d *> *
vtol_topology_object_2d::compute_zero_chains(void)
{       
  cout << "Compute zero_chains" << endl;
  return 0;
}

//---------------------------------------------------------------------------
// -- compute lists of edges 
//---------------------------------------------------------------------------

vcl_vector<vtol_edge_2d *> *vtol_topology_object_2d::compute_edges(void)
{       
  cout << "Compute edges" << endl;
  return 0;
}

//---------------------------------------------------------------------------
// -- compute lists of one chains 
//---------------------------------------------------------------------------
vcl_vector<vtol_one_chain_2d *> *
vtol_topology_object_2d::compute_one_chains(void)
{       
  cout << "Compute one chains" << endl;
  return 0;
}

//---------------------------------------------------------------------------
// -- compute lists of faces 
//---------------------------------------------------------------------------
vcl_vector<vtol_face_2d *> *vtol_topology_object_2d::compute_faces(void)
{       
  cout << "Compute faces" << endl;
  return 0;
}

//---------------------------------------------------------------------------
// -- compute lists of two chains 
//---------------------------------------------------------------------------
vcl_vector<vtol_two_chain_2d *> *
vtol_topology_object_2d::compute_two_chains(void)
{       
  cout << "Compute two chains" << endl;
  return 0;
}

//---------------------------------------------------------------------------
// -- compute lists of blocks 
//---------------------------------------------------------------------------
vcl_vector<vtol_block_2d *> *vtol_topology_object_2d::compute_blocks(void)
{       
  cout << "Compute blocks" << endl;
  return 0;
}

#if 0
vertex_list_2d *vtol_topology_object_2d::vertices(void)
{
  vertex_list_2d *result;
  vertex_list_2d *sublist;

  result=new vcl_vector<vtol_vertex_2d_ref>();

  if(topology_type()<VERTEX)
    {
      vcl_vector<vtol_topology_object_2d_ref>::const_iterator i;
      for(i=_inferiors.begin();i!=_inferiors.end();++i)
        {
          sublist=(*i)->vertices();
          result->insert(result->end(),sublist()->begin(),sublist()->end());
          unique(result);
        }
    }
  else if(topology_type()==VERTEX)
    result->push_back(this);
  else
    {
      vcl_vector<vtol_topology_object_2d_ref>::const_iterator i;
      for(i=_superiors.begin();i!=_superiors.end();++i)
        {
          sublist=(*i)->vertices();
          result->insert(result->end(),sublist()->begin(),sublist()->end());
          unique(result);
        }
    }
  return result;
}
#endif

//#include <vcl_rel_ops.h> // gcc 2.7
//VCL_INSTANTIATE_INLINE(bool operator!=(vtol_topology_object_2d const &, vtol_topology_object_2d const &));
