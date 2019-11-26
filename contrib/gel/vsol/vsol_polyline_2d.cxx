// This is gel/vsol/vsol_polyline_2d.cxx
#include <iostream>
#include "vsol_polyline_2d.h"
//:
// \file

#include <vsol/vsol_point_2d.h>
#include "vgl/vgl_vector_2d.h"
#include "vsl/vsl_vector_io.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <cassert>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor
//---------------------------------------------------------------------------
vsol_polyline_2d::vsol_polyline_2d()
  : vsol_curve_2d()
{
  storage_=new std::vector<vsol_point_2d_sptr>();
  p0_ = nullptr;
  p1_ = nullptr;
}

//---------------------------------------------------------------------------
//: Constructor from a std::vector of points
//---------------------------------------------------------------------------

vsol_polyline_2d::vsol_polyline_2d(std::vector<vsol_point_2d_sptr> const& new_vertices)
  : vsol_curve_2d()
{
  storage_=new std::vector<vsol_point_2d_sptr>(new_vertices);
  int n = storage_->size();
  if (n<1) {
    p0_ = nullptr;
    p1_ = nullptr;
  }
  else {
    p0_ = (*storage_)[0];
    p1_ = (*storage_)[n-1]; // might coincide with p0_
  }
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_polyline_2d::vsol_polyline_2d(vsol_polyline_2d const& other)
  : vsol_curve_2d(other)
{
  storage_=new std::vector<vsol_point_2d_sptr>(*other.storage_);
  for (unsigned int i=0;i<storage_->size();++i)
    (*storage_)[i]=new vsol_point_2d(*((*other.storage_)[i]));
  p0_ = other.p0_;
  p1_ = other.p1_;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_polyline_2d::~vsol_polyline_2d()
{
  for (auto & i : *storage_)
    i = nullptr;
  p0_ = nullptr;
  p1_ = nullptr;
  delete storage_;
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vsol_polyline_2d::clone() const
{
  return new vsol_polyline_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return vertex `i'
//  REQUIRE: valid_index(i)
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_polyline_2d::vertex(const int i) const
{
  // require
  assert(valid_index(i));

  return (*storage_)[i];
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_polyline_2d::operator==(vsol_polyline_2d const& other) const
{
  if (this==&other)
    return true;
  //check endpoint equality since that is cheaper then checking each vertex
  //and if it fails we are done
  bool epts_eq = vsol_curve_2d::endpoints_equal(other);
  if (!epts_eq)
    return false;
  //Do the polylines have the same number of vertices?
  if (storage_->size()!=other.storage_->size())
    return false;
  //The easy tests are done.  Now compare each vertex
  int n = storage_->size();
  for (int i=0; i<n; i++)
    if (*((*storage_)[i])!=*((*other.storage_)[i]))
      return false;
  return true;
}

//: spatial object equality

bool vsol_polyline_2d::operator==(vsol_spatial_object_2d const& obj) const
{
  return
    obj.cast_to_curve() && obj.cast_to_curve()->cast_to_polyline() &&
    *this == *obj.cast_to_curve()->cast_to_polyline();
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the length of `this'
//---------------------------------------------------------------------------
double vsol_polyline_2d::length() const
{
  double l = 0.0;
  for (unsigned int i=0;i+1<storage_->size();++i)
    l += ::length(vgl_vector_2d<double>((*storage_)[i+1]->x(),(*storage_)[i+1]->y())
                 -vgl_vector_2d<double>((*storage_)[i]->x(),(*storage_)[i]->y()));
  return l;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_polyline_2d::compute_bounding_box() const
{
  set_bounding_box((*storage_)[0]->x(), (*storage_)[0]->y());
  for (unsigned int i=1;i<storage_->size();++i)
    add_to_bounding_box((*storage_)[i]->x(), (*storage_)[i]->y());
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first point of the curve
// Require: in(new_p0)
//---------------------------------------------------------------------------
void vsol_polyline_2d::set_p0(vsol_point_2d_sptr const& new_p0)
{
  p0_=new_p0;
  storage_->push_back(p0_);
}

//---------------------------------------------------------------------------
//: Set the last point of the curve
// Require: in(new_p1)
//---------------------------------------------------------------------------
void vsol_polyline_2d::set_p1(vsol_point_2d_sptr const& new_p1)
{
  p1_=new_p1;
  storage_->push_back(p1_);
}

//---------------------------------------------------------------------------
//: Add another point to the curve
//---------------------------------------------------------------------------
void vsol_polyline_2d::add_vertex(vsol_point_2d_sptr const& new_p)
{
  storage_->push_back(new_p);
  //Required to have a consistent curve (JLM)
  p1_ = new_p;
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_polyline_2d::b_write(vsl_b_ostream &os) const
{
  if (!storage_)
    vsl_b_write(os, false); // Indicate null pointer stored
  else
  {
    vsl_b_write(os, true); // Indicate non-null pointer stored
    vsl_b_write(os, version());
    vsl_b_write(os, *storage_);
  }
}

//: Binary load self from stream (not typically used)
void vsol_polyline_2d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  delete storage_;
  storage_ = new std::vector<vsol_point_2d_sptr>();
  p0_=nullptr;
  p1_=nullptr;
  bool null_ptr;
  vsl_b_read(is, null_ptr);
  if (!null_ptr)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1: {
    vsl_b_read(is, *storage_);
    int n = storage_->size();
    if (n>=1) {
      p0_=(*storage_)[0];
      p1_=(*storage_)[n-1]; // might coincide with p0_
    }
    break;
   }
   default:
    std::cerr << "vsol_polyline_2d: unknown I/O version " << ver << '\n';
  }
}

//: Return IO version number;
short vsol_polyline_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_polyline_2d::print_summary(std::ostream &os) const
{
  os << *this;
}

//: Binary save vsol_polyline_2d to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_polyline_2d* p)
{
  if (p==nullptr) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}


//: Binary load vsol_polyline_2d from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_polyline_2d* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new vsol_polyline_2d();
    p->b_read(is);
  }
  else
    p = nullptr;
}

void vsol_polyline_2d::describe(std::ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << "[vsol_polyline_2d";
  for (unsigned int i=0; i<size(); ++i)
    strm << ' ' << *(vertex(i));
  strm << ']' << std::endl;
}
