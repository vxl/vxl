// This is gel/vsol/vsol_box_2d.cxx
#include <vbl/io/vbl_io_bounding_box.h>
#include "vsol_box_2d.h"
//:
// \file
#include <vcl_cassert.h>

vsol_box_2d::vsol_box_2d(vsol_box_2d const &b)
{
  box_ = b.box_;
}

double vsol_box_2d::get_min_x() const
{
  assert(!box_.empty());
  return (box_.min())[0];
}

double vsol_box_2d::get_max_x() const
{
  assert(!box_.empty());
  return (box_.max())[0];
}

double vsol_box_2d::get_min_y() const
{
  assert(!box_.empty());
  return (box_.min())[1];
}

double vsol_box_2d::get_max_y() const
{
  assert(!box_.empty());
  return (box_.max())[1];
}

void vsol_box_2d::add_point(double x, double y)
{
  box_.update(x, y);
}

// compare mins and maxs between this and the comp_box, grow to the bounding box
void vsol_box_2d::grow_minmax_bounds(vsol_box_2d & comp_box)
{
  if (comp_box.box_.empty()) return;
  if (box_.empty()) { operator=(comp_box); return; }
  box_.update(comp_box.get_min_x(),comp_box.get_min_y());
  box_.update(comp_box.get_max_x(),comp_box.get_max_y());
}

//-------------------------------------------------------------------
//:   Determines if this box is inside the right hand side box.
//    That is, all boundaries of *this must be on or inside the boundaries of b.
bool vsol_box_2d::operator< (vsol_box_2d& b)
{
  if (box_.empty()) return true;
  if (b.box_.empty()) return false;
  return
    this->get_min_x() >= b.get_min_x() &&
    this->get_min_y() >= b.get_min_y() &&
    this->get_max_x() <= b.get_max_x() &&
    this->get_max_y() <= b.get_max_y();
}

inline static bool near_same(double f1, double f2, float tolerance)
{
  return f1-f2<tolerance && f2-f1<tolerance;
}

bool vsol_box_2d::near_equal(vsol_box_2d& b, float tolerance)
{
  if (box_.empty() && b.box_.empty()) return true;
  if (b.box_.empty() || b.box_.empty()) return false;
  return
    near_same(this->get_min_x(), b.get_min_x(), tolerance) &&
    near_same(this->get_min_y(), b.get_min_y(), tolerance) &&
    near_same(this->get_max_x(), b.get_max_x(), tolerance) &&
    near_same(this->get_max_y(), b.get_max_y(), tolerance);
}

void vsol_box_2d::reset_bounds()
{
  box_.reset();
}


//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_box_2d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, box_);
}

//: Binary load self from stream (not typically used)
void vsol_box_2d::b_read(vsl_b_istream &is)
{
  if(!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    {
      vsl_b_read(is, box_);
    }
  }
}
//: Return IO version number;
short vsol_box_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_box_2d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

  //: Return a platform independent string identifying the class
vcl_string vsol_box_2d::is_a() const
{
  return vcl_string("vsol_box_2d");
}

  //: Return true if the argument matches the string identifying the class or any parent class
bool vsol_box_2d::is_class(const vcl_string& cls) const
{
  return cls==vsol_box_2d::is_a();
}

//external functions
vcl_ostream& operator<<(vcl_ostream& s, vsol_box_2d const& b)
{
  s << "[(" << b.get_min_x() << ' ' << b.get_min_y() << ")(" 
    << b.get_max_x() << ' ' << b.get_max_y() << ")]";
  return s;
}

//: Binary save vsol_box_2d_sptr to stream.
void
vsl_b_write(vsl_b_ostream &os, vsol_box_2d_sptr const& b)
{
  if (!b){
    vsl_b_write(os, false); // Indicate null boxer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null boxer stored
    b->b_write(os);
  }
}

//: Binary load vsol_box_2d_sptr from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_box_2d_sptr &b)
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
            vbl_bounding_box<double,2> box;
            vsl_b_read(is, box);
            b = new vsol_box_2d(box);
            break;
          }
        default:
          b = 0;
        }
    }
}
