// This is gel/vsol/vsol_triangle_2d.cxx
#include "vsol_triangle_2d.h"
//:
// \file
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsol/vsol_point_2d.h>
#include <vcl_iostream.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor - needed for binary I/O
//---------------------------------------------------------------------------
vsol_triangle_2d::vsol_triangle_2d()
 : vsol_polygon_2d()
{
  storage_->clear();
  storage_->push_back(new vsol_point_2d(0.0,0.0));
  storage_->push_back(new vsol_point_2d(0.0,1.0));
  storage_->push_back(new vsol_point_2d(1.0,0.0));
}

//---------------------------------------------------------------------------
//: Constructor from its 3 vertices
//---------------------------------------------------------------------------
vsol_triangle_2d::vsol_triangle_2d(const vsol_point_2d_sptr &new_p0,
                                   const vsol_point_2d_sptr &new_p1,
                                   const vsol_point_2d_sptr &new_p2)
 : vsol_polygon_2d()
{
  storage_->clear();
  storage_->push_back(new_p0);
  storage_->push_back(new_p1);
  storage_->push_back(new_p2);
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_triangle_2d::vsol_triangle_2d(const vsol_triangle_2d &other)
  : vsol_polygon_2d(other)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_triangle_2d::~vsol_triangle_2d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vsol_triangle_2d::clone(void) const
{
  return new vsol_triangle_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_triangle_2d::p0(void) const
{
  return (*storage_)[0];
}

//---------------------------------------------------------------------------
//: Return the second vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_triangle_2d::p1(void) const
{
  return (*storage_)[1];
}

//---------------------------------------------------------------------------
//: Return the last vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_triangle_2d::p2(void) const
{
  return (*storage_)[2];
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_triangle_2d::operator==(const vsol_triangle_2d &other) const
{
  return vsol_polygon_2d::operator==(other);
}

bool vsol_triangle_2d::operator==(const vsol_polygon_2d &other) const
{
  return vsol_polygon_2d::operator==(other);
}

//: spatial object equality

bool vsol_triangle_2d::operator==(const vsol_spatial_object_2d& obj) const
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
double vsol_triangle_2d::area(void) const
{
  double result;

  result=(((*storage_)[0]->x()-(*storage_)[1]->x())
          *((*storage_)[1]->y()-(*storage_)[2]->y())
          -((*storage_)[1]->x()-(*storage_)[2]->x())
          *((*storage_)[0]->y()-(*storage_)[1]->y()))/2;

  return result;
}

//***************************************************************************
// Element change
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first vertex
//---------------------------------------------------------------------------
void vsol_triangle_2d::set_p0(const vsol_point_2d_sptr &new_p0)
{
  (*storage_)[0]=new_p0;
}

//---------------------------------------------------------------------------
//: Set the second vertex
//---------------------------------------------------------------------------
void vsol_triangle_2d::set_p1(const vsol_point_2d_sptr &new_p1)
{
  (*storage_)[1]=new_p1;
}

//---------------------------------------------------------------------------
//: Set the last vertex
//---------------------------------------------------------------------------
void vsol_triangle_2d::set_p2(const vsol_point_2d_sptr &new_p2)
{
  (*storage_)[2]=new_p2;
}


//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_triangle_2d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsol_polygon_2d::b_write(os);
}

//: Binary load self from stream (not typically used)
void vsol_triangle_2d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsol_polygon_2d::b_read(is);
    if (storage_->size()!=3) {
      vcl_cerr << "I/O ERROR: vsol_triangle_2d::b_read(vsl_b_istream&)\n"
               << "           Incorrect number of vertices: "<< storage_->size() << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
    break;
   default:
    vcl_cerr << "I/O ERROR: vsol_triangle_2d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//: Return IO version number;
short vsol_triangle_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_triangle_2d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//external functions

//: Binary save vsol_triangle_2d_sptr to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_triangle_2d* t)
{
  if (!t){
    vsl_b_write(os, false); // Indicate null triangle stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null triangle stored
    t->b_write(os);
  }
}

//: Binary load vsol_triangle_2d* from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_triangle_2d* &t)
{
  delete t;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    t = new vsol_triangle_2d();
    t->b_read(is);
  }
  else
    t = 0;
}


inline void vsol_triangle_2d::describe(vcl_ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << "<vsol_triangle_2d with corners";
  for (unsigned int i=0; i<size(); ++i)
    strm << ' ' << *(vertex(i));
  strm << '>' << vcl_endl;
}

