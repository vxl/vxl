//:
//  \file

#include "vtol_vertex_2d.h"
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge.h>
#include <vcl_cassert.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(void)
{
  point_=new vsol_point_2d(0,0);
}

//---------------------------------------------------------------------------
//: Constructor from a point (the point is not copied)
// Require: new_point.ptr()!=0
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(vsol_point_2d &new_point)
{
  // Must allocate here, since this pointer will be unref()ed by destructor
  point_=new vsol_point_2d(new_point);
}

//---------------------------------------------------------------------------
//: Constructor from a vector
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(const vnl_double_2 &v)
{
  point_=new vsol_point_2d(v[0],v[1]);
}

//---------------------------------------------------------------------------
//: Constructor from abscissa `new_x' and ordinate `new_y' of the point
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(const double new_x,
                               const double new_y)
{
  point_=new vsol_point_2d(new_x,new_y);
}

//---------------------------------------------------------------------------
//: Copy constructor. Copy the point but not the links
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(const vtol_vertex_2d &other)
{
  point_=new vsol_point_2d(*other.point_);
}

//---------------------------------------------------------------------------
//: make a copy of the geometry
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vtol_vertex_2d::~vtol_vertex_2d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_sptr vtol_vertex_2d::clone(void) const
{
  return new vtol_vertex_2d(*this);
}

/*
 ******************************************************
 *
 *    Accessor Functions
 */

//---------------------------------------------------------------------------
//: Return the point
//---------------------------------------------------------------------------
vsol_point_2d_sptr vtol_vertex_2d::point(void) const
{
  return point_;
}

//---------------------------------------------------------------------------
//: Set the point (the point is not copied)
// Require: new_point.ptr()!=0
//---------------------------------------------------------------------------
void vtol_vertex_2d::set_point(vsol_point_2d_sptr const& new_point)
{
  point_=new_point;
}

//---------------------------------------------------------------------------
//: Return the abscissa of the point
//---------------------------------------------------------------------------
double vtol_vertex_2d::x(void) const
{
  return point_->x();
}

//---------------------------------------------------------------------------
//: Return the ordinate of the point
//---------------------------------------------------------------------------
double vtol_vertex_2d::y(void) const
{
  return point_->y();
}

//---------------------------------------------------------------------------
//: Set the abscissa of the point with `new_x'
//---------------------------------------------------------------------------
void vtol_vertex_2d::set_x(const double new_x)
{
  vsol_point_2d new_point(*point_);
  new_point.set_x(new_x);
  this->touch(); //Timestamp update
  // Must allocate here, since this pointer will be unref()ed by destructor
  point_=new vsol_point_2d(new_point);
}

//---------------------------------------------------------------------------
//: Set the ordinate of the point with `new_y'
//---------------------------------------------------------------------------
void vtol_vertex_2d::set_y(const double new_y)
{
  vsol_point_2d new_point(*point_);
  new_point.set_y(new_y);
  this->touch(); //Timestamp update
  // Must allocate here, since this pointer will be unref()ed by destructor
  point_=new vsol_point_2d(new_point);
}

//*****************************************************
//
//    Print Functions

//: This method outputs a simple text representation of the vertex including its address in memory.
void vtol_vertex_2d::print(vcl_ostream &strm) const
{
  strm<<"<vtol_vertex_2d "<<x()<<","<<y()<<","<<(void const *)this<<"> with id ";
  strm<<get_id()<<vcl_endl;
}


//: This method outputs a detailed description of the vertex including the inferiors and superiors.
void vtol_vertex_2d::describe(vcl_ostream &strm,
                              int blanking) const
{
  for (int i=0; i<blanking; ++i)
    strm << ' ';
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
//: Create a line edge from `this' and `other' only if this edge does not exist.
//  Otherwise it just returns the existing edge.
// Require: other.ptr()!=0 and other.ptr()!=this
//-----------------------------------------------------------------------------
vtol_edge *vtol_vertex_2d::new_edge(vtol_vertex &other)
{
  vtol_vertex_2d *other2d = other.cast_to_vertex_2d();
  assert(other2d!=0);

  // require
  //  assert(other.ptr()!=0);
  assert(&other != this);

  // awf: load vrml speed up by factor of 2 using this loop.
  vtol_edge *result = 0;
  bool found;

  vcl_list<vtol_topology_object_sptr>::const_iterator zp;
  const vcl_vector<vtol_topology_object_sptr> *sups;
  vcl_vector<vtol_topology_object_sptr>::const_iterator ep;
  vtol_edge *e;

  // Scan Zero Chains
  found=false;
  for (zp=_superiors.begin();zp!=_superiors.end()&&!found;++zp)
    {
      // Scan superiors of ZChain (i.e. edges)
      // topology_list *sups=(*zp)->get_superiors();
      sups=(*zp)->superiors();
      for (ep=sups->begin();ep!=sups->end()&&!found;++ep)
        {
          e=(*ep)->cast_to_edge();
          if (e->v1()==&other||e->v2()==&other)
            {
              found=true;
              result=e;
            }
        }
      delete sups;
    }
  if (!found)
    result= (vtol_edge*)(new vtol_edge_2d(*this,*other2d));

  return result;
}

//: Returns the squared distance from the vertex and the vector location, v.
double vtol_vertex_2d::distance_from(const vnl_double_2 &v)
{
  vsol_point_2d point(v(0),v(1));
  return point_->distance(point);
}

//: This method returns the distance, not the squared distance, from this vertex and another vertex.
double vtol_vertex_2d::euclidean_distance(vtol_vertex_2d& v)
{
  return point_->distance(*v.point());
}


//---------------------------------------------------------------------------
//: Assignment of `this' with `other' (copy the point not the links)
//---------------------------------------------------------------------------
vtol_vertex_2d &vtol_vertex_2d::operator=(const vtol_vertex_2d &other)
{
  if (this!=&other)
    {
      this->touch(); //Timestamp update
      // Must allocate here, since this pointer will be unref()ed by destructor
      point_=new vsol_point_2d(*(other.point_));
    }
  return *this;
}

vtol_vertex& vtol_vertex_2d::operator=(const vtol_vertex &other)
{
  if (this!=&other)
    {
      this->touch(); //Timestamp update
      // Must allocate here, since this pointer will be unref()ed by destructor
      point_=new vsol_point_2d(*(other.cast_to_vertex_2d()->point_));
    }
  return *this;
}

//---------------------------------------------------------------------------
//: spatial object equality
//---------------------------------------------------------------------------

bool vtol_vertex_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::TOPOLOGYOBJECT &&
   ((vtol_topology_object const&)obj).topology_type() == vtol_topology_object::VERTEX
  ? *this == (vtol_vertex_2d const&) (vtol_topology_object const&) obj
  : false;
}

//---------------------------------------------------------------------------
//: Does `this' have the same coordinates for its point than `other' ?
//---------------------------------------------------------------------------
bool vtol_vertex_2d::operator== (const vtol_vertex &other) const
{
  return other.cast_to_vertex_2d() && other.cast_to_vertex_2d()->operator==(*this);
}

//---------------------------------------------------------------------------
//: Does `this' have the same coordinates for its point than `other' ?
//---------------------------------------------------------------------------
bool vtol_vertex_2d::operator== (const vtol_vertex_2d &other) const
{
  bool result = this==&other;
  if (!result)
    result=(*point_)==(*(other.point_));
  return result;
}

/*
 ******************************************************
 *
 *    Functions
 */

//: copy the geometry

void vtol_vertex_2d::copy_geometry(const vtol_vertex &other)
{
  if (other.cast_to_vertex_2d()){
    point_ = new vsol_point_2d(*(other.cast_to_vertex_2d()->point()));
  }
}

//---------------------------------------------------------------------------
//: compare the geometry
//---------------------------------------------------------------------------

bool vtol_vertex_2d::compare_geometry(const vtol_vertex &other) const
{
  return other.cast_to_vertex_2d() && (*point_)==(*(other.cast_to_vertex_2d()->point()));
}

void vtol_vertex_2d::compute_bounding_box(void)
{
    if(!this->bounding_box_)
    {
      vcl_cout << "In void vtol_vertex::compute_bounding_box() - shouldn't happen"
               << vcl_endl;
      return;
    }
    bounding_box_->set_min_x(this->x());
    bounding_box_->set_min_y(this->y());
    bounding_box_->set_max_x(this->x());
    bounding_box_->set_max_y(this->y());
}
