
#ifndef TOPOLOGY_CACHE_H
#define TOPOLOGY_CACHE_H

//-----------------------------------------------------------------------------
//
// Class : TopologyCache
//
// .SECTION Description
//  The TopologyCache is a class used to cache vertices, edges, faces, blocks, etc.
//
// .NAME        TopologyCache - Cache inferiors of TopologyObject
// .LIBRARY     Topology
// .HEADER SpatialObjects package
// .INCLUDE     Topology/TopologyCache.h
// .FILE        TopologyCache.h
// .FILE        TopologyCache.C
// .SECTION Author
//     William A. Hoffman
//     ported by Luis E. Galup
//
//-----------------------------------------------------------------------------

#include <vtol/vtol_topology_object.h>
#include <vcl/vcl_vector.h>
#include <vbl/vbl_timestamp.h>

class vtol_geometry_object;

class vtol_topology_cache : public vbl_timestamp
{
private:
  vtol_topology_cache(vtol_topology_object* to_be_cached);
  ~vtol_topology_cache();

  //accessors
  void Vertices(vcl_vector<vtol_vertex*>&);
  void ZeroChains(vcl_vector<vtol_zero_chain*>&);
  void Edges(vcl_vector<vtol_edge*>&);
  void OneChains(vcl_vector<vtol_one_chain*>&);
  void Faces(vcl_vector<vtol_face*>&);
  void TwoChains(vcl_vector<vtol_two_chain*>&);
  void Blocks(vcl_vector<vtol_block*>&);
  void ValidateCache();
  void ClearCache();
  
private:

  //members
  vtol_topology_object* source;
  vcl_vector<vtol_vertex*>* vertices;
  vcl_vector<vtol_zero_chain*>* zerochains;
  vcl_vector<vtol_edge*>* edges;
  vcl_vector<vtol_one_chain*>*  onechains;
  vcl_vector<vtol_face*>* faces;
  vcl_vector<vtol_two_chain*>* twochains;
  vcl_vector<vtol_block*>* blocks;

  //friend classes
  friend class vtol_vertex;
  friend class vtol_zero_chain;
  friend class vtol_edge;
  friend class vtol_one_chain;
  friend class vtol_face;
  friend class vtol_two_chain;
  friend class vtol_block;
  friend class vtol_topology_object;
};


















#endif //TOPOLOGY_CACHE_H
