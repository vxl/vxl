// This is gel/vtol/vtol_topology_cache.cxx
#include "vtol_topology_cache.h"
//:
//  \file

//: Set up the cache
vtol_topology_cache::vtol_topology_cache()
{
  source_ = nullptr;
  vertices_ = nullptr;
  zerochains_ = nullptr;
  edges_  = nullptr;
  onechains_ = nullptr;
  faces_ =nullptr;
  twochains_ =nullptr;
  blocks_ =nullptr;
}

//: Set up the cache
vtol_topology_cache::vtol_topology_cache(vtol_topology_object * to_be_cached)
{
  source_ = to_be_cached;
  vertices_ = nullptr;
  zerochains_ = nullptr;
  edges_  = nullptr;
  onechains_ = nullptr;
  faces_ =nullptr;
  twochains_ =nullptr;
  blocks_ =nullptr;
}

// destructor
vtol_topology_cache::~vtol_topology_cache()
{
  this->clear_cache();
}

//: set the source topology object
void vtol_topology_cache::set_source(vtol_topology_object *to_be_cached)
{
  source_ = to_be_cached;
}

//: reset the list pointers
void vtol_topology_cache::clear_cache()
{
  delete vertices_; vertices_ = nullptr;
  delete zerochains_; zerochains_ = nullptr;
  delete edges_; edges_ = nullptr;
  delete onechains_; onechains_ = nullptr;
  delete faces_; faces_ = nullptr;
  delete twochains_; twochains_ = nullptr;
  delete blocks_; blocks_ = nullptr;
}

//: If cache is out of date as compared to its source object, then clear the cache.

void vtol_topology_cache::validate_cache()
{
  //timestamp here is a static number maintained
  //by our process, owned by vul_timestamp,
  //and incremented everytime you get_unique_timestamp

  // if this cache is younger than the source
  // then the cache does not need to be cleared
  if ( timestamp_ >= source_->get_time_stamp() )
    return;
  // this is out of date compared to the source
  // so clear the cache
  this->clear_cache();

  //now set the time stamp
  touch();
}


//: Get the vertex lists
void vtol_topology_cache::vertices(vertex_list& verts)
{
  this->validate_cache();
  if (!vertices_)
    vertices_ = source_->compute_vertices();

  // copy the lists

  verts.clear();
  for (auto & vertice : *vertices_){
    verts.push_back(vertice);
  }
}

//: Get the zero chain lists
void vtol_topology_cache::zero_chains(zero_chain_list& zchains)
{
  this->validate_cache();
  if (!zerochains_)
    zerochains_ = source_->compute_zero_chains();

    // copy the lists

  zchains.clear();
  for (auto & zerochain : *zerochains_){
    zchains.push_back(zerochain);
  }
}


//: Get the edge lists
void vtol_topology_cache::edges(edge_list& oedges)
{
  this->validate_cache();
  if (!edges_)
    edges_ = source_->compute_edges();
    // copy the lists

  oedges.clear();
  for (auto & edge : *edges_){
    oedges.push_back(edge);
  }
}

//: get the one chain lists
void vtol_topology_cache::one_chains(one_chain_list& ochains)
{
  this->validate_cache();
  if (!onechains_)
    onechains_ = source_->compute_one_chains();

  ochains.clear();
  for (auto & onechain : *onechains_){
    ochains.push_back(onechain);
  }
}

//: get the face lists
void vtol_topology_cache::faces(face_list& ofaces)
{
  this->validate_cache();
  if (!faces_)
    faces_ = source_->compute_faces();

  ofaces.clear();
  for (auto & face : *faces_){
    ofaces.push_back(face);
  }
}


//: get the two_chain lists
void vtol_topology_cache::two_chains(two_chain_list& otwo_chains)
{
  this->validate_cache();
  if (!twochains_)
    twochains_ = source_->compute_two_chains();

  otwo_chains.clear();
  for (auto & twochain : *twochains_){
    otwo_chains.push_back(twochain);
  }
}


//: get the block lists
void vtol_topology_cache::blocks(block_list& oblocks)
{
  this->validate_cache();
  if (!blocks_)
    blocks_ = source_->compute_blocks();

  oblocks.clear();
  for (auto & block : *blocks_){
    oblocks.push_back(block);
  }
}
