// This is gel/vtol/vtol_vertex_2d.cxx
#include "vtol_vertex_2d.h"
//:
// \file

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
//: Constructor from a point (the point is copied)
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
//: Pseudo copy constructor.  Deep copy.
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(vtol_vertex_2d_sptr const& other)
: point_(new vsol_point_2d(*other->point_))
{
}

#if 0 // deprecated
//---------------------------------------------------------------------------
//: Copy constructor. Copy the point but not the links.  Deprecated.
//---------------------------------------------------------------------------
vtol_vertex_2d::vtol_vertex_2d(const vtol_vertex_2d &other)
{
  vcl_cerr << "vtol_vertex_2d copy constructor is deprecated; use vtol_vertex_2d_sptr constructor instead\n";
  point_=new vsol_point_2d(*other.point_);
}
#endif

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
vsol_spatial_object_2d* vtol_vertex_2d::clone(void) const
{
  return new vtol_vertex_2d(vtol_vertex_2d_sptr(const_cast<vtol_vertex_2d*>(this)));
}

//: Return a platform independent string identifying the class
vcl_string vtol_vertex_2d::is_a() const
{
  return vcl_string("vtol_vertex_2d");
}

//*****************************************************
//
//    Accessor Functions

//---------------------------------------------------------------------------
//: Return the point
//---------------------------------------------------------------------------
vsol_point_2d_sptr vtol_vertex_2d::point(void) const
{
  return point_;
}

//---------------------------------------------------------------------------
//: Set the point (the point is not copied)
// Require: new_point!=0
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
  // Must allocate here, since this pointer will be unref()ed by destructor
#if 0
  point_=new vsol_point_2d(new_x,point_->y());
#endif
  point_->set_x(new_x);
  this->touch(); //Timestamp update
}

//---------------------------------------------------------------------------
//: Set the ordinate of the point with `new_y'
//---------------------------------------------------------------------------
void vtol_vertex_2d::set_y(const double new_y)
{
  // Must allocate here, since this pointer will be unref()ed by destructor
#if 0
  point_=new vsol_point_2d(point_->x(),new_y);
#endif
  point_->set_y(new_y);
  this->touch(); //Timestamp update
}

//*****************************************************
//
//    Print Functions

//: This method outputs a simple text representation of the vertex including its address in memory.
void vtol_vertex_2d::print(vcl_ostream &strm) const
{
  strm<<"<vtol_vertex_2d "<<x()<<','<<y()<<','<<(void const *)this<<"> with id "
      <<get_id()<<vcl_endl;
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


//*****************************************************
//
//    Implementor Functions

vtol_edge_sptr vtol_vertex_2d::new_edge(vtol_vertex_2d_sptr const& v)
{
  return new_edge(v->cast_to_vertex());
}

//-----------------------------------------------------------------------------
//: Create a line edge from `this' and `other' only if this edge does not exist.
//  Otherwise it just returns the existing edge.
// Require: other!=*this
//-----------------------------------------------------------------------------
vtol_edge_sptr vtol_vertex_2d::new_edge(vtol_vertex_sptr const& other)
{
  // require
  assert(other != this);

  vtol_vertex_2d_sptr other2d = other->cast_to_vertex_2d();
  assert(other2d);

  // awf: load vrml speed up by factor of 2 using this loop.
  vtol_edge_sptr result = 0;

  // Scan Zero Chains
  bool found = false;
  vcl_list<vtol_topology_object*>::const_iterator zp;
  for (zp=superiors_.begin();zp!=superiors_.end()&&!found;++zp)
    {
      // Scan superiors of ZChain (i.e. edges)
      const vcl_list<vtol_topology_object*> *sups=(*zp)->superiors_list();
      vcl_list<vtol_topology_object*>::const_iterator ep;
      for (ep=sups->begin();ep!=sups->end()&&!found;++ep)
        {
          vtol_edge_sptr e=(*ep)->cast_to_edge();
          if (e->v1()==other||e->v2()==other)
            { result=e; found = true; }
        }
    }
  if (!result)
    result= new vtol_edge_2d(this,other2d);

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
   obj.cast_to_topology_object() &&
   obj.cast_to_topology_object()->cast_to_vertex() &&
   *this == *obj.cast_to_topology_object()->cast_to_vertex();
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
  return this==&other || *point_==*(other.point_);
}

//*****************************************************
//
//    Functions

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

void vtol_vertex_2d::compute_bounding_box(void) const
{
  set_bounding_box(this->x(), this->y());
}
