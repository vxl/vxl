// This is gel/vsol/vsol_point_2d.cxx
#include "vsol_point_2d.h"
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vgl/vgl_distance.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_point_2d::~vsol_point_2d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vsol_point_2d::clone(void) const
{
  return new vsol_point_2d(*this);
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same coordinates than `other' ?
//---------------------------------------------------------------------------
bool vsol_point_2d::operator==(const vsol_point_2d &other) const
{
  return this==&other || p_==other.p_;
}

//: spatial object equality

bool vsol_point_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::POINT
   ? operator== ((vsol_point_2d const&)obj)
   : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the real type of a point. It is a POINT
//---------------------------------------------------------------------------
vsol_spatial_object_2d::vsol_spatial_object_2d_type
vsol_point_2d::spatial_type(void) const
{
  return POINT;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_point_2d::compute_bounding_box(void) const
{
  set_bounding_box(p_.x(),p_.y());
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the abscissa
//---------------------------------------------------------------------------
void vsol_point_2d::set_x(const double new_x)
{
  p_.set(new_x, p_.y());
}

//---------------------------------------------------------------------------
//: Set the ordinate
//---------------------------------------------------------------------------
void vsol_point_2d::set_y(const double new_y)
{
  p_.set(p_.x(), new_y);
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the distance (N2) between `this' and `other'
//---------------------------------------------------------------------------
double vsol_point_2d::distance(const vsol_point_2d &other) const
{
  return vgl_distance(p_,other.p_);
}

double vsol_point_2d::distance(vsol_point_2d_sptr other) const
{
  return vgl_distance(p_,other->p_);
}

//---------------------------------------------------------------------------
//: Return the middle point between `this' and `other'
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_point_2d::middle(const vsol_point_2d &other) const
{
  return new vsol_point_2d(midpoint(p_,other.p_));
}

//---------------------------------------------------------------------------
//: Add `v' to `this'
//---------------------------------------------------------------------------
void vsol_point_2d::add_vector(vgl_vector_2d<double> const& v)
{
  p_ += v;
}

//---------------------------------------------------------------------------
//: Add `v' and `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr
vsol_point_2d::plus_vector(vgl_vector_2d<double> const& v) const
{
  return new vsol_point_2d(p_ + v);
}

//---------------------------------------------------------------------------
//: Return the vector `this',`other'.
//---------------------------------------------------------------------------
vgl_vector_2d<double>
vsol_point_2d::to_vector(const vsol_point_2d &other) const
{
  return vgl_vector_2d<double>(other.x() - x(), other.y() - y());
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_point_2d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsol_spatial_object_2d::b_write(os);
  vsl_b_write(os, p_.x());
  vsl_b_write(os, p_.y());
}

//: Binary load self from stream (not typically used)
void vsol_point_2d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
   case 1:
    vsol_spatial_object_2d::b_read(is);
    { double x=0, y=0;
      vsl_b_read(is, x);
      vsl_b_read(is, y);
      this->p_.set(x, y);
    }
    break;

   default:
    vcl_cerr << "I/O ERROR: vsol_point_2d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//: Return IO version number;
short vsol_point_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_point_2d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//: Return a platform independent string identifying the class
vcl_string vsol_point_2d::is_a() const
{
  return vcl_string("vsol_point_2d");
}

  //: Return true if the argument matches the string identifying the class or any parent class
bool vsol_point_2d::is_class(const vcl_string& cls) const
{
  return cls==vsol_point_2d::is_a();
}

//: Binary save vsol_point_2d to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_point_2d* p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}


//: Binary load vsol_point_2d from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_point_2d* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new vsol_point_2d();
    p->b_read(is);
  }
  else
    p = 0;
}
