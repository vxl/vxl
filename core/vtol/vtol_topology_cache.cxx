
#include <vtol/vtol_topology_cache.h>



vtol_topology_cache::vtol_topology_cache(vtol_topology_object * to_be_cached)
{
  source = to_be_cached;
  vertices = 0;
  zerochains = 0;
  edges  = 0;
  onechains = 0;
  faces = 0;
  twochains = 0;
  blocks = 0;
}


vtol_topology_cache::~vtol_topology_cache()
{
  this->ClearCache();
}


void vtol_topology_cache::ClearCache()
{
  delete vertices; vertices = 0;
  delete zerochains; zerochains = 0;
  delete edges; edges = 0;
  delete onechains; onechains = 0;
  delete faces; faces = 0;
  delete twochains; twochains = 0;
  delete blocks; blocks = 0;
}

// -- If cache is out of date as compared to its source object,
//    then clear the cache.

void vtol_topology_cache::ValidateCache()
{

  //timestamp here is a static number maintained
  //by our process, owned by vbl_timestamp,
  //and incremented everytime you get_unique_timestamp

  // if this cache is younger than the source
  // then the cache does not need to be cleared
  if( timestamp_ >= source->get_time_stamp() )
    return;
  // this is out of date compared to the source
  // so clear the cache
  this->ClearCache();

  //now set the time stamp
  touch();
}

void vtol_topology_cache::Vertices(vcl_vector<vtol_vertex*>& verts)
{
  this->ValidateCache();
  if(!vertices)
    vertices = source->Vertices();
  verts = *vertices;
}

void vtol_topology_cache::ZeroChains(vcl_vector<vtol_zero_chain*>& zchains)
{  
  this->ValidateCache();
  if(!zerochains)
    zerochains = source->ZeroChains();
  zchains = *zerochains;
}

void vtol_topology_cache::Edges(vcl_vector<vtol_edge*>& oedges)
{
  this->ValidateCache();
  if(!edges)
    edges = source->Edges();
  oedges = *edges;
}

void vtol_topology_cache::OneChains(vcl_vector<vtol_one_chain*>& ochains)
{  
  this->ValidateCache();
  if(!onechains)
    onechains = source->OneChains();
  ochains = *onechains;
}

void vtol_topology_cache::Faces(vcl_vector<vtol_face*>& ofaces)
{
  this->ValidateCache();
  if(!faces)
    faces = source->Faces();
  ofaces = *faces;
}

void vtol_topology_cache::TwoChains(vcl_vector<vtol_two_chain*>& tcs)
{ 
  this->ValidateCache();
  if(!twochains)
    twochains = source->TwoChains();
  tcs = *twochains;
}

void vtol_topology_cache::Blocks(vcl_vector<vtol_block*>& oblocks)
{
  this->ValidateCache();
  if(!twochains)
    blocks = source->Blocks();
  oblocks = *blocks;
}


