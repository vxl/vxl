
#ifndef VTOL_TOPOLOGY_CACHE_2D_H
#define VTOL_TOPOLOGY_CACHE_2D_H

//-----------------------------------------------------------------------------
//
// .NAME        vtol_topology_cache_2d - Cache inferiors of TopologyObject
// .LIBRARY     vtol
// .HEADER vxl package
// .INCLUDE     vtol/vtol_topology_cache_2d.h
// .FILE        vtol_topology_cache_2d.cxx
// .SECTION Description
//  The vtol_topology_cache_2d is a class used to cache vertices, edges, faces, blocks, etc.
// .SECTION Author
//     William A. Hoffman
//     ported by Luis E. Galup
//
//-----------------------------------------------------------------------------
//#include <vtol/vtol_topology_cache_2d_ref.h>

#include <vtol/vtol_topology_object_2d.h>
#include <vcl/vcl_vector.h>
#include <vbl/vbl_timestamp.h>

class vtol_geometry_object;

//:
// This class keeps a cache of various topology object lists

class vtol_topology_cache_2d : public vbl_timestamp
{
private:

  //: \brief constructor
  vtol_topology_cache_2d();
  //: \brief constructor
  vtol_topology_cache_2d(vtol_topology_object_2d* to_be_cached);
  //: \brief destructor
  ~vtol_topology_cache_2d();

  //accessors
  //: set the source
  void set_source(vtol_topology_object_2d *to_be_cached);

  
  //: \brief get the vertices
  void vertices(vertex_list_2d&);
  //: \brief get the zero chains
  void zero_chains(zero_chain_list_2d&);
  //: \brief get the edges
  void edges(edge_list_2d&);
  //: \brief get the one chains
  void one_chains(one_chain_list_2d&);
  //: \brief get the faces
  void faces(face_list_2d&);
  //: \brief get the two chains
  void two_chains(two_chain_list_2d&);
  //: \brief get the blocks
  void blocks(block_list_2d&);





  // cache utilities

  //: \brief validate the cache
  void validate_cache();
  //: \brief clear the cache
  void clear_cache();
  
private:

  //members
  vtol_topology_object_2d* source_;
  
  vcl_vector<vtol_vertex_2d*>* vertices_;
  vcl_vector<vtol_zero_chain_2d*>* zerochains_;
  vcl_vector<vtol_edge_2d*>* edges_;
  vcl_vector<vtol_one_chain_2d*>*  onechains_;
  vcl_vector<vtol_face_2d*>*  faces_;
  vcl_vector<vtol_two_chain_2d*>*  twochains_;
  vcl_vector<vtol_block_2d*>*  blocks_;


  //friend classes
  friend class vtol_vertex_2d;
  friend class vtol_zero_chain_2d;
  friend class vtol_edge_2d;
  friend class vtol_one_chain_2d;
  friend class vtol_face_2d;
  friend class vtol_two_chain_2d;
  friend class vtol_block_2d;
  friend class vtol_topology_object_2d;
};



#endif //TOPOLOGY_CACHE_2D_H
