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

ostream &operator<<( ostream &os, const gmvl_database db)
{
  os << "gmvl_database:" << endl;

  os << "  nodes:" << endl;

  os << "    " << db.nodecache_ << endl;

  os << "  connections:" << endl;

  return os;
}
