#include <vtol/vtol_one_chain_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_macros_2d.h>
#include <vtol/vtol_list_functions_2d.h>
#include <vcl/vcl_algorithm.h>

vtol_edge_2d *vtol_one_chain_2d::edge(int i) const
{
  return (vtol_edge_2d *)(_inferiors[i].ptr());
}

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Name: vtol_one_chain_2d
// Task: Default constructor
//---------------------------------------------------------------------------
vtol_one_chain_2d::vtol_one_chain_2d(void)
{
  _is_cycle=false;
}

//---------------------------------------------------------------------------
// Name: vtol_one_chain_2d
// Task: Constructor from an array of edges
//---------------------------------------------------------------------------
vtol_one_chain_2d::vtol_one_chain_2d(edge_list_2d &edgs,
                                     bool new_is_cycle)
{
  edge_list_2d::iterator i;
  _is_cycle=new_is_cycle;
  
  for(i=edgs.begin();i!=edgs.end();i++)
    {
      link_inferior(*(*i));
      _directions.push_back((signed char)1);
    }

  if(_is_cycle)
    determine_edge_directions();
}

//---------------------------------------------------------------------------
// Name: vtol_one_chain_2d
// Task: Constructor from an array of edges and an array of directions
//---------------------------------------------------------------------------
vtol_one_chain_2d::vtol_one_chain_2d(edge_list_2d &edgs,
                                     vcl_vector<signed char> &dirs,
                                     bool new_is_cycle)
{
  edge_list_2d::iterator i;
  vcl_vector<signed char>::iterator j;

  _is_cycle=new_is_cycle;

  for(i=edgs.begin();i!=edgs.end();i++)
    link_inferior(*(*i));
  
  for(j=dirs.begin();j!=dirs.end();j++)
    _directions.push_back(*j);
}

//---------------------------------------------------------------------------
// Name: vtol_one_chain_2d
// Task: Copy constructor
//---------------------------------------------------------------------------
//  Copy Constructor....does a deep copy.
vtol_one_chain_2d::vtol_one_chain_2d(const vtol_one_chain_2d &other)
{
  vtol_one_chain_2d *el;
  vertex_list_2d *verts;
  int len;
  int i;
  vcl_vector<vtol_vertex_2d_ref>::iterator v;
  vtol_vertex_2d_ref ve;
  vcl_vector<signed char>::iterator dir;
  topology_list_2d::iterator inf;
  vtol_edge_2d_ref e;
  vtol_edge_2d_ref newedge;
  const chain_list_2d *hierarchy_infs;
  chain_list_2d::const_iterator h;

  el=(vtol_one_chain_2d *)(&other);
  verts=el->vertices();
  len=verts->size();
  vcl_vector<vtol_topology_object_2d_ref> newverts(len);

  i=0;

  for(v=verts->begin();v!=verts->end();v++)
    {
      ve=*v;
      newverts[i]=(vtol_topology_object_2d *)(ve->clone().ptr());
      ve->set_id(i);
      i++;
    }

  for(dir=el->_directions.begin(),inf=el->_inferiors.begin();
      dir!=el->_directions.end();
      dir++,inf++)
    {
      e=(*inf)->cast_to_edge();

      newedge=new vtol_edge_2d(*(newverts[e->v1()->get_id()]->cast_to_vertex()),
                               *(newverts[e->v2()->get_id()]->cast_to_vertex()));
      link_inferior(*newedge);
      _directions.push_back(*dir);
    }
  set_cycle(el->is_cycle());
  hierarchy_infs=el->chain_inferiors();
  
  for(h=hierarchy_infs->begin();h!=hierarchy_infs->end();h++)
    link_chain_inferior(*((vtol_one_chain_2d *)((*h)->clone().ptr())));
  delete verts;
}

//---------------------------------------------------------------------------
// Name: ~vtol_one_chain_2d
// Task: Destructor
//---------------------------------------------------------------------------
vtol_one_chain_2d::~vtol_one_chain_2d()
{
  unlink_all_chain_inferiors();
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
// Name: clone
// Task: Clone `this': creation of a new object and initialization
//       See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_ref vtol_one_chain_2d::clone(void) const
{
  return new vtol_one_chain_2d(*this);
}

vtol_one_chain_2d *
vtol_one_chain_2d::copy_with_arrays(vcl_vector<vtol_topology_object_2d_ref> &verts,
                                    vcl_vector<vtol_topology_object_2d_ref> &edges) const
{
  vtol_one_chain_2d *result=new vtol_one_chain_2d();
  vcl_vector<signed char>::const_iterator di;
  topology_list_2d::const_iterator ti;
  
  for(di=_directions.begin(),ti =_inferiors.begin();
      ti!=_inferiors.end();
      ti++,di++)
    {
      vtol_edge_2d *e=(*ti)->cast_to_edge();
      vtol_edge_2d *newedge=edges[e->get_id()]->cast_to_edge();
      result->link_inferior(*newedge);
      result->_directions.push_back((*di));
    }
  result->set_cycle(is_cycle());
  const chain_list_2d *hierarchy_infs=chain_inferiors();
  chain_list_2d::const_iterator hi;

  for(hi=hierarchy_infs->begin();hi!=hierarchy_infs->end();hi++)
    {
      vtol_one_chain_2d *oldone=(vtol_one_chain_2d *)((*hi)->clone().ptr());
      result->link_chain_inferior(*(oldone->copy_with_arrays(verts,edges)));
    }
  return result;
}

// ***********************************
//         Accessors
//
// **********************************

//---------------------------------------------------------------------------
// Name: topology_type
// Task: Return the topology type
//---------------------------------------------------------------------------
vtol_one_chain_2d::vtol_topology_object_2d_type
vtol_one_chain_2d::topology_type(void) const
{
  return ONECHAIN;
}

//:
// -- Get the direction of the edge "e" in the onechain.
signed char vtol_one_chain_2d::direction(const vtol_edge_2d &e) const
{
  vcl_vector<signed char>::const_iterator dit;
  topology_list_2d::const_iterator toit;

  dit=_directions.begin();
  for(toit=_inferiors.begin();toit!=_inferiors.end();++toit)
    {
      vtol_edge_2d *ce=(*toit)->cast_to_edge();
      if(ce==&e)
        return *dit;
      ++dit;
    }
  return (signed char)1;
}

//***************************************************************************
// Replaces dynamic_cast<T>
//***************************************************************************

//---------------------------------------------------------------------------
// Name: cast_to_one_chain
// Task: Return `this' if `this' is an one_chain, 0 otherwise
//---------------------------------------------------------------------------
const vtol_one_chain_2d *vtol_one_chain_2d::cast_to_one_chain(void) const
{
  return this;
}

//---------------------------------------------------------------------------
// Name: cast_to_one_chain
// Task: Return `this' if `this' is an one_chain, 0 otherwise
//---------------------------------------------------------------------------
vtol_one_chain_2d *vtol_one_chain_2d::cast_to_one_chain(void)
{
  return this;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Name: valid_inferior_type
// Task: Is `inferior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_one_chain_2d::valid_inferior_type(const vtol_topology_object_2d &inferior) const
{
  return inferior.cast_to_edge()!=0;
}

//---------------------------------------------------------------------------
// Name: valid_superior_type
// Task: Is `superior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_one_chain_2d::valid_superior_type(const vtol_topology_object_2d &superior) const
{
  return superior.cast_to_face()!=0;
}

//---------------------------------------------------------------------------
//: Is `chain_inf_sup' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_one_chain_2d::valid_chain_type(const vtol_chain_2d &chain_inf_sup) const
{
  return chain_inf_sup.cast_to_one_chain()!=0;
}



//:
// Get the outside boundary vertices 

vcl_vector<vtol_vertex_2d *> *
vtol_one_chain_2d::outside_boundary_compute_vertices(void)
{
  vcl_vector<vtol_vertex_2d *> *result;
  vcl_vector<vtol_vertex_2d_ref> *templist;
  vcl_vector<vtol_vertex_2d_ref>::iterator t;
  vtol_edge_2d_ref e;

  topology_list_2d::iterator inf;
  vcl_vector<signed char>::iterator dir;

  result=new vcl_vector<vtol_vertex_2d *>;

  for(inf=_inferiors.begin(),dir=_directions.begin();
      inf!=_inferiors.end();
      inf++,dir++)
    {
      e=(vtol_edge_2d *)(inf->ptr());
      templist=e->vertices();
      t=templist->begin();
      if((*dir)< 0)
        {
          t++;
          result->push_back(t->ptr());
          t--;
          result->push_back(t->ptr());
        }
      else
        {
          result->push_back(t->ptr());
          t++;
          result->push_back(t->ptr());
        }
      delete templist;
    }
  tagged_union((vcl_vector<vsol_spatial_object_2d *> *)result);
  return result;
}

//:
// Get the outside boundary vertices 

vertex_list_2d *vtol_one_chain_2d::outside_boundary_vertices(void)
{
  vertex_list_2d *result;
  vcl_vector<vtol_vertex_2d *> *tmp_list;
  tmp_list=outside_boundary_compute_vertices();
  result=new vertex_list_2d;
  result->reserve(tmp_list->size());
  vcl_vector<vtol_vertex_2d *>::iterator i;
  for(i=tmp_list->begin();i!=tmp_list->end();i++)
    result->push_back((*i));
  delete tmp_list;
  return result;
}

//:
// Get the vertices of this object

vcl_vector<vtol_vertex_2d*> *vtol_one_chain_2d::compute_vertices(void)
{
  // We must collect vertices from subchains as well as
  // from direct Inferiors...so this function only has
  // an ordering if there are no subchains.

  vcl_vector<vtol_vertex_2d *> *verts;
  verts=outside_boundary_compute_vertices();

  // This macro adds the subchain vertices to the verts list.
  
  SUBCHAIN_INF_2d(verts,vtol_one_chain_2d,vtol_vertex_2d,compute_vertices);
}


//:
// Get the outside boundary zero chains

vcl_vector<vtol_zero_chain_2d *> *
vtol_one_chain_2d::outside_boundary_compute_zero_chains(void)
{
  SEL_INF_2d(vtol_zero_chain_2d,compute_zero_chains);
}

//:
// Get the outside boundary zero chains

zero_chain_list_2d *vtol_one_chain_2d::outside_boundary_zero_chains(void)
{
  zero_chain_list_2d *result;
  vcl_vector<vtol_zero_chain_2d *> *ptr_list;
  vcl_vector<vtol_zero_chain_2d *>::iterator i;

  result=new zero_chain_list_2d;
  ptr_list=outside_boundary_compute_zero_chains();
  // copy the lists

  for(i=ptr_list->begin();i!=ptr_list->end();i++)
    result->push_back(*i);
  delete ptr_list;
  
  return result;
}


//:
// Get the zero chains of this object

vcl_vector<vtol_zero_chain_2d*> *vtol_one_chain_2d::compute_zero_chains(void)
{
  vcl_vector<vtol_zero_chain_2d*> *zchs;
  zchs=outside_boundary_compute_zero_chains();

  // This macro adds the subchain zerochains to the zchs list.
 
  SUBCHAIN_INF_2d(zchs,vtol_one_chain_2d,vtol_zero_chain_2d,compute_zero_chains);
}

//:
// get the outside boundary edges

vcl_vector<vtol_edge_2d*> *vtol_one_chain_2d::outside_boundary_compute_edges(void)
{
  
  COPY_INF_2d(vtol_edge_2d);
}


//:
// get the outside boundary edges

edge_list_2d *vtol_one_chain_2d::outside_boundary_edges(void)
{
 
  edge_list_2d *new_ref_list = new edge_list_2d();
  vcl_vector<vtol_edge_2d*>* ptr_list = this->outside_boundary_compute_edges();
  // copy the lists
  
  for(vcl_vector<vtol_edge_2d*>::iterator ti = ptr_list->begin();
      ti != ptr_list->end(); ti++){
    new_ref_list->push_back(*ti);
  }
  delete ptr_list;
  
  return new_ref_list;
}

 



//:
// Get the edges 

vcl_vector<vtol_edge_2d*> *vtol_one_chain_2d::compute_edges(void)
{
  vcl_vector<vtol_edge_2d*> *edges;
  edges=outside_boundary_compute_edges();

  // This macro adds the subchain zerochains to the zchs list.
 
  SUBCHAIN_INF_2d(edges, vtol_one_chain_2d,  vtol_edge_2d, compute_edges);
}



//:
// Get the one chains

vcl_vector<vtol_one_chain_2d*> *vtol_one_chain_2d::compute_one_chains(void)
{
  vcl_vector<vtol_one_chain_2d*> *result=outside_boundary_compute_one_chains();
  chain_list_2d::iterator i;
  // is the clone necessary? 
  for(i=_chain_inferiors.begin();i!=_chain_inferiors.end();i++)
    result->push_back((vtol_one_chain_2d *)((*i)->clone().ptr()));
  return result;
}

  //(vtol_one_chain_2d *)((*hi)->clone().ptr());

//:
// Get the inferior one chains

one_chain_list_2d *vtol_one_chain_2d::inferior_one_chains(void)
{
  one_chain_list_2d *result;
  result=new vcl_vector<vtol_one_chain_2d_ref>();

  chain_list_2d::iterator i;
  for(i=_chain_inferiors.begin();i!=_chain_inferiors.end();i++)
    result->push_back((vtol_one_chain_2d *)((*i)->clone().ptr()));

  return result;
}

//:
// Get the superior one chains

one_chain_list_2d *vtol_one_chain_2d::superior_one_chains(void)
{
  one_chain_list_2d *result;
  result=new vcl_vector<vtol_one_chain_2d_ref>();

  vcl_list<vtol_chain_2d_ref>::iterator i;
  for(i=_chain_superiors.begin();i!=_chain_superiors.end();i++)
    result->push_back((vtol_one_chain_2d *)((*i)->clone().ptr()));

  return result;
}

//:
// get the outside boundary one chains

one_chain_list_2d *vtol_one_chain_2d::outside_boundary_one_chains(void)
{
  vcl_vector<vtol_one_chain_2d*>* ptr_list= outside_boundary_compute_one_chains();
  one_chain_list_2d *ref_list= new one_chain_list_2d();
  
  vcl_vector<vtol_one_chain_2d*>::iterator i;
  for(i=ptr_list->begin();i!=ptr_list->end();i++){
    ref_list->push_back(*i);
  }
  delete ptr_list;
  return ref_list;

}



//:
// get the outside boundary one chains

vcl_vector<vtol_one_chain_2d*> *vtol_one_chain_2d::outside_boundary_compute_one_chains(void)
{
  
  LIST_SELF_2d(vtol_one_chain_2d);
}


//:
// get the faces

vcl_vector<vtol_face_2d*> *vtol_one_chain_2d::compute_faces(void)
{
  vcl_vector<vtol_face_2d*> *result;
  vcl_vector<vtol_face_2d*> *sublist;
  one_chain_list_2d *onech;
  vcl_vector<vtol_one_chain_2d_ref>::iterator i;
  vcl_vector<vtol_face_2d*>::iterator ii;

  if(is_sub_chain())
    {
      result=new vcl_vector<vtol_face_2d*>();
      onech=superior_one_chains();
      
      for(i=onech->begin();i!=onech->end();i++)
        {
          sublist=(*i)->compute_faces();
          if(sublist->size()>0)
            {
              //  result->insert(new_list->end(),sublist->begin(),sublist->end());
              for(ii=sublist->begin();ii!=sublist->end();ii++)
                result->push_back(*ii);
            }
          delete sublist;
        }
      tagged_union((vcl_vector<vsol_spatial_object_2d *> *)result);
      return result;
    }
  else
    {
     
      SEL_SUP_2d(vtol_face_2d, compute_faces);
    }
}

//:
// get the two chains

vcl_vector<vtol_two_chain_2d*> *vtol_one_chain_2d::compute_two_chains(void)
{
  vcl_vector<vtol_two_chain_2d*> *result;
  vcl_vector<vtol_two_chain_2d*> *sublist;
  one_chain_list_2d *onech;
  vcl_vector<vtol_one_chain_2d_ref>::iterator i;
  vcl_vector<vtol_two_chain_2d*>::iterator ii;

  if(is_sub_chain())
    {
      result=new vcl_vector<vtol_two_chain_2d*>;
      onech=superior_one_chains();
      
      for(i=onech->begin();i!=onech->end();i++)
        {
          sublist=(*i)->compute_two_chains();
          if(sublist->size())
            {
              //  result->insert(new_list->end(),sublist->begin(),sublist->end());
              for(ii=sublist->begin();ii!=sublist->end();ii++)
                result->push_back(*ii);
            }
          delete sublist;
        }
      tagged_union((vcl_vector<vsol_spatial_object_2d *> *)result);
      return result;
    }
  else
    {
     
      SEL_SUP_2d(vtol_two_chain_2d, compute_two_chains);
    }
}

//:
// Get the blocks

vcl_vector<vtol_block_2d*> *vtol_one_chain_2d::compute_blocks(void)
{
  vcl_vector<vtol_block_2d*> *result;
  vcl_vector<vtol_block_2d*> *sublist;
  one_chain_list_2d *onech;
  vcl_vector<vtol_one_chain_2d_ref>::iterator i;
  vcl_vector<vtol_block_2d*>::iterator ii;

  if(is_sub_chain())
    {
      result=new vcl_vector<vtol_block_2d*>;
      onech=superior_one_chains();
      for(i=onech->begin();i!=onech->end();i++)
	{
          sublist=(*i)->compute_blocks();
          if(sublist->size())
            {
              // result->insert(new_list->end(),sublist->begin(),sublist->end());
              for(ii=sublist->begin();ii!=sublist->end();ii++){
                result->push_back(*ii);
              }
            }
          delete sublist;
        }
      tagged_union((vcl_vector<vsol_spatial_object_2d *> *)result);
      return result;
    }
  else
    {
       
      SEL_SUP_2d(vtol_block_2d, compute_blocks);
    }
}

//:
// -- Computes the bounding box of a vtol_one_chain_2d from the Edges
//    Just get the bounding box for each Edge and update this's
//    box accordingly.

void vtol_one_chain_2d::compute_bounding_box(void)
{
  vcl_vector<vtol_edge_2d_ref> *edgs;
  vcl_vector<vtol_edge_2d_ref>::iterator eit;
  float max_float;
  float xmin;
  float ymin;
  float xmax;
  float ymax;

  edgs=edges();
  if(edgs->size()==0)//default method, things are screwed up anyway
    {
      vtol_topology_object_2d::compute_bounding_box(); 
      return;
    }

  max_float=1000000; // do we have anything better?
  xmin=max_float;
  ymin=max_float;
  xmax=-max_float;
  ymax=-max_float;
  // float  zmin = max_float, zmax = -max_float ;
  for(eit=edgs->begin();eit!=edgs->end();eit++)
    {
      vtol_edge_2d *e=(*eit);
      vsol_box_2d *b=e->get_bounding_box();
      if(b==0)
	{
	  cout << "In vtol_one_chain_2d::ComputeBoundingBox()"
	       << " - Edge has null bounding box" 
	       << endl;
	  continue;
	}
      if(xmin>b->get_min_x())
        xmin=b->get_min_x();
      if(ymin>b->get_min_y())
        ymin=b->get_min_y();
      // if(zmin > b->get_min_z()) zmin = b->get_min_z();
      if(xmax<b->get_max_x())
        xmax=b->get_max_x();
      if(ymax<b->get_max_y())
        ymax=b->get_max_y();
      // if(zmax < b->get_max_z()) zmax = b->get_max_z();
    }
  delete edgs;
  set_min_x(xmin);
  set_max_x(xmax);
  set_min_y(ymin);
  set_max_y(ymax);
 // this->set_min_z(zmin); this->set_max_z(zmax);
}

//---------------------------------------------------------------------------
// Name: determine_edge_directions
// Task : Redeterming the directions of all edges in the onechain.
// Require: is_cycle()
//---------------------------------------------------------------------------
void vtol_one_chain_2d::determine_edge_directions(void)
{
  // require
  assert(is_cycle());

  int num_edges;
  vtol_edge_2d_ref first_edge;
  vtol_edge_2d_ref second_edge;
  vtol_vertex_2d_ref tweeney;
  
  topology_list_2d::iterator i;

  // Clear out any old info...
  _directions.clear();
  num_edges=numinf();
  if(num_edges>=2)
    {
      // Determining the cycle direction
      // with reference to the first edge.
      
      i=_inferiors.begin();
      
      first_edge=(*i)->cast_to_edge();
      i++;
      
      second_edge=(*i)->cast_to_edge();
      
      if(second_edge->is_endpoint1(*(first_edge->v1())))
        {
          _directions.push_back((signed char)(-1));
          tweeney=second_edge->v2();
          _directions.push_back((signed char)1);
        }
      else if(second_edge->is_endpoint2(*(first_edge->v1())))
        {
          _directions.push_back((signed char)(-1));
          _directions.push_back((signed char)(-1));
          tweeney=second_edge->v1();
        }
      else
        {
          _directions.push_back((signed char)1);
          if(second_edge->is_endpoint1(*(first_edge->v2())))
            {
              tweeney=second_edge->v2();
              _directions.push_back((signed char)1);
            }
          else
            {
              tweeney=second_edge->v1();
              _directions.push_back((signed char)(-1));
            }
        }
      if(num_edges>2)
        {
          vtol_edge_2d *cur_edge;
          i++;
          while(i!=_inferiors.end())
            {
              cur_edge=(*i)->cast_to_edge();
              if(cur_edge->is_endpoint1(*tweeney))
                {
                  tweeney=cur_edge->v2();
                  _directions.push_back((signed char)1);
                }
              else
                {
                  tweeney=cur_edge->v1();
                  _directions.push_back((signed char)(-1));
                }
              i++;     
            }
        }
    }
  else if(num_edges==1)
    _directions.push_back((signed char)1);
}

//:
// add an edge

void vtol_one_chain_2d::add_edge(vtol_edge_2d &new_edge,
                                 bool dir)
{
  if(dir)
    _directions.push_back((signed char)1);
  else
    _directions.push_back((signed char)(-1));
  link_inferior(new_edge);
}

//:
// remove an edge

void vtol_one_chain_2d::remove_edge(vtol_edge_2d &doomed_edge,
                                    bool force_it)
{
  // require
  assert(force_it||!is_cycle());

  topology_list_2d::const_iterator i;
  int index;
  vcl_vector<signed char>::iterator j;
  vtol_topology_object_2d_ref t;

  t=&doomed_edge;

  // int index = _inferiors.position(doomed_edge);
  i=vcl_find(_inferiors.begin(),_inferiors.end(),t);
  index=i-_inferiors.begin();
  
  if(index>=0)
    {
      j=_directions.begin();
      j=j+index;
      _directions.erase(j);// get rid of the direction associated with the edge
      touch();
      unlink_inferior(doomed_edge);
    }
}

//:
// comparison operator

bool vtol_one_chain_2d::operator==(const vtol_one_chain_2d &other) const
{
  bool result;

  const topology_list_2d *inf1=inferiors();
  const topology_list_2d *inf2=other.inferiors();

  result=this==&other;

  if(!result)
    {
      // Check to see if the number of vertices is the same
      
      /* this should not be necessary - if the inferiors are the
         same then the vertices should be the same */
      /*
        const vcl_vector<vtol_vertex_2d*>* verts1 = this->vertices();
        const vcl_vector<vtol_vertex_2d*>* verts2 = ch.vertices();
        if(verts1->size() != verts2->size())
        {
        delete verts1;
        delete verts2;
        return false;
        }
        delete verts1;
        delete verts2;
        
      */
      
      result=inf1->size()==inf2->size();
      if(result)
        {
          topology_list_2d::const_iterator i1;
          topology_list_2d::const_iterator i2;
          
          for(i1=inf1->begin() , i2 = inf2->begin(); i1 != inf1->end(); i1++ , i2++){
            if (!( *(*i1) == *(*i2) ))
              return false;
            
            // Comparing the _directions
            const vcl_vector<signed char> *dir1=directions();
            const vcl_vector<signed char> *dir2=other.directions();
            
            if ((dir1->size()!=dir2->size())||(_is_cycle!=other.is_cycle()))
              return false;
        
            vcl_vector<signed char>::const_iterator d1;
            vcl_vector<signed char>::const_iterator d2;
            
            for(d1=dir1->begin(), d2=dir2->begin(); d1 != dir1->end(); d1++, d2++)
              if (!(*d1 == *d2))
            return false;
            
            // compare onechains that make up any holes
            const chain_list_2d &righth=_chain_inferiors;
            const chain_list_2d &lefth=other._chain_inferiors;
            if(righth.size() != lefth.size())
              return false;
            
            chain_list_2d::const_iterator r;
            chain_list_2d::const_iterator l;
            
            for(r=righth.begin(), l=lefth.begin(); r!=righth.end(); r++, l++)
              if( *((vsol_spatial_object_2d*)(*r)) != *((vsol_spatial_object_2d*)((l))))
                return false;
          }
        }
      return true;
        }
  return result;
}


//:
// reverse the direction of the one chain

void vtol_one_chain_2d::reverse_directions(void)
{
  
  // This function reverses the direction{
  // array in the list.
  
 

  for(vcl_vector<signed char>::iterator di=_directions.begin(); 
      di !=_directions.end();di++)
      (*di) = - (*di);

  // reverse the inferiors 

  topology_list_2d inf_tmp(_inferiors.size());

  // vcl_reverse_copy(_inferiors.begin(),_inferiors.end(),inf_tmp.begin());
  // reverse copy does not seem to work do this the hard way
  int i;
  int s= _inferiors.size();
  for(i=0;i<s;i++){
    inf_tmp[i]=_inferiors[s-1-i];
  }
  
  _inferiors.clear();
  vcl_copy(inf_tmp.begin(),inf_tmp.end(),_inferiors.begin());
  

  vcl_vector<signed char> dir_tmp(_directions.size());
  
  // vcl_reverse_copy(_directions.begin(),_directions.end(),dir_tmp.begin());
  s=_directions.size();

  for(i=0;i<s;i++){
    dir_tmp[i]=_directions[s-1-i];
  }
  
  
  _directions.clear();
  vcl_copy(dir_tmp.begin(),dir_tmp.end(),_directions.begin());


  chain_list_2d::iterator hi;

  for (hi=_chain_inferiors.begin();hi!=_chain_inferiors.end();hi++ )
    ((vtol_one_chain_2d *)((*hi)->clone().ptr()))->reverse_directions();
}

#if 0
//:
// comparison operator

bool vtol_one_chain_2d::operator==(const vsol_spatial_object_2d &obj) const
{
 
  if ((obj.spatial_type() == vsol_spatial_object_2d::TOPOLOGYOBJECT) &&
      (((vtol_topology_object_2d&)obj).topology_type() == vtol_topology_object_2d::ONECHAIN))
    return (vtol_one_chain_2d &)*this == (vtol_one_chain_2d&) (vtol_topology_object_2d&) obj;
  else return false;

}
#endif
//:
// Print Methods

void vtol_one_chain_2d::print(ostream &strm) const
{
  strm << "<one_chain_2d " << _inferiors.size() << "  " << (void *) this << ">"  << endl;
}

//:
// Describe the directions

void vtol_one_chain_2d::describe_directions(ostream &strm,
                                            int) const
{
  strm << "<Dirs [" << _directions.size() << "]: ";

  vcl_vector<signed char>::const_iterator d1;
  for (d1=_directions.begin();d1!=_directions.end();d1++)
    strm << (int)(*d1) << "  ";
  strm << endl;
}

//:
// Describe the one chain 

void vtol_one_chain_2d::describe(ostream &strm,
                                 int blanking) const
{
  print(strm);
   describe_inferiors(strm, blanking);
   describe_directions(strm, blanking);
   describe_superiors(strm, blanking);
}

#if 0
//:
// -- For each inferior, this method unlinks the inferior
//    from this object.  If the inferior now has zero superiors,
//    the function is called recursively on it.  Finally, this
//    object is pushed onto the list removed. (RYF 7-16-98)
//


void vtol_one_chain_2d::deep_remove( vcl_vector< vtol_topology_object_2d * > & removed )
{
  // cout << "            Entering vtol_one_chain::DeepDeleteInferiors\n";

  // Make a copy of the object's inferiors
  topology_list_2d * tmp = get_inferiors();

  vcl_vector< vtol_edge_2d * > inferiors;

  topology_list_2d::iterator ti;

  for (ti= tmp->begin(); ti!=tmp->end();ti++ )
      inferiors.push_back( (vtol_edge_2d *)(*ti) );
  
  vcl_vector< vtol_edge_2d * >::iterator ii;

  for (ii= inferiors.begin(); ii!=inferiors.end();ii++ )
  {
      vtol_edge_2d * inferior = (*ii);

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
#endif
