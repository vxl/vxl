// This is gel/vsol/vsol_line_3d.cxx
#include "vsol_line_3d.h"
//:
// \file

#include <vcl_cassert.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsol/vsol_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_homg_point_3d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from the direction and the middle point
//---------------------------------------------------------------------------
vsol_line_3d::vsol_line_3d(vgl_vector_3d<double> const& new_direction,
                           vsol_point_3d_sptr const& new_middle)
: p0_(new vsol_point_3d(*(new_middle->plus_vector(-new_direction/2)))),
  p1_(new vsol_point_3d(*(new_middle->plus_vector(new_direction/2))))
{
}

//---------------------------------------------------------------------------
//: Constructor from two vsol_point_3d (end points)
//---------------------------------------------------------------------------
vsol_line_3d::vsol_line_3d(vsol_point_3d_sptr const& new_p0,
                           vsol_point_3d_sptr const& new_p1)
: p0_(new_p0), p1_(new_p1)
{
}

//---------------------------------------------------------------------------
//: Copy constructor
// Description: no duplication of the points
//---------------------------------------------------------------------------
vsol_line_3d::vsol_line_3d(vsol_line_3d const& other)
: vsol_curve_3d(other), p0_(other.p0_), p1_(other.p1_)
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
vsol_spatial_object_3d* vsol_line_3d::clone(void) const
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
bool vsol_line_3d::operator==(vsol_line_3d const& other) const
{
  bool result;

  result=this==&other;
  if (!result)
    result=(((*p0_)==(*(other.p0_)))&&((*p1_)==(*(other.p1_))))
      ||(((*p0_)==(*(other.p1_)))&&((*p1_)==(*(other.p0_))));
  return result;
}

//: spatial object equality

bool vsol_line_3d::operator==(vsol_spatial_object_3d const& obj) const
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
void vsol_line_3d::set_p0(vsol_point_3d_sptr const& new_p0)
{
  p0_=new_p0;
}

//---------------------------------------------------------------------------
//: Set the last point of the straight line segment
//---------------------------------------------------------------------------
void vsol_line_3d::set_p1(vsol_point_3d_sptr const& new_p1)
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
bool vsol_line_3d::in(vsol_point_3d_sptr const& p) const
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
vsol_line_3d::tangent_at_point(vsol_point_3d_sptr const& p) const
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

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_line_3d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsol_spatial_object_3d::b_write(os);
  vsl_b_write(os, p0_);
  vsl_b_write(os, p1_);
}

//: Binary load self from stream. (not typically used)
void vsol_line_3d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsol_spatial_object_3d::b_read(is);
    vsl_b_read(is, p0_);
    vsl_b_read(is, p1_);
    break;
   default:
    vcl_cerr << "vsol_line_3d: unknown I/O version " << ver << '\n';
  }
}
//: Return IO version number;
short vsol_line_3d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_line_3d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//: Binary save vsol_line_3d to stream.
void
vsl_b_write(vsl_b_ostream &os, vsol_line_3d const* p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}

//: Binary load vsol_line_3d from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_line_3d* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new vsol_line_3d(new vsol_point_3d(0.0,0.0,0.0),new vsol_point_3d(0.0,0.0,0.0));
    p->b_read(is);
  }
  else
    p = 0;
}
