/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gmvl_database"
#endif
#include "gmvl_database.h"



// specific node references

void gmvl_database::add_node( const gmvl_node_ref node)
{
  nodecache_.add( node);
}

void gmvl_database::remove_node( const gmvl_node_ref node)
{
  nodecache_.remove( node);
  connectioncache_.rebuild();
}

// specific connection references

void gmvl_database::add_connection( const gmvl_node_ref node1, const gmvl_node_ref node2)
{
  if( !nodecache_.cached( node1))
    add_node( node1);

  if( !nodecache_.cached( node2))
    add_node( node2);

  connectioncache_.add( node1, node2);
  //  connectioncache_.rebuild();
}

void gmvl_database::add_connections( const gmvl_node_ref node1, vcl_vector<gmvl_node_ref> nodes)
{
  for( int i=0; i< nodes.size(); i++)
    add_connection( node1, nodes[i]);
}

// clever accessors
vcl_vector<gmvl_node_ref> gmvl_database::get_nodes( const vcl_string type) const
{
  return nodecache_.get( type);
}


// one way connection
vcl_vector<gmvl_node_ref> gmvl_database::get_connected_nodes( const gmvl_node_ref node) const
{
  vcl_vector<int> c= connectioncache_.get_connected_nodes( node);
  vcl_vector<gmvl_node_ref> l;

  for( int i=0; i< c.size(); i++)
    {
      l.push_back( nodecache_.get( c[i]));
    }
  
  return l;
}

// two way connection
vcl_vector<gmvl_node_ref> gmvl_database::get_connected_nodes( const gmvl_node_ref node1, 
							      const gmvl_node_ref node2) const
{
  vcl_vector<int> c1= connectioncache_.get_connected_nodes( node1);
  vcl_vector<int> c2= connectioncache_.get_connected_nodes( node2);
  vcl_vector<gmvl_node_ref> l;

  for( int i=0; i< c1.size(); i++)
    {
      for( int j=0; j< c2.size(); j++)
	{
	  if( c1[i]== c2[j])
	    {
	      l.push_back( nodecache_.get( c1[i]));
	      break;
	    }
	}
    }

  return l;
}

// three way connection
vcl_vector<gmvl_node_ref> gmvl_database::get_connected_nodes( const gmvl_node_ref node1, 
							      const gmvl_node_ref node2, 
							      const gmvl_node_ref node3) const
{
  vcl_vector<int> c1= connectioncache_.get_connected_nodes( node1);
  vcl_vector<int> c2= connectioncache_.get_connected_nodes( node2);
  vcl_vector<int> c3= connectioncache_.get_connected_nodes( node3);
  vcl_vector<gmvl_node_ref> l;

  for( int i=0; i< c1.size(); i++)
    {
      bool found= false;

      for( int j=0; ( j< c2.size()) && !found; j++)
	{
	  for( int k=0; ( k< c3.size()) && !found; k++)
	    {
	      if(( c1[i]== c2[j]) && ( c2[j]== c3[k]))
		{
		  l.push_back( nodecache_.get( c1[i]));
		  found= true;
		}
	    }
	}  
    }

  return l;
}


// input / output

ostream &operator<<( ostream &os, const gmvl_database db)
{
  os << "gmvl_database:" << endl;

  os << "  nodes:" << endl;

  os << "    " << db.nodecache_ << endl;

  os << "  connections:" << endl;

  os << "    " << db.connectioncache_ << endl;

  return os;
}
