#ifndef VTOL_TOPOLOGY_CACHE_3D_H
#define VTOL_TOPOLOGY_CACHE_3D_H

//-----------------------------------------------------------------------------
//
// .NAME        vtol_topology_cache_3d - Cache inferiors of TopologyObject
// .LIBRARY     vtol
// .HEADER vxl package
// .INCLUDE     vtol/vtol_topology_cache_3d.h
// .FILE        vtol_topology_cache_3d.cxx
// .SECTION Description
//  The vtol_topology_cache_3d is a class used to cache vertices, edges, faces, blocks, etc.
// .SECTION Author
//     William A. Hoffman
//     ported by Luis E. Galup
//
//-----------------------------------------------------------------------------

#include <vtol/vtol_topology_object_3d.h>
#include <vcl/vcl_vector.h>
#include <vbl/vbl_timestamp.h>

class vtol_geometry_object;

//:
// This class keeps a cache of various topology object lists

class vtol_topology_cache_3d : public vbl_timestamp
{
private:

  //: \brief constructor
  vtol_topology_cache_3d(vtol_topology_object_3d* to_be_cached);
  //: \brief destructor
  ~vtol_topology_cache_3d();

  //accessors
  
  //: \brief get the vertices
  void vertices(vertex_list_3d&);
  //: \brief get the zero chains
  void zero_chains(zero_chain_list_3d&);
  //: \brief get the edges
  void edges(edge_list_3d&);
  //: \brief get the one chains
  void one_chains(one_chain_list_3d&);

  // cache utilities

  //: \brief validate the cache
  void validate_cache();
  //: \brief clear the cache
  void clear_cache();
  
private:

  //members
  vtol_topology_object_3d* source_;
  vertex_list_3d* vertices_;
  zero_chain_list_3d* zerochains_;
  edge_list_3d* edges_;
  one_chain_list_3d*  onechains_;

  //friend classes
  friend class vtol_vertex_3d;
  friend class vtol_zero_chain_3d;
  friend class vtol_edge_3d;
  friend class vtol_one_chain_3d;

  friend class vtol_topology_object_3d;
};



#endif //TOPOLOGY_CACHE_3D_H
