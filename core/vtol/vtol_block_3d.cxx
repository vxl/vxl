
#include <vtol/vtol_block_3d.h>
// #include <vtol/vtol_two_chain_3d.h>
#include <vtol/vtol_edge_3d.h>
//#include <vtol/vtol_face_3d.h>
//#include <vtol/vtol_vertex_3d.h>
#include <vtol/vtol_macros_3d.h>
#include <vtol/vtol_list_functions_3d.h>
#include <vsol/vsol_box_3d.h>
#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_vector.h>
//#include <vtol/some_stubs.h>
#include <vtol/vtol_two_chain_3d.h>
//:
// default constructor
vtol_block_3d::vtol_block_3d()
{
}

//: 
// copy copnstructor
// This is a deep copy of
// the old block to the new block...
vtol_block_3d::vtol_block_3d(vtol_block_3d const& block)
{
 
  vtol_block_3d* oldblock = (vtol_block_3d*)(&block);
  vcl_vector<vtol_edge_3d*>* edges = oldblock->edges();
  vcl_vector<vtol_vertex_3d*>* verts = oldblock->vertices();
  int vlen = verts->size();
  int elen = edges->size();

  vcl_vector<vtol_topology_object_3d*> newedges(elen);
  vcl_vector<vtol_topology_object_3d*> newverts(vlen);
 

  int i =0;
  vcl_vector<vtol_vertex_3d*>::iterator vi;
  for(vi = verts->begin(); vi != verts->end(); ++vi, ++i)
    {
      vtol_vertex_3d* v = (*vi);
      newverts[i] = v->copy();
      v->set_id(i);
    }
  int j =0;

  vcl_vector<vtol_edge_3d*>::iterator ei;

  for(ei=edges->begin();ei!= edges->end();++ei, ++j)
    {
      vtol_edge_3d* e = (*ei);
      newedges[j] = new vtol_edge_3d(newverts[e->get_v1()->get_id()]->cast_to_vertex_3d(),
                             newverts[e->get_v2()->get_id()]->cast_to_vertex_3d());
      e->set_id(j);
    }

  vcl_vector<vtol_two_chain_3d*> *old2chains = oldblock->two_chains();
  vtol_two_chain_3d *new2ch;

  for ( i = 0; i < old2chains->size(); ++i)
    {
      new2ch = ((*old2chains)[i])->copy_with_arrays(newverts, newedges);
      link_inferior(new2ch);
    }
  delete edges;
  delete verts;
  delete old2chains;
}

//:
//constructor with a two chain
vtol_block_3d::vtol_block_3d (vtol_two_chain_3d* faceloop)
{	
   link_inferior(faceloop);
}


//:
// This method assumes that the first faceloop
// is the outside boundary. The remaining
// two_chains are considered holes inside the
// the outside volume.

vtol_block_3d::vtol_block_3d (vcl_vector<vtol_two_chain_3d*> &faceloops)
  
{	
  
  if (faceloops.size() > 0)
    link_inferior((vtol_topology_object_3d*)faceloops[0]);

  vtol_two_chain_3d *twoch = get_boundary_cycle();

  if (twoch)
    for (int i = 1; i < faceloops.size(); ++i)
      twoch->add_inferior_two_chain(faceloops[i]);
}


//: 
//constructor using face list

vtol_block_3d::vtol_block_3d (vcl_vector<vtol_face_3d*> &new_face_list) 
 {
  
  tagged_union((vcl_vector<vsol_spatial_object_3d*> *)&new_face_list);
  link_inferior(new vtol_two_chain_3d(new_face_list));
}


vtol_block_3d::~vtol_block_3d()
{

}

//---------------------------------------------------------------------------
// Name: clone
// Task: Clone `this': creation of a new object and initialization
//       See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_ref vtol_block_3d::clone(void) const
{
  return new vtol_block_3d(*this);
}

/*
 ******************************************************
 *
 *    Accessor Functions
 */

//---------------------------------------------------------------------------
// Name: topology_type
// Task: Return the topology type
//---------------------------------------------------------------------------
vtol_block_3d::vtol_topology_object_3d_type
vtol_block_3d::topology_type(void) const
{
  return BLOCK;
}

//:
// outside boundary vertices 

vcl_vector<vtol_vertex_3d*>* vtol_block_3d::outside_boundary_vertices()
{
  OUTSIDE_BOUNDARY_3d(vtol_vertex_3d, vtol_two_chain_3d, vertices);
}

//:
// get vertex list 

vcl_vector<vtol_vertex_3d*>* vtol_block_3d::vertices()
{
  SEL_INF_3d(vtol_vertex_3d,vertices);
}


//:
// get outside boundary zero chains

vcl_vector<vtol_zero_chain_3d*>* vtol_block_3d::outside_boundary_zero_chains()
{
  OUTSIDE_BOUNDARY_3d(vtol_zero_chain_3d, vtol_two_chain_3d, zero_chains);
}

//:
// get zero chains

vcl_vector<vtol_zero_chain_3d*>* vtol_block_3d::zero_chains()
{
  SEL_INF_3d(vtol_zero_chain_3d, zero_chains);
}

//:
// outside boundary edges

vcl_vector<vtol_edge_3d*>* vtol_block_3d::outside_boundary_edges()
{
  OUTSIDE_BOUNDARY_3d(vtol_edge_3d, vtol_two_chain_3d, edges);
}

//:
// get edges 
vcl_vector<vtol_edge_3d*>* vtol_block_3d::edges()
{
  SEL_INF_3d(vtol_edge_3d, edges);
}


//:
// get outside boundary one chains

vcl_vector<vtol_one_chain_3d*>* vtol_block_3d::outside_boundary_one_chains()
{
  OUTSIDE_BOUNDARY_3d(vtol_one_chain_3d, vtol_two_chain_3d, one_chains);
}

//: 
// get the one chains
vcl_vector<vtol_one_chain_3d*>* vtol_block_3d::one_chains()
{
  SEL_INF_3d(vtol_one_chain_3d,one_chains);
}

//:
// get the outside boundary faces

vcl_vector<vtol_face_3d*>* vtol_block_3d::outside_boundary_faces()
{
  OUTSIDE_BOUNDARY_3d(vtol_face_3d, vtol_two_chain_3d, faces);
}

//:
// get the faces 
vcl_vector<vtol_face_3d*>* vtol_block_3d::faces()
{
 SEL_INF_3d(vtol_face_3d,faces);
}

//:
// get the outside boundary two chains

vcl_vector<vtol_two_chain_3d*>* vtol_block_3d::outside_boundary_two_chains()
{
  OUTSIDE_BOUNDARY_3d(vtol_two_chain_3d, vtol_two_chain_3d, two_chains);
}

//:
// get the two chains

vcl_vector<vtol_two_chain_3d*>* vtol_block_3d::two_chains()
{
  SEL_INF_3d(vtol_two_chain_3d, two_chains);
}

//:
// get
vcl_vector<vtol_block_3d*>* vtol_block_3d::blocks()
{
  LIST_SELF_3d(vtol_block_3d);
}


/*
 ******************************************************
 *
 *    Operators Functions
 */

//:
// This is hardly an equality test...but we`ll leave it for now....pav
// June 1992.

bool vtol_block_3d::operator==(const vtol_block_3d& bl) const
{
  vtol_two_chain_3d *twoch1, *twoch2;

  if (this == &bl) return true;

  if (_inferiors.size() != bl._inferiors.size())
    return false;
  else
    {
      
      topology_list_3d::const_iterator bi1, bi2;

      bl._inferiors.begin();
      for (bi1=_inferiors.begin(), bi2= bl._inferiors.begin(); bi1!=_inferiors.end();++bi1,++bi2)
        {
          twoch1 = (*bi1)->cast_to_two_chain_3d();
         
	  twoch2 = (*bi2)->cast_to_two_chain_3d();
	  if (!(*twoch1 == *twoch2))
	    return false;
	}
    }
  
  return true;
}

//:
// spatial object equality

bool vtol_block_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::TOPOLOGYOBJECT &&
   ((vtol_topology_object_3d const&)obj).topology_type() == vtol_topology_object_3d::BLOCK
  ? *this == (vtol_block_3d const&) (vtol_topology_object_3d const&) obj
  : false;
}

/*
 ******************************************************
 *
 *    Editing and Utility  Functions
 */


//:
// deep copy 
vtol_block_3d* vtol_block_3d::copy()
{
  // This is a deep copy of
  // the source block to the new block...

  vcl_vector<vtol_two_chain_3d*> twochlist;
  vtol_two_chain_3d *new2ch;

  for (int i = 0 ; i < _inferiors.size(); ++i)
    {
      new2ch = ((vtol_two_chain_3d *)_inferiors[i])->copy();
      twochlist.push_back(new2ch);
    }

  vtol_block_3d *newblock = new vtol_block_3d(twochlist);
  return newblock;
}


//: shallow copy
vtol_topology_object_3d * vtol_block_3d::shallow_copy_with_no_links( void )
{
    vtol_block_3d * newblock = new vtol_block_3d;
    return newblock;
}

//:
// add a face loop
bool vtol_block_3d::add_face_loop(vtol_two_chain_3d* new_face_loop)
{
  this->touch();
  return link_inferior(new_face_loop);
}

//:
//    This method removes the object from the topological structure
//    by unlinking it.  The removal of the object may recursively cause
//    the removal of some of the object's superiors if these superiors
//    are no longer valid.  In addition, inferiors of the object are
//    recursively checked to see if they should be removed.  They
//    are removed if all their superiors have been removed.  Note: since
//    blocks have no superiors, this method is much shorter than the
//    corresponding methods of the other topological objects.
//
//    A record of the changes to the topological structure is returned 
//    through the parameters changes and deleted.  For details on what is
//    stored in these parameters, see vtol_edge_3d::disconnect( changes, deleted ).
//    (RYF 7-16-98)
//
bool vtol_block_3d::disconnect( vcl_vector< vtol_topology_object_3d * > &,
                        vcl_vector< vtol_topology_object_3d * > & deleted )
{
  deep_remove( deleted );
  return true;
}

//:
//    Removes the two chain from the block by unlinking it.  If 
//    the two chain is not a hole, its removal invalidates the
//    the block and the block is unlinked from the topological
//    structure and appended to the list deleted.  Any holes
//    it has are appended to the list changes.
//    If the two chain is a hole, the block is still valid even with
//    its removal.  In this case, the hole is unlinked from the
//    the block and appended to the list changes.
//    For more details, see vtol_edge_3d::disconnect( changes, deleted )
//    (RYF 7-16-98)
//
bool vtol_block_3d::remove( vtol_two_chain_3d * two_chain,
                    vcl_vector< vtol_topology_object_3d * > & changes,
                    vcl_vector< vtol_topology_object_3d * > & deleted )
{
  //cout << "                  Entering vtol_block_3d::Remove\n";

  // If two_chain is a hole, just unlink hole and return
  vcl_vector< vtol_two_chain_3d * > * holes = get_hole_cycles();
 
  bool is_hole = (vcl_find(holes->begin(),holes->end(),two_chain) != holes->end());
 
  delete holes;
  if ( is_hole )
  {
      //cout << "            Removing hole from vtol_block_3d.\n";
      vtol_two_chain_3d * outer = get_boundary_cycle();
      if ( outer )
          outer->remove_inferior_two_chain( (vtol_two_chain_3d *) two_chain );
      //cout << "                  Exiting vtol_block_3d::Remove\n";
      return true;
  }

  // Record changes - only holes remain, block is deleted
  vcl_vector< vtol_two_chain_3d * > * twochains = this->get_hole_cycles();
  
  vcl_vector< vtol_two_chain_3d * >::iterator ti;
 
  for (ti= twochains->begin();ti!= twochains->end(); ++ti )
      changes.push_back( (*ti) );
  delete twochains;
  deleted.push_back( this );

  unlink_all_superiors_twoway( this );
  unlink_all_inferiors_twoway( this );

  //cout << "                  Exiting vtol_block_3d::Remove\n";
  return false;
}

//:
//    For each inferior, this method unlinks the inferior
//    from this object.  If the inferior now has zero superiors,
//    the function is called recursively on it.  Finally, this
//    object is pushed onto the list removed.  (RYF 7-16-98)
//
void vtol_block_3d::deep_remove( vcl_vector< vtol_topology_object_3d * > & removed )
{
 
  // Remove links to holes first
  vcl_vector< vtol_two_chain_3d * > inferiors;
  vtol_two_chain_3d * outer = get_boundary_cycle();
  vcl_vector< vtol_two_chain_3d * > * holes = get_hole_cycles();
 
  vcl_vector< vtol_two_chain_3d * >::iterator tci;
  
  
  for ( tci=holes->begin();tci!= holes->end(); ++tci )
      inferiors.push_back( (*tci) );

  
  for ( tci=inferiors.begin(); tci!=inferiors.end();++tci )
  {
      vtol_two_chain_3d * inferior = (*tci);

      // Unlink inferior from its superior
      if ( outer ) outer->remove_inferior_two_chain( inferior );

      // Test if inferior now has 0 superiors.  If so, 
      // recursively remove its inferiors.
      if ( inferior->get_superiors()->size() 
           + inferior->get_hierarchy_superiors()->size() == 0 )
          inferior->deep_remove( removed );
  }

  // Now remove links to remaining inferiors
  inferiors.clear();
  topology_list_3d * tmp = get_inferiors();
 
  topology_list_3d::iterator ti;


  for ( ti=tmp->begin(); ti!=tmp->end();++ti )
      inferiors.push_back( (vtol_two_chain_3d *)(*ti) );

 
  for (tci= inferiors.begin(); tci!=inferiors.end(); ++tci )
  {
      vtol_two_chain_3d * inferior = (*tci);

      // Unlink inferior from its superior
      inferior->unlink_superior( this );  

      // Test if inferior now has 0 superiors.  If so, 
      // recursively remove its inferiors.
      if ( inferior->numsup() == 0 )  
          inferior->deep_remove( removed );
  }

  removed.push_back( this );

  //cout << "   vtol_block_3d #inferiors = " << this->numinf() << endl;
  //cout << "   Exiting vtol_block_3d::DeepDeleteInferiors\n";
}

//:
// get a hole cycle
vcl_vector< vtol_two_chain_3d* > * vtol_block_3d::get_hole_cycles()
{
  vcl_vector<vtol_two_chain_3d*>* new_list = new vcl_vector<vtol_two_chain_3d*>();
 topology_list_3d::iterator ti;

  for(ti=_inferiors.begin(); ti!= _inferiors.end(); ++ti)
    {
      vcl_vector<vtol_two_chain_3d*>* templist =
                    (*ti)->cast_to_two_chain_3d()->inferior_two_chains();
      // new_list->insert_after(*templist);
      // new_list->insert(new_list->end(),templist->begin(),templist->end());
      for(vcl_vector<vtol_two_chain_3d*>::iterator ii=templist->begin();ii != templist->end();++ii){
        new_list->push_back(*ii);
      }

      delete templist;
    }

  return new_list;
}


//:
//add a hole cycle

bool vtol_block_3d::add_hole_cycle( vtol_two_chain_3d * new_hole ) 
{
  vtol_two_chain_3d *twoch = get_boundary_cycle();

  if ( twoch )
  {
      twoch->add_inferior_two_chain( new_hole );
      return true;
  } else
      return false;
}

//:
// add a two chain

bool vtol_block_3d::add_two_chain(vtol_two_chain_3d* new_face_loop)
{
  this->touch();
  return link_inferior(new_face_loop);
}

//:
// remove a face loop

bool vtol_block_3d::remove_face_loop(vtol_two_chain_3d* doomed_face_loop)
{
  this->touch();
  return unlink_inferior(doomed_face_loop);
}

//:
// remove a two chain 
bool vtol_block_3d::remove_two_chain(vtol_two_chain_3d* doomed_face_loop)
{
  this->touch();
  return unlink_inferior(doomed_face_loop);
}


/*
 ******************************************************
 *
 *    Print Functions
 */
  

//:
// print data 
 
void vtol_block_3d::print(ostream& strm) const
{
  strm << "<vtol_block_3d " << _inferiors.size() << "  " << (void *)this << ">"  << endl;
}

void vtol_block_3d::describe(ostream& strm, int blanking) const
{
  for (int i=0; i<blanking; ++i) strm << ' ';
  print(strm);
  describe_inferiors(strm, blanking);
  describe_superiors(strm, blanking);
}

