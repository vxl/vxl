
#include <vtol/vtol_topology_cache_3d.h>


// -- Set up the cache 
vtol_topology_cache_3d::vtol_topology_cache_3d(vtol_topology_object_3d * to_be_cached)
{
  source_ = to_be_cached;
  vertices_ = 0;
  zerochains_ = 0;
  edges_  = 0;
  onechains_ = 0;
}

// destructor
vtol_topology_cache_3d::~vtol_topology_cache_3d()
{
  this->clear_cache();
}

// -- reset the list pointers
void vtol_topology_cache_3d::clear_cache()
{
  delete vertices_; vertices_ = 0;
  delete zerochains_; zerochains_ = 0;
  delete edges_; edges_ = 0;
  delete onechains_; onechains_ = 0;

}

// -- If cache is out of date as compared to its source object,
//    then clear the cache.

void vtol_topology_cache_3d::validate_cache()
{

  //timestamp here is a static number maintained
  //by our process, owned by vbl_timestamp,
  //and incremented everytime you get_unique_timestamp

  // if this cache is younger than the source
  // then the cache does not need to be cleared
  if( timestamp_ >= source_->get_time_stamp() )
    return;
  // this is out of date compared to the source
  // so clear the cache
  this->clear_cache();

  //now set the time stamp
  touch();
}


// -- Get the vertex lists
void vtol_topology_cache_3d::vertices(vertex_list_3d& verts)
{
  this->validate_cache();
  if(!vertices_)
    vertices_ = source_->vertices();
  verts = *vertices_;
}

// -- Get the zero chain lists
void vtol_topology_cache_3d::zero_chains(zero_chain_list_3d& zchains)
{  
  this->validate_cache();
  if(!zerochains_)
    zerochains_ = source_->zero_chains();
  zchains = *zerochains_;
}


// -- Get the edge lists 
void vtol_topology_cache_3d::edges(edge_list_3d& oedges)
{
  this->validate_cache();
  if(!edges_)
    edges_ = source_->edges();
  oedges = *edges_;
}

// -- get the one chain lists
void vtol_topology_cache_3d::one_chains(one_chain_list_3d& ochains)
{  
  this->validate_cache();
  if(!onechains_)
    onechains_ = source_->one_chains();
  ochains = *onechains_;
}


