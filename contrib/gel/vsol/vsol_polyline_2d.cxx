// This is gel/vsol/vsol_polyline_2d.cxx
#include <vsl/vsl_vector_io.h>
#include "vsol_polyline_2d.h"
//:
// \file

#include <vsol/vsol_point_2d.h>

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor
//---------------------------------------------------------------------------
vsol_polyline_2d::vsol_polyline_2d()
{
  storage_=new vcl_vector<vsol_point_2d_sptr>();
  p0_ = 0;
  p1_ = 0;
}

//---------------------------------------------------------------------------
//: Constructor from a vcl_vector of points
//---------------------------------------------------------------------------

vsol_polyline_2d::vsol_polyline_2d(const vcl_vector<vsol_point_2d_sptr> &new_vertices)
{
  storage_=new vcl_vector<vsol_point_2d_sptr>(new_vertices);
  int n = storage_->size();
  if(n<2)
    {
      p0_ = 0;
      p1_ = 0;
      return;
    }
  p0_ = (*storage_)[0];
  p1_ = (*storage_)[n-1];
}
//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_polyline_2d::vsol_polyline_2d(const vsol_polyline_2d &other)
{
  storage_=new vcl_vector<vsol_point_2d_sptr>(*other.storage_);
  for (unsigned int i=0;i<storage_->size();++i)
    (*storage_)[i]=new vsol_point_2d(*((*other.storage_)[i]));
  p0_ = other.p0_;
  p1_ = other.p1_;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_polyline_2d::~vsol_polyline_2d()
{
  delete storage_;
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_sptr vsol_polyline_2d::clone(void) const
{
  return new vsol_polyline_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_polyline_2d::p0(void) const
{
  return p0_;
}

//---------------------------------------------------------------------------
//: Return the last point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_polyline_2d::p1(void) const
{
  return p1_;
}

//---------------------------------------------------------------------------
//: Return vertex `i'
//  REQUIRE: valid_index(i)
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_polyline_2d::vertex(const int i) const
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
bool vsol_polyline_2d::operator==(const vsol_polyline_2d &other) const
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

bool vsol_polyline_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::CURVE &&
   ((vsol_curve_2d const&)obj).curve_type() == vsol_curve_2d::POLYLINE
  ? *this == (vsol_polyline_2d const&) (vsol_polyline_2d const&) obj
  : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the real type of a conic. It is a CURVE
//---------------------------------------------------------------------------
vsol_spatial_object_2d::vsol_spatial_object_2d_type
vsol_polyline_2d::spatial_type(void) const
{
  return CURVE;
}

//---------------------------------------------------------------------------
//: Return the length of `this'
//---------------------------------------------------------------------------
double vsol_polyline_2d::length(void) const
{
  assert(false); // TO DO
  return -1;
}


//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_polyline_2d::compute_bounding_box(void) const
{
  set_bounding_box((*storage_)[0]->x(), (*storage_)[0]->y());
  for (unsigned int i=1;i<storage_->size();++i)
    add_to_bounding_box((*storage_)[i]->x(), (*storage_)[i]->y());
}

//---------------------------------------------------------------------------
//: Return the number of vertices
//---------------------------------------------------------------------------
int vsol_polyline_2d::size(void) const
{
  return storage_->size();
}


//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first point of the curve
// Require: in(new_p0)
//---------------------------------------------------------------------------
void vsol_polyline_2d::set_p0(const vsol_point_2d_sptr &new_p0)
{
  p0_=new_p0;
  storage_->push_back(p0_);
}

//---------------------------------------------------------------------------
//: Set the last point of the curve
// Require: in(new_p1)
//---------------------------------------------------------------------------
void vsol_polyline_2d::set_p1(const vsol_point_2d_sptr &new_p1)
{
  p1_=new_p1;
  storage_->push_back(p0_);
}

//---------------------------------------------------------------------------
//: Add another point to the curve
//---------------------------------------------------------------------------
void vsol_polyline_2d::add_vertex(const vsol_point_2d_sptr &new_p)
{
  storage_->push_back(new_p);
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_polyline_2d::b_write(vsl_b_ostream &os) const
{
  if(!storage_)
    vsl_b_write(os, false); // Indicate null pointer stored
  else
    {
      vsl_b_write(os, true); // Indicate non-null pointer stored
      vsl_b_write(os, version());
      vsl_b_write(os, *storage_);
    }
}
//: Binary load self from stream (not typically used)
void vsol_polyline_2d::b_read(vsl_b_istream &is)
{
  if(!is)
    return;
  delete storage_;
  storage_ = new vcl_vector<vsol_point_2d_sptr>();
  p0_=0;
  p1_=0;
  bool null_ptr;
  vsl_b_read(is, null_ptr);
  if(!null_ptr)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    {
      vsl_b_read(is, *storage_);
      int n = storage_->size();
      if(n<2)
        break;
      p0_=(*storage_)[0];
      p1_=(*storage_)[n-1];
    }
  }
}
//: Return IO version number;
short vsol_polyline_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_polyline_2d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

  //: Return a platform independent string identifying the class
vcl_string vsol_polyline_2d::is_a() const
{
  return vcl_string("vsol_polyline_2d");
}

  //: Return true if the argument matches the string identifying the class or any parent class
bool vsol_polyline_2d::is_class(const vcl_string& cls) const
{
  return cls==vsol_polyline_2d::is_a();
}

//external functions
//just print the endpoints 
vcl_ostream& operator<<(vcl_ostream& s, vsol_polyline_2d const& p)
{
  vsol_point_2d_sptr p0 = p.p0(), p1 = p.p1();
  if(!p0||!p1)
    {
      s << "[null]";
      return s;      
    }
  s << '[' << *(p.p0()) << ' ' << *(p.p1()) << ']';
  return s;
}

//: Binary save vsol_polyline_2d_sptr to stream.
void
vsl_b_write(vsl_b_ostream &os, vsol_polyline_2d_sptr const& p)
{
  if (!p){
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    //non-null pointer will be written if internals of p are ok
    p->b_write(os);
  }
}

//: Binary load vsol_polyline_2d_sptr from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_polyline_2d_sptr &p)
{
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
    {
      short ver;
      vsl_b_read(is, ver);
      switch(ver)
        {
        case 1:
          {
            vcl_vector<vsol_point_2d_sptr> points;
            vsl_b_read(is, points);
            p = new vsol_polyline_2d(points);
            break;
          }
        default:
          p = 0;
        }
    }
}

