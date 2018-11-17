// This is gel/gmvl/gmvl_node_cache.cxx
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include "gmvl_node_cache.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// constructors and destructors
gmvl_node_cache::gmvl_node_cache() = default;

gmvl_node_cache::~gmvl_node_cache() = default;

// trivial accessors
void gmvl_node_cache::add( const gmvl_node_sptr& node)
{
  node->ref_= nodes_.size();
  nodes_.push_back( node);

  // add to cache
  bool found= false;

  for (unsigned int j=0; j< typecache_.size() && !found; ++j)
  {
    if (typecache_[j].first== node->type_)
    {
      typecache_[j].second.push_back( node);
      found= true;
    }
  }

  if (!found)
  {
    std::pair<std::string,std::vector<gmvl_node_sptr> > pair;

    pair.first= node->type_;
    pair.second.push_back( node);

    typecache_.push_back( pair);
  }
}

void gmvl_node_cache::remove( const gmvl_node_sptr& node)
{
  std::vector<gmvl_node_sptr> newnodes;

  for (auto & i : nodes_)
  {
    if (i.ptr()!= node.ptr())
    {
      i->ref_= newnodes.size();
      newnodes.push_back( i);
    }
  }

  nodes_= newnodes;

  rebuild();
}

bool gmvl_node_cache::cached( const gmvl_node_sptr& node) const
{
  return node->ref_!= -1;
}

// clever accessors

std::vector<gmvl_node_sptr> gmvl_node_cache::get( const std::string& type) const
{
  std::vector<gmvl_node_sptr> empty;

  for (const auto & i : typecache_)
  {
    if (i.first== type)
    {
      return i.second;
    }
  }

  return empty;
}

void gmvl_node_cache::rebuild()
{
  typecache_.clear();

  for (auto & node : nodes_)
  {
    bool found= false;

    for (unsigned int j=0; j< typecache_.size() && !found; ++j)
    {
      if (typecache_[j].first== node->type_)
      {
        typecache_[j].second.push_back( node);
        found= true;
      }
    }

    if (!found)
    {
      std::pair<std::string,std::vector<gmvl_node_sptr> > pair;

      pair.first= node->type_;
      pair.second.push_back( node);

      typecache_.push_back( pair);
    }
  }
}

// input and output
std::ostream &operator<<( std::ostream &os, const gmvl_node_cache &c)
{
  for (const auto & node : c.nodes_)
    os << *node << ' ';

  return os;
}
