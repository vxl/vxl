/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gmvl_database"
#endif
#include "gmvl_database.h"


// vcl_vector<gmvl_node_ref> gmvl_database::get_nodes( const vcl_string name) const
// {
// }


void gmvl_database::add_node( const gmvl_node_ref node)
{
  nodecache_.add( node);
}

void gmvl_database::remove_node( const gmvl_node_ref node)
{
  nodecache_.remove( node);
}

void gmvl_database::add_connection( const gmvl_node_ref node1, const gmvl_node_ref node2)
{
  if( !nodecache_.cached( node1))
    add_node( node1);

  if( !nodecache_.cached( node2))
    add_node( node2);

  connectioncache_.add( node1, node2);
}

ostream &operator<<( ostream &os, const gmvl_database db)
{
  os << "gmvl_database:" << endl;

  os << "  nodes:" << endl;

  os << "    " << db.nodecache_ << endl;

  os << "  connections:" << endl;

  os << "    " << db.connectioncache_ << endl;

  return os;
}
