
#include <vtol/vtol_one_chain_3d.h>
#include <vtol/vtol_edge_3d.h>
//#include <vtol/vtol_vertex_3d.h>
//#include <vtol/vtol_face_3d.h>
#include <vtol/vtol_macros_3d.h>
#include <vtol/vtol_list_functions_3d.h>
//#include <vtol/some_stubs.h>
#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_rel_ops.h>

//:
// constructors for the vtol_one_chain_3d

vtol_one_chain_3d::vtol_one_chain_3d()
{
  _cycle_p = false;
}


vtol_one_chain_3d::vtol_one_chain_3d(vcl_vector<vtol_edge_3d*>& edges, bool iscycle)
 {
  _cycle_p = iscycle;


  for(vcl_vector<vtol_edge_3d*>::iterator i = edges.begin(); i != edges.end(); ++i)
    {
    link_inferior(*i);
    _directions.push_back((signed char)1);
  }

  if (_cycle_p )
    determine_edge_directions();
}


vtol_one_chain_3d::vtol_one_chain_3d(vcl_vector<vtol_edge_3d*>& edges, vcl_vector<signed char> &dirs, bool iscycle)
{
  _cycle_p = iscycle;

  for(vcl_vector<vtol_edge_3d*>::iterator i = edges.begin(); i != edges.end(); ++i)
    {
    link_inferior(*i);
  }

  for(vcl_vector<signed char>::iterator j = dirs.begin(); j != dirs.end(); ++j)
    {
      _directions.push_back(*j);
    }
}

  
 
//:
//  Copy Constructor....does a deep copy.
vtol_one_chain_3d::vtol_one_chain_3d (vtol_one_chain_3d const& onechain)
{
  vtol_one_chain_3d* el = (vtol_one_chain_3d*)(&onechain);
  vcl_vector<vtol_vertex_3d*>* verts = el->vertices();
  int len = verts->size();
  vcl_vector<vtol_topology_object_3d*> newverts(len);
  int i = 0;
  vcl_vector<vtol_vertex_3d*>::iterator v;

  for(v=verts->begin();v!=verts->end();++v)
    {
      vtol_vertex_3d* ve = *v;
      newverts[i] = ve->copy();
      ve->set_id(i);
      ++i;
    }

  vcl_vector<signed char>::iterator dir;
  topology_list_3d::iterator inf;

  for(dir=el->_directions.begin(), inf=el->_inferiors.begin(); dir!=el->_directions.end(); ++dir, ++inf)
    {
      vtol_edge_3d* e = (*inf)->cast_to_edge_3d();

      vtol_edge_3d* newedge = new vtol_edge_3d(newverts[e->get_v1()->get_id()]->cast_to_vertex_3d(),
                               newverts[e->get_v2()->get_id()]->cast_to_vertex_3d());
      link_inferior(newedge);
      _directions.push_back(*dir);
    }
  set_cycle_p(el->get_cycle_p());
  hierarchy_node_list_3d *hierarchy_infs = el->get_hierarchy_inferiors();
  
  hierarchy_node_list_3d::iterator h;
  for (h=hierarchy_infs->begin();h!= hierarchy_infs->end();++h)
    add_hierarchy_inferior(((vtol_one_chain_3d*)(*h))->copy());
  delete verts;
}

vtol_one_chain_3d::~vtol_one_chain_3d()
{
}

//---------------------------------------------------------------------------
// Name: clone
// Task: Clone `this': creation of a new object and initialization
//       See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_ref vtol_one_chain_3d::clone(void) const
{
  return new vtol_one_chain_3d(*this);
}


vtol_one_chain_3d* vtol_one_chain_3d::copy_with_arrays(vcl_vector<vtol_topology_object_3d*>& verts,
                                   vcl_vector<vtol_topology_object_3d*>& edges)
{
  vtol_one_chain_3d* newone = new vtol_one_chain_3d();
  vcl_vector<signed char>::iterator di;
  topology_list_3d::iterator ti;
  
  for(di=_directions.begin(),ti =_inferiors.begin();ti != _inferiors.end();++ti,++di)
    {
      vtol_edge_3d *e = (*ti)->cast_to_edge_3d();
      vtol_edge_3d *newedge = edges[e->get_id()]->cast_to_edge_3d();
      newone->link_inferior(newedge);
      newone->_directions.push_back((*di));
    }
  newone->set_cycle_p(get_cycle_p());
  hierarchy_node_list_3d *hierarchy_infs = get_hierarchy_inferiors();
  hierarchy_node_list_3d::iterator hi;

  for (hi=hierarchy_infs->begin(); hi !=hierarchy_infs->end();++hi)
    {
      vtol_one_chain_3d* oldone = (vtol_one_chain_3d*)(*hi);
      newone->add_hierarchy_inferior(oldone->copy_with_arrays(verts, edges));
    }
  return newone;
}


// ***********************************
//         Accessors
//
// **********************************

//---------------------------------------------------------------------------
// Name: topology_type
// Task: Return the topology type
//---------------------------------------------------------------------------
vtol_one_chain_3d::vtol_topology_object_3d_type
vtol_one_chain_3d::topology_type(void) const
{
  return ONECHAIN;
}

//:
// Get the direction of the edge "e" in the onechain.
signed char vtol_one_chain_3d::get_direction(vtol_edge_3d* e)
{
  vcl_vector<signed char>::iterator dit = _directions.begin();
  for(topology_list_3d::iterator toit = _inferiors.begin(); toit != _inferiors.end(); ++toit)
    {
      vtol_edge_3d* ce = (*toit)->cast_to_edge_3d();
      if(ce == e)
        return *dit;
      ++dit;
    }
  return (signed char)1;
}


//:
// Get the outside boundary vertices 

vcl_vector<vtol_vertex_3d*>* vtol_one_chain_3d::outside_boundary_vertices()
{
  
  vcl_vector<vtol_vertex_3d*> *verts = new vcl_vector<vtol_vertex_3d*>();
  vcl_vector<vtol_vertex_3d*> *templist;
  vtol_edge_3d *e;

  topology_list_3d::iterator inf;
  vcl_vector<signed char>::iterator dir;

  for(inf=_inferiors.begin(), dir=_directions.begin(); inf !=_inferiors.end();++inf, ++dir)
    {
      e = (vtol_edge_3d*)(*inf);
      templist = e->vertices();

      if ((*dir) < 0){
	verts->push_back((*templist)[1]);
	verts->push_back((*templist)[0]);
      }
      else{
	verts->push_back((*templist)[0]);
	verts->push_back((*templist)[1]);
      }
      delete templist;
    }
  tagged_union((vcl_vector<vsol_spatial_object_3d*> *)verts);
  return verts;
}

//:
// Get the vertices of this object

vcl_vector<vtol_vertex_3d*>* vtol_one_chain_3d::vertices()
{
  // We must collect vertices from subchains as well as
  // from direct Inferiors...so this function only has
  // an ordering if there are no subchains.

  vcl_vector<vtol_vertex_3d*> *verts;
  verts = outside_boundary_vertices();

  // This macro adds the subchain vertices to the verts list.
  SUBCHAIN_INF_3d(verts, vtol_one_chain_3d, vtol_vertex_3d, vertices);
}


//:
// Get the outside boundary zero chains

vcl_vector<vtol_zero_chain_3d*>* vtol_one_chain_3d::outside_boundary_zero_chains()
{
  SEL_INF_3d(vtol_zero_chain_3d,zero_chains);
}


//:
// Get the zero chains of this object

vcl_vector<vtol_zero_chain_3d*>* vtol_one_chain_3d::zero_chains()
{
  vcl_vector<vtol_zero_chain_3d*> *zchs;
  zchs = outside_boundary_zero_chains();

  // This macro adds the subchain zerochains to the zchs list.
  SUBCHAIN_INF_3d(zchs, vtol_one_chain_3d,  vtol_zero_chain_3d, zero_chains);
}

//:
// get the outside boundary edges

vcl_vector<vtol_edge_3d*>* vtol_one_chain_3d::outside_boundary_edges()
{
  COPY_INF_3d(vtol_edge_3d);
}


//:
// Get the edges 

vcl_vector<vtol_edge_3d*>* vtol_one_chain_3d::edges()
{
  vcl_vector<vtol_edge_3d*> *edges;
  edges = outside_boundary_edges();

  // This macro adds the subchain zerochains to the zchs list.
  SUBCHAIN_INF_3d(edges, vtol_one_chain_3d,  vtol_edge_3d, edges);
}



//:
// Get the one chains

vcl_vector<vtol_one_chain_3d*>* vtol_one_chain_3d::one_chains()
{
  vcl_vector<vtol_one_chain_3d*> *onechs = outside_boundary_one_chains();
  
  hierarchy_node_list_3d::iterator i;
  for (i=_hierarchy_inferiors.begin();i!= _hierarchy_inferiors.end(); ++i)
    onechs->push_back((vtol_one_chain_3d*)(*i));

  return onechs;
}



//:
// Get the inferior one chains

vcl_vector<vtol_one_chain_3d*>* vtol_one_chain_3d::inferior_one_chains()
{
  vcl_vector<vtol_one_chain_3d*> *onechs = new vcl_vector<vtol_one_chain_3d*>();


  hierarchy_node_list_3d::iterator i;
  for (i=_hierarchy_inferiors.begin();i!= _hierarchy_inferiors.end(); ++i)
     onechs->push_back((vtol_one_chain_3d*)(*i));

  return onechs;
}


//:
// Get the superior one chains

vcl_vector<vtol_one_chain_3d*>* vtol_one_chain_3d::superior_one_chains()
{
  vcl_vector<vtol_one_chain_3d*> *onechs = new vcl_vector<vtol_one_chain_3d*>();

  hierarchy_node_list_3d::iterator i;
  for (i=_hierarchy_superiors.begin();i!= _hierarchy_superiors.end(); ++i)
    onechs->push_back((vtol_one_chain_3d*)(*i));

  return onechs;

}


//:
// get the outside boundary one chains

vcl_vector<vtol_one_chain_3d*>* vtol_one_chain_3d::outside_boundary_one_chains()
{
  LIST_SELF_3d(vtol_one_chain_3d);
}


//:
// get the faces

vcl_vector<vtol_face_3d*>* vtol_one_chain_3d::faces()
{
  if(this->is_sub_chain())
    {
      vcl_vector<vtol_face_3d*> *new_list = new vcl_vector<vtol_face_3d*>, *sublist;
      vcl_vector<vtol_one_chain_3d*>* onech = this->superior_one_chains();
      
      for(vcl_vector<vtol_one_chain_3d*>::iterator i = onech->begin(); i != onech->end();
	  ++i){
	sublist = (*i)->faces();
	if(sublist->size()){
          //  new_list->insert(new_list->end(),sublist->begin(),sublist->end());
          vcl_vector<vtol_face_3d*>::iterator ii;
          for(ii=sublist->begin();ii!=sublist->end();++ii){
            new_list->push_back(*ii);
          }
          
        }
	delete sublist;
      }
      tagged_union((vcl_vector<vsol_spatial_object_3d*> *)new_list);
      return new_list;
    }
  else
    {
      SEL_SUP_3d(vtol_face_3d, faces);
    }

}

//:
// get the two chains

vcl_vector<vtol_two_chain_3d*>* vtol_one_chain_3d::two_chains()
{
  if(this->is_sub_chain())
    {
      vcl_vector<vtol_two_chain_3d*> *new_list = new vcl_vector<vtol_two_chain_3d*>, *sublist;
      vcl_vector<vtol_one_chain_3d*>* onech = this->superior_one_chains();

      vcl_vector<vtol_one_chain_3d*>::iterator i;

      for(i=onech->begin();i!=onech->end();++i)
        {
          sublist = (*i)->two_chains();

       
          if(sublist->size()){
            //  new_list->insert(new_list->end(),sublist->begin(),sublist->end());
            vcl_vector<vtol_two_chain_3d*>::iterator ii;
            for(ii=sublist->begin();ii!=sublist->end();++ii){
              new_list->push_back(*ii);
            }
          }

          delete sublist;
        }
      tagged_union((vcl_vector<vsol_spatial_object_3d*> *)new_list);
      return new_list;
    }
  else
    {
      SEL_SUP_3d(vtol_two_chain_3d, two_chains);
    }
}

//:
// Get the blocks

vcl_vector<vtol_block_3d*>* vtol_one_chain_3d::blocks()
{
   if(this->is_sub_chain())
    {
      vcl_vector<vtol_block_3d*> *new_list = new vcl_vector<vtol_block_3d*>, *sublist;
      vcl_vector<vtol_one_chain_3d*>* onech = this->superior_one_chains();
      
      vcl_vector<vtol_one_chain_3d*>::iterator i;

      for(i=onech->begin();i!=onech->end();++i)
	{
          sublist = (*i)->blocks();

          
          if(sublist->size()){
            //  new_list->insert(new_list->end(),sublist->begin(),sublist->end());
            vcl_vector<vtol_block_3d*>::iterator ii;
            for(ii=sublist->begin();ii!=sublist->end();++ii){
              new_list->push_back(*ii);
            }
          }

          delete sublist;
        }
      tagged_union((vcl_vector<vsol_spatial_object_3d*> *)new_list);
      return new_list;
    }
  else
    {
      SEL_SUP_3d(vtol_block_3d, blocks);
    }
}

//:
//    Computes the bounding box of a vtol_one_chain_3d from the Edges
//    Just get the bounding box for each Edge and update this's
//    box accordingly.

void vtol_one_chain_3d::compute_bounding_box()
{
  vcl_vector<vtol_edge_3d*>* edges = this->edges();
  if(!edges->size())//default method, things are screwed up anyway
    {
      vtol_topology_object_3d::compute_bounding_box(); 
      return;
    }

  float max_float = 1000000; // do we have anything better?
  float xmin=max_float, ymin = max_float;
  float xmax=-max_float, ymax = -max_float;
  // float  zmin = max_float, zmax = -max_float ;
  for(vcl_vector<vtol_edge_3d*>::iterator eit = edges->begin();
      eit!=edges->end(); ++eit)
    {
      vtol_edge_3d* e = (*eit);
      vsol_box_3d* b = e->get_bounding_box();
      if(!b)
	{
	  cout << "In vtol_one_chain_3d::ComputeBoundingBox()"
	       << " - Edge has null bounding box" 
	       << endl;
	  continue;
	}
      if(xmin > b->get_min_x()) xmin = b->get_min_x();
      if(ymin > b->get_min_y()) ymin = b->get_min_y();
      // if(zmin > b->get_min_z()) zmin = b->get_min_z();
      if(xmax < b->get_max_x()) xmax = b->get_max_x();
      if(ymax < b->get_max_y()) ymax = b->get_max_y();
      // if(zmax < b->get_max_z()) zmax = b->get_max_z();
    }
  delete edges;
  this->set_min_x(xmin); this->set_max_x(xmax);
  this->set_min_y(ymin); this->set_max_y(ymax);
  // this->set_min_z(zmin); this->set_max_z(zmax);
}

//:
// reset the object

void vtol_one_chain_3d::clear()
{
  _directions.clear();
  unlink_all_inferiors(this);
}


//:
// Redeterming the directions of all edges in the onechain.

void vtol_one_chain_3d::determine_edge_directions()
{
  // Assuming that this is a connected chain...
  // Clear out any old info...

  _directions.clear();
  int num_edges = _inferiors.size();
  if (num_edges == 0)  return;
  if (num_edges == 1)
    {
      _directions.push_back((signed char)1);
      return;
    }

  // Determining the cycle direction
  // with reference to the first edge.

  vtol_edge_3d *first_edge, *second_edge;
  vtol_vertex_3d *tweeney;
  
  topology_list_3d::iterator i;
  i=_inferiors.begin();

  first_edge = (*i)->cast_to_edge_3d();
  ++i;

  second_edge = (*i)->cast_to_edge_3d();

  if (second_edge->is_endpoint1(first_edge->get_v1()))
    {
      _directions.push_back((signed char)(-1));
      tweeney = second_edge->get_v2();
      _directions.push_back((signed char)1);
    }
  else
    if (second_edge->is_endpoint2(first_edge->get_v1()))
      {
        _directions.push_back((signed char)(-1));
        _directions.push_back((signed char)(-1));
        tweeney = second_edge->get_v1();
      }
    else
      {
        _directions.push_back((signed char)1);
        if (second_edge->is_endpoint1(first_edge->get_v2()))
          {
            tweeney = second_edge->get_v2();
            _directions.push_back((signed char)1);
          }
        else
          {
            tweeney = second_edge->get_v1();
            _directions.push_back((signed char)(-1));
          }
      }

  if (num_edges > 2)
    {
      vtol_edge_3d *cur_edge;
      ++i;
      while (i!=_inferiors.end())
        {
          cur_edge = (*i)->cast_to_edge_3d();
          if (cur_edge->is_endpoint1(tweeney))
            {
              tweeney = cur_edge->get_v2();
              _directions.push_back((signed char)1);
            } else {
              tweeney = cur_edge->get_v1();
              _directions.push_back((signed char)(-1));
            }
	  ++i;
	      
        }

    }
}


//:
// add an inferior one chain

void vtol_one_chain_3d::add_inferior_one_chain(vtol_one_chain_3d* onechain )
{
  // Add it to the hierarchy.
  add_hierarchy_inferior(onechain);
}

//:
// remove an inferior one chain

void vtol_one_chain_3d::remove_inferior_one_chain(vtol_one_chain_3d* onechain )
{
  // Remove onechain from hierarchy.
  remove_hierarchy_inferior(onechain);
}


//:
// add an edge

bool vtol_one_chain_3d::add_edge(vtol_edge_3d* new_edge, bool dir )
{
  if (dir == true)
    _directions.push_back((signed char)1);
  else _directions.push_back((signed char)(-1));
  this->touch();
  return link_inferior(new_edge);
}

//:
// remove an edge

bool vtol_one_chain_3d::remove_edge(vtol_edge_3d* doomed_edge, bool force_it)
{
  if (!force_it && _cycle_p)
    {
      cerr << "Tried to remove edge to cyclic edge loop" << endl;
      return false;
    } else {
      // int index = _inferiors.position(doomed_edge);
      topology_list_3d::iterator i = vcl_find(_inferiors.begin(),_inferiors.end(),
					      (vtol_topology_object_3d*)doomed_edge);
      int index = i-_inferiors.begin();
      


      if(index >= 0)
        {
	  vcl_vector<signed char>::iterator j = _directions.begin();
	  j=j+index;
	  _directions.erase(j);        // get rid of the direction associated with the edge
          this->touch();
          return unlink_inferior(doomed_edge);
        }
      return false;
    }
}

//:
// comparison operator

bool vtol_one_chain_3d::operator==(const vtol_one_chain_3d& ch) const
{
  const topology_list_3d* inf1 = this->get_inferiors();
  const topology_list_3d* inf2 = ch.get_inferiors();

  if (this == &ch) return true;

  // Check to see if the number of vertices is the same
#if 0 // 10 lines commented out
  vcl_vector<vtol_vertex_3d*>* verts1 = this->vertices();
  vcl_vector<vtol_vertex_3d*>* verts2 = ch.vertices();
  if(verts1->size() != verts2->size())
    {
      delete verts1;
      delete verts2;
      return false;
    }
  delete verts1;
  delete verts2;
#endif
  
  if (inf1->size() != inf2->size())
    return false;
  
  
  topology_list_3d::const_iterator i1;
  topology_list_3d::const_iterator i2;
  
  for(i1=inf1->begin() , i2 = inf2->begin(); i1 != inf1->end(); ++i1 , ++i2){
    if (!( *(*i1) == *(*i2) ))
      return false;

    // Comparing the _directions
   const vcl_vector<signed char> *dir1 = this->get_directions();
   const  vcl_vector<signed char> *dir2 = ch.get_directions();
    
    
    if ((dir1->size() != dir2->size()) || (_cycle_p != ch.cycle()))
      return false;
    
    vcl_vector<signed char>::const_iterator d1;
    vcl_vector<signed char>::const_iterator d2;
    
    for(d1=dir1->begin(), d2=dir2->begin(); d1 != dir1->end(); ++d1, ++d2)
      if (!(*d1 == *d2))
	return false;
    
    // compare onechains that make up any holes
    const hierarchy_node_list_3d& righth = this->_hierarchy_inferiors;
    const hierarchy_node_list_3d& lefth = ch._hierarchy_inferiors;
    if(righth.size() != lefth.size())
      return false;
    
    
    hierarchy_node_list_3d::const_iterator r;
    hierarchy_node_list_3d::const_iterator l;
    
    for(r=righth.begin(), l=lefth.begin(); r!=righth.end(); ++r, ++l)
      if( *((vsol_spatial_object_3d*)(*r)) != *((vsol_spatial_object_3d*)((l))))
	return false;
  }
  return true;
}


//:
// reverse the direction of the one chain

void  vtol_one_chain_3d::reverse_directions()
{
  
  // This function reverses the direction{
  // array in the list.
  
 

  for(vcl_vector<signed char>::iterator di=_directions.begin(); 
      di !=_directions.end();++di)
      (*di) = - (*di);

  // reverse the inferiors 

  topology_list_3d inf_tmp(_inferiors.size());

  // vcl_reverse_copy(_inferiors.begin(),_inferiors.end(),inf_tmp.begin());
  // reverse copy does not seem to work do this the hard way
  int i;
  int s= _inferiors.size();
  for(i=0;i<s;++i){
    inf_tmp[i]=_inferiors[s-1-i];
  }
  
  _inferiors.clear();
  vcl_copy(inf_tmp.begin(),inf_tmp.end(),_inferiors.begin());
  

  vcl_vector<signed char> dir_tmp(_directions.size());
  
  // vcl_reverse_copy(_directions.begin(),_directions.end(),dir_tmp.begin());
  s=_directions.size();

  for(i=0;i<s;++i){
    dir_tmp[i]=_directions[s-1-i];
  }
  
  
  _directions.clear();
  vcl_copy(dir_tmp.begin(),dir_tmp.end(),_directions.begin());


  hierarchy_node_list_3d::iterator hi;

  for (hi=_hierarchy_inferiors.begin();hi!= _hierarchy_inferiors.end();++hi )
    ((vtol_one_chain_3d*)(*hi))->reverse_directions();
}


//:
// spatial object equality

bool vtol_one_chain_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::TOPOLOGYOBJECT &&
   ((vtol_topology_object_3d const&)obj).topology_type() == vtol_topology_object_3d::ONECHAIN
  ? *this == (vtol_one_chain_3d const&) (vtol_topology_object_3d const&) obj
  : false;
}


//:
// Print Methods


void vtol_one_chain_3d::print(ostream& strm) const
{
  strm << "<one_chain_3d " << _inferiors.size() << "  " << (void *) this << ">"  << endl;
}


//:
// Describe the directions

void vtol_one_chain_3d::describe_directions(ostream& strm, int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  strm << "<Dirs [" << _directions.size() << "]: ";

  vcl_vector<signed char>::const_iterator d1;
  for (d1=_directions.begin();d1!=_directions.end();++d1)
    strm << (int)(*d1) << "  ";
  strm << endl;
}


//:
// Describe the one chain 

void vtol_one_chain_3d::describe(ostream& strm, int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  print(strm);
  describe_inferiors(strm, blanking);
  describe_directions(strm, blanking);
  describe_superiors(strm, blanking);
}

//:
//    For each inferior, this method unlinks the inferior
//    from this object.  If the inferior now has zero superiors,
//    the function is called recursively on it.  Finally, this
//    object is pushed onto the list removed. (RYF 7-16-98)
//


void vtol_one_chain_3d::deep_remove( vcl_vector< vtol_topology_object_3d * > & removed )
{
  // cout << "            Entering vtol_one_chain::DeepDeleteInferiors\n";

  // Make a copy of the object's inferiors
  topology_list_3d * tmp = get_inferiors();

  vcl_vector< vtol_edge_3d * > inferiors;

  topology_list_3d::iterator ti;

  for (ti= tmp->begin(); ti!=tmp->end();++ti )
      inferiors.push_back( (vtol_edge_3d *)(*ti) );
  
  vcl_vector< vtol_edge_3d * >::iterator ii;

  for (ii= inferiors.begin(); ii!=inferiors.end();++ii )
  {
      vtol_edge_3d * inferior = (*ii);

      // Unlink inferior from its superior
      inferior->unlink_superior( this );  

      // Test if inferior now has 0 superiors.  If so, 
      // recursively remove its inferiors.
      if ( inferior->numsup() == 0 )  
          inferior->deep_remove( removed );
  }
  _directions.clear();
  removed.push_back( this );

  // cout << "            Exiting vtol_one_chain::DeepDeleteInferiors\n";
}
