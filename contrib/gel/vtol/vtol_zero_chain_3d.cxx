//#include <vtol/some_stubs.h>
#include <vtol/vtol_zero_chain_3d.h>
#include <vtol/vtol_macros_3d.h>
#include <vtol/vtol_list_functions_3d.h>
#include <vtol/vtol_edge_3d.h>

vtol_zero_chain_3d::vtol_zero_chain_3d() 
{
}


vtol_zero_chain_3d::vtol_zero_chain_3d(vtol_vertex_3d* v1, vtol_vertex_3d* v2)   
 {
 link_inferior(v1); 
 link_inferior(v2); 
}

vtol_zero_chain_3d::vtol_zero_chain_3d(vcl_vector<vtol_vertex_3d*> & newvertices) 
{
  vcl_vector<vtol_vertex_3d*>::iterator i;
  for (i=newvertices.begin();i!= newvertices.end();++i )
    link_inferior((*i)); 
}


// -- Copy Constructor....does a deep copy.
vtol_zero_chain_3d::vtol_zero_chain_3d (vtol_zero_chain_3d const& zchain)
{
  vtol_zero_chain_3d* zc = (vtol_zero_chain_3d*)(&zchain);
  topology_list_3d *infs = zc->get_inferiors();
  vtol_vertex_3d *newvertex;
  
  for (int i = 0; i < infs->size() ; ++i)
    {
      newvertex = ((*infs)[i])->cast_to_vertex_3d()->copy();
      link_inferior(newvertex);
    }
}

//---------------------------------------------------------------------------
// -- Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_ref vtol_zero_chain_3d::clone(void) const
{
  return new vtol_zero_chain_3d(*this);
}

// -- perfrom a shallow copy 

vtol_topology_object_3d* vtol_zero_chain_3d::shallow_copy_with_no_links ( void )  
{
    return new vtol_zero_chain_3d;
}

// destructor
vtol_zero_chain_3d::~vtol_zero_chain_3d() 
{
}

//---------------------------------------------------------------------------
// -- Return the topology type
//---------------------------------------------------------------------------
vtol_zero_chain_3d::vtol_topology_object_3d_type
vtol_zero_chain_3d::topology_type(void) const
{
  return ZEROCHAIN;
}

// -- get list of vertices
vcl_vector<vtol_vertex_3d*>* vtol_zero_chain_3d::vertices()
{
  COPY_INF_3d(vtol_vertex_3d);
}

// -- get list of zero chains
vcl_vector<vtol_zero_chain_3d*>* vtol_zero_chain_3d::zero_chains()
{
  LIST_SELF_3d(vtol_zero_chain_3d);
}

// -- get list of edges 
vcl_vector<vtol_edge_3d*>* vtol_zero_chain_3d::edges()
{
  SEL_SUP_3d(vtol_edge_3d, edges);
}

// -- get list of one chains

vcl_vector<vtol_one_chain_3d*>* vtol_zero_chain_3d::one_chains() 
{
  SEL_SUP_3d(vtol_one_chain_3d, one_chains);
}

// -- get list of faces 

vcl_vector<vtol_face_3d*>* vtol_zero_chain_3d::faces()
{
  SEL_SUP_3d(vtol_face_3d, faces);
}

// -- get list of two chain

vcl_vector<vtol_two_chain_3d*>* vtol_zero_chain_3d::two_chains()
{
  SEL_SUP_3d(vtol_two_chain_3d, two_chains);
}

// -- get list of blocks 
vcl_vector<vtol_block_3d*>* vtol_zero_chain_3d::blocks()
{
  SEL_SUP_3d(vtol_block_3d, blocks);
}



/*******  Utility Methods   *************/

// -- copy the zero chain

vtol_zero_chain_3d* vtol_zero_chain_3d::copy()
{
  // This is a deep copy of the vtol_zero_chain_3d.

  vtol_zero_chain_3d *newzerochain = new vtol_zero_chain_3d();
  vtol_vertex_3d *newvertex;
  
  for (int i = 0; i < _inferiors.size() ; ++i)
    {
      newvertex = ((vtol_vertex_3d *)_inferiors[i])->copy();
      newzerochain->link_inferior(newvertex);
    }
  return newzerochain;
}


// operators

bool vtol_zero_chain_3d::operator== (const vtol_zero_chain_3d & z2) const 
{
  
  if (this == &z2)
    return true;
  
  const topology_list_3d *inf1 = this->get_inferiors();
  const topology_list_3d *inf2 = z2.get_inferiors();
  vtol_topology_object_3d *v1, *v2;

  
  if (inf1->size() == inf2->size())
    {
      topology_list_3d::const_iterator i1,i2;

      i2=inf2->begin(); 
      for (i1=inf1->begin(); i1 !=inf1->end(); ++i1,++i2)
	{
	  v1 = (*i1);
	  v2 = (*i2);
	  if (!(*v1 == *v2))
	    {
	      return false;
	    }
	}
    }
  else
    {
      return false;
    }
  return true;
}



// -- spatial object equality

bool vtol_zero_chain_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::TOPOLOGYOBJECT &&
   ((vtol_topology_object_3d const&)obj).topology_type() == vtol_topology_object_3d::ZEROCHAIN
  ? *this == (vtol_zero_chain_3d const&) (vtol_topology_object_3d const&) obj
  : false;
}


/*******  Print Methods   *************/

// -- print the object 

void vtol_zero_chain_3d::print(ostream& strm) const
{
  strm << "<vtol_zero_chain_3d " << _inferiors.size() << " " << (void *)this << ">" << endl;
}

void vtol_zero_chain_3d::describe(ostream& strm, int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  print(strm);
  describe_inferiors(strm, blanking);
  describe_superiors(strm, blanking);
}


// --
//    This method removes the object from the topological structure
//    by unlinking it.  The removal of the object may recursively cause
//    the removal of some of the object's superiors if these superiors
//    are no longer valid.  In addition, inferiors of the object are
//    recursively checked to see if they should be removed.  They
//    are removed if all their superiors have been removed.
//
//    A record of the changes to the topological structure is returned 
//    through the parameters changes and deleted.  For details on what is
//    stored in these parameters, see vtol_edge_3d::disconnect( changes, deleted ).
//    (RYF 7-16-98)
//
bool vtol_zero_chain_3d::disconnect( vcl_vector< vtol_topology_object_3d * > & changes,
                            vcl_vector< vtol_topology_object_3d * > & deleted )
{
  // Unlink this object from its superiors
  topology_list_3d * tmp = get_superiors();
  vcl_vector< vtol_edge_3d * > sup;
 
  topology_list_3d::iterator i;

  for (i= tmp->begin(); i!=tmp->end(); ++i )
      sup.push_back( (vtol_edge_3d *) (*i) );
  
  vcl_vector< vtol_edge_3d * >::iterator j;
  

  for (j= sup.begin();j!= sup.end();++j )
     ((*j))->remove( this, changes, deleted );
  

  unlink_all_superiors_twoway( this );
  deep_remove( deleted );

  return true;
}

// --
//    Removes the vertex from the zero chain.  If the removal of
//    the vertex invalidates the superior edge, then the zero
//    chain is recursively removed from the superior edge.
//    For more details, see vtol_edge_3d::disconnect( changes, deleted )
//    (RYF 7-16-98)
//
bool vtol_zero_chain_3d::remove( vtol_vertex_3d * vertex,
                        vcl_vector< vtol_topology_object_3d * > & changes,
                        vcl_vector< vtol_topology_object_3d * > & deleted )
{
  // cout << "   Entering vtol_zero_chain_3d::Remove\n";

  // Check if removing the vertex affects the superior edge.
  // If so, remove the vtol_zero_chain_3d from the vtol_edge_3d.  This will
  // destroy the edge.
  topology_list_3d * tmp = get_superiors();
  int num_superiors_prior = tmp->size();

  vcl_vector< vtol_edge_3d * > sup;

  topology_list_3d::iterator i;
  for (i= tmp->begin(); i!=tmp->end(); ++i )
      sup.push_back( (vtol_edge_3d *) (*i) );



  vcl_vector< vtol_edge_3d * >::iterator s;
  for (s= sup.begin(); s!=sup.end();++s )
  {
      if ( ((vtol_edge_3d *) (*s))->is_endpoint( (vtol_vertex_3d *) vertex ) )
          (*s)->remove( this, changes, deleted );
  }


  // If it had superiors prior to the removal but none now, add to changes
  //cout << "   num_superiors before = " << num_superiors_prior  << endl
  //     << "   num_superiors after = " << _Superiors.length() << endl;
  if (( num_superiors_prior ) && ( !(_superiors.size()) ))
      changes.push_back( this );

  // cout << "   Exiting vtol_zero_chain_3d::Remove\n";

  if ( _inferiors.size() == 1 )  // last vertex in chain
  {
      deleted.push_back( this );
      unlink_all_superiors_twoway( this );
      unlink_all_inferiors_twoway( this );
      return false; 
  }
  else
  {
      unlink_inferior( vertex );
      return true;
  }

}

// --
//    For each inferior, this method unlinks the inferior
//    from this object.  If the inferior now has zero superiors,
//    the function is called recursively on it.  Finally, this
//    object is pushed onto the list removed.  (RYF 7-16-98)
//
void vtol_zero_chain_3d::deep_remove( vcl_vector< vtol_topology_object_3d * > & removed )
{
  // cout << "                  Entering vtol_zero_chain_3d::DeepDeleteInferiors\n";

  // Make a copy of the object's inferiors
  topology_list_3d * tmp = get_inferiors();
  vcl_vector< vtol_vertex_3d * > inferiors;
  
  topology_list_3d::iterator t;

  for ( t=tmp->begin(); t!=tmp->end();++t )
      inferiors.push_back( (vtol_vertex_3d *) (*t) );
  
  vcl_vector< vtol_vertex_3d * >::iterator i;
  for ( i=inferiors.begin(); i!=inferiors.end();++i )
  {
      vtol_vertex_3d * inferior = (*i);

      // Unlink inferior from its superior
      inferior->unlink_superior( this );  

      // Test if inferior now has 0 superiors.  If so, 
      // recursively remove its inferiors.
      /* need to fix 
      if ( inferior->numsup() == 0 )  
          inferior->deep_remove( removed );
      */
  }
  removed.push_back( this );

  // cout << "                  Exiting vtol_zero_chain_3d::DeepDeleteInferiors\n";
}


