/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "gmvl_node_cache.h"


// constructors and destructors
gmvl_node_cache::gmvl_node_cache()
{
}

gmvl_node_cache::~gmvl_node_cache()
{
}

// trivial accessors
void gmvl_node_cache::add( const gmvl_node_ref node)
{
  node->ref_= nodes_.size();
  nodes_.push_back( node);

  // add to cache
  bool found= false;
  
  for( unsigned int j=0; j< typecache_.size() && !found; ++j)
    {
      if( typecache_[j].first== node->type_)
	{
	  typecache_[j].second.push_back( node);
	  found= true;
	}
    }
  
  if( !found)
    {
      vcl_pair<vcl_string,vcl_vector<gmvl_node_ref> > pair;
      
      pair.first= node->type_;
      pair.second.push_back( node);

      typecache_.push_back( pair);
    }

}

void gmvl_node_cache::remove( const gmvl_node_ref node)
{
  vcl_vector<gmvl_node_ref> newnodes;

  for( unsigned int i=0; i< nodes_.size(); ++i)
    {
      if( nodes_[i].ptr()!= node.ptr())
	{
	  nodes_[i]->ref_= newnodes.size();
	  newnodes.push_back( nodes_[i]);
	}
    }

  nodes_= newnodes;

  rebuild();
}

bool gmvl_node_cache::cached( const gmvl_node_ref node) const
{
  if( node->ref_== -1)
    return false;

  return true;
}

// clever accessors

vcl_vector<gmvl_node_ref> gmvl_node_cache::get( const vcl_string type) const
{
  vcl_vector<gmvl_node_ref> empty;

  for( unsigned int i=0; i< typecache_.size(); ++i)
    {
      if( typecache_[i].first== type)
	{
	  return typecache_[i].second;
	}
    }

  return empty;
}

void gmvl_node_cache::rebuild()
{
  typecache_.clear();

  for( unsigned int i=0; i< nodes_.size(); ++i)
    {
      bool found= false;

      for( unsigned int j=0; j< typecache_.size() && !found; ++j)
	{
	  if( typecache_[j].first== nodes_[i]->type_)
	    {
	      typecache_[j].second.push_back( nodes_[i]);
	      found= true;
	    }
	}

      if( !found)
	{
	  vcl_pair<vcl_string,vcl_vector<gmvl_node_ref> > pair;

	  pair.first= nodes_[i]->type_;
	  pair.second.push_back( nodes_[i]);

	  typecache_.push_back( pair);
	}
    }

}

// input and output
vcl_ostream &operator<<( vcl_ostream &os, const gmvl_node_cache &c)
{
  for( unsigned int i=0; i< c.nodes_.size(); ++i)
    os << *c.nodes_[i];

  return os;
}
