// This is gel/vsol/vsol_polygon_2d.cxx
#include <vsl/vsl_vector_io.h>
#include "vsol_polygon_2d.h"
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_vector_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from a vcl_vector (not a geometric vector but a list of points)
// Require: new_vertices.size()>=3
//---------------------------------------------------------------------------
vsol_polygon_2d::vsol_polygon_2d(const vcl_vector<vsol_point_2d_sptr> &new_vertices)
{
  // require
  assert(new_vertices.size()>=3);

  storage_=new vcl_vector<vsol_point_2d_sptr>(new_vertices);
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_polygon_2d::vsol_polygon_2d(const vsol_polygon_2d &other)
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
        for (int j=1;j<size()&&result;++i,++j)
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
   obj.spatial_type() == vsol_spatial_object_2d::REGION &&
   ((vsol_region_2d const&)obj).region_type() == vsol_region_2d::POLYGON
  ? *this == (vsol_polygon_2d const&) (vsol_region_2d const&) obj
  : false;
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
//: Return the number of vertices
//---------------------------------------------------------------------------
int vsol_polygon_2d::size(void) const
{
  return storage_->size();
}

//---------------------------------------------------------------------------
//: Return the area of `this'
//---------------------------------------------------------------------------
double vsol_polygon_2d::area(void) const
{
  vcl_cerr << "Warning: vsol_polygon_2d::area() has not yet been implemented\n";
  return -1; // TO DO
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

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_polygon_2d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsol_spatial_object_2d::b_write(os);
  if(!storage_)
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
  switch(ver)
  {
  case 1:
    {
      vsol_spatial_object_2d::b_read(is);

      delete storage_;
      storage_ = new vcl_vector<vsol_point_2d_sptr>();
      bool null_ptr;
      vsl_b_read(is, null_ptr);
      if(!null_ptr)
        return;
      vsl_b_read(is, *storage_);
    }
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

  //: Return a platform independent string identifying the class
vcl_string vsol_polygon_2d::is_a() const
{
  return vcl_string("vsol_polygon_2d");
}

  //: Return true if the argument matches the string identifying the class or any parent class
bool vsol_polygon_2d::is_class(const vcl_string& cls) const
{
  return cls==vsol_polygon_2d::is_a();
}

//external functions
vcl_ostream& operator<<(vcl_ostream& s, vsol_polygon_2d const& p)
{
  if(p.size())
    {
      s << "[Nverts:" << p.size()  << " p0:" << *(p.vertex(0)) << ']';
      return s;
    }
  s << "[null]";
  return s;
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