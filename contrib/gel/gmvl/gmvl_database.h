// This is gel/gmvl/gmvl_database.h
#ifndef gmvl_database_h_
#define gmvl_database_h_
//:
// \file
// \author crossge@crd.ge.com
// this provides a (not necessary fast or efficient) multi-access database.
//   only dependency is on 'tag' nodes, although tag nodes are in all other
//   sense identical to other nodes.

#include <iostream>
#include <iosfwd>
#include <vector>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <gmvl/gmvl_node_cache.h>
#include <gmvl/gmvl_connection_cache.h>

class gmvl_database
{
 public:

  // methods on database nodes
  void add_node( const gmvl_node_sptr& node);
  void remove_node( const gmvl_node_sptr& node);

  // methods on connections
  void add_connection( const gmvl_node_sptr& node1, const gmvl_node_sptr& node2);
  void add_connections( const gmvl_node_sptr& node1, std::vector<gmvl_node_sptr> nodes);

  // clever accessors

  //   nodes of a particular type
  std::vector<gmvl_node_sptr> get_nodes( const std::string& type) const;

  //   nodes with a particular connection list
  std::vector<gmvl_node_sptr> get_connected_nodes( const gmvl_node_sptr& node) const;
  std::vector<gmvl_node_sptr> get_connected_nodes( const gmvl_node_sptr& node1,
                                                  const gmvl_node_sptr& node2) const;
  std::vector<gmvl_node_sptr> get_connected_nodes( const gmvl_node_sptr& node1,
                                                  const gmvl_node_sptr& node2,
                                                  const gmvl_node_sptr& node3) const;
  std::vector<gmvl_node_sptr> get_connected_nodes( const std::vector<gmvl_node_sptr>& nodes) const;

  //   nodes with a particular connection list and a particular type
  std::vector<gmvl_node_sptr> get_connected_nodes( const gmvl_node_sptr& node, const std::string& type) const;
  std::vector<gmvl_node_sptr> get_connected_nodes( const std::vector<gmvl_node_sptr>& nodes, const std::string& type) const;

  // lookup a particular tag (returns null if not found)
  gmvl_node_sptr find_tag( const std::string &string) const;

  // output
  friend std::ostream &operator<<( std::ostream &os, const gmvl_database &db);

 protected:

  gmvl_node_cache nodecache_;
  gmvl_connection_cache connectioncache_;
};

std::ostream &operator<<( std::ostream &os, const gmvl_database &db);

#endif // gmvl_database_h_
