// This is gel/vsol/vsol_box_3d.cxx
#include <vbl/io/vbl_io_bounding_box.h>
#include "vsol_box_3d.h"
//:
// \file
#include <vcl_cassert.h>

double vsol_box_3d::get_min_x() const
{
  assert(!box_.empty());
  return (box_.min())[0];
}

double vsol_box_3d::get_max_x() const
{
  assert(!box_.empty());
  return (box_.max())[0];
}

double vsol_box_3d::get_min_y() const
{
  assert(!box_.empty());
  return (box_.min())[1];
}

double vsol_box_3d::get_max_y() const
{
  assert(!box_.empty());
  return (box_.max())[1];
}

double vsol_box_3d::get_min_z() const
{
  assert(!box_.empty());
  return (box_.min())[2];
}

double vsol_box_3d::get_max_z() const
{
  assert(!box_.empty());
  return (box_.max())[2];
}

void vsol_box_3d::add_point(double x, double y, double z)
{
  box_.update(x, y, z);
}

//: compare mins and maxs between this and the comp_box, grow to the bounding box
void vsol_box_3d::grow_minmax_bounds(vsol_box_3d & comp_box)
{
  if (comp_box.box_.empty()) return;
  if (box_.empty()) { operator=(comp_box); return; }
  box_.update(comp_box.get_min_x(),comp_box.get_min_y(),comp_box.get_min_z());
  box_.update(comp_box.get_max_x(),comp_box.get_max_y(),comp_box.get_max_z());
}

//-------------------------------------------------------------------
//:   Determines if this box is inside the right hand side box.
//    That is, all boundaries of *this must be on or inside the boundaries of b.
bool vsol_box_3d::operator< (vsol_box_3d& b) const
{
  if (box_.empty()) return true;
  if (b.box_.empty()) return false;
  return
    this->get_min_x() >= b.get_min_x() &&
    this->get_min_y() >= b.get_min_y() &&
    this->get_min_z() >= b.get_min_z() &&
    this->get_max_x() <= b.get_max_x() &&
    this->get_max_y() <= b.get_max_y() &&
    this->get_max_z() <= b.get_max_z();
}

inline static bool near_same(double f1, double f2, float tolerance)
{
  return f1-f2<tolerance && f2-f1<tolerance;
}

bool vsol_box_3d::near_equal(vsol_box_3d const& b, float tolerance) const
{
  if (box_.empty() && b.box_.empty()) return true;
  if (b.box_.empty() || b.box_.empty()) return false;
  return
    near_same(this->get_min_x(), b.get_min_x(), tolerance) &&
    near_same(this->get_min_y(), b.get_min_y(), tolerance) &&
    near_same(this->get_min_z(), b.get_min_z(), tolerance) &&
    near_same(this->get_max_x(), b.get_max_x(), tolerance) &&
    near_same(this->get_max_y(), b.get_max_y(), tolerance) &&
    near_same(this->get_max_z(), b.get_max_z(), tolerance);
}

void vsol_box_3d::reset_bounds()
{
  box_.reset();
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_box_3d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, box_);
}

//: Binary load self from stream (not typically used)
void vsol_box_3d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsl_b_read(is, box_);
    break;
   default:
    vcl_cerr << "vsol_box_3d: unknown I/O version " << ver << '\n';
  }
}

//: Return IO version number;
short vsol_box_3d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_box_3d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//external functions
vcl_ostream& operator<<(vcl_ostream& s, vsol_box_3d const& b)
{
  s << "[(" << b.get_min_x() << ' ' << b.get_min_y() << ' ' << b.get_min_z() << ")("
    << b.get_max_x() << ' ' << b.get_max_y() << ' ' << b.get_max_z() << ")]";
  return s;
}

//: Binary save vsol_box_3d_sptr to stream.
void
vsl_b_write(vsl_b_ostream &os, vsol_box_3d_sptr const& b)
{
  if (!b){
    vsl_b_write(os, false); // Indicate null boxer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null boxer stored
    b->b_write(os);
  }
}

//: Binary load vsol_box_3d_sptr from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_box_3d_sptr &b)
{
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
    {
     case 1: {
      vbl_bounding_box<double,3> box;
      vsl_b_read(is, box);
      b = new vsol_box_3d(box);
      break;
     }
     default:
      b = 0;
    }
  }
}
