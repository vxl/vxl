// This is gel/vsol/vsol_line_2d.cxx
#include "vsol_line_2d.h"
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_point_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor
//---------------------------------------------------------------------------
vsol_line_2d::vsol_line_2d()
: p0_(new vsol_point_2d),
  p1_(new vsol_point_2d)
{
}

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
//: Constructor from the direction and the middle point
//---------------------------------------------------------------------------
vsol_line_2d::vsol_line_2d(const vgl_vector_2d<double> &new_direction,
                           const vgl_point_2d<double> &new_middle)
: p0_(new vsol_point_2d(*(vsol_point_2d(new_middle).plus_vector(-(new_direction)/2)))),
  p1_(new vsol_point_2d(*(vsol_point_2d(new_middle).plus_vector((new_direction)/2))))
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
//: Constructor
//---------------------------------------------------------------------------
vsol_line_2d::vsol_line_2d(const vgl_line_segment_2d<double> &l)
{
  p0_ = new vsol_point_2d(l.point1());
  p1_ = new vsol_point_2d(l.point2());
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
// Destructor
//---------------------------------------------------------------------------
vsol_line_2d::~vsol_line_2d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
//  See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vsol_line_2d::clone(void) const
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
  return this==&other
         || (*p0_ == *(other.p0_) && *p1_ == *(other.p1_))
         || (*p0_ == *(other.p1_) && *p1_ == *(other.p0_));
}

//: spatial object equality

bool vsol_line_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
    obj.spatial_type() == vsol_spatial_object_2d::CURVE &&
    ((vsol_curve_2d const&)obj).curve_type() == vsol_curve_2d::LINE
  ? operator==(static_cast<vsol_line_2d const&>(static_cast<vsol_curve_2d const&>(obj)))
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
void vsol_line_2d::compute_bounding_box(void) const
{
  set_bounding_box(   p0_->x(), p0_->y());
  add_to_bounding_box(p1_->x(), p1_->y());
}

//---------------------------------------------------------------------------
//: Return the length of `this'
//---------------------------------------------------------------------------
double vsol_line_2d::length(void) const
{
  return p0_->distance(p1_);
}

//---------------------------------------------------------------------------
//: Return the tangent angle in degrees  of `this'.
//  By convention, the angle is in degrees and lies in the interval [0, 360].
//---------------------------------------------------------------------------
double vsol_line_2d::tangent_angle(void) const
{
  static const double deg_per_rad = 180.0/vnl_math::pi;
  double dy = p1_->y()-p0_->y();
  double dx = p1_->x()-p0_->x();

  double ang;
  // do special cases separately, to avoid rounding errors:
  if      (dx == 0)  ang = dy<0 ? 270.0 : 90.0; // vertical line
  else if (dy == 0)  ang = dx<0 ? 180.0 :  0.0; // horizontal line
  else if (dy == dx) ang = dy<0 ? 225.0 : 45.0;
  else if (dy+dx==0) ang = dy<0 ? 315.0 :135.0;
  // the general case:
  else               ang = deg_per_rad * vcl_atan2(dy,dx);
  if (ang<0) ang+= 360.0;
  return ang;
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
  // `p' belongs to the straight line
  bool result=(p0_->y()-p1_->y())*p->x()+(p1_->x()-p0_->x())*p->y()
              +p0_->x()*p1_->y()         -p0_->y()*p1_->x()        ==0;

  if (result) // `p' belongs to the segment
    {
      double dot_product=(p->x()-p0_->x())*(p1_->x()-p0_->x())
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

//--------------------------------------------------------------------
//: compute an infinite homogeneous line corresponding to *this
//--------------------------------------------------------------------
vgl_homg_line_2d<double> vsol_line_2d::vgl_hline_2d() const
{
  vgl_homg_point_2d<double> vp0(p0_->x(), p0_->y());
  vgl_homg_point_2d<double> vp1(p1_->x(), p1_->y());
  vgl_homg_line_2d<double> l(vp0, vp1);
  return l;
}

//--------------------------------------------------------------------
//: compute a vgl line segment corresponding to *this
//--------------------------------------------------------------------
vgl_line_segment_2d<double> vsol_line_2d::vgl_seg_2d() const
{
  vgl_homg_point_2d<double> vp0(p0_->x(), p0_->y());
  vgl_homg_point_2d<double> vp1(p1_->x(), p1_->y());
  vgl_line_segment_2d<double> l(vp0, vp1);
  return l;
}


//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_line_2d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsol_spatial_object_2d::b_write(os);
  vsl_b_write(os, p0_);
  vsl_b_write(os, p1_);
}

//: Binary load self from stream. (not typically used)
void vsol_line_2d::b_read(vsl_b_istream &is)
{
  if(!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    {
      vsol_spatial_object_2d::b_read(is);
      vsl_b_read(is, p0_);
      vsl_b_read(is, p1_);
    }
  }
}
//: Return IO version number;
short vsol_line_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_line_2d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

  //: Return a platform independent string identifying the class
vcl_string vsol_line_2d::is_a() const
{
  return vcl_string("vsol_line_2d");
}

  //: Return true if the argument matches the string identifying the class or any parent class
bool vsol_line_2d::is_class(const vcl_string& cls) const
{
  return cls==vsol_line_2d::is_a();
}

//external functions
vcl_ostream& operator<<(vcl_ostream& s, vsol_line_2d const& l)
{
  s << '[' << *(l.p0()) << ' ' << *(l.p1()) << ']';
  return s;
}



//: Binary save vsol_line_2d to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_line_2d* p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}


//: Binary load vsol_line_2d from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_line_2d* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new vsol_line_2d();
    p->b_read(is);
  }
  else
    p = 0;
}
