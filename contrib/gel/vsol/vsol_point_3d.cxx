// This is gel/vsol/vsol_point_3d.cxx
#include "vsol_point_3d.h"
//:
// \file

#include <vgl/vgl_distance.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_point_3d::~vsol_point_3d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d* vsol_point_3d::clone(void) const
{
  return new vsol_point_3d(*this);
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same coordinates than `other' ?
//---------------------------------------------------------------------------
bool vsol_point_3d::operator==(vsol_point_3d const& other) const
{
  return this==&other || p_==other.p_;
}

//: spatial object equality

bool vsol_point_3d::operator==(vsol_spatial_object_3d const& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::POINT
  ? operator==((vsol_point_3d const&)obj)
  : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the real type of a point. It is a POINT
//---------------------------------------------------------------------------
vsol_spatial_object_3d::vsol_spatial_object_3d_type
vsol_point_3d::spatial_type(void) const
{
  return POINT;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_point_3d::compute_bounding_box(void) const
{
  set_bounding_box(p_.x(),p_.y(),p_.z());
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the abscissa
//---------------------------------------------------------------------------
void vsol_point_3d::set_x(const double new_x)
{
  p_.set(new_x, p_.y(), p_.z());
}

//---------------------------------------------------------------------------
//: Set the ordinate
//---------------------------------------------------------------------------
void vsol_point_3d::set_y(const double new_y)
{
  p_.set(p_.x(), new_y, p_.z());
}

//---------------------------------------------------------------------------
//: Set the ordinate
//---------------------------------------------------------------------------
void vsol_point_3d::set_z(const double new_z)
{
  p_.set(p_.x(), p_.y(), new_z);
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the distance (N2) between `this' and `other'
//---------------------------------------------------------------------------
double vsol_point_3d::distance(vsol_point_3d const& other) const
{
  return vgl_distance(p_,other.p_);
}

double vsol_point_3d::distance(vsol_point_3d_sptr other) const
{
  return vgl_distance(p_,other->p_);
}

//---------------------------------------------------------------------------
//: Return the middle point between `this' and `other'
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_point_3d::middle(vsol_point_3d const& other) const
{
  return new vsol_point_3d(midpoint(p_,other.p_));
}

//---------------------------------------------------------------------------
//: Add `v' to `this'
//---------------------------------------------------------------------------
void vsol_point_3d::add_vector(vgl_vector_3d<double> const& v)
{
  p_ += v;
}

//---------------------------------------------------------------------------
//: Add `v' and `this'
//---------------------------------------------------------------------------
vsol_point_3d_sptr
vsol_point_3d::plus_vector(vgl_vector_3d<double> const& v) const
{
  return new vsol_point_3d(p_ + v);
}

//---------------------------------------------------------------------------
//: Return the vector `this',`other'.
//---------------------------------------------------------------------------
vgl_vector_3d<double>
vsol_point_3d::to_vector(vsol_point_3d const& other) const
{
  return vgl_vector_3d<double>(other.x() - x(),other.y() - y(),other.z() - z());
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_point_3d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsol_spatial_object_3d::b_write(os);
  vsl_b_write(os, p_.x());
  vsl_b_write(os, p_.y());
  vsl_b_write(os, p_.z());
}

//: Binary load self from stream (not typically used)
void vsol_point_3d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsol_spatial_object_3d::b_read(is);
    { double x=0, y=0, z=0;
      vsl_b_read(is, x);
      vsl_b_read(is, y);
      vsl_b_read(is, z);
      this->p_.set(x, y, z);
    }
    break;

   default:
    vcl_cerr << "I/O ERROR: vsol_point_3d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//: Return IO version number;
short vsol_point_3d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_point_3d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//: Binary save vsol_point_3d to stream.
void
vsl_b_write(vsl_b_ostream &os, vsol_point_3d const* p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}

//: Binary load vsol_point_3d from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_point_3d* &p)
{
  delete p; p=0;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new vsol_point_3d(0.0,0.0,0.0);
    p->b_read(is);
  }
}
