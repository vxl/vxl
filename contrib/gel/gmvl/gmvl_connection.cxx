/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gmvl_connection"
#endif
#include "gmvl_connection.h"



gmvl_connection::gmvl_connection( const gmvl_node_ref node1, const gmvl_node_ref node2)
  : n1_( node1), n2_( node2)
{
}

gmvl_connection::~gmvl_connection()
{
}
