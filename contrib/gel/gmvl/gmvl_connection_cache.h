// This is gel/gmvl/gmvl_connection_cache.h
#ifndef gmvl_connection_cache_h_
#define gmvl_connection_cache_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author crossge@crd.ge.com

#include <vcl_vector.h>

#include <vbl/vbl_bit_array_2d.h>
#include <gmvl/gmvl_connection_sptr.h>

class gmvl_connection_cache
{
 public:

  // constructors / destructors
  gmvl_connection_cache();
  ~gmvl_connection_cache();

  // simple accessors
  void add( const gmvl_node_sptr node1, const gmvl_node_sptr node2);

  // clever accessors
  vcl_vector<int> get_connected_nodes( const gmvl_node_sptr node) const { return cache_[node->ref_]; }
  vcl_vector<int> get_connected_nodes( const gmvl_node_sptr node1,
                                       const gmvl_node_sptr node2) const;
  vcl_vector<int> get_connected_nodes( const gmvl_node_sptr node1,
                                       const gmvl_node_sptr node2,
                                       const gmvl_node_sptr node3) const;
  vcl_vector<int> get_connected_nodes( const vcl_vector<gmvl_node_sptr> nodes) const;

  void rebuild();

  // input output
  friend vcl_ostream &operator<<( vcl_ostream &os, const gmvl_connection_cache &c);

 protected:

  // raw connections
  vcl_vector<gmvl_connection_sptr> connections_;

  // cached connections
  vcl_vector<vcl_vector<int> > cache_;

  // and more caching...
  vbl_bit_array_2d cachebool_;
};

vcl_ostream &operator<<( vcl_ostream &os, const gmvl_connection_cache &c);

#endif // gmvl_connection_cache_h_
