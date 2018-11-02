// This is gel/vsol/vsol_poly_set_2d.cxx
#include <iostream>
#include <cmath>
#include "vsol_poly_set_2d.h"
//:
// \file
#include <vsl/vsl_vector_io.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsol/vsol_polygon_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from a std::vector (not a geometric vector but a list of points)
// Require: new_vertices.size()>=3
//---------------------------------------------------------------------------
vsol_poly_set_2d::vsol_poly_set_2d(const std::vector<vsol_polygon_2d_sptr> &new_polys)
  : vsol_region_2d()
{
  storage_=new std::vector<vsol_polygon_2d_sptr>(new_polys);
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_poly_set_2d::vsol_poly_set_2d(const vsol_poly_set_2d &other)
  : vsol_region_2d(other)
{
  //vsol_point_2d_sptr p;
  storage_=new std::vector<vsol_polygon_2d_sptr>(*other.storage_);
  for (unsigned int i=0;i<storage_->size();++i)
    (*storage_)[i]=new vsol_polygon_2d(*((*other.storage_)[i]));
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_poly_set_2d::~vsol_poly_set_2d()
{
  for (auto & i : *storage_)
   i = nullptr;
  delete storage_;
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vsol_poly_set_2d::clone(void) const
{
  return new vsol_poly_set_2d(*this);
}

//***************************************************************************
// Safe casting
//***************************************************************************

#if 0
vsol_poly_set_2d* vsol_poly_set_2d::cast_to_poly_set(void)
{
  if (!cast_to_triangle()||!cast_to_rectangle())
    return this;
  else
    return 0;
}

const vsol_poly_set_2d* vsol_poly_set_2d::cast_to_poly_set(void) const
{
  if (!cast_to_triangle()||!cast_to_rectangle())
    return this;
  else
    return 0;
}

vsol_triangle_2d* vsol_poly_set_2d::cast_to_triangle(void){return 0;}
const vsol_triangle_2d* vsol_poly_set_2d::cast_to_triangle(void) const{return 0;}

vsol_rectangle_2d* vsol_poly_set_2d::cast_to_rectangle(void){return 0;}
const vsol_rectangle_2d* vsol_poly_set_2d::cast_to_rectangle(void) const{return 0;}

#endif // 0

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return vertex `i'
// Require: valid_index(i)
//---------------------------------------------------------------------------
vsol_polygon_2d_sptr vsol_poly_set_2d::poly(const int i) const
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
bool vsol_poly_set_2d::operator==(const vsol_poly_set_2d &other) const
{
  bool result = (this==&other);

  if (!result)
  {
    result = (storage_->size()==other.storage_->size());
    if (result)
    {
      vsol_polygon_2d_sptr p=(*storage_)[0];

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

bool vsol_poly_set_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
    obj.cast_to_region() && obj.cast_to_region()->cast_to_poly_set() &&
    *this == *obj.cast_to_region()->cast_to_poly_set();
}


//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_poly_set_2d::compute_bounding_box(void) const
{
  set_bounding_box((*storage_)[0]->get_bounding_box());
  for (unsigned int i=1;i<storage_->size();++i)
    add_to_bounding_box((*storage_)[i]->get_bounding_box());
}

//---------------------------------------------------------------------------
//: Return the area of `this'
//---------------------------------------------------------------------------
double vsol_poly_set_2d::area(void) const
{
  double area = 0.0;
  unsigned int last = storage_->size()-1;

  for (unsigned int i=0; i<last; ++i)
    area += ((*storage_)[i]->area());;

  return std::abs(area / 2.0);
}

//---------------------------------------------------------------------------
//: Return the centroid of `this'
//---------------------------------------------------------------------------
// The centroid is computed by using Green's theorem to compute the
// area-weighted 1st moments of the poly_set.
//  Green's theorem relates the surface integral to the line integral around
//  the boundary as:
//     Int(surface) x dxdy = 0.5 * Int(boundary) x*x dy
//     Int(surface) y dxdy = 0.5 * Int(boundary) y*y dx
//  The centroid is given by
//     xc = Int(surface) x dxdy / Int(surface) dxdy  = Int(surface) x dxdy/area
//     yc = Int(surface) y dxdy / Int(surface) dxdy  = Int(surface) y dxdy/area
//
//  For a poly_set: with vertices x[i], y[i]
//   0.5 * Int(boundary) x*x dy =
//   1/6 * Sum(i)( x[i+1] + x[i] ) * ( x[i] * y[i+1] - x[i+1] * y[i] )
//
//   0.5 * Int(boundary) y*y dx =
//   1/6 * Sum(i)( y[i+1] + y[i] ) * ( x[i] * y[i+1] - x[i+1] * y[i] )
//
//  In the case of degenerate poly_sets, where area == 0, return the average of
//  the vertex locations.
//
vsol_point_2d_sptr vsol_poly_set_2d::centroid(void) const
{
  std::vector<vsol_point_2d_sptr> p;
  for (auto & i : *storage_)
  {
    vsol_point_2d_sptr c = i->centroid();
    p.push_back(c);
  }
  vsol_polygon_2d poly(p);
  return poly.centroid();
}

//---------------------------------------------------------------------------
//: Is `this' convex ?
// A poly_set is convex if the direction of "turning" at every vertex is
// the same.  This is checked by calculating the cross product of two
// consecutive edges and verifying that these all have the same sign.
//---------------------------------------------------------------------------
bool vsol_poly_set_2d::is_convex(void) const
{
  // First find a non-zero cross product.  This is certainly present,
  // unless the poly_set collapses to a line segment.
  // Note that cross-product=0 means that two edges are parallel, which
  // is perfectly valid, but the other "turnings" should still all be in
  // the same direction.  An earlier implementation allowed for turning
  // in the other direction after a cross-product=0.

  for (auto & i : *storage_)
  {
    if (i->is_convex())
      return true;
  }
  return false;
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_poly_set_2d::b_write(vsl_b_ostream &os) const
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
void vsol_poly_set_2d::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsol_spatial_object_2d::b_read(is);

    delete storage_;
    storage_ = new std::vector<vsol_polygon_2d_sptr>();
    bool null_ptr;
    vsl_b_read(is, null_ptr);
    if (!null_ptr)
      return;
    vsl_b_read(is, *storage_);
    break;
   default:
    std::cerr << "vsol_poly_set_2d: unknown I/O version " << ver << '\n';
  }
}

//: Return IO version number;
short vsol_poly_set_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_poly_set_2d::print_summary(std::ostream &os) const
{
  os << *this;
}

//***************************************************************************
// Implementation
//***************************************************************************

//---------------------------------------------------------------------------
//: Default constructor.
//---------------------------------------------------------------------------
vsol_poly_set_2d::vsol_poly_set_2d(void)
{
  storage_=new std::vector<vsol_polygon_2d_sptr>();
}


//: Binary save vsol_poly_set_2d to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_poly_set_2d* p)
{
  if (p==nullptr) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}


//: Binary load vsol_poly_set_2d from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_poly_set_2d* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new vsol_poly_set_2d();
    p->b_read(is);
  }
  else
    p = nullptr;
}


inline void vsol_poly_set_2d::describe(std::ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  if (size() == 0)
    strm << "[null]";
  else {
    strm << "[Nverts=" << size()
         << " Area=" << area();
    for (unsigned int i=0; i<size(); ++i)
      strm << " p" << i << ':' << *(poly(i));
    strm << ']';
  }
  strm << std::endl;
}
