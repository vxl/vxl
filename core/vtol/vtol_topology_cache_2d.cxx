
#include <vtol/vtol_topology_cache_2d.h>

//:
// Set up the cache 
vtol_topology_cache_2d::vtol_topology_cache_2d()
{
  source_ = 0;
  vertices_ = 0;
  zerochains_ = 0;
  edges_  = 0;
  onechains_ = 0;
  faces_ =0;
  twochains_ =0;
  blocks_ =0;
}

//:
// Set up the cache 
vtol_topology_cache_2d::vtol_topology_cache_2d(vtol_topology_object_2d * to_be_cached)
{
  source_ = to_be_cached;
  vertices_ = 0;
  zerochains_ = 0;
  edges_  = 0;
  onechains_ = 0;
  faces_ =0;
  twochains_ =0;
  blocks_ =0;
}

//:
// destructor
vtol_topology_cache_2d::~vtol_topology_cache_2d()
{
  this->clear_cache();
}

//:
// set the source topology object
void vtol_topology_cache_2d::set_source(vtol_topology_object_2d *to_be_cached)
{
  source_ = to_be_cached;
}

//:
// reset the list pointers
void vtol_topology_cache_2d::clear_cache()
{
  delete vertices_; vertices_ = 0;
  delete zerochains_; zerochains_ = 0;
  delete edges_; edges_ = 0;
  delete onechains_; onechains_ = 0;
  delete faces_; faces_ = 0;
  delete twochains_; twochains_ = 0;
  delete blocks_; blocks_ = 0;

}

//:
// -- If cache is out of date as compared to its source object,
//    then clear the cache.

void vtol_topology_cache_2d::validate_cache()
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


//:
// Get the vertex lists
void vtol_topology_cache_2d::vertices(vertex_list_2d& verts)
{
  this->validate_cache();
  if(!vertices_)
    vertices_ = source_->compute_vertices();
  
  // copy the lists 
 
  verts.clear();
  for(vcl_vector<vtol_vertex_2d*>::iterator it = vertices_->begin();
      it != vertices_->end(); it++){
    verts.push_back(*it);
  }
    
}

//:
// Get the zero chain lists
void vtol_topology_cache_2d::zero_chains(zero_chain_list_2d& zchains)
{  
  this->validate_cache();
  if(!zerochains_)
    zerochains_ = source_->compute_zero_chains();

    // copy the lists 
 
  zchains.clear();
  for(vcl_vector<vtol_zero_chain_2d*>::iterator it = zerochains_->begin();
      it != zerochains_->end(); it++){
    zchains.push_back(*it);
  }


}


//:
// Get the edge lists 
void vtol_topology_cache_2d::edges(edge_list_2d& oedges)
{
  this->validate_cache();
  if(!edges_)
    edges_ = source_->compute_edges();
    // copy the lists 
 
  oedges.clear();
  for(vcl_vector<vtol_edge_2d*>::iterator it = edges_->begin();
      it != edges_->end(); it++){
    oedges.push_back(*it);
  }

}

//:
// get the one chain lists
void vtol_topology_cache_2d::one_chains(one_chain_list_2d& ochains)
{  
  this->validate_cache();
  if(!onechains_)
    onechains_ = source_->compute_one_chains();
  
  ochains.clear();
  for(vcl_vector<vtol_one_chain_2d*>::iterator it = onechains_->begin();
      it != onechains_->end(); it++){
    ochains.push_back(*it);
  }
  
}

//:
// get the face lists
void vtol_topology_cache_2d::faces(face_list_2d& ofaces)
{  
  this->validate_cache();
  if(!faces_)
    faces_ = source_->compute_faces();
  
  ofaces.clear();
  for(vcl_vector<vtol_face_2d*>::iterator it = faces_->begin();
      it != faces_->end(); it++){
    ofaces.push_back(*it);
  }
  
}


//:
// get the two_chain lists
void vtol_topology_cache_2d::two_chains(two_chain_list_2d& otwo_chains)
{  
  this->validate_cache();
  if(!twochains_)
    twochains_ = source_->compute_two_chains();
  
  otwo_chains.clear();
  for(vcl_vector<vtol_two_chain_2d*>::iterator it = twochains_->begin();
      it != twochains_->end(); it++){
    otwo_chains.push_back(*it);
  }
  
}



//:
// get the block lists
void vtol_topology_cache_2d::blocks(block_list_2d& oblocks)
{  
  this->validate_cache();
  if(!blocks_)
    blocks_ = source_->compute_blocks();
  
  oblocks.clear();
  for(vcl_vector<vtol_block_2d*>::iterator it = blocks_->begin();
      it != blocks_->end(); it++){
    oblocks.push_back(*it);
  }
  
}

