
// 05/13/98  RIH replaced append by insert_after to avoid n^2 behavior

#include <vcl/vcl_vector.h>
#include <vcl/vcl_algorithm.h>
 
#include <vtol/vtol_list_functions_3d.h>
#include <vtol/vtol_two_chain_3d.h>
#include <vtol/vtol_face_3d.h>
#include <vtol/vtol_edge_3d.h>
#include <vtol/vtol_vertex_3d.h>
#include <vtol/vtol_block_3d.h>
#include <vtol/vtol_macros_3d.h>


vtol_two_chain_3d::vtol_two_chain_3d()
{
  _cycle_p = false;
}

vtol_two_chain_3d::vtol_two_chain_3d(int num_faces)
{
  _cycle_p = false;
}

vtol_two_chain_3d::vtol_two_chain_3d (vcl_vector<vtol_face_3d*>& faces, bool cyc) 
{

  vcl_vector<vtol_face_3d*>::iterator i;
  for (i=faces.begin(); i!=faces.end();i++)
    {
      link_inferior((*i));
      // all face normals point outward.
      _directions.push_back((signed char)1);
    }
  _cycle_p = cyc;
}



vtol_two_chain_3d::vtol_two_chain_3d (vcl_vector<vtol_face_3d*>& faces, vcl_vector<signed char>& dirs, bool cyc) 
{
  vcl_vector<signed char>::iterator di; 
  vcl_vector<vtol_face_3d*>::iterator fi;
	
  for (di= dirs.begin(),fi= faces.begin();fi!= faces.end() && di!=dirs.end();fi++, di++ )
    {
      if((*di) < 0)
        (*fi)->reverse_normal();
      link_inferior((*fi));
      _directions.push_back((signed char)1);
    }
  _cycle_p = cyc;
}

//:
// Copy Constructor
vtol_two_chain_3d::vtol_two_chain_3d (vtol_two_chain_3d const& two_chain)
{
  vtol_two_chain_3d* fl = (vtol_two_chain_3d*)(&two_chain);
  vcl_vector<vtol_edge_3d*>* edges = fl->edges();
  vcl_vector<vtol_vertex_3d*>* verts = fl->vertices();
  int vlen = verts->size();
  int elen = edges->size();

  vcl_vector<vtol_topology_object_3d*> newedges(elen);
  vcl_vector<vtol_topology_object_3d*> newverts(vlen);

  	
  int i =0;
  for( vcl_vector<vtol_vertex_3d*>::iterator vi=verts->begin();vi!= verts->end();vi++, i++)
    {
      vtol_vertex_3d* v = (*vi);
      newverts[i] = v->copy();
      v->set_id(i);
    }
  int j =0;
  vcl_vector<vtol_edge_3d*>::iterator ei;
	
  for(ei=edges->begin();ei!= edges->end();ei++, j++)
    {
      vtol_edge_3d* e = (*ei);
      newedges[j] = new vtol_edge_3d(newverts[e->get_v1()->get_id()]->cast_to_vertex_3d(),
                             newverts[e->get_v2()->get_id()]->cast_to_vertex_3d());
      e->set_id(j);
    }

  vcl_vector<signed char>& dirs = fl->_directions;
  topology_list_3d& infs = fl->_inferiors;
  
  vcl_vector<signed char>::iterator ddi;
  topology_list_3d::iterator tti;

  for(ddi=dirs.begin(),tti= infs.begin();tti!= infs.end() && ddi!= dirs.end(); ddi++, tti++)
    {
      vtol_face_3d* f = (*tti)->cast_to_face_3d();
      link_inferior(f->copy_with_arrays(newverts, newedges));
      _directions.push_back((*ddi));
    }

  set_cycle_p(fl->get_cycle_p());
  hierarchy_node_list_3d *hierarchy_infs = fl->get_hierarchy_inferiors();
  
  hierarchy_node_list_3d::iterator hhi;
  for (hhi=hierarchy_infs->begin();hhi!= hierarchy_infs->end();hhi++)
    add_hierarchy_inferior(((vtol_two_chain_3d*)(*hhi))->copy_with_arrays(newverts, newedges));
  delete edges;
  delete verts;
}


//:
// copy with arrays
vtol_two_chain_3d* vtol_two_chain_3d::copy_with_arrays(vcl_vector<vtol_topology_object_3d*>& newverts,
                                   vcl_vector<vtol_topology_object_3d*>& newedges)
{
  vcl_vector<signed char>& dirs = _directions;
  topology_list_3d& infs = _inferiors;
  vtol_two_chain_3d* newtc = new vtol_two_chain_3d(infs.size());
 
  topology_list_3d::iterator ti;
  vcl_vector<signed char>::iterator di;

  for(di=dirs.begin(),ti= infs.begin(); ti!=infs.end() &&di != dirs.end();ti++,di++)
    {
      vtol_face_3d* f = (*ti)->cast_to_face_3d();
      newtc->link_inferior(f->copy_with_arrays(newverts, newedges));
      newtc->_directions.push_back((*di));
    }

  newtc->set_cycle_p(get_cycle_p());
  hierarchy_node_list_3d *hierarchy_infs = get_hierarchy_inferiors();
 
  hierarchy_node_list_3d::iterator hi;
  for (hi=hierarchy_infs->begin();hi!= hierarchy_infs->end();hi++)
    newtc->add_hierarchy_inferior(((vtol_two_chain_3d*)(*hi))->copy_with_arrays(newverts,
                                                                                newedges));
  return newtc;
}


//:
// destructor 
vtol_two_chain_3d::~vtol_two_chain_3d()
{
  clear();
}

//---------------------------------------------------------------------------
// Name: clone
// Task: Clone `this': creation of a new object and initialization
//       See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_ref vtol_two_chain_3d::clone(void) const
{
  return new vtol_two_chain_3d(*this);
}

//:
// shallow copy with no links
vtol_topology_object_3d * vtol_two_chain_3d::shallow_copy_with_no_links( void )
{
    vtol_two_chain_3d * newvtol_two_chain_3d = new vtol_two_chain_3d();    
    newvtol_two_chain_3d->_cycle_p = _cycle_p;
    vcl_vector<signed char>::iterator di;
	
    for (di= _directions.begin();di!= _directions.end();di++ )
        newvtol_two_chain_3d->_directions.push_back((*di));
    return newvtol_two_chain_3d;
}

//---------------------------------------------------------------------------
// Name: topology_type
// Task: Return the topology type
//---------------------------------------------------------------------------
vtol_two_chain_3d::vtol_topology_object_3d_type
vtol_two_chain_3d::topology_type(void) const
{
  return TWOCHAIN;
}

/*
 ******************************************************
 *
 *   Editing Functions
 */

//:
// add the superiors from the parent

void vtol_two_chain_3d::add_superiors_from_parent(vcl_vector<vtol_topology_object_3d*>& sups)
{
  vcl_vector<vtol_topology_object_3d*>::iterator si;
  for (si=sups.begin();si != sups.end();si++)
  	{
	if(vcl_find(_superiors.begin(),_superiors.end(),(*si))==_superiors.end()){
	  _superiors.push_back((*si));
	}
   }
	
  hierarchy_node_list_3d::iterator hi;
  for (hi=_hierarchy_inferiors.begin();hi!= _hierarchy_inferiors.end();hi++)
    ((vtol_two_chain_3d*)(*hi))->add_superiors_from_parent(sups);
}


void vtol_two_chain_3d::remove_superiors_of_parent(vcl_vector<vtol_topology_object_3d*>& sups)
{
  vcl_vector<vtol_topology_object_3d*>::iterator si;
  for (si=sups.begin();si != sups.end();si++)	
      _superiors.erase((si));

  hierarchy_node_list_3d::iterator hi;
  for (hi=_hierarchy_inferiors.begin();hi!= _hierarchy_inferiors.end();hi++)	
      ((vtol_two_chain_3d*)(*hi))->remove_superiors_of_parent(sups);
}

void vtol_two_chain_3d::remove_superiors()
{
  _superiors.clear();
  hierarchy_node_list_3d::iterator hi;
  for (hi=_hierarchy_inferiors.begin();hi!= _hierarchy_inferiors.end();hi++)
    ((vtol_two_chain_3d*)(*hi))->remove_superiors();
}



void vtol_two_chain_3d::update_superior_list_p_from_hierarchy_parent()
{
  vtol_two_chain_3d *hierarchy_parent = NULL;

  // Check to see if there is a parent node in the tree.

  if (_hierarchy_superiors.size() > 0)
    hierarchy_parent = (vtol_two_chain_3d*) _hierarchy_superiors[0];


  // If vtol_two_chain_3d is a child of another vtol_two_chain_3d...the superiors
  // lists are updated.

  if (hierarchy_parent)
    {
      topology_list_3d *parent_superiors = hierarchy_parent->get_superiors();

      // Clear all previous superiors.
      _superiors.clear();
      topology_list_3d::iterator ti;
     for (ti=parent_superiors->begin();ti!= parent_superiors->end();ti++ )
        {
	   if(vcl_find(_superiors.begin(),_superiors.end(),*ti)==_superiors.end()){
	       _superiors.push_back((*ti));
	   }	
	}

      // Recursively update all children.
      vcl_vector<vtol_two_chain_3d*> * chains = inferior_two_chains();
   
      vcl_vector<vtol_two_chain_3d*>::iterator ci;
      for (ci=chains->begin(); ci!=chains->end();ci++)
        ((*ci))->update_superior_list_p_from_hierarchy_parent();

      delete chains;
    }
}

//:
// add an inferior two chain
void vtol_two_chain_3d::add_inferior_two_chain(vtol_two_chain_3d* vtol_two_chain_3d )
{
  // Add it to the hierarchy.
  add_hierarchy_inferior(vtol_two_chain_3d);

}

//:
// remove an inferior two chain
void vtol_two_chain_3d::remove_inferior_two_chain(vtol_two_chain_3d* vtol_two_chain_3d )
{
  // Remove vtol_two_chain_3d from hierarchy.
  remove_hierarchy_inferior(vtol_two_chain_3d);

}

//:
// -- This method removes the object from the topological structure
//    by unlinking it.  The removal of the object may recursively cause
//    the removal of some of the object's superiors if these superiors
//    are no longer valid.  In addition, inferiors of the object are
//    recursively checked to see if they should be removed.  They
//    are removed if all their superiors have been removed.
//
//    A record of the changes to the topological structure is returned 
//    through the parameters changes and deleted.  For details on what is
//    stored in these parameters, see Edge::Disconnect( changes, deleted ).
//    (RYF 7-16-98)
//
bool vtol_two_chain_3d::disconnect( vcl_vector< vtol_topology_object_3d * > & changes,
                           vcl_vector< vtol_topology_object_3d * > & deleted )
{
  // Unlink this object from its superiors
  topology_list_3d * tmp = get_superiors();
  hierarchy_node_list_3d * htmp = get_hierarchy_superiors();
  if( (/* tmp->size() >= 0 && */ ( htmp->size() == 0 ))
          || (( tmp->size() == 0 ) /* && htmp->size() >= 0 */) ){
	return false;
  }

  vcl_vector< vtol_block_3d * > sup;
 
  topology_list_3d::iterator ttti;
  for (ttti= tmp->begin();ttti!= tmp->end();ttti++ )
      sup.push_back( (vtol_block_3d *) (*ttti) );
 
  vcl_vector< vtol_block_3d * >::iterator bi; 
  for (bi= sup.begin();bi!= sup.end();bi++ )
      ((*bi))->remove( this, changes, deleted );

  // in case it is a hole
  vcl_vector< vtol_two_chain_3d * > hsup;
 
  hierarchy_node_list_3d::iterator hti;

  for (hti= htmp->begin();hti!= htmp->end();hti++ )
      hsup.push_back( (vtol_two_chain_3d *)(*hti) );

  vcl_vector< vtol_two_chain_3d * >::iterator hsi;
  for ( hsi=hsup.begin();hsi!= hsup.end();hsi++ )
  {
      tmp = ((*hsi))->get_superiors();
      sup.clear();
      topology_list_3d::iterator tti;

      for (tti= tmp->begin(); tti!=tmp->end();tti++ )
         sup.push_back( (vtol_block_3d *)(*tti) );

      vcl_vector< vtol_block_3d * >::iterator ssi;
      for (ssi= sup.begin();ssi!= sup.end();ssi++ )
           ((*ssi))->remove( this, changes, deleted );
  }

  unlink_all_superiors_twoway( this );  // probably unnecessary
  deep_remove( deleted );
  _directions.clear();

  return true;
}

//:
//    Removes the face from the two chain.  The removal of
//    the face may invalidate superior blocks, so the two chain
//    is recursively removed its superior blocks.
//    For more details, see Edge::Disconnect( changes, deleted )
//
bool vtol_two_chain_3d::remove( vtol_face_3d * face,
                       vcl_vector< vtol_topology_object_3d * > & changes,
                       vcl_vector< vtol_topology_object_3d * > & deleted ) 
{
  //cout << "               Entering vtol_two_chain_3d::Remove\n";


  // Remove this object from its superiors
  topology_list_3d * tmp = get_superiors();
  hierarchy_node_list_3d * htmp = get_hierarchy_superiors();
  int num_superiors_before = get_superiors()->size()
                             + get_hierarchy_superiors()->size();

  if( (/* tmp->size() >= 0 && */ ( htmp->size() == 0 ))
          || (( tmp->size() == 0 ) /* && htmp->size() >= 0 */) ){
    return false;
  }

 
  vcl_vector< vtol_block_3d * > sup;

  topology_list_3d::iterator ti;
  for (ti= tmp->begin();ti!= tmp->end(); ti++ )
      sup.push_back( (vtol_block_3d *)(*ti) );

  vcl_vector< vtol_block_3d * >::iterator si;
  for (si= sup.begin();si!= sup.end();si++ )
      ((*si))->remove( this, changes, deleted );

  // in case it is a hole
  vcl_vector< vtol_two_chain_3d * > hsup;

   hierarchy_node_list_3d::iterator hi;
  for (hi= htmp->begin();hi!= htmp->end();hi++ )
      hsup.push_back( (vtol_two_chain_3d *) (*hi) );

 vcl_vector< vtol_two_chain_3d * >::iterator tci;
  for (tci= hsup.begin(); tci!=hsup.end();tci++ )
  {
      tmp = ((*tci))->get_superiors();
      sup.clear();
  
 
      for (ti= tmp->begin();ti!= tmp->end();ti++ )
         sup.push_back( (vtol_block_3d *) (*ti) );
  

       for (si= sup.begin();si!= sup.end();si++ )
          ((*si))->remove( this, changes, deleted );
  }


  int num_superiors_after = get_superiors()->size()
                            + get_hierarchy_superiors()->size();

  if( num_superiors_after == 0 ){
    return false;
  }


  // check if the last face in chain is being removed
  if ( _inferiors.size() == 1 )
  {
      remove_face( face );
      deleted.push_back( this );
      unlink_all_inferiors_twoway( this );
      return false; 
  } 

  else
    {
      // check if the second to last face in chain is being removed
      if ( _inferiors.size() == 2 ) 
        {
          remove_face( face );
          if( _inferiors.size() == 1 )
            return false;
          
          vtol_face_3d * remaining_face = (_inferiors[0])->cast_to_face_3d();
          remove_face( remaining_face );
          if ( remaining_face->numsup() == 0 )
            changes.push_back( remaining_face );
          unlink_all_inferiors_twoway( this );
          deleted.push_back( this );
          return false; 
        }
    
      else
        {
          remove_face( face );
          // see if face's removal leaves a disconnected vtol_two_chain_3d
          vcl_vector< vtol_topology_object_3d * > components;
          if ( this->break_into_connected_components( components ) )
            {
              this->unlink_all_inferiors_twoway( this );
              deleted.push_back( this );
              
              vcl_vector< vtol_topology_object_3d * >::iterator ci;
              
              for (ci= components.begin();ci!= components.end();ci++ )
                if ((*ci)->numsup() == 0 )
                  changes.insert(changes.begin(),(*ci) );
              return false;
            }
          else // vtol_two_chain_3d is still connected
            {
              if (  num_superiors_before ) changes.push_back( this );
              set_cycle_p( false );
              return true;
            }
        }
    }
}

//:
//    For each inferior, this method unlinks the inferior
//    from this object.  If the inferior now has zero superiors,
//    the function is called recursively on it.  Finally, this
//    object is pushed onto the list removed. (RYF 7-16-98)
//
void vtol_two_chain_3d::deep_remove( vcl_vector< vtol_topology_object_3d * > & removed )
{
  //cout << "      Entering vtol_two_chain_3d::DeepDeleteInferiors\n";

  // Make a copy of the object's inferiors
  topology_list_3d * tmp = get_inferiors();
  vcl_vector< vtol_face_3d * > inferiors;
 

  
  for (topology_list_3d::iterator ti= tmp->begin();ti!= tmp->end();ti++ )
      inferiors.push_back( (vtol_face_3d *)(*ti) );
  for (vcl_vector<vtol_face_3d * >::iterator fi= inferiors.begin();fi!= inferiors.end();fi++ )
  {
      vtol_face_3d * inferior = (*fi);

      // Unlink inferior from its superior
      inferior->unlink_superior( this );  

      // Test if inferior now has 0 superiors.  If so, 
      // recursively remove its inferiors.
      if ( inferior->numsup() == 0 )  
          inferior->deep_remove( removed );
  }
  _directions.clear();
  removed.push_back( this );

  //cout << "      Exiting vtol_two_chain_3d::DeepDeleteInferiors\n";
}

//:
// If the invoking object consists of more than 1 connected components,
// this method determines the connected components 
// and creates a new topological object for each component.
// The components are returned as a linked list of two_chains
// and/or faces.  A component is of type Face if it consists
// of a single Face; it is of type vtol_two_chain_3d otherwise.  
// If the vtol_two_chain_3d consists of a single component, false is returned
// and the component list is unchanged.  If the vtol_two_chain_3d consists
// of multiple components, true is returned and the component
// list contains the components.  
// 
// Note -- A valid vtol_two_chain_3d consists of only a single connected component.
// This function is only used to fix an invalid vtol_two_chain_3d
//


bool
vtol_two_chain_3d::break_into_connected_components( vcl_vector<vtol_topology_object_3d*> & components )
{
  cerr << "vtol_two_chain_3d::break_into_connected_components() not implemented yet\n";
  return false; // TO DO
}

/* todo
bool
vtol_two_chain_3d::break_into_connected_components( vcl_vector<vtol_topology_object_3d*> & components )
{
    topology_list_3d * tmp = get_inferiors();
    int numfaces = tmp->size();
    if ( numfaces == 0 ) return false;
    vcl_vector< vtol_face_3d * > faces( numfaces );
    int i;
    for ( i = 0,topology_list_3d::iterator ti= tmp->begin(); ti!=tmp->end();ti++, i++ )
        faces[ i ] = (vtol_face_3d *) (*ti);

    // compnum[i] stores the component number of the ith face
    int * compnum = new int[ numfaces ];
    assert( compnum );

    // adjslists stores the adjacency lists representation of the face graph
    vcl_vector< int > * adjlists = new vcl_vector< int > [ numfaces ];
    assert( adjlists );

    // build the adjacency list representation
    int j;
    for ( i = 0; i < numfaces; i++ )
    {
        compnum[ i ] = -1;  // -1 signals it is not part of any component yet
        for ( j = i+1; j < numfaces; j++ )
          if ( faces[ i ]->shares_edge_with( faces[ j ] ) )
	  {
              adjlists[i].push_back( j );
              adjlists[j].push_back( i );
	  }
    }

    // use depth first search to find connected components
    int numcomps = 0;
    for ( i = 0; i < numfaces; i++ )
    {
        if ( compnum[ i ] == -1 ) // unvisited
	{
            compnum[ i ] = numcomps;
            vcl_vector<int> to_be_explored;
            to_be_explored.push( i );
            while ( to_be_explored.size() )
	    {
                int f = to_be_explored.pop();
                for ( adjlists[ f ].begin(); adjlists[ f ].end(); )
		{
                    if ( compnum[ adjlists[ f ].value() ] == -1 )
		    {
                        compnum[ adjlists[ f ].value() ] = numcomps;
                        to_be_explored.push( adjlists[ f ].value() );
		    }
		}
	    }
            numcomps++;
	}
    }

    if ( numcomps > 1 )
        for ( i = 0; i < numcomps; i++ )
        {
            vcl_vector< vtol_face_3d * > component_faces;
	    // gather faces belonging to the ith component 
            for ( j = 0; j < numfaces; j++ )
                if ( compnum[ j ] == i ) 
                    component_faces.push_back( faces[ j ] );
            // create either a two chain or a face out of the component
            if ( component_faces.size() == 1 ) 
                components.push_back( component_faces.get( 0 ) );
    	    else 
            {
                vtol_two_chain_3d * newvtol_two_chain_3d = new vtol_two_chain_3d(component_faces, false);
	        // need to check if it is a cycle??
                components.push_back( newvtol_two_chain_3d );
     	    }
            component_faces.clear(); // necessary??
         }

    delete []compnum;
    delete []adjlists;

    if ( numcomps == 1 ) return false;
    else return true;
}

*/

bool vtol_two_chain_3d::add_face(vtol_face_3d* new_face, signed char dir)
{
  _directions.push_back(dir);
  this->touch();
  return link_inferior(new_face);
}

bool vtol_two_chain_3d::remove_face(vtol_face_3d* doomed_face)
{

   topology_list_3d::iterator i = vcl_find(_inferiors.begin(),_inferiors.end(),
				      (vtol_topology_object_3d*)doomed_face);
  
	
   int index = i-_inferiors.begin();
  if(index >= 0)
    {
      vcl_vector<signed char>::iterator j = _directions.begin(); 
      j=j+index;

     _directions.erase(j);
      this->touch();
      return unlink_inferior(doomed_face);
    }
  return false;
}

bool vtol_two_chain_3d::add_part(vtol_block_3d* new_part)
{
  return link_superior(new_part);
}

bool vtol_two_chain_3d::add_block(vtol_block_3d* new_block)
{
  return link_superior(new_block);
}

bool vtol_two_chain_3d::remove_part(vtol_block_3d* doomed_part)
{
  return unlink_superior(doomed_part);
}

bool vtol_two_chain_3d::remove_block(vtol_block_3d* doomed_block)
{
  return unlink_superior(doomed_block);
}

void vtol_two_chain_3d::clear()
{
  _directions.clear();
  unlink_all_inferiors(this);
}

/*
 ******************************************************
 *
 *    Accessor Functions
 */


//: 
// outside boundary vertices

vcl_vector<vtol_vertex_3d*>* vtol_two_chain_3d::outside_boundary_vertices()
{
  SEL_INF_3d(vtol_vertex_3d,vertices);
}

//:
// list of vertices

vcl_vector<vtol_vertex_3d*>* vtol_two_chain_3d::vertices()
{
  vcl_vector<vtol_vertex_3d*> *verts;
  verts = outside_boundary_vertices();
  
  // Set current position to the end
  // verts->set_position(verts->size()-1); - not sure what is supposed to happen here

  SUBCHAIN_INF_3d(verts, vtol_two_chain_3d, vtol_vertex_3d, vertices);
}

//:
// outside boundary zero chains

vcl_vector<vtol_zero_chain_3d*>* vtol_two_chain_3d::outside_boundary_zero_chains()
{
  SEL_INF_3d(vtol_zero_chain_3d,zero_chains);
}

//:
//list of zero chains
vcl_vector<vtol_zero_chain_3d*>* vtol_two_chain_3d::zero_chains()
{
  vcl_vector<vtol_zero_chain_3d*> *zchs;
  zchs = outside_boundary_zero_chains();
  SUBCHAIN_INF_3d(zchs, vtol_two_chain_3d, vtol_zero_chain_3d, zero_chains);
}

//:
// outside boundary edges
vcl_vector<vtol_edge_3d*>* vtol_two_chain_3d::outside_boundary_edges()
{
  SEL_INF_3d(vtol_edge_3d,edges);
}


//:
// list of edges 
vcl_vector<vtol_edge_3d*>* vtol_two_chain_3d::edges()
{
  vcl_vector<vtol_edge_3d*> *edges;
  edges = outside_boundary_edges();
  SUBCHAIN_INF_3d(edges, vtol_two_chain_3d, vtol_edge_3d, edges);
}

//:
// outside one chains
vcl_vector<vtol_one_chain_3d*>* vtol_two_chain_3d::outside_boundary_one_chains()
{
  SEL_INF_3d(vtol_one_chain_3d,one_chains);
}


//:
// one chains
vcl_vector<vtol_one_chain_3d*>* vtol_two_chain_3d::one_chains()
{
  vcl_vector<vtol_one_chain_3d*> *onechs;
  onechs = outside_boundary_one_chains();
  SUBCHAIN_INF_3d(onechs, vtol_two_chain_3d, vtol_one_chain_3d, one_chains);
}

//:
// outside faces 
vcl_vector<vtol_face_3d*>* vtol_two_chain_3d::outside_boundary_faces()
{
  COPY_INF_3d(vtol_face_3d);
}

//:
// faces 
vcl_vector<vtol_face_3d*>* vtol_two_chain_3d::faces()
{
  vcl_vector<vtol_face_3d*> *faces;
  faces = outside_boundary_faces();
  SUBCHAIN_INF_3d(faces, vtol_two_chain_3d, vtol_face_3d, faces);
}

//:
// list of blocks
vcl_vector<vtol_block_3d*>* vtol_two_chain_3d::blocks()
{
  if(this->is_sub_chain())
    {
      vcl_vector<vtol_block_3d*> *new_list = new vcl_vector<vtol_block_3d*>, *sublist;
      vcl_vector<vtol_two_chain_3d*>* onech = this->superior_two_chains();

       vcl_vector<vtol_two_chain_3d*>::iterator tci;

      for(tci=onech->begin();tci!=onech->end();tci++)
        {
          sublist = (*tci)->blocks();
          if(sublist->size())
            {
              // new_list->insert(new_list->end(),sublist->begin(),sublist->end());
              for( vcl_vector<vtol_block_3d*>::iterator si = sublist->begin(); 
                   si != sublist->end();si++){
                new_list->push_back(*si);
              }

 
            }
          delete sublist;
        }
      tagged_union((vcl_vector<vsol_spatial_object_3d*> *)new_list); // from vsol_spatial_object_3dListExtras.h
      return new_list;
    }
  else
    {
      SEL_SUP_3d(vtol_block_3d, blocks);
    }

}

//:
// list of two chains 

vcl_vector<vtol_two_chain_3d*>* vtol_two_chain_3d::two_chains()
{
  vcl_vector<vtol_two_chain_3d*> *twochs;
  twochs =  outside_boundary_two_chains();

  hierarchy_node_list_3d::iterator hi;
  for (hi=_hierarchy_inferiors.begin();hi!= _hierarchy_inferiors.end();hi++ )
    twochs->push_back((vtol_two_chain_3d*)((*hi)));

  return twochs;
}


vcl_vector<vtol_two_chain_3d*>* vtol_two_chain_3d::inferior_two_chains()
{
  vcl_vector<vtol_two_chain_3d*>* two_chains = new vcl_vector<vtol_two_chain_3d*>();

	 
  hierarchy_node_list_3d::iterator hi;
  for (hi=_hierarchy_inferiors.begin();hi!= _hierarchy_inferiors.end();hi++ )
    two_chains->push_back((vtol_two_chain_3d*)(*hi));

  return two_chains;
}


vcl_vector<vtol_two_chain_3d*>* vtol_two_chain_3d::superior_two_chains()
{
  vcl_vector<vtol_two_chain_3d*>* two_chains = new vcl_vector<vtol_two_chain_3d*>();

   hierarchy_node_list_3d::iterator hi;	
  for (hi=_hierarchy_superiors.begin();hi!= _hierarchy_superiors.end();hi++ )
    two_chains->push_back((vtol_two_chain_3d*)(*hi));

  return two_chains;
}


vcl_vector<vtol_two_chain_3d*>* vtol_two_chain_3d::outside_boundary_two_chains()
{
  LIST_SELF_3d(vtol_two_chain_3d);
}



/*
 ******************************************************
 *
 *    Operator Functions
 */

//:
// equality operator 

bool vtol_two_chain_3d::operator==(const vtol_two_chain_3d& obj) const
{
  if (this == &obj) return true;

  if (_inferiors.size() != obj._inferiors.size())
    return false;

  topology_list_3d::const_iterator ti1,ti2;
  for (ti1=obj._inferiors.begin(),ti2= _inferiors.begin();ti2!= _inferiors.end() && ti1!= obj._inferiors.end(); ti1++,ti2++)
    {
      vtol_face_3d* f1 = (*ti2)->cast_to_face_3d();
      vtol_face_3d* f2 = (*ti1)->cast_to_face_3d();
      if (!(*f1 == *f2))
        return false;
    }
  const hierarchy_node_list_3d& righth = this->_hierarchy_inferiors;
  const hierarchy_node_list_3d& lefth = obj._hierarchy_inferiors;
  if(righth.size() != lefth.size())
    return false;
 
   hierarchy_node_list_3d::const_iterator hi1,hi2;

  for(hi1=righth.begin(),hi2= lefth.begin();hi1!= righth.end() && hi2!=lefth.end();hi1++,hi2++)
    if( *((vsol_spatial_object_3d*)(*hi1)) != *((vsol_spatial_object_3d*)(*hi2)))
      return false;
  return true;
}


bool vtol_two_chain_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  
 if ((obj.spatial_type() == vsol_spatial_object_3d::TOPOLOGYOBJECT) &&
      (((vtol_topology_object_3d&)obj).topology_type() == vtol_topology_object_3d::TWOCHAIN))
    return (vtol_two_chain_3d &)*this == (vtol_two_chain_3d&) (vtol_topology_object_3d&) obj;
  else return false;
 

}


/*
 ******************************************************
 *
 *    Utility Functions
 */

//:
// correct the chain directions
void vtol_two_chain_3d::correct_chain_directions()
{
  // TO DO
  cerr << "vtol_two_chain_3d::correct_chain_directions() not yet implemented\n";
  
#if 0
  vcl_vector<vtol_face_3d*> *allfaces = this->faces();
 
	
  for (vcl_vector<vtol_face_3d*>::iterator fi=allfaces->begin();fi!= allfaces->end();fi++)
    {
      (*fi)->verify();
    }
  delete allfaces;
#endif
}

/*
 ******************************************************
 *
 *    Print Functions
 */

void vtol_two_chain_3d::print(ostream& strm)
{
  strm << "<vtol_two_chain_3d " << _inferiors.size() << "  "  << (void *)this << ">"  << endl;
}



void vtol_two_chain_3d::describe_directions(ostream& strm, int )
{
  strm << "<Dirs [" << _directions.size() << "]: ";

  for (vcl_vector<signed char>::iterator di=_directions.begin();di!=_directions.end();di++)
    strm << (*di) << "  ";
  strm << endl;
}

void vtol_two_chain_3d::describe(ostream& strm, int blanking)
{
  print(strm);
  describe_inferiors(strm, blanking);
  describe_directions(strm,blanking);
  describe_superiors(strm, blanking);
}




















