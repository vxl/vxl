#ifndef gmvl_node_cache_h_
#define gmvl_node_cache_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_utility.h>

#include <gmvl/gmvl_node_sptr.h>

class gmvl_node_cache
{
public:

  // constructors and destructors
  gmvl_node_cache();
  ~gmvl_node_cache();

  // normal accessors
  void add( const gmvl_node_sptr node);
  void remove( const gmvl_node_sptr node);
  gmvl_node_sptr get( const int index) const { return nodes_[index]; }

  bool cached( const gmvl_node_sptr node) const;

  // clever accessors
  vcl_vector<gmvl_node_sptr> get( const vcl_string type) const;

  // input / output
  friend vcl_ostream &operator<<( vcl_ostream &os, const gmvl_node_cache &c);

protected:

  vcl_vector<gmvl_node_sptr> nodes_;

  // node type cache
  vcl_vector<vcl_pair<vcl_string,vcl_vector<gmvl_node_sptr> > > typecache_;

  void rebuild();
};

vcl_ostream &operator<<( vcl_ostream &os, const gmvl_node_cache &c);

#endif
