// This is gel/vsol/vsol_polygon_3d.cxx
#include <iostream>
#include <cmath>
#include "vsol_polygon_3d.h"
//:
// \file
#include <cassert>
#include <vcl_compiler.h>
#include <vsl/vsl_vector_io.h>
#include <vsol/vsol_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>

//***************************************************************************
// Initialization
//***************************************************************************
void vsol_polygon_3d::compute_plane()
{
  std::vector<vgl_homg_point_3d<double> > pts;
  for (auto & pit : *storage_)
    pts.emplace_back(pit->x(),pit->y(),pit->z(),1.0);
  vgl_fit_plane_3d<double> fp(pts);
  fp.fit(0.1, &std::cerr);
  plane_ = fp.get_plane();
  plane_.normalize();
}

//----------------------------------------------------------------
// Default constructor
//----------------------------------------------------------------
vsol_polygon_3d::vsol_polygon_3d()
: storage_(nullptr)
{
}

//---------------------------------------------------------------------------
//: Constructor from a std::vector (not a geometric vector but a list of points).
// Require: new_vertices.size()>=3 and valid_vertices(new_vertices)
//---------------------------------------------------------------------------
vsol_polygon_3d::vsol_polygon_3d(std::vector<vsol_point_3d_sptr> const& new_vertices)
{
  // require
  assert(new_vertices.size()>=3);
  assert(valid_vertices(new_vertices));

  storage_=new std::vector<vsol_point_3d_sptr>(new_vertices);
  this->compute_plane();
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_polygon_3d::vsol_polygon_3d(vsol_polygon_3d const& other)
  : vsol_region_3d(other)
{
  //vsol_point_3d_sptr p;

  storage_=new std::vector<vsol_point_3d_sptr>(*other.storage_);
  for (unsigned int i=0;i<storage_->size();++i)
    (*storage_)[i]=new vsol_point_3d(*((*other.storage_)[i]));
  plane_ = other.plane_;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_polygon_3d::~vsol_polygon_3d()
{
  delete storage_;
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization.
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d* vsol_polygon_3d::clone(void) const
{
  return new vsol_polygon_3d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return vertex `i'.
// Require: valid_index(i)
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_polygon_3d::vertex(const int i) const
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
bool vsol_polygon_3d::operator==(vsol_polygon_3d const& other) const
{
  bool result = (this==&other);

  if (!result)
  {
    result = (storage_->size()==other.storage_->size());

    if (result)
    {
      vsol_point_3d_sptr p=(*storage_)[0];

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

//---------------------------------------------------------------------------
//: spatial object equality
//---------------------------------------------------------------------------
bool vsol_polygon_3d::operator==(vsol_spatial_object_3d const& obj) const
{
  return
    obj.cast_to_region() && obj.cast_to_region()->cast_to_polygon() &&
    *this == *obj.cast_to_region()->cast_to_polygon();
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_polygon_3d::compute_bounding_box(void) const
{
  set_bounding_box((*storage_)[0]->x(),
                   (*storage_)[0]->y(),
                   (*storage_)[0]->z());
  for (unsigned int i=1;i<storage_->size();++i)
    add_to_bounding_box((*storage_)[i]->x(),
                        (*storage_)[i]->y(),
                        (*storage_)[i]->z());
}

//---------------------------------------------------------------------------
//: Return the area of `this'
//---------------------------------------------------------------------------
double vsol_polygon_3d::area(void) const
{
  // TO DO
  std::cerr << "Warning: vsol_polygon_3d::area() has not been implemented yet\n";
  return -1;
}

//---------------------------------------------------------------------------
//: Is `this' convex ?
// A polygon is convex if it is planar and if moreover the direction of
// "turning" at every vertex is the same.  This is checked by calculating
// the cross product of two consecutive edges and verifying that these
// all have the same direction, i.e., that their pairwise dot products
// are all nonnegative (which proves the "turning") and equal to the
// product of their lengths (which proves coplanarity).
//---------------------------------------------------------------------------
bool vsol_polygon_3d::is_convex(void) const
{
   if (storage_->size()==3) return true; // A triangle is always convex

   // First find a non-zero cross product.  This is certainly present,
   // unless the polygon collapses to a line segment.
   // Note that cross-product=0 means that two edges are parallel, which
   // is perfectly valid, but the other "turnings" should still all be in
   // the same direction.  An earlier implementation allowed for turning
   // in the other direction after a cross-product=0.

   vgl_vector_3d<double> n = vgl_vector_3d<double>(0.0,0.0,0.0);
   for (unsigned int i=0; i<storage_->size(); ++i)
   {
     int j = (i>1) ? i-2 : i-2+storage_->size();
     int k = (i>0) ? i-1 : i-1+storage_->size();
     vsol_point_3d_sptr p0=(*storage_)[k];
     vsol_point_3d_sptr p1=(*storage_)[j];
     vsol_point_3d_sptr p2=(*storage_)[i];
     vgl_vector_3d<double> v1=p0->to_vector(*p1);
     vgl_vector_3d<double> v2=p1->to_vector(*p2);
     n = cross_product(v1,v2);
     if (n != vgl_vector_3d<double>(0.0,0.0,0.0))
       break;
   }
   if (n == vgl_vector_3d<double>(0.0,0.0,0.0))
     return true;

   for (unsigned int i=0; i<storage_->size(); ++i)
   {
     int j = (i>1) ? i-2 : i-2+storage_->size();
     int k = (i>0) ? i-1 : i-1+storage_->size();
     vsol_point_3d_sptr p0=(*storage_)[k];
     vsol_point_3d_sptr p1=(*storage_)[j];
     vsol_point_3d_sptr p2=(*storage_)[i];
     vgl_vector_3d<double> v1=p0->to_vector(*p1);
     vgl_vector_3d<double> v2=p1->to_vector(*p2);
     vgl_vector_3d<double> n2 = cross_product(v1,v2);
     if (dot_product(n2,n) < 0)
       return false; // turns in the other direction
     if (!parallel(n2,n,1e-6))
       return false; // non-planar
   }
   return true;
}

//---------------------------------------------------------------------------
//: Are `new_vertices' valid vertices to build a polygon of the current type?
//  That is are all vertices in the same plane ?
//---------------------------------------------------------------------------
bool vsol_polygon_3d::valid_vertices(const std::vector<vsol_point_3d_sptr> new_vertices) const
{
  double tol = 1e-06;
  if (new_vertices.size() <= 3) return true; // a triangle is always in a plane

  vsol_point_3d_sptr p0=new_vertices[0];
  vsol_point_3d_sptr p1=new_vertices[1];
  vsol_point_3d_sptr p2=new_vertices[2];

  vgl_vector_3d<double> v1 (p1->x()-p0->x(),
                            p1->y()-p0->y(),
                            p1->z()-p0->z());

  vgl_vector_3d<double> v2 (p2->x()-p0->x(),
                            p2->y()-p0->y(),
                            p2->z()-p0->z());
  vgl_vector_3d<double> n = normalized(cross_product(v1,v2));// normal to the plane made by the vertices

  for (unsigned int i=3;i<new_vertices.size();++i)
  {
    p2=new_vertices[i];
    v2=vgl_vector_3d<double>(p2->x()-p0->x(),
                             p2->y()-p0->y(),
                             p2->z()-p0->z());
#if 0
    if (dot_product(n,v2)!=0)
      return false;
#endif
    double dp = std::fabs(dot_product(n,v2));
    if (dp>tol)
      return false;
  }

  return true;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `p' in `this' ?
// \todo not yet implemented
//---------------------------------------------------------------------------
bool vsol_polygon_3d::in(vsol_point_3d_sptr const& ) const
{
  std::cerr << "Warning: vsol_polygon_3d::in() has not been implemented yet\n";
  return false;
}

//---------------------------------------------------------------------------
//: Return the unit normal vector at point `p'.
//---------------------------------------------------------------------------
vgl_vector_3d<double>
vsol_polygon_3d::normal_at_point(vsol_point_3d_sptr const& /*no p needed*/) const
{
  return plane_.normal();
}
//---------------------------------------------------------------------------
//: Return the unit normal vector
//---------------------------------------------------------------------------
vgl_vector_3d<double> vsol_polygon_3d::normal() const
{
  return plane_.normal();
}

//***************************************************************************
// Implementation
//***************************************************************************


inline void vsol_polygon_3d::describe(std::ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  if (size() == 0)
    strm << "[vsol_polygon_3d null]";
  else {
    strm << "[vsol_polygon_3d Nverts=" << size()
         << " Area=" << area();
    for (unsigned int i=0; i<size(); ++i)
      strm << " p" << i << ':' << *(vertex(i));
    strm << ']';
  }
  strm << std::endl;
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_polygon_3d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsol_spatial_object_3d::b_write(os);
  if (!storage_)
    vsl_b_write(os, false); // Indicate null pointer stored
  else
  {
    vsl_b_write(os, true); // Indicate non-null pointer stored
    vsl_b_write(os, *storage_);
  }
}

//: Binary load self from stream (not typically used)
void vsol_polygon_3d::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsol_spatial_object_3d::b_read(is);

    delete storage_;
    storage_ = new std::vector<vsol_point_3d_sptr>();
    bool valid_ptr;
    vsl_b_read(is, valid_ptr);
    if (!valid_ptr)
      return;
    vsl_b_read(is, *storage_);
    this->compute_plane();
    break;
   default:
    std::cerr << "vsol_polygon_3d: unknown I/O version " << ver << '\n';
  }
}

//: Return IO version number;
short vsol_polygon_3d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_polygon_3d::print_summary(std::ostream &os) const
{
  os << *this;
}

//: Binary save vsol_polygon_3d to stream.
void
vsl_b_write(vsl_b_ostream &os, vsol_polygon_3d const* p)
{
  if (p==nullptr) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}

//: Binary load vsol_polygon_3d from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_polygon_3d* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
   // p = new vsol_polygon_3d(std::vector<vsol_point_3d_sptr>());
    p = new vsol_polygon_3d();
    p->b_read(is);
  }
  else
    p = nullptr;
}
