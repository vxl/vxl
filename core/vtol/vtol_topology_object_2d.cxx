
#include <vtol/vtol_topology_object_2d.h>
#include <vtol/vtol_topology_cache_2d.h>
//#include <vtol/vtol_zero_chain_2d.h>
#include <vtol/vtol_one_chain_2d.h>
//#include <vtol/vtol_edge_2d.h>
//#include <vtol/vtol_vertex_2d.h>
//#include <vtol/some_stubs.h>


//:
//  This is the destructor for topology_object_2d.
vtol_topology_object_2d::~vtol_topology_object_2d()
{
  delete inf_sup_cache;
}


//:
// constructor
vtol_topology_object_2d::vtol_topology_object_2d(int num_inferiors, int num_superiors) :
  vtol_topology_hierarchy_node_2d(num_inferiors, num_superiors)
{
  inf_sup_cache = 0;
}

//:
// method to delete object if under the right conditions

void vtol_topology_object_2d::protected_destroy()
{
  vtol_one_chain_2d* onechain = this->cast_to_one_chain_2d();
  if(onechain && onechain->is_sub_chain())
    {
      return;
    }


  if (( (this->is_referenced())) // if referenced
      || (this->numsup() > 0))  // or has superiors
    {
      return;
    }
  vtol_topology_object_2d::destroy(this);
}


//:
// Recursively traverse the inferiors of objects to
// delete the entire hierarchy of a topological entity
// if 1) the object is not protected from outside
//    2) the object is the highest node...ie has no superiors.
//    3) the object is not on a SpatialGroup...ie a SpatialGroup is the superior.


void vtol_topology_object_2d::destroy(vtol_topology_object_2d* topobj)
{
  // Recursively traverse the inferiors of objects to
  // delete the entire hierarchy of a topological entity
  // if 1) the object is not protected from outside
  //    2) the object is the highest node...ie has no superiors.
  //    3) the object is not on a SpatialGroup...ie a SpatialGroup is the superior.

  vtol_one_chain_2d* onechain = topobj->cast_to_one_chain_2d();

  if(onechain && onechain->is_sub_chain())
    return;

  if (( !(topobj->is_referenced())) // not referenced
      && (topobj->numsup() == 0))  // no superiors
    {
      
      while(topobj->get_inferiors()->size())
        {
          vtol_topology_object_2d *infptr;
          infptr =  *(topobj->get_inferiors()->begin());
        
          topobj->vtol_topology_hierarchy_node_2d::unlink_inferior(topobj, infptr);

          vtol_topology_object_2d::destroy(infptr);
        }

      if(onechain && onechain->contains_sub_chains())
        {
          one_chain_list_2d* inflist = onechain->inferior_one_chains();

	    for (one_chain_list_2d::iterator i = inflist->begin(); 
	       i != inflist->end(); ++i)
            {
              vtol_one_chain_2d* subchain = *i;
              onechain->remove_inferior_one_chain(subchain);
            }
	    for (one_chain_list_2d::iterator i = inflist->begin(); 
		 i != inflist->end(); ++i)
	      vtol_topology_object_2d::destroy((vtol_topology_object_2d*)(*i));
          delete inflist;
        }
 
      // Finally delete the object itself.
      delete topobj;
    }
 }

//---------------------------------------------------------------------------
// Name: get_spatial_type
// Name: Return the spatial type
//---------------------------------------------------------------------------
vtol_topology_object_2d::vsol_spatial_object_2d_type
vtol_topology_object_2d::spatial_type(void) const
{
  return TOPOLOGYOBJECT;
}

//---------------------------------------------------------------------------
// Name: topology_type
// Task: Return the topology type
//---------------------------------------------------------------------------
vtol_topology_object_2d::vtol_topology_object_2d_type
vtol_topology_object_2d::topology_type(void) const
{
  return TOPOLOGY_NO_TYPE;
}

//:
// Get list of vertices 

vertex_list_2d* vtol_topology_object_2d::vertices()
{
  cerr << "Barfing Merrily, vtol_topology_object_2d::vertices()" << endl;
  return new vertex_list_2d;
}


//:
// set list of vertices 

void vtol_topology_object_2d::vertices(vertex_list_2d& verts)
{
  if(!inf_sup_cache)
    inf_sup_cache = new vtol_topology_cache_2d(this);
  inf_sup_cache->vertices(verts); 
}

//:
// Get list of zero_chains 
zero_chain_list_2d* vtol_topology_object_2d::zero_chains()
{
  cerr << "Barfing Merrily, vtol_topology_object_2d::vertices()" << endl;
  return new zero_chain_list_2d;
}


//:
// set list of zero chains
void vtol_topology_object_2d::zero_chains( zero_chain_list_2d& zerochains)
{
  if(!inf_sup_cache)
    inf_sup_cache = new vtol_topology_cache_2d(this);
  inf_sup_cache->zero_chains(zerochains);
}

//:
// get list of one chains

one_chain_list_2d* vtol_topology_object_2d::one_chains()
{
  cerr << "Barfing Merrily, vtol_topology_object_2d::vertices()" << endl;
  return new one_chain_list_2d;
}

//:
// set list of one chains

void vtol_topology_object_2d::one_chains( one_chain_list_2d& onechains)
{
  if(!inf_sup_cache)
    inf_sup_cache = new vtol_topology_cache_2d(this);
  inf_sup_cache->one_chains(onechains);
}

//:
// get list of edges

edge_list_2d* vtol_topology_object_2d::edges()
{
  cerr << "Barfing Merrily, vtol_topology_object_2d::vertices()" << endl;
  return new edge_list_2d;
}

//:
// set list of edges

void vtol_topology_object_2d::edges(edge_list_2d& edges)
{
  if(!inf_sup_cache)
    inf_sup_cache = new vtol_topology_cache_2d(this);
  inf_sup_cache->edges(edges);
}

//:
// print the object
void vtol_topology_object_2d::print (ostream& strm =cout)
{
  strm << "<vtol_topology_object_2d " << (void *)this << ">" << endl;
}
