// This is gel/vsol/vsol_polygon_3d.cxx
#include "vsol_polygon_3d.h"
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vsol/vsol_point_3d.h>
#include <vgl/vgl_vector_3d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from a vcl_vector (not a geometric vector but a list of points).
// Require: new_vertices.size()>=3 and valid_vertices(new_vertices)
//---------------------------------------------------------------------------
vsol_polygon_3d::vsol_polygon_3d(const vcl_vector<vsol_point_3d_sptr> &new_vertices)
{
  // require
  assert(new_vertices.size()>=3);
  assert(valid_vertices(new_vertices));

  storage_=new vcl_vector<vsol_point_3d_sptr>(new_vertices);
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_polygon_3d::vsol_polygon_3d(const vsol_polygon_3d &other)
{
  //vsol_point_3d_sptr p;

  storage_=new vcl_vector<vsol_point_3d_sptr>(*other.storage_);
  for (unsigned int i=0;i<storage_->size();++i)
    (*storage_)[i]=new vsol_point_3d(*((*other.storage_)[i]));
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
vsol_spatial_object_3d_sptr vsol_polygon_3d::clone(void) const
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
bool vsol_polygon_3d::operator==(const vsol_polygon_3d &other) const
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
bool vsol_polygon_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::REGION &&
   ((vsol_region_3d const&)obj).region_type() == vsol_region_3d::POLYGON
  ? *this == (vsol_polygon_3d const&) (vsol_region_3d const&) obj
  : false;
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
  vcl_cerr << "Warning: vsol_polygon_3d::area() has not been implemented yet\n";
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
bool vsol_polygon_3d::valid_vertices(const vcl_vector<vsol_point_3d_sptr> new_vertices) const
{
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
  vgl_vector_3d<double> n = cross_product(v1,v2);// normal to the plane made by the vertices

  for (unsigned int i=3;i<new_vertices.size();++i)
  {
    p2=new_vertices[i];
    v2=vgl_vector_3d<double>(p2->x()-p0->x(),
                             p2->y()-p0->y(),
                             p2->z()-p0->z());
    if (dot_product(n,v2)!=0)
      return false;
  }

  return true;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `p' in `this' ?
//---------------------------------------------------------------------------
bool vsol_polygon_3d::in(const vsol_point_3d_sptr& ) const
{
  // TODO
  vcl_cerr << "Warning: vsol_polygon_3d::in() has not been implemented yet\n";
  return false;
}

//---------------------------------------------------------------------------
//: Return the unit normal vector at point `p'.
// Require: in(p)
//---------------------------------------------------------------------------
vgl_vector_3d<double>
vsol_polygon_3d::normal_at_point(const vsol_point_3d_sptr &p) const
{
  // require
  assert(in(p));

  // Since a polygon is planar, the answer is independent of p:
  vsol_point_3d_sptr p0=(*storage_)[0];
  vsol_point_3d_sptr p1=(*storage_)[1];
  vsol_point_3d_sptr p2=(*storage_)[2];

  vgl_vector_3d<double> v1(p1->x()-p0->x(),
                           p1->y()-p0->y(),
                           p1->z()-p0->z());
  vgl_vector_3d<double> v2(p2->x()-p0->x(),
                           p2->y()-p0->y(),
                           p2->z()-p0->z());

  return normalized(cross_product(v1,v2));
}

//***************************************************************************
// Implementation
//***************************************************************************

//---------------------------------------------------------------------------
//: Default constructor. Do nothing. Just to enable inheritance.
//---------------------------------------------------------------------------
vsol_polygon_3d::vsol_polygon_3d(void)
{
}

inline void vsol_polygon_3d::describe(vcl_ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << "[vsol_polyline_3d";
  for (unsigned int i=0; i<size(); ++i)
    strm << ' ' << *(vertex(i));
  strm << ']' << vcl_endl;
}
