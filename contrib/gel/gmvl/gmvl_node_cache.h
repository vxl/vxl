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
#include <vcl/vcl_utility.h>

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
  gmvl_node_ref get( const int index) const { return nodes_[index]; }

  bool cached( const gmvl_node_ref node) const;
  
  // clever accessors
  vcl_vector<gmvl_node_ref> get( const vcl_string type) const; 

  // input / output
  friend ostream &operator<<( ostream &os, const gmvl_node_cache &c);

protected:

  vcl_vector<gmvl_node_ref> nodes_;

  // node type cache
  vcl_vector<vcl_pair<vcl_string,vcl_vector<gmvl_node_ref> > > typecache_;

  void rebuild();
};

ostream &operator<<( ostream &os, const gmvl_node_cache &c);

#endif
