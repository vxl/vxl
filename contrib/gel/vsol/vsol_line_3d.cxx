// This is gel/vsol/vsol_line_3d.cxx
#include "vsol_line_3d.h"
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vsol/vsol_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from the direction and the middle point
//---------------------------------------------------------------------------
vsol_line_3d::vsol_line_3d(const vgl_vector_3d<double> &new_direction,
                           const vsol_point_3d_sptr &new_middle)
: p0_(new vsol_point_3d(*(new_middle->plus_vector(-new_direction/2)))),
  p1_(new vsol_point_3d(*(new_middle->plus_vector(new_direction/2))))
{
}

//---------------------------------------------------------------------------
//: Constructor from two vgl_point_3d (end points)
//---------------------------------------------------------------------------
vsol_line_3d::vsol_line_3d(const vsol_point_3d_sptr &new_p0,
                           const vsol_point_3d_sptr &new_p1)
: p0_(new_p0), p1_(new_p1)
{
}

//---------------------------------------------------------------------------
//: Copy constructor
// Description: no duplication of the points
//---------------------------------------------------------------------------
vsol_line_3d::vsol_line_3d(const vsol_line_3d &other)
: p0_(other.p0_), p1_(other.p1_)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_line_3d::~vsol_line_3d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
//  See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_sptr vsol_line_3d::clone(void) const
{
  return new vsol_line_3d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Middle point of the straight line segment
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_line_3d::middle(void) const
{
  return p0_->middle(*p1_);
}

//---------------------------------------------------------------------------
//: direction of the straight line segment.
//---------------------------------------------------------------------------
vgl_vector_3d<double> vsol_line_3d::direction(void) const
{
  return p0_->to_vector(*p1_);
}

//---------------------------------------------------------------------------
//: First point of the straight line segment
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_line_3d::p0(void) const
{
  return p0_;
}

//---------------------------------------------------------------------------
//: Last point of the straight line segment
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_line_3d::p1(void) const
{
  return p1_;
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' ?
//---------------------------------------------------------------------------
bool vsol_line_3d::operator==(const vsol_line_3d &other) const
{
  bool result;

  result=this==&other;
  if (!result)
    result=(((*p0_)==(*(other.p0_)))&&((*p1_)==(*(other.p1_))))
      ||(((*p0_)==(*(other.p1_)))&&((*p1_)==(*(other.p0_))));
  return result;
}

//: spatial object equality

bool vsol_line_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::CURVE &&
   ((vsol_curve_3d const&)obj).curve_type() == vsol_curve_3d::LINE
  ? operator== ((vsol_line_3d const&)(vsol_curve_3d const&)obj)
  : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the real type of a line. It is a CURVE
//---------------------------------------------------------------------------
vsol_spatial_object_3d::vsol_spatial_object_3d_type
vsol_line_3d::spatial_type(void) const
{
  return CURVE;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_line_3d::compute_bounding_box(void) const
{
  set_bounding_box(   p0_->x(),p0_->y(),p0_->z());
  add_to_bounding_box(p1_->x(),p1_->y(),p1_->z());
}

//---------------------------------------------------------------------------
//: Return the length of `this'
//---------------------------------------------------------------------------
double vsol_line_3d::length(void) const
{
  return p0_->distance(p1_);
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first point of the straight line segment
//---------------------------------------------------------------------------
void vsol_line_3d::set_p0(const vsol_point_3d_sptr &new_p0)
{
  p0_=new_p0;
}

//---------------------------------------------------------------------------
//: Set the last point of the straight line segment
//---------------------------------------------------------------------------
void vsol_line_3d::set_p1(const vsol_point_3d_sptr &new_p1)
{
  p1_=new_p1;
}

//---------------------------------------------------------------------------
//: Set the length of `this'. Doesn't change middle point and orientation.
//    If p0 and p1 are equal then the direction is set to (1,0,0)
// Require: new_length>=0
//---------------------------------------------------------------------------
void vsol_line_3d::set_length(const double new_length)
{
  // require
  assert(new_length>=0);

  vsol_point_3d_sptr m=middle();
  vgl_vector_3d<double> d =
    (*p0_)==(*p1_) ? vgl_vector_3d<double>(1.0,0.0,0.0)
                   : normalized(direction());
  d *= new_length;

  p0_=new vsol_point_3d(*(m->plus_vector(-d/2)));
  p1_=new vsol_point_3d(*(m->plus_vector(d/2)));
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `p' in `this' ?
//---------------------------------------------------------------------------
bool vsol_line_3d::in(const vsol_point_3d_sptr &p) const
{
  // `p' belongs to the straight line
  double ax=p1_->x()-p0_->x();
  double ay=p1_->y()-p0_->y();
  double az=p1_->z()-p0_->z();
  double bx=p->x()-p0_->x();
  double by=p->y()-p0_->y();
  double bz=p->z()-p0_->z();

  bool result = ay*bz==az*by && az*bx==ax*bz && ax*by==ay*bx;
  if (result) // `p' belongs to the segment
  {
    double dot_product=bx*ax+by*ay+bz*az;
    result= dot_product>=0 && dot_product<ax*ax+ay*ay+az*az;
  }
  return result;
}

//---------------------------------------------------------------------------
//: Return the tangent to `this' at `p'.  Has to be deleted manually
// Require: in(p)
//---------------------------------------------------------------------------
vgl_homg_line_3d_2_points<double> *
vsol_line_3d::tangent_at_point(const vsol_point_3d_sptr &p) const
{
  assert(false); // TODO
  // require
  assert(in(p));

  vgl_homg_line_3d_2_points<double> *result;
  vgl_homg_point_3d<double> a(p0_->x(),p0_->y(),p0_->z());
  vgl_homg_point_3d<double> b(p1_->x(),p1_->y(),p1_->z());

  result=new vgl_homg_line_3d_2_points<double>(a,b);

  return result;
}

void vsol_line_3d::describe(vcl_ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << '[' << *(p0()) << ' ' << *(p1()) << ']' << vcl_endl;
}
