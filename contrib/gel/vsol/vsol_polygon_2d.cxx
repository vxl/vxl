// This is gel/vsol/vsol_polygon_2d.cxx
#include "vsol_polygon_2d.h"
//:
// \file
#include <vsl/vsl_vector_io.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_lineseg_test.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from a vcl_vector (not a geometric vector but a list of points)
// Require: new_vertices.size()>=3
//---------------------------------------------------------------------------
vsol_polygon_2d::vsol_polygon_2d(const vcl_vector<vsol_point_2d_sptr> &new_vertices)
  : vsol_region_2d()
{
  // require
  assert(new_vertices.size()>=3);

  storage_=new vcl_vector<vsol_point_2d_sptr>(new_vertices);
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_polygon_2d::vsol_polygon_2d(const vsol_polygon_2d &other)
  : vsol_region_2d(other)
{
  //vsol_point_2d_sptr p;
  storage_=new vcl_vector<vsol_point_2d_sptr>(*other.storage_);
  for (unsigned int i=0;i<storage_->size();++i)
    (*storage_)[i]=new vsol_point_2d(*((*other.storage_)[i]));
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_polygon_2d::~vsol_polygon_2d()
{
  delete storage_;
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vsol_polygon_2d::clone(void) const
{
  return new vsol_polygon_2d(*this);
}

//***************************************************************************
// Safe casting
//***************************************************************************

vsol_polygon_2d* vsol_polygon_2d::cast_to_polygon(void)
{
  if (!cast_to_triangle()||!cast_to_rectangle())
    return this;
  else
    return 0;
}

const vsol_polygon_2d* vsol_polygon_2d::cast_to_polygon(void) const
{
  if (!cast_to_triangle()||!cast_to_rectangle())
    return this;
  else
    return 0;
}

vsol_triangle_2d* vsol_polygon_2d::cast_to_triangle(void){return 0;}
const vsol_triangle_2d* vsol_polygon_2d::cast_to_triangle(void) const
{
  return 0;
}

vsol_rectangle_2d* vsol_polygon_2d::cast_to_rectangle(void){return 0;}
const vsol_rectangle_2d* vsol_polygon_2d::cast_to_rectangle(void) const
{
  return 0;
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return vertex `i'
// Require: valid_index(i)
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_polygon_2d::vertex(const int i) const
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
bool vsol_polygon_2d::operator==(const vsol_polygon_2d &other) const
{
  bool result = (this==&other);

  if (!result)
  {
    result = (storage_->size()==other.storage_->size());
    if (result)
    {
      vsol_point_2d_sptr p=(*storage_)[0];

      unsigned int i=0;
      for (result=false;i<storage_->size()&&!result;++i)
        result = (*p==*(*other.storage_)[i]);
      if (result)
      {
        for (unsigned int j=1;j<size()&&result;++i,++j)
        {
          if (i>=storage_->size()) i=0;
          result = ((*storage_)[i]==(*storage_)[j]);
        }
      }
    }
  }
  return result;
}

//: spatial object equality

bool vsol_polygon_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
    obj.cast_to_region() && obj.cast_to_region()->cast_to_polygon() &&
    *this == *obj.cast_to_region()->cast_to_polygon();
}


//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_polygon_2d::compute_bounding_box(void) const
{
  set_bounding_box((*storage_)[0]->x(), (*storage_)[0]->y());
  for (unsigned int i=1;i<storage_->size();++i)
    add_to_bounding_box((*storage_)[i]->x(), (*storage_)[i]->y());
}

//---------------------------------------------------------------------------
//: Return the area of `this'
//---------------------------------------------------------------------------
double vsol_polygon_2d::area(void) const
{
  double area = 0.0;
  unsigned int last = storage_->size()-1;

  for (unsigned int i=0; i<last; ++i)
    area += ((*storage_)[i]->x() * (*storage_)[i+1]->y())
          - ((*storage_)[i+1]->x() * (*storage_)[i]->y());

  area += ((*storage_)[last]->x() * (*storage_)[0]->y())
        - ((*storage_)[0]->x() * (*storage_)[last]->y());

  return vcl_abs(area / 2.0);
}

//---------------------------------------------------------------------------
//: Is `this' convex ?
// A polygon is convex if the direction of "turning" at every vertex is
// the same.  This is checked by calculating the cross product of two
// consecutive edges and verifying that these all have the same sign.
//---------------------------------------------------------------------------
bool vsol_polygon_2d::is_convex(void) const
{
   if (storage_->size()==3) return true; // A triangle is always convex

   // First find a non-zero cross product.  This is certainly present,
   // unless the polygon collapses to a line segment.
   // Note that cross-product=0 means that two edges are parallel, which
   // is perfectly valid, but the other "turnings" should still all be in
   // the same direction.  An earlier implementation allowed for turning
   // in the other direction after a cross-product=0.

   double n = 0.0;
   for (unsigned int i=0; i<storage_->size(); ++i)
   {
     int j = (i>1) ? i-2 : i-2+storage_->size();
     int k = (i>0) ? i-1 : i-1+storage_->size();
     vsol_point_2d_sptr p0=(*storage_)[k];
     vsol_point_2d_sptr p1=(*storage_)[j];
     vsol_point_2d_sptr p2=(*storage_)[i];
     vgl_vector_2d<double> v1=p0->to_vector(*p1);
     vgl_vector_2d<double> v2=p1->to_vector(*p2);
     n = cross_product(v1,v2);
     if (n != 0.0)
       break;
   }
   if (n == 0.0)
     return true;

   for (unsigned int i=0; i<storage_->size(); ++i)
   {
     int j = (i>1) ? i-2 : i-2+storage_->size();
     int k = (i>0) ? i-1 : i-1+storage_->size();
     vsol_point_2d_sptr p0=(*storage_)[k];
     vsol_point_2d_sptr p1=(*storage_)[j];
     vsol_point_2d_sptr p2=(*storage_)[i];
     vgl_vector_2d<double> v1=p0->to_vector(*p1);
     vgl_vector_2d<double> v2=p1->to_vector(*p2);
     double n2 = cross_product(v1,v2);
     if (n2*n < 0)
       return false; // turns in the other direction
   }
   return true;
}
//---------------------------------------------------------------------------
//: Is testpt inside 'this' ?
// draw a line btw testpt and an arbitrary point inside polygon
// count no. of crossings. if odd number of crossings tespt is
// outside the polygon
//---------------------------------------------------------------------------
bool vsol_polygon_2d::is_inside(const vsol_point_2d_sptr testpt)
 {
   // compute centre
   double cx = 0;
   double cy = 0;
   int size = (*storage_).size();
   for (unsigned i=0; i<size; ++i) {
     cx += (*storage_)[i]->x();
     cy += (*storage_)[i]->y();
   }
   cx /= size;
   cy /= size;

   // compute a point outside the polygon.
   double ox = 0, oy = 0;
   for (unsigned i=0; i<size; ++i) {
     double tmp;
 
     tmp = (*storage_)[i]->x()-cx;
     if (tmp<0) tmp = -tmp;
     if (tmp>ox) ox = tmp;
 
     tmp = (*storage_)[i]->y()-cy;
     if (tmp<0) tmp = -tmp;
     if (tmp>oy) oy = tmp;
   }
   ox = cx + ox + oy + 1;
   oy = cy + ox + oy + 1;

   // count crossings.
   unsigned crossings = 0;
   for (unsigned i=0; i<size; ++i)
     if (vgl_lineseg_test_lineseg( (double) (*storage_)[i]->x(), (double) (*storage_)[i]->y(), (double)(*storage_)[(i+1)%size]->x(), (double) (*storage_)[(i+1)%size]->y(),  (double) ox, (double)oy,(double) testpt->x(), (double) testpt->y() ) )
       ++crossings;
 
   // inside iff there was an odd number of crossings.
   return crossings % 2 != 0;
 }


//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_polygon_2d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsol_spatial_object_2d::b_write(os);
  if (!storage_)
    vsl_b_write(os, false); // Indicate null pointer stored
  else
  {
    vsl_b_write(os, true); // Indicate non-null pointer stored
    vsl_b_write(os, *storage_);
  }
}

//: Binary load self from stream (not typically used)
void vsol_polygon_2d::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsol_spatial_object_2d::b_read(is);

    delete storage_;
    storage_ = new vcl_vector<vsol_point_2d_sptr>();
    bool null_ptr;
    vsl_b_read(is, null_ptr);
    if (!null_ptr)
      return;
    vsl_b_read(is, *storage_);
    break;
   default:
    vcl_cerr << "vsol_polygon_2d: unknown I/O version " << ver << '\n';
  }
}

//: Return IO version number;
short vsol_polygon_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_polygon_2d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//***************************************************************************
// Implementation
//***************************************************************************

//---------------------------------------------------------------------------
//: Default constructor.
//---------------------------------------------------------------------------
vsol_polygon_2d::vsol_polygon_2d(void)
{
  storage_=new vcl_vector<vsol_point_2d_sptr>();
}

bool vsol_polygon_2d::valid_vertices(const vcl_vector<vsol_point_2d_sptr> ) const
{
  return true;
}


//: Binary save vsol_polygon_2d to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_polygon_2d* p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}


//: Binary load vsol_polygon_2d from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_polygon_2d* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new vsol_polygon_2d();
    p->b_read(is);
  }
  else
    p = 0;
}


inline void vsol_polygon_2d::describe(vcl_ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  if (size() == 0)
    strm << "[null]";
  else {
    strm << "[Nverts=" << size()
         << " Area=" << area();
    for (unsigned int i=0; i<size(); ++i)
      strm << " p" << i << ':' << *(vertex(i));
    strm << ']';
  }
  strm << vcl_endl;
}
