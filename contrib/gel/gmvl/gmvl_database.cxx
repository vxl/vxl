// This is gel/gmvl/gmvl_database.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gmvl_database.h"
#include <gmvl/gmvl_tag_node.h>
#include <vcl_iostream.h>

// specific node references

void gmvl_database::add_node( const gmvl_node_sptr node)
{
  nodecache_.add( node);
}

void gmvl_database::remove_node( const gmvl_node_sptr node)
{
  nodecache_.remove( node);
  connectioncache_.rebuild();
}

// returns the first occurrence if there are multiple tags of the same type
gmvl_node_sptr gmvl_database::find_tag( const vcl_string &string) const
{
  vcl_vector<gmvl_node_sptr> tags= nodecache_.get( "gmvl_tag_node");

  for (unsigned int i=0; i< tags.size(); i++)
  {
    gmvl_node *ptr= tags[i].ptr();

    if (((gmvl_tag_node*)ptr)->get()== string)
      return tags[i];
  }

  return gmvl_node_sptr(0);
}

// specific connection references

void gmvl_database::add_connection( const gmvl_node_sptr node1, const gmvl_node_sptr node2)
{
  if (!nodecache_.cached( node1))
    add_node( node1);

  if (!nodecache_.cached( node2))
    add_node( node2);

  connectioncache_.add( node1, node2);
}

void gmvl_database::add_connections( const gmvl_node_sptr node1, vcl_vector<gmvl_node_sptr> nodes)
{
  for (unsigned int i=0; i< nodes.size(); i++)
    add_connection( node1, nodes[i]);
}

// clever accessors
vcl_vector<gmvl_node_sptr> gmvl_database::get_nodes( const vcl_string type) const
{
  return nodecache_.get( type);
}


// one way connection
vcl_vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const gmvl_node_sptr node) const
{
  vcl_vector<int> c= connectioncache_.get_connected_nodes( node);
  vcl_vector<gmvl_node_sptr> l;

  for (unsigned int i=0; i< c.size(); i++)
  {
    l.push_back( nodecache_.get( c[i]));
  }

  return l;
}

vcl_vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const gmvl_node_sptr node, const vcl_string type) const
{
  vcl_vector<gmvl_node_sptr> l= get_connected_nodes( node);
  vcl_vector<gmvl_node_sptr> m;

  for (unsigned int i=0; i< l.size(); i++)
  {
    if (l[i]->type()== type)
      m.push_back( l[i]);
  }

  return m;
}

// two way connection
vcl_vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const gmvl_node_sptr node1,
                                                              const gmvl_node_sptr node2) const
{
  vcl_vector<int> c= connectioncache_.get_connected_nodes( node1, node2);
  vcl_vector<gmvl_node_sptr> l;

  for (unsigned int i=0; i< c.size(); i++)
  {
    l.push_back( nodecache_.get( c[i]));
  }

  return l;
}

// three way connection
vcl_vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const gmvl_node_sptr node1,
                                                              const gmvl_node_sptr node2,
                                                              const gmvl_node_sptr node3) const
{
  vcl_vector<int> c= connectioncache_.get_connected_nodes( node1, node2, node3);
  vcl_vector<gmvl_node_sptr> l;

  for (unsigned int i=0; i< c.size(); i++)
  {
    l.push_back( nodecache_.get( c[i]));
  }

  return l;
}

// n way connection
vcl_vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const vcl_vector<gmvl_node_sptr> nodes) const
{
  vcl_vector<int> c= connectioncache_.get_connected_nodes( nodes);
  vcl_vector<gmvl_node_sptr> l;

  for (unsigned int i=0; i< c.size(); i++)
  {
    l.push_back( nodecache_.get( c[i]));
  }

  return l;
}

vcl_vector<gmvl_node_sptr> gmvl_database::get_connected_nodes( const vcl_vector<gmvl_node_sptr> nodes, const vcl_string type) const
{
  vcl_vector<int> c= connectioncache_.get_connected_nodes( nodes);
  vcl_vector<gmvl_node_sptr> l;

  for (unsigned int i=0; i< c.size(); i++)
  {
    l.push_back( nodecache_.get( c[i]));
  }

  vcl_vector<gmvl_node_sptr> m;

  for (unsigned int i=0; i< l.size(); i++)
  {
    if (l[i]->type()== type)
      m.push_back( l[i]);
  }

  return m;
}


// input / output

vcl_ostream &operator<<( vcl_ostream &os, const gmvl_database &db)
{
  return
  os << "gmvl_database:\n  nodes:\n    " << db.nodecache_
     << "\n  connections:\n    " << db.connectioncache_ << '\n';
}
