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
      
      // and add to the cache
      assert( node1->ref_>= 0);
      assert( node2->ref_>= 0);

      if( node1->ref_>= cache_.size()) cache_.resize( node1->ref_+1);
      if( node2->ref_>= cache_.size()) cache_.resize( node2->ref_+1);

      cache_[node1->ref_].push_back( node2->ref_);
      cache_[node2->ref_].push_back( node1->ref_);
    }
}


// clever accessors

void gmvl_connection_cache::rebuild()
{
  cache_.clear();

  for( int i=0; i< connections_.size(); i++)
    {
      gmvl_node_ref node1= connections_[i]->get_node1();
      gmvl_node_ref node2= connections_[i]->get_node2();

      assert( node1->ref_>= 0);
      assert( node2->ref_>= 0);

      if( node1->ref_>= cache_.size()) cache_.resize( node1->ref_+1);
      if( node2->ref_>= cache_.size()) cache_.resize( node2->ref_+1);

      cache_[node1->ref_].push_back( node2->ref_);
      cache_[node2->ref_].push_back( node1->ref_);
    }
}

// input / output

ostream &operator<<( ostream &os, const gmvl_connection_cache c)
{
  //  for( int i=0; i< c.connections_.size(); i++)
  //    os << *c.connections_[i];

  for( int i=0; i< c.cache_.size(); i++)
    {
      os << "<" << i << " - ";

      for( int j=0; j< c.cache_[i].size(); j++)
	{
	  os << c.cache_[i][j];
	  if( j!= (c.cache_[i].size()-1))
	    os << ", ";
	}
      
      os << ">";
    }

  return os;
}
