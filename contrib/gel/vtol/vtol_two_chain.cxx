// 05/13/98  RIH replaced append by insert_after to avoid n^2 behavior

//:
//  \file

#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vtol/vtol_list_functions.h>
#include <vtol/vtol_two_chain.h>
#include <vtol/vtol_face.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_block.h>
#include <vtol/vtol_macros.h>
#include <vcl_cassert.h>


//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default constructor
//---------------------------------------------------------------------------
vtol_two_chain::vtol_two_chain(void)
{
  _is_cycle=false;
}

//---------------------------------------------------------------------------
//: Constructor
//---------------------------------------------------------------------------
vtol_two_chain::vtol_two_chain(int num_faces)
{
  _is_cycle=false;
}

//---------------------------------------------------------------------------
//: Constructor
//---------------------------------------------------------------------------
vtol_two_chain::vtol_two_chain(face_list &faces,
                               bool new_is_cycle)
{
  face_list::iterator i;
  for (i=faces.begin(); i!=faces.end();++i)
    {
      link_inferior(*(*i));
      // all face normals point outward.
      _directions.push_back((signed char)1);
    }
  _is_cycle=new_is_cycle;
}

//---------------------------------------------------------------------------
//: Constructor
//---------------------------------------------------------------------------
vtol_two_chain::vtol_two_chain(face_list &faces,
                               vcl_vector<signed char> &dirs,
                               bool new_is_cycle)
{
  vcl_vector<signed char>::iterator di;
  face_list::iterator fi;

  for(di=dirs.begin(),fi=faces.begin();
      fi!=faces.end()&&di!=dirs.end();
      ++fi,++di)
    {
      if((*di)<0)
        (*fi)->reverse_normal();
      link_inferior(*(*fi));
      _directions.push_back(*di);
    }
  _is_cycle=new_is_cycle;
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vtol_two_chain::vtol_two_chain(vtol_two_chain const &other)
{
  vtol_two_chain *fl;
  edge_list *edges;
  vertex_list *verts;
  int vlen;
  int elen;
  int i;
  vertex_list::iterator vi;
  int j;
  edge_list::iterator ei;
  vcl_vector<signed char>::iterator ddi;
  topology_list::const_iterator tti;
  vtol_face *f;
  const chain_list *hierarchy_infs;
  chain_list::const_iterator hhi;

  fl=(vtol_two_chain *)(&other); // const violation

  edges=fl->edges();
  verts=fl->vertices();
  vlen=verts->size();
  elen=edges->size();

  topology_list newedges(elen);
  topology_list newverts(vlen);

  i=0;

  // make a copy of the vertices

  for(vi=verts->begin();vi!=verts->end();++vi,++i)
    {
      vtol_vertex_sptr v = *vi;
      newverts[i] = v->clone()->cast_to_topology_object();
      v->set_id(i);
    }

  // make a copy of the edges

  j=0;
  for(ei=edges->begin();ei!= edges->end();++ei,++j)
    {
      vtol_edge_sptr e = *ei;

      newedges[j]= newverts[e->v1()->get_id()]->cast_to_vertex()->new_edge(*(newverts[e->v2()->get_id()]->cast_to_vertex()));

      e->set_id(j);
    }

  vcl_vector<signed char> &dirs=fl->_directions;
  vcl_vector<vtol_topology_object_sptr> &infs=fl->_inferiors;

  for(ddi=dirs.begin(),tti= infs.begin();
      tti!=infs.end()&&ddi!=dirs.end();
      ++ddi,++tti)
    {
      f=(*tti)->cast_to_face();

      vtol_face_sptr new_f = f->copy_with_arrays(newverts,newedges);
      vcl_cout << "f" << vcl_endl;
      f->describe();
      vcl_cout << "new f" << vcl_endl;
      new_f->describe();

      assert(*new_f == *f);

      link_inferior(*new_f);
      _directions.push_back((*ddi));
    }

  set_cycle(fl->is_cycle());
  hierarchy_infs=fl->chain_inferiors();

  for(hhi=hierarchy_infs->begin();hhi!=hierarchy_infs->end();++hhi)
    link_chain_inferior(*(((vtol_two_chain *)(hhi->ptr()))->copy_with_arrays(newverts,newedges)));
  delete edges;
  delete verts;
}

vtol_two_chain *
vtol_two_chain::copy_with_arrays(vcl_vector<vtol_topology_object_sptr> &newverts,
                                 vcl_vector<vtol_topology_object_sptr> &newedges) const
{
  vtol_two_chain *result;
  vcl_vector<vtol_topology_object_sptr>::const_iterator ti;
  vcl_vector<signed char>::const_iterator di;
  vtol_face *f;
  const chain_list *hierarchy_infs;
  chain_list::const_iterator hi;

  const vcl_vector<signed char> &dirs=_directions;
  //  topology_list& infs = _inferiors;
  const vcl_vector<vtol_topology_object_sptr> &infs=_inferiors;

  result=new vtol_two_chain(infs.size());

  //topology_list::iterator ti;
  for(di=dirs.begin(),ti=infs.begin();
      ti!=infs.end()&&di!=dirs.end();
      ++ti,++di)
    {
      f=(*ti)->cast_to_face();
      result->link_inferior(*(f->copy_with_arrays(newverts,newedges)));
      result->_directions.push_back((*di));
    }

  result->set_cycle(is_cycle());
  hierarchy_infs=chain_inferiors();

  for(hi=hierarchy_infs->begin();hi!=hierarchy_infs->end();++hi)
    result->link_chain_inferior(*(((vtol_two_chain *)(hi->ptr()))->copy_with_arrays(newverts,newedges)));
  return result;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_two_chain::~vtol_two_chain()
{
  clear();
  unlink_all_chain_inferiors();
  //  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_sptr vtol_two_chain::clone(void) const
{
  return new vtol_two_chain(*this);
}

//---------------------------------------------------------------------------
//: Shallow copy with no links
//---------------------------------------------------------------------------
vtol_topology_object *
vtol_two_chain::shallow_copy_with_no_links(void) const
{
  vtol_two_chain *result;
  result=new vtol_two_chain();
  result->_is_cycle=_is_cycle;
  vcl_vector<signed char>::const_iterator di;

  for(di=_directions.begin();di!=_directions.end();++di)
    result->_directions.push_back((*di));
  return result;
}

//---------------------------------------------------------------------------
//: Return the topology type
//---------------------------------------------------------------------------
vtol_two_chain::vtol_topology_object_type
vtol_two_chain::topology_type(void) const
{
  return TWOCHAIN;
}

//***************************************************************************
//   Editing Functions
//***************************************************************************

//: add the superiors from the parent

void vtol_two_chain::add_superiors_from_parent(topology_list &sups)
{
  vcl_vector<vtol_topology_object_sptr>::iterator si;
  chain_list::iterator hi;

  for(si=sups.begin();si!=sups.end();++si)
    (*si)->link_inferior(*this);
  //  if(vcl_find(_superiors.begin(),_superiors.end(),(*si))==_superiors.end())
  //    _superiors.push_back((*si));

  for(hi=_chain_inferiors.begin();hi!=_chain_inferiors.end();++hi)
    ((vtol_two_chain *)(hi->ptr()))->add_superiors_from_parent(sups);
}

void vtol_two_chain::remove_superiors_of_parent(topology_list &sups)
{
  vcl_vector<vtol_topology_object_sptr>::iterator si;
  chain_list::iterator hi;

  for(si=sups.begin();si!=sups.end();++si)
    (*si)->unlink_inferior(*this);

  for(hi=_chain_inferiors.begin();hi!=_chain_inferiors.end();++hi)
    ((vtol_two_chain *)(hi->ptr()))->remove_superiors_of_parent(sups);
}

void vtol_two_chain::remove_superiors(void)
{
  _superiors.clear();
  chain_list::iterator hi;
  for(hi=_chain_inferiors.begin();hi!=_chain_inferiors.end();++hi)
    ((vtol_two_chain *)(hi->ptr()))->remove_superiors();
}

void vtol_two_chain::update_superior_list_p_from_hierarchy_parent(void)
{
  const vtol_two_chain *hierarchy_parent=0;
  const topology_list *parent_superiors;
  topology_list::const_iterator ti;

  // Check to see if there is a parent node in the tree.

  if(_chain_superiors.size()>0)
    hierarchy_parent=(vtol_two_chain *)((*(_chain_superiors.begin())).ptr());


  // If vtol_two_chain is a child of another vtol_two_chain...the superiors
  // lists are updated.

  if(hierarchy_parent!=0)
    {
      parent_superiors=hierarchy_parent->superiors();

      // Clear all previous superiors.
      _superiors.clear();
      for(ti=parent_superiors->begin();ti!= parent_superiors->end();++ti)
        if(vcl_find(_superiors.begin(),_superiors.end(),*ti)==_superiors.end())
          _superiors.push_back((*ti));

      // Recursively update all children.
      vcl_vector<vtol_two_chain_sptr> *chains=inferior_two_chains();

      vcl_vector<vtol_two_chain_sptr>::iterator ci;
      for(ci=chains->begin();ci!=chains->end();++ci)
        ((*ci))->update_superior_list_p_from_hierarchy_parent();
      delete chains;
    }
}

bool
vtol_two_chain::break_into_connected_components( vcl_vector<vtol_topology_object*> & components )
{
  vcl_cerr << "vtol_two_chain::break_into_connected_components() not implemented yet\n";
  return false; // TO DO
}

#if 0 // TODO
bool
vtol_two_chain::break_into_connected_components( vcl_vector<vtol_topology_object*> & components )
{
  topology_list * tmp = get_inferiors();
  int numfaces = tmp->size();
  if ( numfaces == 0 ) return false;
  vcl_vector< vtol_face * > faces( numfaces );
  int i;
  for ( i = 0,topology_list::iterator ti= tmp->begin(); ti!=tmp->end();++ti, ++i )
      faces[ i ] = (vtol_face *) (*ti);

  // compnum[i] stores the component number of the ith face
  int * compnum = new int[ numfaces ];
  assert( compnum );

  // adjslists stores the adjacency lists representation of the face graph
  vcl_vector< int > * adjlists = new vcl_vector< int > [ numfaces ];
  assert( adjlists );

  // build the adjacency list representation
  int j;
  for ( i = 0; i < numfaces; ++i )
  {
    compnum[ i ] = -1;  // -1 signals it is not part of any component yet
    for ( j = i+1; j < numfaces; ++j )
      if ( faces[ i ]->shares_edge_with( faces[ j ] ) )
      {
        adjlists[i].push_back( j );
        adjlists[j].push_back( i );
      }
  }

  // use depth first search to find connected components
  int numcomps = 0;
  for ( i = 0; i < numfaces; ++i )
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
      ++numcomps;
    }
  }

  if ( numcomps > 1 )
    for ( i = 0; i < numcomps; ++i )
    {
      vcl_vector< vtol_face * > component_faces;
      // gather faces belonging to the ith component
      for ( j = 0; j < numfaces; ++j )
        if ( compnum[ j ] == i )
          component_faces.push_back( faces[ j ] );
      // create either a two chain or a face out of the component
      if ( component_faces.size() == 1 )
        components.push_back( component_faces.get( 0 ) );
      else
      {
        vtol_two_chain * newvtol_two_chain = new vtol_two_chain(component_faces, false);
        // need to check if it is a cycle??
        components.push_back( newvtol_two_chain );
      }
      component_faces.clear(); // necessary??
    }

  delete []compnum;
  delete []adjlists;

  if ( numcomps == 1 ) return false;
  else return true;
}

#endif // 0

void vtol_two_chain::add_face(vtol_face &new_face,
                              signed char dir)
{
  _directions.push_back(dir);
  link_inferior(new_face);
}

void vtol_two_chain::remove_face(vtol_face &doomed_face)
{
  topology_list::const_iterator i;
  vtol_topology_object_sptr t;
  t=&doomed_face;

  i=vcl_find(_inferiors.begin(),_inferiors.end(),t);
  int index=i-_inferiors.begin();
  if(index>=0)
    {
      vcl_vector<signed char>::iterator j=_directions.begin();
      j=j+index;

      _directions.erase(j);
      touch();
      unlink_inferior(doomed_face);
    }
  //  return false;
}

void vtol_two_chain::add_block(vtol_block &new_block)
{
  new_block.link_inferior(*this);
}

void vtol_two_chain::remove_block(vtol_block &doomed_block)
{
  doomed_block.unlink_inferior(*this);
}

//***************************************************************************
// Replaces dynamic_cast<T>
//***************************************************************************

//---------------------------------------------------------------------------
//: Return `this' if `this' is a two_chain, 0 otherwise
//---------------------------------------------------------------------------
const vtol_two_chain *
vtol_two_chain::cast_to_two_chain(void) const
{
  return this;
}

//---------------------------------------------------------------------------
//: Return `this' if `this' is a two_chain, 0 otherwise
//---------------------------------------------------------------------------
vtol_two_chain *vtol_two_chain::cast_to_two_chain(void)
{
  return this;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `inferior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_two_chain::valid_inferior_type(vtol_topology_object const &inferior) const
{
  return inferior.cast_to_face()!=0;
}

//---------------------------------------------------------------------------
//: Is `superior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_two_chain::valid_superior_type(vtol_topology_object const &superior) const
{
  return superior.cast_to_block()!=0;
}

//---------------------------------------------------------------------------
//: Is `chain_inf_sup' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_two_chain::valid_chain_type(vtol_chain const& chain_inf_sup) const
{
  return chain_inf_sup.cast_to_two_chain()!=0;
}

//***************************************************************************
//    Accessor Functions
//***************************************************************************

//: outside boundary vertices

vertex_list *vtol_two_chain::outside_boundary_vertices(void)
{
  vertex_list *new_ref_list = new vertex_list();
  vcl_vector<vtol_vertex*>* ptr_list = this->outside_boundary_compute_vertices();
  // copy the lists

  for(vcl_vector<vtol_vertex*>::iterator ti = ptr_list->begin();
      ti != ptr_list->end(); ++ti)
    new_ref_list->push_back(*ti);

  delete ptr_list;

  return new_ref_list;
}

vcl_vector<vtol_vertex *> *
vtol_two_chain::outside_boundary_compute_vertices(void)
{
  SEL_INF(vtol_vertex,compute_vertices);
}

//: list of vertices

vcl_vector<vtol_vertex *> *vtol_two_chain::compute_vertices(void)
{
  vcl_vector<vtol_vertex *> *verts;
  verts=outside_boundary_compute_vertices();

  // Set current position to the end
  // verts->set_position(verts->size()-1); - not sure what is supposed to happen here

  SUBCHAIN_INF(verts,vtol_two_chain,vtol_vertex,compute_vertices);
}

//: outside boundary zero chains

zero_chain_list *vtol_two_chain::outside_boundary_zero_chains(void)
{
  zero_chain_list *new_ref_list = new zero_chain_list();
  vcl_vector<vtol_zero_chain*>* ptr_list = this->outside_boundary_compute_zero_chains();
  // copy the lists

  for(vcl_vector<vtol_zero_chain*>::iterator ti = ptr_list->begin();
      ti != ptr_list->end(); ++ti)
    new_ref_list->push_back(*ti);

  delete ptr_list;

  return new_ref_list;
}


vcl_vector<vtol_zero_chain*> *vtol_two_chain::outside_boundary_compute_zero_chains(void)
{
  SEL_INF(vtol_zero_chain,compute_zero_chains);
}

//: list of zero chains
vcl_vector<vtol_zero_chain*> *vtol_two_chain::compute_zero_chains(void)
{
  vcl_vector<vtol_zero_chain*> *zchs;
  zchs=outside_boundary_compute_zero_chains();

  SUBCHAIN_INF(zchs,vtol_two_chain,vtol_zero_chain,compute_zero_chains);
}

//: outside boundary edges
edge_list *vtol_two_chain::outside_boundary_edges(void)
{
  edge_list *new_ref_list = new edge_list();
  vcl_vector<vtol_edge*>* ptr_list = this->outside_boundary_compute_edges();
  // copy the lists

  for(vcl_vector<vtol_edge*>::iterator ti = ptr_list->begin();
      ti != ptr_list->end(); ++ti)
    new_ref_list->push_back(*ti);

  delete ptr_list;

  return new_ref_list;
}

//: outside boundary edges
vcl_vector<vtol_edge*> *vtol_two_chain::outside_boundary_compute_edges(void)
{
  SEL_INF(vtol_edge,compute_edges);
}

//: list of edges
vcl_vector<vtol_edge*> *vtol_two_chain::compute_edges(void)
{
  vcl_vector<vtol_edge*> *edgs;
  edgs=outside_boundary_compute_edges();

  SUBCHAIN_INF(edgs, vtol_two_chain, vtol_edge, compute_edges);
}

//: outside one chains
one_chain_list *vtol_two_chain::outside_boundary_one_chains(void)
{
  vcl_vector<vtol_one_chain*>* ptr_list= outside_boundary_compute_one_chains();
  one_chain_list *ref_list= new one_chain_list();

  vcl_vector<vtol_one_chain*>::iterator i;
  for(i=ptr_list->begin();i!=ptr_list->end();++i)
    ref_list->push_back(*i);

  delete ptr_list;
  return ref_list;
}

vcl_vector<vtol_one_chain*> *vtol_two_chain::outside_boundary_compute_one_chains(void)
{
 SEL_INF(vtol_one_chain,compute_one_chains);
}
//: one chains
vcl_vector<vtol_one_chain*> *vtol_two_chain::compute_one_chains(void)
{
  vcl_vector<vtol_one_chain*> *onechs;
  onechs=outside_boundary_compute_one_chains();
  SUBCHAIN_INF(onechs, vtol_two_chain, vtol_one_chain, compute_one_chains);
}

//: outside faces
face_list *vtol_two_chain::outside_boundary_faces(void)
{
  vcl_vector<vtol_face*>* ptr_list= outside_boundary_compute_faces();
  face_list *ref_list= new face_list();

  vcl_vector<vtol_face*>::iterator i;
  for(i=ptr_list->begin();i!=ptr_list->end();++i)
    ref_list->push_back(*i);

  delete ptr_list;
  return ref_list;
}

// outside faces
vcl_vector<vtol_face*> *vtol_two_chain::outside_boundary_compute_faces(void)
{
 COPY_INF(vtol_face);
}

//: faces
vcl_vector<vtol_face*> *vtol_two_chain::compute_faces(void)
{
  vcl_vector<vtol_face*> *facs;
  facs=outside_boundary_compute_faces();
  SUBCHAIN_INF(facs, vtol_two_chain, vtol_face, compute_faces);
}

//: list of blocks
vcl_vector<vtol_block*> *vtol_two_chain::compute_blocks(void)
{
  vcl_vector<vtol_block*>*result;
  vcl_vector<vtol_block*> *sublist;
  two_chain_list *onech;
  two_chain_list::iterator tci;
  vcl_vector<vtol_block*>::iterator si;

  if(is_sub_chain())
    {
      result=new vcl_vector<vtol_block*>;
      onech=superior_two_chains();
      for(tci=onech->begin();tci!=onech->end();++tci)
        {
          sublist=(*tci)->compute_blocks();
          if(sublist->size())
            // new_list->insert(new_list->end(),sublist->begin(),sublist->end());
            for(si=sublist->begin();si!=sublist->end();++si)
              result->push_back(*si);
          delete sublist;
        }
      tagged_union((vcl_vector<vsol_spatial_object_3d*> *)result); // from vsol_spatial_object_3dListExtras.h
      return result;
    }
  else
    {
     SEL_SUP(vtol_block,compute_blocks);
    }
}

//: list of two chains

vcl_vector<vtol_two_chain*> *vtol_two_chain::compute_two_chains(void)
{
  vcl_vector<vtol_two_chain*> *result;
  chain_list::iterator hi;

  result=outside_boundary_compute_two_chains();

  for(hi=_chain_inferiors.begin();hi!=_chain_inferiors.end();++hi )
    result->push_back((vtol_two_chain *)((hi->ptr())));

  return result;
}

two_chain_list *vtol_two_chain::inferior_two_chains(void)
{
  two_chain_list *result;
  chain_list::iterator hi;

  result=new vcl_vector<vtol_two_chain_sptr>();
  for(hi=_chain_inferiors.begin();hi!=_chain_inferiors.end();++hi)
    result->push_back((vtol_two_chain *)(hi->ptr()));
  return result;
}


two_chain_list *vtol_two_chain::superior_two_chains(void)
{
  two_chain_list *result;
  vcl_list<vtol_chain_sptr>::iterator hi;

  result=new vcl_vector<vtol_two_chain_sptr>();
  for(hi=_chain_superiors.begin();hi!=_chain_superiors.end();++hi)
    result->push_back((vtol_two_chain *)((*hi).ptr()));
  return result;
}

two_chain_list *vtol_two_chain::outside_boundary_two_chains(void)
{
  vcl_vector<vtol_two_chain*>* ptr_list= outside_boundary_compute_two_chains();
  two_chain_list *ref_list= new two_chain_list();

  vcl_vector<vtol_two_chain*>::iterator i;
  for(i=ptr_list->begin();i!=ptr_list->end();++i)
    ref_list->push_back(*i);

  delete ptr_list;
  return ref_list;
}

vcl_vector<vtol_two_chain*>  *vtol_two_chain::outside_boundary_compute_two_chains(void)
{
  LIST_SELF(vtol_two_chain);
}

//***************************************************************************
//     Operator Functions
//***************************************************************************

//: equality operator

bool vtol_two_chain::operator==(vtol_two_chain const& other) const
{
  if(this==&other){
    return true;
  }

  if(_inferiors.size()!=other._inferiors.size())
    return false;

  topology_list::const_iterator ti1,ti2;
  for(ti1=other._inferiors.begin(),ti2=_inferiors.begin();
      ti2!=_inferiors.end() && ti1!=other._inferiors.end();
      ++ti1,++ti2)
    {
      vtol_face *f1=(*ti2)->cast_to_face();
      vtol_face *f2=(*ti1)->cast_to_face();
      if(!(*f1==*f2))
        return false;
    }

  // check out the directions

  vcl_vector<signed char>::const_iterator d1;
  vcl_vector<signed char>::const_iterator d2;

  const vcl_vector<signed char> *dir1=this->directions();
  const vcl_vector<signed char> *dir2=other.directions();

  for(d1=dir1->begin(), d2=dir2->begin(); d1 != dir1->end(); ++d1, ++d2)
    if (!(*d1 == *d2))
      return false;

  const chain_list &righth=_chain_inferiors;
  const chain_list &lefth=other._chain_inferiors;
  if(righth.size()!=lefth.size())
    return false;

  chain_list::const_iterator hi1,hi2;

  for(hi1=righth.begin(),hi2=lefth.begin();
      hi1!=righth.end()&&hi2!=lefth.end();
      ++hi1,++hi2)

    if( !(*(*hi1) == *(*hi2)))
      return false;

  return true;
}

//: spatial object equality

bool vtol_two_chain::operator==(vsol_spatial_object_3d const& obj) const
{
  return obj.spatial_type() == vsol_spatial_object_3d::TOPOLOGYOBJECT &&
   ((vtol_topology_object const&)obj).topology_type() == vtol_topology_object::TWOCHAIN
  ? *this == (vtol_two_chain const&) (vtol_topology_object const&) obj
  : false;
}

//***************************************************************************
//    Utility Functions
//***************************************************************************

//: correct the chain directions
void vtol_two_chain::correct_chain_directions(void)
{
  vcl_cerr << "vtol_two_chain::correct_chain_directions() not yet implemented\n";
}

//***************************************************************************
//    Print Functions
//***************************************************************************

void vtol_two_chain::print(vcl_ostream &strm) const
{
  strm << "<vtol_two_chain " << _inferiors.size() << "  "  << (void const *)this << ">"  << vcl_endl;
}

void vtol_two_chain::describe_directions(vcl_ostream &strm,
                                         int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  strm << "<Dirs [" << _directions.size() << "]: ";

  vcl_vector<signed char>::const_iterator di;
  for (di=_directions.begin();di!=_directions.end();++di)
    strm << (*di) << "  ";
  strm << vcl_endl;
}

void vtol_two_chain::describe(vcl_ostream &strm,
                              int blanking) const
{
  for (int j=0; j<blanking; ++j) strm << ' ';
  print(strm);
  describe_inferiors(strm,blanking);
  describe_directions(strm,blanking);
  describe_superiors(strm,blanking);
}

signed char vtol_two_chain::direction(vtol_face const& f) const
{
  // return the direction of the face

  vcl_vector<signed char>::const_iterator dit;
  topology_list::const_iterator toit;

  dit=_directions.begin();
  for(toit=_inferiors.begin();toit!=_inferiors.end();++toit)
    {
      vtol_face *cf=(*toit)->cast_to_face();
      if(cf==&f)
        return *dit;
      ++dit;
    }
  return (signed char)1;
}
