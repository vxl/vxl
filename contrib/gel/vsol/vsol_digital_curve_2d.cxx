// This is gel/vsol/vsol_digital_curve_2d.cxx
#include "vsol_digital_curve_2d.h"
//:
// \file

#include <vsol/vsol_point_2d.h>
#include <vcl_iostream.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_distance.h>
#include <vsl/vsl_vector_io.h>
#include <vcl_cmath.h>

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor
//---------------------------------------------------------------------------
vsol_digital_curve_2d::vsol_digital_curve_2d()
 : vsol_curve_2d(), samples_()
{
}

//---------------------------------------------------------------------------
//: Constructor from a vcl_vector of points
//---------------------------------------------------------------------------

vsol_digital_curve_2d::vsol_digital_curve_2d(const vcl_vector<vsol_point_2d_sptr> &samples)
 : vsol_curve_2d(), samples_(samples)
{
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_digital_curve_2d::vsol_digital_curve_2d(const vsol_digital_curve_2d &other)
  : vsol_curve_2d(other), samples_()
{
  for ( vcl_vector<vsol_point_2d_sptr>::const_iterator itr=other.samples_.begin();
        itr != other.samples_.end();  ++itr )
    this->samples_.push_back(new vsol_point_2d(**itr));
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_digital_curve_2d::~vsol_digital_curve_2d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vsol_digital_curve_2d::clone(void) const
{
  return new vsol_digital_curve_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_digital_curve_2d::p0(void) const
{
  if ( samples_.empty() )
    return NULL;

  return samples_.front();
}

//---------------------------------------------------------------------------
//: Return the last point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_digital_curve_2d::p1(void) const
{
  if ( samples_.empty() )
    return NULL;

  return samples_.back();
}

//---------------------------------------------------------------------------
//: Return point `i'
//  REQUIRE: valid_index(i)
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_digital_curve_2d::point(const int i) const
{
  // require
  assert(valid_index(i));

  return samples_[i];
}

//---------------------------------------------------------------------------
//: Interpolate a point on the curve given a floating point index
//  Linear interpolation is used for now
//---------------------------------------------------------------------------
vgl_point_2d<double>
vsol_digital_curve_2d::interp(double index) const
{
  assert(index >= 0.0);
  assert(index <= double(samples_.size()-1));

  int i1 = (int)vcl_floor(index);
  if ( vcl_floor(index) == index )
    return samples_[i1]->get_p();

  int i2 = (int)vcl_ceil(index);
  double f = index - vcl_floor(index);

  vgl_point_2d<double> p1 = samples_[i1]->get_p();
  vgl_point_2d<double> p2 = samples_[i2]->get_p();
  return p1 + f*(p2-p1);
}


//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_digital_curve_2d::operator==(const vsol_digital_curve_2d &other) const
{
  if (this==&other)
    return true;
  //check endpoint equality since that is cheaper then checking each vertex
  //and if it fails we are done
  bool epts_eq = vsol_curve_2d::endpoints_equal(other);
  if (!epts_eq)
    return false;
  //Do the polylines have the same number of vertices?
  if (samples_.size()!=other.samples_.size())
    return false;
  //The easy tests are done.  Now compare each vertex
  int n = samples_.size();
  for (int i=0; i<n; i++)
    if (*(samples_[i])!=*(other.samples_[i]))
      return false;
  return true;
}

//: spatial object equality

bool vsol_digital_curve_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::CURVE &&
   ((vsol_curve_2d const&)obj).curve_type() == vsol_curve_2d::DIGITAL
  ? *this == (vsol_digital_curve_2d const&) (vsol_digital_curve_2d const&) obj
  : false;
}

//***************************************************************************
// Status report
//***************************************************************************


//---------------------------------------------------------------------------
//: Return the length of `this'
//---------------------------------------------------------------------------
double vsol_digital_curve_2d::length(void) const
{
  double curve_length = 0.0;
  for ( vcl_vector<vsol_point_2d_sptr>::const_iterator itr=samples_.begin();
        itr+1 != samples_.end();  ++itr )
  {
    curve_length += ((*(itr+1))->get_p() - (*itr)->get_p()).length();
  }
  return curve_length;
}


//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_digital_curve_2d::compute_bounding_box(void) const
{
  // valid under linear interpolation
  set_bounding_box(samples_[0]->x(), samples_[0]->y());
  for (unsigned int i=1; i<samples_.size(); ++i)
    add_to_bounding_box(samples_[i]->x(), samples_[i]->y());
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first point of the curve
// Require: in(new_p0)
//---------------------------------------------------------------------------
void vsol_digital_curve_2d::set_p0(const vsol_point_2d_sptr &new_p0)
{
  samples_.front() = new_p0;
}

//---------------------------------------------------------------------------
//: Set the last point of the curve
// Require: in(new_p1)
//---------------------------------------------------------------------------
void vsol_digital_curve_2d::set_p1(const vsol_point_2d_sptr &new_p1)
{
  samples_.back() = new_p1;
}

//---------------------------------------------------------------------------
//: Add another point to the curve
//---------------------------------------------------------------------------
void vsol_digital_curve_2d::add_vertex(const vsol_point_2d_sptr &new_p)
{
  samples_.push_back(new_p);
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_digital_curve_2d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, samples_);
}


//: Binary load self from stream
void vsol_digital_curve_2d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   default:
    assert(!"vsol_digital_curve_2d I/O version should be 1");
   case 1:
    vsl_b_read(is, samples_);
  }
}


//: Return IO version number;
short vsol_digital_curve_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_digital_curve_2d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//: Return a platform independent string identifying the class
vcl_string vsol_digital_curve_2d::is_a() const
{
  return vcl_string("vsol_digital_curve_2d");
}

//: Return true if the argument matches the string identifying the class or any parent class
bool vsol_digital_curve_2d::is_class(const vcl_string& cls) const
{
  return cls==vsol_digital_curve_2d::is_a();
}


//----------------------------------------------------------------
// ================  External Methods ========================
//----------------------------------------------------------------


//: Binary save vsol_digital_curve_2d to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_digital_curve_2d* p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}


//: Binary load vsol_digital_curve_2d from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_digital_curve_2d* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new vsol_digital_curve_2d();
    p->b_read(is);
  }
  else
    p = 0;
}

void vsol_digital_curve_2d::describe(vcl_ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << "[vsol_digital_curve_2d";
  for (unsigned int i=0; i<size(); ++i)
    strm << ' ' << *(point(i));
  strm << ']' << vcl_endl;
}


//: Return the floating point index of the point on the curve nearest to \p point
double closest_index(const vgl_point_2d<double>& pt,
                     const vsol_digital_curve_2d_sptr& curve)
{
  int index = 0;
  const int n = curve->size();
  double x1=curve->point(0)->x(),  y1=curve->point(0)->y(),
         x2=curve->point(1)->x(),  y2=curve->point(1)->y();
  double d = vgl_distance2_to_linesegment(x1,y1,x2,y2,pt.x(),pt.y());

  // find the closest line segment of on the curve
  for (int i=1; i<n-1; ++i)
  {
    x1=curve->point(i)->x();   y1=curve->point(i)->y(),
    x2=curve->point(i+1)->x(); y2=curve->point(i+1)->y();

    // skip duplicate points
    if (x1 == x2 && y1 == y2)
      continue;

    double e = vgl_distance2_to_linesegment(x1,y1,x2,y2,pt.x(),pt.y());
    if (e < d) { d=e; index = i;}
  }

  // find the closest point on the closest line segment
  vgl_vector_2d<double> v1 = curve->point(index+1)->get_p() - curve->point(index)->get_p();
  vgl_vector_2d<double> v2 = pt - curve->point(index)->get_p();
  // project the point onto the line segment
  double fraction = dot_product(normalized(v1),v2) / v1.length();
  if (fraction < 0.0) fraction = 0.0;
  if (fraction > 1.0) fraction = 1.0;

  return (double)index + fraction;
}


//: Split the input curve into two pieces at the floating point index
bool split(const vsol_digital_curve_2d_sptr &input,
           double index,
           vsol_digital_curve_2d_sptr &output1,
           vsol_digital_curve_2d_sptr &output2)
{
  const int n = input->size();
  if (index <= 0.0 || index >= double(n-1))
    return false;

  vcl_vector<vsol_point_2d_sptr> vec1, vec2;
  vgl_point_2d<double> break_point = input->interp(index);
  vec2.push_back(new vsol_point_2d(break_point));
  for (int i=0; i<n; ++i) {
    if ( double(i) < index )
      vec1.push_back(input->point(i));
    if ( double(i) > index )
      vec2.push_back(input->point(i));
    // if index == i then ignore this point
  }
  vec1.push_back(new vsol_point_2d(break_point));

  output1 = new vsol_digital_curve_2d(vec1);
  output2 = new vsol_digital_curve_2d(vec2);
  return true;
}
