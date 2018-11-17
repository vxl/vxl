// This is gel/gmvl/gmvl_connection_cache.h
#ifndef gmvl_connection_cache_h_
#define gmvl_connection_cache_h_
//:
// \file
// \author crossge@crd.ge.com

#include <vector>
#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vbl/vbl_bit_array_2d.h>
#include <gmvl/gmvl_connection_sptr.h>

class gmvl_connection_cache
{
 public:

  // constructors / destructors
  gmvl_connection_cache();
  ~gmvl_connection_cache();

  // simple accessors
  void add( const gmvl_node_sptr& node1, const gmvl_node_sptr& node2);

  // clever accessors
  std::vector<int> get_connected_nodes( const gmvl_node_sptr node) const { return cache_[node->ref_]; }
  std::vector<int> get_connected_nodes( const gmvl_node_sptr& node1,
                                       const gmvl_node_sptr& node2) const;
  std::vector<int> get_connected_nodes( const gmvl_node_sptr& node1,
                                       const gmvl_node_sptr& node2,
                                       const gmvl_node_sptr& node3) const;
  std::vector<int> get_connected_nodes( const std::vector<gmvl_node_sptr>& nodes) const;

  void rebuild();

  // input output
  friend std::ostream &operator<<( std::ostream &os, const gmvl_connection_cache &c);

 protected:

  // raw connections
  std::vector<gmvl_connection_sptr> connections_;

  // cached connections
  std::vector<std::vector<int> > cache_;

  // and more caching...
  vbl_bit_array_2d cachebool_;
};

std::ostream &operator<<( std::ostream &os, const gmvl_connection_cache &c);

#endif // gmvl_connection_cache_h_
