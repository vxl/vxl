#ifndef gmvl_connection_cache_h_
#define gmvl_connection_cache_h_
#ifdef __GNUC__
#pragma interface "gmvl_connection_cache"
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl/vcl_vector.h>
#include <gmvl/gmvl_connection_ref.h>

class gmvl_connection_cache
{
public:

  // constructors / destructors
  gmvl_connection_cache();
  ~gmvl_connection_cache();

  // simple accessors
  void add( const gmvl_node_ref node1, const gmvl_node_ref node2);

  // input output
  friend ostream &operator<<( ostream &os, const gmvl_connection_cache c);

protected:

  vcl_vector<gmvl_connection_ref> connections_;

};

ostream &operator<<( ostream &os, const gmvl_connection_cache c);

#endif
