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
// Name: vtol_vertex_2d
// Task: Default constructor
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(void)
{
  _point=new vsol_point_2d(0,0);
}

//---------------------------------------------------------------------------
// Name: vtol_vertex_2d
// Task: Constructor from a point (the point is not copied)
// Require: new_point.ptr()!=0
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(vsol_point_2d &new_point)
{
  // require
  //  assert(new_point.ptr()!=0);

  _point=&new_point;
}

//---------------------------------------------------------------------------
// Name: vtol_vertex_2d
// Task: Constructor from a vector
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(const vnl_double_2 &v)
{
  _point=new vsol_point_2d(v[0],v[1]);
}

//---------------------------------------------------------------------------
// Name: vtol_vertex_2d
// Task: Constructor from abscissa `new_x' and ordinate `new_y' of the point
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(const double new_x,
                               const double new_y)
{
  _point=new vsol_point_2d(new_x,new_y);
}

//---------------------------------------------------------------------------
// Name: vtol_vertex_2d
// Task: Copy constructor. Copy the point but not the links
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(const vtol_vertex_2d &other)
{
  _point=new vsol_point_2d(*other._point);
}

//---------------------------------------------------------------------------
// Name: ~vtol_vertex_2d
// Task: Destructor
//---------------------------------------------------------------------------
vtol_vertex_2d::~vtol_vertex_2d()
{
  unlink_all_inferiors();
}

//---------------------------------------------------------------------------
// Name: clone
// Task: Clone `this': creation of a new object and initialization
//       See Prototype pattern
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
// Name: topology_type
// Task: Return the topology type
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_topology_object_2d_type
vtol_vertex_2d::topology_type(void) const
{
  return VERTEX;
}

//---------------------------------------------------------------------------
// Name: point
// Task: Return the point
//---------------------------------------------------------------------------
vsol_point_2d *vtol_vertex_2d::point(void) const
{
  return _point;
}

//---------------------------------------------------------------------------
// Name: set_point
// Task: Set the point (the point is not copied)
// Require: new_point.ptr()!=0
//---------------------------------------------------------------------------
void vtol_vertex_2d::set_point(vsol_point_2d &new_point)
{
  // require
  //  assert(new_point.ptr()!=0);

  _point=&new_point;
}

//:
// compute_vertices() --
// Returns a list of Vertices which only contains a pointer to itself.
vcl_vector<vtol_vertex_2d*> *vtol_vertex_2d::compute_vertices(void)
{
  LIST_SELF_2d(vtol_vertex_2d);
}

//:
// zero_chain_list* vtol_vertex_2d::ZeroChains() --
// Returns a list of ZeroChains that contain the vertex. This is the vertex superiors list.
vcl_vector<vtol_zero_chain_2d*>* vtol_vertex_2d::compute_zero_chains(void)
{
  SEL_SUP_2d(vtol_zero_chain_2d,compute_zero_chains);
}


//:
// edges() --
// Returns a list of Edges which contain the vertex.
 vcl_vector<vtol_edge_2d*>* vtol_vertex_2d::compute_edges(void)
{
  SEL_SUP_2d(vtol_edge_2d,compute_edges);
}

//:
// one_chain_list* vtol_vertex_2d::OneChains() --
// Returns a list of OneChains which contain the vertex.
vcl_vector<vtol_one_chain_2d*>* vtol_vertex_2d::compute_one_chains(void)
{
  SEL_SUP_2d(vtol_one_chain_2d,compute_one_chains);
}

//:
// face_list_2d* vtol_vertex_2d::Faces() --
// Returns a list of Faces which contain the vertex.
 vcl_vector<vtol_face_2d*>* vtol_vertex_2d::compute_faces(void)
{
  SEL_SUP_2d(vtol_face_2d,compute_faces);
}

//:
// CoolListP<TwoChain*>* vtol_vertex_2d::TwoChains() --
// Returns a list of TwoChains which contain the vertex.
  vcl_vector<vtol_two_chain_2d*>* vtol_vertex_2d::compute_two_chains(void)
{
  SEL_SUP_2d(vtol_two_chain_2d,compute_two_chains);
}

//:
// CoolListP<Block*>* vtol_vertex_2d::Blocks() --
// Returns a list of Blocks which contain the vertex.
vcl_vector<vtol_block_2d*>* vtol_vertex_2d::compute_blocks(void)
{
  
  SEL_SUP_2d(vtol_block_2d,compute_blocks);
}

//---------------------------------------------------------------------------
// Name: x
// Task: Return the abscissa of the point
//---------------------------------------------------------------------------
double vtol_vertex_2d::x(void) const
{
  return _point->x();
}

//---------------------------------------------------------------------------
// Name: y
// Task: Return the ordinate of the point
//---------------------------------------------------------------------------
double vtol_vertex_2d::y(void) const
{
  return _point->y();
}

//---------------------------------------------------------------------------
// Name: set_x
// Task: Set the abscissa of the point with `new_x'
//---------------------------------------------------------------------------
void vtol_vertex_2d::set_x(const double new_x)
{
  _point->set_x(new_x);
  this->touch(); //Timestamp update
}

//---------------------------------------------------------------------------
// Name: set_y
// Task: Set the ordinate of the point with `new_y'
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
// Name: cast_to_vertex
// Task: Return `this' if `this' is a vertex, 0 otherwise
//---------------------------------------------------------------------------
const vtol_vertex_2d *vtol_vertex_2d::cast_to_vertex(void) const
{
  return this;
}

//---------------------------------------------------------------------------
// Name: cast_to_vertex
// Task: Return `this' if `this' is a vertex, 0 otherwise
//---------------------------------------------------------------------------
vtol_vertex_2d *vtol_vertex_2d::cast_to_vertex(void)
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
vtol_vertex_2d::valid_inferior_type(const vtol_topology_object_2d &inferior) const
{
  return false;
}

//---------------------------------------------------------------------------
// Name: valid_superior_type
// Task: Is `superior' type valid for `this' ?
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
//:
// void vtol_vertex_2d::Print(ostream& strm) --
// This method outputs a simple text representation of the vertex including its
// address in memory.
void vtol_vertex_2d::print(ostream &strm) const
{
  strm<<"<vtol_vertex_2d "<<x()<<","<<y()<<","<<(void *)this<<"> with id ";
  strm<<get_id()<<endl;
}


//:
// void vtol_vertex_2d::Describe(ostream& strm, int blanking) --
// This method outputs a detailed description of the vertex including the
// inferiors and superiors.
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
// Name: new_edge
// Task: Create a line edge from `this' and `other' only if this edge does not
//       exist. Otherwise it just returns the existing edge
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

//:
// double vtol_vertex_2d::distance_from(const vector_2d &v)  --
// Returns the squared distance from the vertex and the vector location, v.
double vtol_vertex_2d::distance_from(const vnl_double_2 &v)
{
  //  return _point->distance_from(v);
  cerr << "vtol_vertex_2d::distance_from() not implemented yet\n";
  return -1; // TO DO
}

//:
// double vtol_vertex_2d::EuclideanDistance(vtol_vertex_2d& v)  --
// This method returns the distance, not the squared distance,
// from this vertex and another vertex.
double vtol_vertex_2d::euclidean_distance(vtol_vertex_2d& v)
{
  //  return _point->euclidean_distance(*v.get_point());
  cerr << "vtol_vertex_2d::euclidean_distance() not yet implemented\n";
  return -1; // TO DO
}

//-----------------------------------------------------------------------------
// Name: is_connected
// Task: Is `this' connected with `v2' ?
//       ie has a superior of `this' `v2' as inferior ?
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

/*
// Vertex* vtol_vertex_2d::CalculateAverageNormal(vector_2d& vertnorm) --
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
*/
//:
// vtol_vertex_2d* vtol_vertex_2d::Vertex_diff(Vertex& diffvert) --
// This method returns a new Vertex whose location is the vector difference of the vertex and diffvert.
vtol_vertex_2d *vtol_vertex_2d::vertex_diff(vtol_vertex_2d& diffvert)
{
  // return new vtol_vertex_2d((*_point) - (*(diffvert._point)));
  cerr << "vtol_vertex_2d::vertex_diff() not implemented yet\n";
  return 0; // TO DO
}

//:
// This method returns true if the Edge, edg, is on the superior list of the vertex.
bool vtol_vertex_2d::is_endpointp(const vtol_edge_2d &edg)
{
  bool result;
  const edge_list_2d *edge_list;
  //  vtol_edge_2d_ref e;

  edge_list=edges();
  //  e=&edg

  result=vcl_find(edge_list->begin(),edge_list->end(),&edg)!=edge_list->end();
  
  delete edge_list;
  return result;
}

//---------------------------------------------------------------------------
// Name: =
// Task: Assignment of `this' with `other' (copy the point not the links)
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

#if 0
bool vtol_vertex_2d::operator==(const vsol_spatial_object_2d &obj) const
{
 
  if ((obj.spatial_type() == vsol_spatial_object_2d::TOPOLOGYOBJECT) &&
      (((vtol_topology_object_2d&)obj).topology_type() == vtol_topology_object_2d::VERTEX))
    return (vtol_vertex_2d &)*this == (vtol_vertex_2d&) (vtol_topology_object_2d&) obj;
  else return false;
 
 
}
#endif

//---------------------------------------------------------------------------
// Name: ==
// Task: Is `this' has the same coordinates for its point than `other' ?
//---------------------------------------------------------------------------
bool vtol_vertex_2d::operator== (const vtol_vertex_2d &other) const
{ 
  bool result;

  result=this==&other;
  if(!result)
    result=(*_point)==(*(other._point));
  return result;
}
#if 0
//:
// void subst_vertex_on_edge(Vertex* v1, Vertex* v2, Edge* edge)  --
// This function shouldn't be in here....pav January 93.
// It substitutes vertex, v1 for vertex, v2 on Edge, edge.

void subst_vertex_on_edge(vtol_vertex_2d *v1,
                          vtol_vertex_2d *v2,
                          vtol_edge_2d *edge)
{
  vtol_vertex_2d *V1=edge->v1();
  vtol_vertex_2d *V2=edge->v2();
  if(V1==v2)
    {
      edge->set_v1(v1);
      return;
    }
  if(V2==v2)
    {
      edge->set_v2(v1);
      return;
    }
  cout<<"In vtol_vertex_2d::merge_references(.) Shouldn't happen"<<endl;
}

// vtol_vertex_2d::merge_references(vtol_vertex_2d* v2)  --
// Should not be a method on Vertex...not tested.
void vtol_vertex_2d::merge_references(vtol_vertex_2d *v2)
{
  edge_list_2d *edgelist=v2->edges();
  for (int i=0;i<edgelist->size();i++)
    subst_vertex_on_edge(this,v2,((*edgelist)[i])->cast_to_edge());
}
#endif
/*
 ******************************************************
 *
 *    Functions
 */

//:
// Vertex* min_z_vertex(vertex_list_2d* Verts) --
// not implemented.
# if 0 // use in 3d 
// TO DO
vtol_vertex_2d* min_z_vertex(vertex_list_2d* Verts)
{
  vtol_vertex_2d* min_vertex;
  if (Verts->size() > 0) min_vertex = ((*Verts)[0])->cast_to_vertex_2d(); // Get first element.
  else min_vertex = 0;
  vtol_vertex_2d* tVert;
  double min_z = fabs(min_vertex->z());
  double tz;
  for(int i = 1; i < Verts->size();i++) 
    {
      tVert = Verts[i];
      tz = fabs(tVert->z());
      if (tz < min_z) 
	{
	  min_vertex = tVert;
	  min_z = tz;
	}
    }
  return min_vertex;
}

#endif
#if 0
// vertex_list_2d* merge_vertices_listwise(vertex_list_2d* Verts) --
// not tested....should not be a method.
vertex_list_2d *merge_vertices_listwise(vertex_list_2d *Verts)
{
  vertex_list_2d *newl=new vertex_list_2d();
  vtol_vertex_2d *tVertex;
  vtol_vertex_2d *tVertex2;
  for(int a=0;a<Verts->size();a++) 
    {
      ((*Verts)[a])->unset_tagged_union_flag();
    }
  for(int i=0;i<Verts->size();i++)
    {
      tVertex=(*Verts)[i];
      if(!tVertex->get_tagged_union_flag())
	{
	  for(int k=0;k<Verts->size();k++) 
	    {
	      if(k!=i) 
		{
		  tVertex2=(*Verts)[k];
		  if(tVertex==tVertex2) 
		    {
		      tVertex->merge_references(tVertex2);
		      tVertex2->set_tagged_union_flag();
		    }
		}
	    }
	  newl->push_back(tVertex);
	}
    }
  return newl;
}

//:
// vertex_list_2d* merge_vertices_all(vertex_list_2d* Verts) --
// Should not be a method....not tested.
vertex_list_2d *merge_vertices_all(vertex_list_2d *Verts)
{
  return merge_vertices_listwise(Verts);
}
#endif
//:
//-----------------------------------------------------------------------------
//
// -- Determine which other vertices share edges with this. Add any of these which
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
