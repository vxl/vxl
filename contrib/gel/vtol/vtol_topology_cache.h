#ifndef vtol_topology_cache_h_
#define vtol_topology_cache_h_
//:
// \file
// \brief Cache inferiors of TopologyObject
//
//  The vtol_topology_cache is a class used to cache vertices, edges, faces, blocks, etc.
//
// \author
//     William A. Hoffman
//     ported by Luis E. Galup
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <vtol/vtol_topology_object.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timestamp.h>

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

class vtol_topology_cache : public vul_timestamp
{
  //: private default constructor
  vtol_topology_cache();
  //: private constructor
  vtol_topology_cache(vtol_topology_object* to_be_cached);
  //: private destructor
  ~vtol_topology_cache() override;

  //accessors

  //: set the source
  void set_source(vtol_topology_object *to_be_cached);

  //: get the vertices
  void vertices(vertex_list&);
  //: get the zero chains
  void zero_chains(zero_chain_list&);
  //: get the edges
  void edges(edge_list&);
  //: get the one chains
  void one_chains(one_chain_list&);
  //: get the faces
  void faces(face_list&);
  //: get the two chains
  void two_chains(two_chain_list&);
  //: get the blocks
  void blocks(block_list&);

  // cache utilities

  //: validate the cache
  void validate_cache();
  //: clear the cache
  void clear_cache();

 private:

  //members
  vtol_topology_object* source_;

  std::vector<vtol_vertex*>* vertices_;
  std::vector<vtol_zero_chain*>* zerochains_;
  std::vector<vtol_edge*>* edges_;
  std::vector<vtol_one_chain*>*  onechains_;
  std::vector<vtol_face*>*  faces_;
  std::vector<vtol_two_chain*>*  twochains_;
  std::vector<vtol_block*>*  blocks_;

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

#endif // vtol_topology_cache_h_
