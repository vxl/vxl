
#ifndef VTOL_TOPOLOGY_CACHE_H
#define VTOL_TOPOLOGY_CACHE_H

//-----------------------------------------------------------------------------
//
// .NAME        vtol_topology_cache - Cache inferiors of TopologyObject
// .LIBRARY     vtol
// .HEADER      gel package
// .INCLUDE     vtol/vtol_topology_cache.h
// .FILE        vtol_topology_cache.cxx
// .SECTION Description
//  The vtol_topology_cache is a class used to cache vertices, edges, faces, blocks, etc.
// .SECTION Author
//     William A. Hoffman
//     ported by Luis E. Galup
//
//-----------------------------------------------------------------------------
//#include <vtol/vtol_topology_cache_ref.h>

#include <vtol/vtol_topology_object.h>
#include <vcl_vector.h>
#include <vbl/vbl_timestamp.h>

class vtol_vertex;
class vtol_edge;
class vtol_zero_chain;
class vtol_one_chain;
class vtol_face;
class vtol_two_chain;
class vtol_block;
class vtol_topology_object;

//:
// This class keeps a cache of various topology object lists

class vtol_topology_cache : public vbl_timestamp
{
private:

  //: \brief constructor
  vtol_topology_cache();
  //: \brief constructor
  vtol_topology_cache(vtol_topology_object* to_be_cached);
  //: \brief destructor
  ~vtol_topology_cache();

  //accessors
  //: set the source
  void set_source(vtol_topology_object *to_be_cached);

  
  //: \brief get the vertices
  void vertices(vertex_list&);
  //: \brief get the zero chains
  void zero_chains(zero_chain_list&);
  //: \brief get the edges
  void edges(edge_list&);
  //: \brief get the one chains
  void one_chains(one_chain_list&);
  //: \brief get the faces
  void faces(face_list&);
  //: \brief get the two chains
  void two_chains(two_chain_list&);
  //: \brief get the blocks
  void blocks(block_list&);





  // cache utilities

  //: \brief validate the cache
  void validate_cache();
  //: \brief clear the cache
  void clear_cache();
  
private:

  //members
  vtol_topology_object* source_;
  
  vcl_vector<vtol_vertex*>* vertices_;
  vcl_vector<vtol_zero_chain*>* zerochains_;
  vcl_vector<vtol_edge*>* edges_;
  vcl_vector<vtol_one_chain*>*  onechains_;
  vcl_vector<vtol_face*>*  faces_;
  vcl_vector<vtol_two_chain*>*  twochains_;
  vcl_vector<vtol_block*>*  blocks_;


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
