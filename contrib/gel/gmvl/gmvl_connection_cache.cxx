/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gmvl_connection_cache"
#endif
#include "gmvl_connection_cache.h"


// constructors / destructors

gmvl_connection_cache::gmvl_connection_cache()
{
}

gmvl_connection_cache::~gmvl_connection_cache()
{
}

// simple accessors

void gmvl_connection_cache::add( const gmvl_node_ref node1, const gmvl_node_ref node2)
{
  if( node1.ptr()!= node2.ptr()) 
    { 
      gmvl_connection_ref c= new gmvl_connection( node1, node2);

      connections_.push_back( c);
    }
}

// input / output

ostream &operator<<( ostream &os, const gmvl_connection_cache c)
{
  for( int i=0; i< c.connections_.size(); i++)
    os << *c.connections_[i];

  return os;
}
