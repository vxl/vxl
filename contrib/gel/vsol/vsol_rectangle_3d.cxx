// This is gel/vsol/vsol_rectangle_3d.cxx
#include "vsol_rectangle_3d.h"
//:
// \file

#include <vcl_cassert.h>
#include <vgl/vgl_vector_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vcl_iostream.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from 3 points.
// Description: `new_p0' is the origin of the rectangle. `new_p1' defines
//              the abscissa axis and the width. `new_p2' defines the
//              ordinate axis and the height.
// Require: valid_vertices(new_p0,new_p1,new_p2)
//---------------------------------------------------------------------------
vsol_rectangle_3d::vsol_rectangle_3d(vsol_point_3d_sptr const& new_p0,
                                     vsol_point_3d_sptr const& new_p1,
                                     vsol_point_3d_sptr const& new_p2)
{
  storage_=new vcl_vector<vsol_point_3d_sptr>(3);
  (*storage_)[0]=new_p0;
  (*storage_)[1]=new_p1;
  (*storage_)[2]=new_p2;
  // require
  assert(valid_vertices(*storage_));
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_rectangle_3d::vsol_rectangle_3d(vsol_rectangle_3d const& other)
  : vsol_polygon_3d(other)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_rectangle_3d::~vsol_rectangle_3d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d* vsol_rectangle_3d::clone(void) const
{
  return new vsol_rectangle_3d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_rectangle_3d::p0(void) const
{
  return (*storage_)[0];
}

//---------------------------------------------------------------------------
//: Return the second vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_rectangle_3d::p1(void) const
{
  return (*storage_)[1];
}

//---------------------------------------------------------------------------
//: Return the third vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_rectangle_3d::p2(void) const
{
  return (*storage_)[2];
}

//---------------------------------------------------------------------------
//: Return the last vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_rectangle_3d::p3(void) const
{
  vsol_point_3d_sptr result=new vsol_point_3d(*(*storage_)[0]);
  vgl_vector_3d<double> v=(*storage_)[1]->to_vector(*(*storage_)[2]);
  result->add_vector(v);
  return result;
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_rectangle_3d::operator==(vsol_rectangle_3d const& other) const
{
  return vsol_polygon_3d::operator==(other);
}

bool vsol_rectangle_3d::operator==(vsol_polygon_3d const& other) const
{
  return vsol_polygon_3d::operator==(other);
}

//: spatial object equality

bool vsol_rectangle_3d::operator==(vsol_spatial_object_3d const& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::REGION &&
   ((vsol_region_3d const&)obj).region_type() == vsol_region_3d::POLYGON
  ? *this == (vsol_polygon_3d const&) (vsol_region_3d const&) obj
  : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the width
//---------------------------------------------------------------------------
double vsol_rectangle_3d::width(void) const
{
  return (*storage_)[0]->distance((*storage_)[1]);
}

//---------------------------------------------------------------------------
//: Return the height
//---------------------------------------------------------------------------
double vsol_rectangle_3d::height(void) const
{
  return (*storage_)[1]->distance((*storage_)[2]);
}

//---------------------------------------------------------------------------
//: Return the area of `this'
//---------------------------------------------------------------------------
double vsol_rectangle_3d::area(void) const
{
  double result;
  result=width()*height();
  return result;
}

//---------------------------------------------------------------------------
//: Are `new_vertices' valid to build a rectangle ?
//---------------------------------------------------------------------------
bool vsol_rectangle_3d::valid_vertices(const vcl_vector<vsol_point_3d_sptr> new_vertices) const
{
  if (new_vertices.size() != 3) return false;
  vgl_vector_3d<double> a=new_vertices[0]->to_vector(*(new_vertices[1]));
  vgl_vector_3d<double> b=new_vertices[1]->to_vector(*(new_vertices[2]));
  // the two vectors should be orthogonal:
  return dot_product(a,b)==0;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `p' in `this' ?
//---------------------------------------------------------------------------
bool vsol_rectangle_3d::in(vsol_point_3d_sptr const& ) const
{
  // TODO
  vcl_cerr << "Warning: vsol_rectangle_3d::in() has not been implemented yet\n";
  return true;
}

//---------------------------------------------------------------------------
//: Return the unit normal vector at point `p'.
// Require: in(p)
//---------------------------------------------------------------------------
vgl_vector_3d<double>
vsol_rectangle_3d::normal_at_point(vsol_point_3d_sptr const& p) const
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

inline void vsol_rectangle_3d::describe(vcl_ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << "<vsol_rectangle_3d with corners";
  for (unsigned int i=0; i<size(); ++i)
    strm << ' ' << *(vertex(i));
  strm << '>' << vcl_endl;
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_rectangle_3d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsol_polygon_3d::b_write(os);
}

//: Binary load self from stream (not typically used)
void vsol_rectangle_3d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsol_polygon_3d::b_read(is);
    if (storage_->size()!=4){
      vcl_cerr << "I/O ERROR: vsol_rectangle_3d::b_read(vsl_b_istream&)\n"
               << "           Incorrect number of vertices: "<< storage_->size() << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
    break;

   default:
    vcl_cerr << "I/O ERROR: vsol_rectangle_3d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
//: Return IO version number;
short vsol_rectangle_3d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_rectangle_3d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//external functions

//: Binary save vsol_rectangle_3d* to stream.
void
vsl_b_write(vsl_b_ostream &os, vsol_rectangle_3d const* r)
{
  if (!r) {
    vsl_b_write(os, false); // Indicate null rectangle stored
  }
  else {
    vsl_b_write(os,true); // Indicate non-null rectangle stored
    r->b_write(os);
  }
}

//: Binary load vsol_rectangle_3d* from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_rectangle_3d* &r)
{
  delete r;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    r = new vsol_rectangle_3d(new vsol_point_3d(0.0,0.0,0.0),new vsol_point_3d(0.0,0.0,0.0),new vsol_point_3d(0.0,0.0,0.0));
    r->b_read(is);
  }
  else
    r = 0;
}
