// This is gel/vsol/vsol_triangle_3d.cxx
#include <iostream>
#include "vsol_triangle_3d.h"
//:
// \file
#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <vsol/vsol_point_3d.h>
#include "vgl/vgl_vector_3d.h"
#include "vnl/vnl_math.h"

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from its 3 vertices
//---------------------------------------------------------------------------
vsol_triangle_3d::vsol_triangle_3d(vsol_point_3d_sptr const& new_p0,
                                   vsol_point_3d_sptr const& new_p1,
                                   vsol_point_3d_sptr const& new_p2)
{
  storage_=new std::vector<vsol_point_3d_sptr>(3);
  (*storage_)[0]=new_p0;
  (*storage_)[1]=new_p1;
  (*storage_)[2]=new_p2;
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_triangle_3d::vsol_triangle_3d(vsol_triangle_3d const& other) = default;

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_triangle_3d::~vsol_triangle_3d() = default;

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d *vsol_triangle_3d::clone() const {
  return new vsol_triangle_3d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_triangle_3d::p0() const { return (*storage_)[0]; }

//---------------------------------------------------------------------------
//: Return the second vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_triangle_3d::p1() const { return (*storage_)[1]; }

//---------------------------------------------------------------------------
//: Return the last vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_triangle_3d::p2() const { return (*storage_)[2]; }

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_triangle_3d::operator==(vsol_triangle_3d const& other) const
{
  return vsol_polygon_3d::operator==(other);
}

bool vsol_triangle_3d::operator==(vsol_polygon_3d const& other) const
{
  return vsol_polygon_3d::operator==(other);
}

//: spatial object equality

bool vsol_triangle_3d::operator==(vsol_spatial_object_3d const& obj) const
{
  return
    obj.cast_to_region() && obj.cast_to_region()->cast_to_polygon() &&
    *this == *obj.cast_to_region()->cast_to_polygon();
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the area of `this'
//---------------------------------------------------------------------------
double vsol_triangle_3d::area() const {
  double dx02=(*storage_)[0]->x()-(*storage_)[2]->x();
  double dy02=(*storage_)[0]->y()-(*storage_)[2]->y();
  double dz02=(*storage_)[0]->z()-(*storage_)[2]->z();
  double dx12=(*storage_)[1]->x()-(*storage_)[2]->x();
  double dy12=(*storage_)[1]->y()-(*storage_)[2]->y();
  double dz12=(*storage_)[1]->z()-(*storage_)[2]->z();
  return std::sqrt( vnl_math::sqr(dy02*dz12-dy12*dz02)
                 + vnl_math::sqr(dz02*dx12-dz12*dx02)
                 + vnl_math::sqr(dx02*dy12-dx12*dy02))/2;
}
//***************************************************************************
// Element change
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first vertex
//---------------------------------------------------------------------------
void vsol_triangle_3d::set_p0(vsol_point_3d_sptr const& new_p0)
{
  (*storage_)[0]=new_p0;
}

//---------------------------------------------------------------------------
//: Set the second vertex
//---------------------------------------------------------------------------
void vsol_triangle_3d::set_p1(vsol_point_3d_sptr const& new_p1)
{
  (*storage_)[1]=new_p1;
}

//---------------------------------------------------------------------------
//: Set the last vertex
//---------------------------------------------------------------------------
void vsol_triangle_3d::set_p2(vsol_point_3d_sptr const& new_p2)
{
  (*storage_)[2]=new_p2;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `p' in `this' ?
// \todo not yet implemented
//---------------------------------------------------------------------------
bool vsol_triangle_3d::in(vsol_point_3d_sptr const& ) const
{
  std::cerr << "Warning: vsol_triangle_3d::in() has not been implemented yet\n";
  return true;
}

//---------------------------------------------------------------------------
//: Return the unit normal vector at point `p'.
// This is the normal to the place in which the triangle lies.
// If the vertices are collinear, the normal is the null vector.
// Require: in(p)
//---------------------------------------------------------------------------
vgl_vector_3d<double>
vsol_triangle_3d::normal_at_point(vsol_point_3d_sptr const& p) const
{
  // require
  assert(in(p));

  // Since a rectangle is planar, the answer is independent of p:
  vgl_vector_3d<double> v1((*storage_)[1]->x()-(*storage_)[0]->x(),
                           (*storage_)[1]->y()-(*storage_)[0]->y(),
                           (*storage_)[1]->z()-(*storage_)[0]->z());
  vgl_vector_3d<double> v2((*storage_)[2]->x()-(*storage_)[0]->x(),
                           (*storage_)[2]->y()-(*storage_)[0]->y(),
                           (*storage_)[2]->z()-(*storage_)[0]->z());

  return normalized(cross_product(v1,v2));
}

inline void vsol_triangle_3d::describe(std::ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << "<vsol_triangle_3d with corners";
  for (unsigned int i=0; i<size(); ++i)
    strm << ' ' << *(vertex(i));
  strm << '>' << std::endl;
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_triangle_3d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsol_polygon_3d::b_write(os);
}

//: Binary load self from stream (not typically used)
void vsol_triangle_3d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsol_polygon_3d::b_read(is);
    if (storage_->size()!=3) {
      std::cerr << "I/O ERROR: vsol_triangle_3d::b_read(vsl_b_istream&)\n"
               << "           Incorrect number of vertices: "<< storage_->size() << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
    break;

   default:
    std::cerr << "I/O ERROR: vsol_triangle_3d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
//: Return IO version number;
short vsol_triangle_3d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_triangle_3d::print_summary(std::ostream &os) const
{
  os << *this;
}

//external functions

//: Binary save vsol_triangle_3d_sptr to stream.
void
vsl_b_write(vsl_b_ostream &os, vsol_triangle_3d const* t)
{
  if (!t) {
    vsl_b_write(os, false); // Indicate null triangle stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null triangle stored
    t->b_write(os);
  }
}

//: Binary load vsol_triangle_3d* from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_triangle_3d* &t)
{
  delete t;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    t = new vsol_triangle_3d(new vsol_point_3d(0.0,0.0,0.0),new vsol_point_3d(0.0,0.0,0.0),new vsol_point_3d(0.0,0.0,0.0));
    t->b_read(is);
  }
  else
    t = nullptr;
}
