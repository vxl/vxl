// This is gel/gmvl/gmvl_connection_cache.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gmvl_connection_cache.h"

#include <vcl_cassert.h>
#include <vnl/vnl_math.h>

// constructors / destructors

gmvl_connection_cache::gmvl_connection_cache()
{
}

gmvl_connection_cache::~gmvl_connection_cache()
{
}

// simple accessors

void gmvl_connection_cache::add(const gmvl_node_sptr node1, const gmvl_node_sptr node2)
{
  if (node1.ptr()!= node2.ptr())
  {
    gmvl_connection_sptr c= new gmvl_connection(node1, node2);

    connections_.push_back(c);

    // and add to the cache
    assert (node1->ref_>= 0);
    assert (node2->ref_>= 0);

    if (node1->ref_>= int(cache_.size())) cache_.resize(node1->ref_+1);
    if (node2->ref_>= int(cache_.size())) cache_.resize(node2->ref_+1);

    unsigned int biggest = vnl_math_max(node1->ref_, node2->ref_);

    if (biggest>= cachebool_.rows())
    {
      cachebool_.enlarge((biggest+1)*2, (biggest+1)*2);
    }

    cache_[node1->ref_].push_back(node2->ref_);
    cache_[node2->ref_].push_back(node1->ref_);

    cachebool_.put(node1->ref_, node2->ref_, true);
    cachebool_.put(node2->ref_, node1->ref_, true);
  }
}


// clever accessors

vcl_vector<int> gmvl_connection_cache::get_connected_nodes(const gmvl_node_sptr node1,
                                                           const gmvl_node_sptr node2) const
{
  vcl_vector<int> c= get_connected_nodes(node1);
  vcl_vector<int> d;

  for (unsigned int i=0; i< c.size(); ++i)
    if (cachebool_(node2->ref_,c[i]))
      d.push_back(c[i]);

  return d;
}

vcl_vector<int> gmvl_connection_cache::get_connected_nodes(const gmvl_node_sptr node1,
                                                           const gmvl_node_sptr node2,
                                                           const gmvl_node_sptr node3) const
{
  vcl_vector<int> c= get_connected_nodes(node1);
  vcl_vector<int> d;

  for (unsigned int i=0; i< c.size(); ++i)
    if (cachebool_(node2->ref_,c[i]) &&
        cachebool_(node3->ref_,c[i]))
      d.push_back(c[i]);

  return d;
}

vcl_vector<int> gmvl_connection_cache::get_connected_nodes(const vcl_vector<gmvl_node_sptr> nodes) const
{
  vcl_vector<int> c= get_connected_nodes(nodes[0]);
  vcl_vector<int> d;

  for (unsigned int i=0; i< c.size(); ++i)
  {
    bool ok= true;

    for (unsigned int j=1; j< nodes.size() && ok; ++j)
    {
      if (!cachebool_(nodes[j]->ref_,c[i]))
        ok= false;
    }

    if (ok)
      d.push_back(c[i]);
  }

  return d;
}

// house-keeping

void gmvl_connection_cache::rebuild()
{
  cache_.clear();
  assert(false);

  for (unsigned int i=0; i< connections_.size(); ++i)
  {
    gmvl_node_sptr node1= connections_[i]->get_node1();
    gmvl_node_sptr node2= connections_[i]->get_node2();

    assert (node1->ref_>= 0);
    assert (node2->ref_>= 0);

    if (node1->ref_>= int(cache_.size())) cache_.resize(node1->ref_+1);
    if (node2->ref_>= int(cache_.size())) cache_.resize(node2->ref_+1);

    unsigned int biggest= vnl_math_max(node1->ref_, node2->ref_);

    if (biggest>= cachebool_.rows())
    {
      vbl_bit_array_2d temp(biggest+1, biggest+1, false);

      for (unsigned int ci=0; ci< cachebool_.rows(); ++ci)
        for (unsigned int cj=0; cj< cachebool_.cols(); ++cj)
          temp.put(ci,cj, cachebool_(ci,cj));

      cachebool_= temp;
    }

    cache_[node1->ref_].push_back(node2->ref_);
    cache_[node2->ref_].push_back(node1->ref_);

    cachebool_.put(node1->ref_, node2->ref_, true);
    cachebool_.put(node2->ref_, node1->ref_, true);
  }
}

// input / output

vcl_ostream &operator<<(vcl_ostream &os, const gmvl_connection_cache &c)
{
#if 0
  for (int i=0; i< c.connections_.size(); i++)
    os << *c.connections_[i];
#endif
  for (unsigned int i=0; i< c.cache_.size(); ++i)
  {
    os << '<' << i << " - ";

    for (unsigned int j=0; j< c.cache_[i].size(); ++j)
    {
      os << c.cache_[i][j];
      if (j+1 != c.cache_[i].size())
        os << ", ";
    }
    os << '>';
  }

  return os;
}
