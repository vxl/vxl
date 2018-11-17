// This is gel/gmvl/gmvl_database.cxx
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include "gmvl_database.h"
#include <gmvl/gmvl_tag_node.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// specific node references

void gmvl_database::add_node( const gmvl_node_sptr& node)
{
  nodecache_.add( node);
}

void gmvl_database::remove_node( const gmvl_node_sptr& node)
{
  nodecache_.remove( node);
  connectioncache_.rebuild();
}

// returns the first occurrence if there are multiple tags of the same type
gmvl_node_sptr gmvl_database::find_tag( const std::string &string) const
{
  std::vector<gmvl_node_sptr> tags= nodecache_.get( "gmvl_tag_node");

  for (auto & tag : tags)
  {
    gmvl_node *ptr= tag.ptr();

    if (((gmvl_tag_node*)ptr)->get()== string)
      return tag;
  }

  return gmvl_node_sptr(nullptr);
}

// specific connection references

void gmvl_database::add_connection( const gmvl_node_sptr& node1, const gmvl_node_sptr& node2)
{
  if (!nodecache_.cached( node1))
    add_node( node1);

  if (!nodecache_.cached( node2))
    add_node( node2);

  connectioncache_.add( node1, node2);
}

void gmvl_database::add_connections( const gmvl_node_sptr& node1, std::vector<gmvl_node_sptr> nodes)
{
  for (const auto & node : nodes)
    add_connection( node1, node);
}

// clever accessors
std::vector<gmvl_node_sptr> gmvl_database::get_nodes( const std::string& type) const
{
  return nodecache_.get( type);
}


// one way connection
std::vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const gmvl_node_sptr& node) const
{
  std::vector<int> c= connectioncache_.get_connected_nodes( node);
  std::vector<gmvl_node_sptr> l;

  l.reserve(c.size());
for (int i : c)
  {
    l.push_back( nodecache_.get( i));
  }

  return l;
}

std::vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const gmvl_node_sptr& node, const std::string& type) const
{
  std::vector<gmvl_node_sptr> l= get_connected_nodes( node);
  std::vector<gmvl_node_sptr> m;

  for (auto & i : l)
  {
    if (i->type()== type)
      m.push_back( i);
  }

  return m;
}

// two way connection
std::vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const gmvl_node_sptr& node1,
                                                              const gmvl_node_sptr& node2) const
{
  std::vector<int> c= connectioncache_.get_connected_nodes( node1, node2);
  std::vector<gmvl_node_sptr> l;

  l.reserve(c.size());
for (int i : c)
  {
    l.push_back( nodecache_.get( i));
  }

  return l;
}

// three way connection
std::vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const gmvl_node_sptr& node1,
                                                              const gmvl_node_sptr& node2,
                                                              const gmvl_node_sptr& node3) const
{
  std::vector<int> c= connectioncache_.get_connected_nodes( node1, node2, node3);
  std::vector<gmvl_node_sptr> l;

  l.reserve(c.size());
for (int i : c)
  {
    l.push_back( nodecache_.get( i));
  }

  return l;
}

// n way connection
std::vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const std::vector<gmvl_node_sptr>& nodes) const
{
  std::vector<int> c= connectioncache_.get_connected_nodes( nodes);
  std::vector<gmvl_node_sptr> l;

  l.reserve(c.size());
for (int i : c)
  {
    l.push_back( nodecache_.get( i));
  }

  return l;
}

std::vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const std::vector<gmvl_node_sptr>& nodes, const std::string& type) const
{
  std::vector<int> c= connectioncache_.get_connected_nodes( nodes);
  std::vector<gmvl_node_sptr> l;

  l.reserve(c.size());
for (int i : c)
  {
    l.push_back( nodecache_.get( i));
  }

  std::vector<gmvl_node_sptr> m;

  for (auto & i : l)
  {
    if (i->type()== type)
      m.push_back( i);
  }

  return m;
}


// input / output

std::ostream &operator<<( std::ostream &os, const gmvl_database &db)
{
  return
  os << "gmvl_database:\n  nodes:\n    " << db.nodecache_
     << "\n  connections:\n    " << db.connectioncache_ << '\n';
}
