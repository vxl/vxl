#ifndef gmvl_node_cache_h_
#define gmvl_node_cache_h_
#ifdef __GNUC__
#pragma interface "gmvl_node_cache"
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl/vcl_vector.h>
#include <vcl/vcl_iostream.h>

#include <gmvl/gmvl_node_ref.h>

class gmvl_node_cache
{
public:

  // constructors and destructors
  gmvl_node_cache();
  ~gmvl_node_cache();
  
  // normal accessors
  void add( const gmvl_node_ref node);
  void remove( const gmvl_node_ref node);
  
  // clever accessors

  // input / output
  friend ostream &operator<<( ostream &os, const gmvl_node_cache &c);

protected:

  vcl_vector<gmvl_node_ref> nodes_;
};

ostream &operator<<( ostream &os, const gmvl_node_cache &c);

#endif
