// This is gel/vsol/vsol_digital_curve_2d.cxx
#include "vsol_digital_curve_2d.h"
#include <cmath>
#include <iostream>
#include <utility>
//:
// \file
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_closest_point.h>
#include <vsl/vsl_vector_io.h>
#include <vcl_compiler.h>
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
//: Constructor from a std::vector of points
//---------------------------------------------------------------------------

vsol_digital_curve_2d::vsol_digital_curve_2d(std::vector<vsol_point_2d_sptr> samples)
 : vsol_curve_2d(), samples_(std::move(samples))
{
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_digital_curve_2d::vsol_digital_curve_2d(const vsol_digital_curve_2d &other)
  : vsol_curve_2d(other), samples_()
{
  for ( std::vector<vsol_point_2d_sptr>::const_iterator itr=other.samples_.begin();
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
    return nullptr;

  return samples_.front();
}

//---------------------------------------------------------------------------
//: Return the last point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_digital_curve_2d::p1(void) const
{
  if ( samples_.empty() )
    return nullptr;

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

  int i1 = (int)std::floor(index);
  if ( std::floor(index) == index )
    return samples_[i1]->get_p();

  int i2 = (int)std::ceil(index);
  double f = index - std::floor(index);

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
    obj.cast_to_curve() && obj.cast_to_curve()->cast_to_digital_curve() &&
    *this == *obj.cast_to_curve()->cast_to_digital_curve();
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
  for ( std::vector<vsol_point_2d_sptr>::const_iterator itr=samples_.begin();
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
void vsol_digital_curve_2d::print_summary(std::ostream &os) const
{
  os << *this;
}


//----------------------------------------------------------------
// ================  External Methods ========================
//----------------------------------------------------------------


//: Binary save vsol_digital_curve_2d to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_digital_curve_2d* p)
{
  if (p==nullptr) {
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
    p = nullptr;
}

void vsol_digital_curve_2d::describe(std::ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << "[vsol_digital_curve_2d";
  for (unsigned int i=0; i<size(); ++i)
    strm << ' ' << *(point(i));
  strm << ']' << std::endl;
}


//: Return the floating point index of the point on the curve nearest to \p pt
double closest_index(const vgl_point_2d<double>& pt,
                     const vsol_digital_curve_2d_sptr& curve)
{
  const unsigned int n = curve->size();
  double *px = new double[n], *py = new double[n];
  for (unsigned int i=0; i<n; ++i)
    px[i]=curve->point(i)->x(), py[i]=curve->point(i)->y();
  double x, y;
  int index=vgl_closest_point_to_non_closed_polygon(x,y,px,py,n,pt.x(),pt.y());
  double dx = px[index+1]-px[index], dy = py[index+1]-py[index];
  double f = dx==0 ? (dy==0 ? 0.5 : (y-py[index])/dy) : (x-px[index])/dx;
  delete [] px; delete[] py;
  return f + index;
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

  std::vector<vsol_point_2d_sptr> vec1, vec2;
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
