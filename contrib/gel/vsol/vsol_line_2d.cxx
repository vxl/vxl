// This is gel/vsol/vsol_line_2d.cxx
#include <vsol/vsol_line_2d.h>
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vnl/vnl_math.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_point_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from the direction and the middle point
//---------------------------------------------------------------------------
vsol_line_2d::vsol_line_2d(const vgl_vector_2d<double> &new_direction,
                           const vsol_point_2d_sptr &new_middle)
: p0_(new vsol_point_2d(*(new_middle->plus_vector(-(new_direction)/2)))),
  p1_(new vsol_point_2d(*(new_middle->plus_vector((new_direction)/2))))
{
}

//---------------------------------------------------------------------------
//: Constructor from two vgl_point_2d (end points)
//---------------------------------------------------------------------------
vsol_line_2d::vsol_line_2d(vgl_point_2d<double> const& p0,
                           vgl_point_2d<double> const& p1)
: p0_(new vsol_point_2d(p0)),
  p1_(new vsol_point_2d(p1))
{
}

//---------------------------------------------------------------------------
//: Constructor
//---------------------------------------------------------------------------
vsol_line_2d::vsol_line_2d(const vsol_point_2d_sptr &new_p0,
                           const vsol_point_2d_sptr &new_p1)
: p0_(new_p0), p1_(new_p1)
{
}

//---------------------------------------------------------------------------
//: Copy constructor
// Description: no duplication of the points
//---------------------------------------------------------------------------
vsol_line_2d::vsol_line_2d(const vsol_line_2d &other)
: p0_(other.p0_), p1_(other.p1_)
{
}

//---------------------------------------------------------------------------
//: Destructor
//---------------------------------------------------------------------------
vsol_line_2d::~vsol_line_2d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
//       See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_sptr vsol_line_2d::clone(void) const
{
  return new vsol_line_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Middle point of the straight line segment
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_line_2d::middle(void) const
{
  return p0_->middle(*p1_);
}

//---------------------------------------------------------------------------
//: direction of the straight line segment.
//---------------------------------------------------------------------------
vgl_vector_2d<double> vsol_line_2d::direction(void) const
{
  return p0_->to_vector(*p1_);
}

//---------------------------------------------------------------------------
//: First point of the straight line segment
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_line_2d::p0(void) const
{
  return p0_;
}

//---------------------------------------------------------------------------
//: Last point of the straight line segment
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_line_2d::p1(void) const
{
  return p1_;
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' ?
//---------------------------------------------------------------------------
bool vsol_line_2d::operator==(const vsol_line_2d &other) const
{
  bool result;

  result=this==&other;
  if (!result)
    result=(((*p0_)==(*(other.p0_)))&&((*p1_)==(*(other.p1_))))
      ||(((*p0_)==(*(other.p1_)))&&((*p1_)==(*(other.p0_))));
  return result;
}

//: spatial object equality

bool vsol_line_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::CURVE &&
   ((vsol_curve_2d const&)obj).curve_type() == vsol_curve_2d::LINE
  ? *this == (vsol_line_2d const&) (vsol_curve_2d const&) obj
  : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the real type of a line. It is a CURVE
//---------------------------------------------------------------------------
vsol_spatial_object_2d::vsol_spatial_object_2d_type
vsol_line_2d::spatial_type(void) const
{
  return CURVE;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_line_2d::compute_bounding_box(void)
{
  double xmin=p0_->x();
  double ymin=p0_->y();
  double xmax=xmin;
  double ymax=ymin;

  if      (p1_->x()<xmin) xmin=p1_->x();
  else if (p1_->x()>xmax) xmax=p1_->x();
  if      (p1_->y()<ymin) ymin=p1_->y();
  else if (p1_->y()>ymax) ymax=p1_->y();

  if (bounding_box_==0)
    bounding_box_=new vsol_box_2d;
  bounding_box_->set_min_x(xmin);
  bounding_box_->set_max_x(xmax);
  bounding_box_->set_min_y(ymin);
  bounding_box_->set_max_y(ymax);
}

//---------------------------------------------------------------------------
//: Return the length of `this'
//---------------------------------------------------------------------------
double vsol_line_2d::length(void) const
{
  return p0_->distance(p1_);
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first point of the straight line segment
//---------------------------------------------------------------------------
void vsol_line_2d::set_p0(const vsol_point_2d_sptr &new_p0)
{
  p0_=new_p0;
}

//---------------------------------------------------------------------------
//: Set the last point of the straight line segment
//---------------------------------------------------------------------------
void vsol_line_2d::set_p1(const vsol_point_2d_sptr &new_p1)
{
  p1_=new_p1;
}

//---------------------------------------------------------------------------
//: Set the length of `this'. Doesn't change middle point and orientation.
//    If p0 and p1 are equal then the direction is set to (1,0)
// Require: new_length>=0
//---------------------------------------------------------------------------
void vsol_line_2d::set_length(const double new_length)
{
  // require
  assert(new_length>=0);

  vsol_point_2d_sptr m=middle();
  vgl_vector_2d<double> d =
    (*p0_)==(*p1_) ? vgl_vector_2d<double>(1.0,0.0)
                   : normalized(direction());
  d *= new_length;

  p0_=new vsol_point_2d(*(m->plus_vector(-d/2)));
  p1_=new vsol_point_2d(*(m->plus_vector(d/2)));
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `p' in `this' ?
//---------------------------------------------------------------------------
bool vsol_line_2d::in(const vsol_point_2d_sptr &p) const
{
  bool result;
  double dot_product;

  // `p' belongs to the straight line
  result=(p0_->y()-p1_->y())*p->x()+(p1_->x()-p0_->x())*p->y()
    +p0_->x()*p1_->y()-p0_->y()*p1_->x()==0;

  if (result) // `p' belongs to the segment
    {
      dot_product=(p->x()-p0_->x())*(p1_->x()-p0_->x())
        +(p->y()-p0_->y())*(p1_->y()-p0_->y());
      result=(dot_product>=0)&&
        (dot_product<(vnl_math_sqr(p1_->x()-p0_->x())
                      +vnl_math_sqr(p1_->y()-p0_->y())));
    }
  return result;
}

//---------------------------------------------------------------------------
//: Return the tangent to `this' at `p'.  Has to be deleted manually
// Require: in(p)
//---------------------------------------------------------------------------
vgl_homg_line_2d<double> *
vsol_line_2d::tangent_at_point(const vsol_point_2d_sptr &p) const
{
  // require
  assert(in(p));

  return new vgl_homg_line_2d<double>(p0_->y()-p1_->y(),p1_->x()-p0_->x(),
                                      p0_->x()*p1_->y()-p0_->y()*p1_->x());
}
