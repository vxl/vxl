// This is gel/vsol/vsol_rectangle_2d.cxx
#include "vsol_rectangle_2d.h"
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vnl/vnl_math.h>
#include <vcl_cmath.h> // for fabs()
#include <vcl_iostream.h>
#include <vgl/vgl_vector_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vbl/io/vbl_io_smart_ptr.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor - needed for binary I/O
//---------------------------------------------------------------------------
vsol_rectangle_2d::vsol_rectangle_2d()
 : vsol_polygon_2d()
{
  storage_->clear();
  storage_->push_back(new vsol_point_2d(0.0,0.0));
  storage_->push_back(new vsol_point_2d(0.0,1.0));
  storage_->push_back(new vsol_point_2d(1.0,1.0));
  storage_->push_back(new vsol_point_2d(1.0,0.0));
}

//---------------------------------------------------------------------------
//: Constructor from 4 points, the corners of the rectangle
//---------------------------------------------------------------------------
vsol_rectangle_2d::vsol_rectangle_2d(const vsol_point_2d_sptr &new_p0,
                                     const vsol_point_2d_sptr &new_p1,
                                     const vsol_point_2d_sptr &new_p2,
                                     const vsol_point_2d_sptr &new_p3)
 : vsol_polygon_2d()
{
  storage_->clear();
  storage_->push_back(new_p0);
  storage_->push_back(new_p1);
  storage_->push_back(new_p2);
  storage_->push_back(new_p3);
}

//---------------------------------------------------------------------------
//: Constructor from 3 points.
// Description: `new_pc' is the origin of the rectangle. `new_pabs' defines
//              the abscissa axis and the width/2. `new_pord' defines the
//              ordinate axis and the height/2.
// Require: valid_vertices(new_pc,new_pabs,new_pord)
//---------------------------------------------------------------------------
vsol_rectangle_2d::vsol_rectangle_2d(const vsol_point_2d_sptr &new_pc,
                                     const vsol_point_2d_sptr &new_pabs,
                                     const vsol_point_2d_sptr &new_pord)
 : vsol_polygon_2d()
{
  vgl_vector_2d<double> a(new_pabs->x(), new_pabs->y());
  vgl_vector_2d<double> o(new_pord->x(), new_pord->y());
  vgl_vector_2d<double> c(new_pc->x(), new_pc->y());
  // require
  assert(vcl_fabs(dot_product(a,o))<1e-06);
  vgl_vector_2d<double> v0 = c-a-o;
  vgl_vector_2d<double> v1 = c+a-o;
  vgl_vector_2d<double> v2 = c+a+o;
  vgl_vector_2d<double> v3 = c-a+o;

  storage_->clear();
  storage_->push_back(new vsol_point_2d(v0.x(), v0.y()));
  storage_->push_back(new vsol_point_2d(v1.x(), v1.y()));
  storage_->push_back(new vsol_point_2d(v2.x(), v2.y()));
  storage_->push_back(new vsol_point_2d(v3.x(), v3.y()));
}
//---------------------------------------------------------------------------
//: Constructor from center, half_width, half_height, angle(ccw from x axis, in deg/rad)
//---------------------------------------------------------------------------
vsol_rectangle_2d::vsol_rectangle_2d(const vsol_point_2d_sptr &center,
                                     const double half_width,
                                     const double half_height,
                                     const double angle,
                                     const bool deg)
 : vsol_polygon_2d()
{
  assert(half_width>0&&half_height>0);
  double ang = angle;
  if (deg)
    ang = (vnl_math::pi*angle)/180.0;
  double c = vcl_cos(ang), s = vcl_sin(ang);
  vgl_vector_2d<double> a(half_width*c, half_width*s);
  vgl_vector_2d<double> b(-half_height*s, half_height*c);
  vgl_vector_2d<double> v0 = -a-b;
  vgl_vector_2d<double> v1 = a-b;
  vgl_vector_2d<double> v2 = a+b;
  vgl_vector_2d<double> v3 = -a+b;
  storage_->clear();
  storage_->push_back(center->plus_vector(v0));
  storage_->push_back(center->plus_vector(v1));
  storage_->push_back(center->plus_vector(v2));
  storage_->push_back(center->plus_vector(v3));
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_rectangle_2d::vsol_rectangle_2d(const vsol_rectangle_2d &other)
  : vsol_polygon_2d(other)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_rectangle_2d::~vsol_rectangle_2d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vsol_rectangle_2d::clone(void) const
{
  return new vsol_rectangle_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_rectangle_2d::p0(void) const
{
  return (*storage_)[0];
}

//---------------------------------------------------------------------------
//: Return the second vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_rectangle_2d::p1(void) const
{
  return (*storage_)[1];
}

//---------------------------------------------------------------------------
//: Return the third vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_rectangle_2d::p2(void) const
{
  return (*storage_)[2];
}

//---------------------------------------------------------------------------
//: Return the last vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_rectangle_2d::p3(void) const
{
  return (*storage_)[3];
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_rectangle_2d::operator==(const vsol_rectangle_2d &other) const
{
  return vsol_polygon_2d::operator==(other);
}

bool vsol_rectangle_2d::operator==(const vsol_polygon_2d &other) const
{
  return vsol_polygon_2d::operator==(other);
}

//: spatial object equality

bool vsol_rectangle_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
    obj.spatial_type() == vsol_spatial_object_2d::REGION &&
    ((vsol_region_2d const&)obj).region_type() == vsol_region_2d::POLYGON
    ? *this == (vsol_polygon_2d const&) (vsol_region_2d const&) obj
    : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the width
//---------------------------------------------------------------------------
double vsol_rectangle_2d::width(void) const
{
  return (*storage_)[0]->distance((*storage_)[1]);
}

//---------------------------------------------------------------------------
//: Return the height
//---------------------------------------------------------------------------
double vsol_rectangle_2d::height(void) const
{
  return (*storage_)[1]->distance((*storage_)[2]);
}

//---------------------------------------------------------------------------
//: Return the area of `this'
//---------------------------------------------------------------------------
double vsol_rectangle_2d::area(void) const
{
  double result;
  result=width()*height();
  return result;
}

//---------------------------------------------------------------------------
//: Are `new_vertices' valid to build a rectangle ?
//---------------------------------------------------------------------------
bool vsol_rectangle_2d::valid_vertices(const vcl_vector<vsol_point_2d_sptr> new_vertices) const
{
  if (new_vertices.size() != 3) return false;
  vgl_vector_2d<double> a=new_vertices[0]->to_vector(*(new_vertices[1]));
  vgl_vector_2d<double> b=new_vertices[1]->to_vector(*(new_vertices[2]));
  // the two vectors should be orthogonal:
  return dot_product(a,b)==0;
}


//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_rectangle_2d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsol_polygon_2d::b_write(os);
}

//: Binary load self from stream (not typically used)
void vsol_rectangle_2d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsol_polygon_2d::b_read(is);
    if (storage_->size()!=4){
      vcl_cerr << "I/O ERROR: vsol_rectangle_2d::b_read(vsl_b_istream&)\n"
               << "           Incorrect number of vertices: "<< storage_->size() << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
    break;

   default:
    vcl_cerr << "I/O ERROR: vsol_rectangle_2d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
//: Return IO version number;
short vsol_rectangle_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_rectangle_2d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//: Return true if the argument matches the string identifying the class or any parent class
bool vsol_rectangle_2d::is_class(const vcl_string& cls) const
{
  return cls==vsol_rectangle_2d::is_a();
}

//external functions

//: Binary save vsol_rectangle_2d* to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_rectangle_2d* r)
{
  if (!r) {
    vsl_b_write(os, false); // Indicate null rectangle stored
  }
  else {
    vsl_b_write(os,true); // Indicate non-null rectangle stored
    r->b_write(os);
  }
}

//: Binary load vsol_rectangle_2d* from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_rectangle_2d* &r)
{
  delete r;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    r = new vsol_rectangle_2d();
    r->b_read(is);
  }
  else
    r = 0;
}

inline void vsol_rectangle_2d::describe(vcl_ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << "<vsol_rectangle_2d with corners";
  for (unsigned int i=0; i<size(); ++i)
    strm << ' ' << *(vertex(i));
  strm << '>' << vcl_endl;
}
