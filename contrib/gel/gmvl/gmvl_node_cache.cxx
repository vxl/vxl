/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gmvl_node_cache"
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
  nodes_.push_back( node);
  node->ref_= nodes_.size();
}

void gmvl_node_cache::remove( const gmvl_node_ref node)
{
  vcl_vector<gmvl_node_ref> newnodes;

  for( int i=0; i< nodes_.size(); i++)
    {
      if( nodes_[i].ptr()!= node.ptr())
	{
	  newnodes.push_back( nodes_[i]);
	  nodes_[i]->ref_= newnodes.size();
	}
    }

  nodes_= newnodes;
}

// clevel accessors

// input and output
ostream &operator<<( ostream &os, const gmvl_node_cache &c)
{
  for( int i=0; i< c.nodes_.size(); i++)
    os << *c.nodes_[i];

  return os;
}
