#include <vtol/vtol_vertex_2d.h>

#include <vtol/vtol_zero_chain_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_macros_2d.h>
#include <vnl/vnl_math.h>
#include <vtol/vtol_list_functions_2d.h>

#include <vcl/vcl_cassert.h>
#include <vsol/vsol_point_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(void)
{
  _point=new vsol_point_2d(0,0);
}

//---------------------------------------------------------------------------
// -- Constructor from a point (the point is not copied)
// Require: new_point.ptr()!=0
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(vsol_point_2d &new_point)
{
  // require
  //  assert(new_point.ptr()!=0);

  _point=&new_point;
}

//---------------------------------------------------------------------------
// -- Constructor from a vector
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(const vnl_double_2 &v)
{
  _point=new vsol_point_2d(v[0],v[1]);
}

//---------------------------------------------------------------------------
// -- Constructor from abscissa `new_x' and ordinate `new_y' of the point
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(const double new_x,
                               const double new_y)
{
  _point=new vsol_point_2d(new_x,new_y);
}

//---------------------------------------------------------------------------
// -- Copy constructor. Copy the point but not the links
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(const vtol_vertex_2d &other)
{
  _point=new vsol_point_2d(*other._point);
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_vertex_2d::~vtol_vertex_2d()
{
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
// -- Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_ref vtol_vertex_2d::clone(void) const
{
  return new vtol_vertex_2d(*this);
}

/*
 ******************************************************
 *
 *    Accessor Functions
 */

//---------------------------------------------------------------------------
// -- Return the topology type
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_topology_object_2d_type
vtol_vertex_2d::topology_type(void) const
{
  return VERTEX;
}

//---------------------------------------------------------------------------
// -- Return the point
//---------------------------------------------------------------------------
vsol_point_2d_ref vtol_vertex_2d::point(void) const
{
  return _point;
}

//---------------------------------------------------------------------------
// -- Set the point (the point is not copied)
// Require: new_point.ptr()!=0
//---------------------------------------------------------------------------
void vtol_vertex_2d::set_point(vsol_point_2d &new_point)
{
  // require
  //  assert(new_point.ptr()!=0);

  _point=&new_point;
}

// -- Returns a list of Vertices which only contains a pointer to itself.
vcl_vector<vtol_vertex_2d*> *vtol_vertex_2d::compute_vertices(void)
{
  LIST_SELF_2d(vtol_vertex_2d);
}

// -- Returns a list of ZeroChains that contain the vertex. This is the vertex superiors list.
vcl_vector<vtol_zero_chain_2d*>* vtol_vertex_2d::compute_zero_chains(void)
{
  SEL_SUP_2d(vtol_zero_chain_2d,compute_zero_chains);
}


// -- Returns a list of Edges which contain the vertex.
 vcl_vector<vtol_edge_2d*>* vtol_vertex_2d::compute_edges(void)
{
  SEL_SUP_2d(vtol_edge_2d,compute_edges);
}

// -- Returns a list of OneChains which contain the vertex.
vcl_vector<vtol_one_chain_2d*>* vtol_vertex_2d::compute_one_chains(void)
{
  SEL_SUP_2d(vtol_one_chain_2d,compute_one_chains);
}

// -- Returns a list of Faces which contain the vertex.
 vcl_vector<vtol_face_2d*>* vtol_vertex_2d::compute_faces(void)
{
  SEL_SUP_2d(vtol_face_2d,compute_faces);
}

// -- Returns a list of TwoChains which contain the vertex.
  vcl_vector<vtol_two_chain_2d*>* vtol_vertex_2d::compute_two_chains(void)
{
  SEL_SUP_2d(vtol_two_chain_2d,compute_two_chains);
}

// -- Returns a list of Blocks which contain the vertex.
vcl_vector<vtol_block_2d*>* vtol_vertex_2d::compute_blocks(void)
{
  
  SEL_SUP_2d(vtol_block_2d,compute_blocks);
}

//---------------------------------------------------------------------------
// -- Return the abscissa of the point
//---------------------------------------------------------------------------
double vtol_vertex_2d::x(void) const
{
  return _point->x();
}

//---------------------------------------------------------------------------
// -- Return the ordinate of the point
//---------------------------------------------------------------------------
double vtol_vertex_2d::y(void) const
{
  return _point->y();
}

//---------------------------------------------------------------------------
// -- Set the abscissa of the point with `new_x'
//---------------------------------------------------------------------------
void vtol_vertex_2d::set_x(const double new_x)
{
  _point->set_x(new_x);
  this->touch(); //Timestamp update
}

//---------------------------------------------------------------------------
// -- Set the ordinate of the point with `new_y'
//---------------------------------------------------------------------------
void vtol_vertex_2d::set_y(const double new_y)
{
  _point->set_y(new_y);
  this->touch();
}

//***************************************************************************
// Replaces dynamic_cast<T>
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a vertex, 0 otherwise
//---------------------------------------------------------------------------
const vtol_vertex_2d *vtol_vertex_2d::cast_to_vertex(void) const
{
  return this;
}

//---------------------------------------------------------------------------
// -- Return `this' if `this' is a vertex, 0 otherwise
//---------------------------------------------------------------------------
vtol_vertex_2d *vtol_vertex_2d::cast_to_vertex(void)
{
  return this;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// -- Is `inferior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_vertex_2d::valid_inferior_type(const vtol_topology_object_2d &inferior) const
{
  return false;
}

//---------------------------------------------------------------------------
// -- Is `superior' type valid for `this' ?
//---------------------------------------------------------------------------
bool
vtol_vertex_2d::valid_superior_type(const vtol_topology_object_2d &superior) const
{
  return superior.cast_to_zero_chain()!=0;
}

/*
 ******************************************************
 *
 *    Print Functions
 */
// -- This method outputs a simple text representation of the vertex including its address in memory.
void vtol_vertex_2d::print(ostream &strm) const
{
  strm<<"<vtol_vertex_2d "<<x()<<","<<y()<<","<<(void *)this<<"> with id ";
  strm<<get_id()<<endl;
}


// -- This method outputs a detailed description of the vertex including the inferiors and superiors.
void vtol_vertex_2d::describe(ostream &strm,
                              int blanking) const
{
  for (int i=0; i<blanking; ++i) strm << ' ';
  print(strm);
  describe_inferiors(strm, blanking);
  describe_superiors(strm, blanking);
}


/*
 ******************************************************
 *
 *    Implementor Functions
 */

//-----------------------------------------------------------------------------
// -- Create a line edge from `this' and `other' only if this edge does not
//    exist. Otherwise it just returns the existing edge
// Require: other.ptr()!=0 and other.ptr()!=this
//-----------------------------------------------------------------------------
vtol_edge_2d *vtol_vertex_2d::new_edge(vtol_vertex_2d &other)
{
  // require
  //  assert(other.ptr()!=0);
  assert(&other!=this);

  // awf: load vrml speed up by factor of 2 using this loop.
  vtol_edge_2d *result;
  bool found;

  vcl_list<vtol_topology_object_2d_ref>::const_iterator zp;
  const vcl_vector<vtol_topology_object_2d_ref> *sups;
  vcl_vector<vtol_topology_object_2d_ref>::const_iterator ep;
  vtol_edge_2d *e;

  // Scan Zero Chains
  found=false;
  for(zp=_superiors.begin();zp!=_superiors.end()&&!found;++zp)
    {
      // Scan superiors of ZChain (i.e. edges)
      // topology_list_2d *sups=(*zp)->get_superiors();
      sups=(*zp)->superiors();
      for(ep=sups->begin();ep!=sups->end()&&!found;++ep)
        {
          e=(*ep)->cast_to_edge();
          if(e->v1()==&other||e->v2()==&other)
            {
              found=true;
              result=e;
            }
        }
      delete sups;
    }
  if(!found)
    result=new vtol_edge_2d(*this,other);
  return result;
}

// -- Returns the squared distance from the vertex and the vector location, v.
double vtol_vertex_2d::distance_from(const vnl_double_2 &v)
{
  //  return _point->distance_from(v);
  cerr << "vtol_vertex_2d::distance_from() not implemented yet\n";
  return -1; // TO DO
}

// -- This method returns the distance, not the squared distance, from this vertex and another vertex.
double vtol_vertex_2d::euclidean_distance(vtol_vertex_2d& v)
{
  //  return _point->euclidean_distance(*v.get_point());
  cerr << "vtol_vertex_2d::euclidean_distance() not yet implemented\n";
  return -1; // TO DO
}

//-----------------------------------------------------------------------------
// -- Is `this' connected with `v2' ?
//    ie has a superior of `this' `v2' as inferior ?
//-----------------------------------------------------------------------------
bool vtol_vertex_2d::is_connected(const vtol_vertex_2d &v2)
{
  edge_list_2d *vertedges=edges();
  edge_list_2d::const_iterator i;
  for(i=vertedges->begin();i!=vertedges->end();++i)
    if((*i)->is_endpoint(v2))
      {
        delete vertedges;
        return true;
      }
  delete vertedges;
  return false;
}

#if 0 // TODO
// This method calculates the vertex normal from averaging normals of connected faces.
void vtol_vertex_2d::calculate_average_normal(IUE_vector<double> & vertnorm)
{
  face_list_2d faces;
  vtol_topology_object_2d::faces(faces);

  vector_2d fnormsum((double)0.0, (double)0.0);

for (face_list_2d::iterator i = faces.begin(); 
	       i != faces.end(); ++i)
    {
      vector_2d *cvf= (*i)->get_normal();
      fnormsum += *cvf;
    }

  vertnorm = fnormsum / faces.size();
}
#endif

// -- This method returns a new Vertex whose location is the vector difference of the vertex and diffvert.
vtol_vertex_2d *vtol_vertex_2d::vertex_diff(vtol_vertex_2d& diffvert)
{
  // return new vtol_vertex_2d((*_point) - (*(diffvert._point)));
  cerr << "vtol_vertex_2d::vertex_diff() not implemented yet\n";
  return 0; // TO DO
}

// -- This method returns true if the Edge, edg, is on the superior list of the vertex.
bool vtol_vertex_2d::is_endpointp(const vtol_edge_2d &edg)
{
  vtol_edge_2d_ref e=(vtol_edge_2d*)(&edg);
  const edge_list_2d *edge_list=edges();
  bool result=vcl_find(edge_list->begin(),edge_list->end(),e)!=edge_list->end();
  delete edge_list;
  return result;
}

//---------------------------------------------------------------------------
// -- Assignment of `this' with `other' (copy the point not the links)
//---------------------------------------------------------------------------
vtol_vertex_2d &vtol_vertex_2d::operator=(const vtol_vertex_2d &other)
{
  if(this!=&other)
    {
      _point->set_x(other._point->x());
      _point->set_y(other._point->y()); 
      touch();
    }
  return *this;
}

// -- spatial object equality

bool vtol_vertex_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::TOPOLOGYOBJECT &&
   ((vtol_topology_object_2d const&)obj).topology_type() == vtol_topology_object_2d::VERTEX
  ? *this == (vtol_vertex_2d const&) (vtol_topology_object_2d const&) obj
  : false;
}


//---------------------------------------------------------------------------
// -- Is `this' has the same coordinates for its point than `other' ?
//---------------------------------------------------------------------------
bool vtol_vertex_2d::operator== (const vtol_vertex_2d &other) const
{ 
  bool result;

  result=this==&other;
  if(!result)
    result=(*_point)==(*(other._point));
  return result;
}

/*
 ******************************************************
 *
 *    Functions
 */

//
//-----------------------------------------------------------------------------
// --
// Determine which other vertices share edges with this. Add any of these which
// are not in the list to it, and recursively call ExploreVertex on them. The
// method is intended to recover all of the vertices in a single topological
// structure which is composed of connected edges.
//
void vtol_vertex_2d::explore_vertex(vertex_list_2d &verts)
{
  edge_list_2d *edges_;
  edge_list_2d::iterator i;
  vtol_vertex_2d_ref vv;
  vtol_edge_2d_ref e;

  edges_=edges();
  for(i=edges_->begin();i!=edges_->end();++i)
    {
      e=*i;
      if(e->v1()==this)
        vv=e->v1();
      else if(e->v2()==this)
        vv=e->v2();
      else
        {
          cerr << "Explorevtol_vertex_2d: shouldn't get this\n";
          continue;
        }
      
      if(vcl_find(verts.begin(),verts.end(),vv)!=verts.end())
        {
          verts.push_back(vv);
          vv->explore_vertex(verts);
        }
    }
  delete edges_;
}

#include <vcl/vcl_rel_ops.h> // gcc 2.7
VCL_INSTANTIATE_INLINE(bool operator!=(vtol_vertex_2d const &, vtol_vertex_2d const &));
