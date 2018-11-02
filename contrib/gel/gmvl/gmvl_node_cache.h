// This is gel/gmvl/gmvl_node_cache.h
#ifndef gmvl_node_cache_h_
#define gmvl_node_cache_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author crossge@crd.ge.com

#include <vector>
#include <iostream>
#include <iosfwd>
#include <utility>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  std::vector<gmvl_node_sptr> get( const std::string type) const;

  // input / output
  friend std::ostream &operator<<( std::ostream &os, const gmvl_node_cache &c);

 protected:

  std::vector<gmvl_node_sptr> nodes_;

  // node type cache
  std::vector<std::pair<std::string,std::vector<gmvl_node_sptr> > > typecache_;

  void rebuild();
};

std::ostream &operator<<( std::ostream &os, const gmvl_node_cache &c);

#endif // gmvl_node_cache_h_
