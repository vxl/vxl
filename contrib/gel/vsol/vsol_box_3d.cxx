// This is gel/vsol/vsol_box_3d.cxx
#include "vsol_box_3d.h"
//:
// \file
#include <vcl_cassert.h>

vsol_box_3d::vsol_box_3d(vsol_box_3d const &b) : vbl_ref_count(), vul_timestamp()
{
  box_ = b.box_;
}

double vsol_box_3d::get_min_x()
{
  assert(!box_.empty());
  return (box_.min())[0];
}

double vsol_box_3d::get_max_x()
{
  assert(!box_.empty());
  return (box_.max())[0];
}

double vsol_box_3d::get_min_y()
{
  assert(!box_.empty());
  return (box_.min())[1];
}

double vsol_box_3d::get_max_y()
{
  assert(!box_.empty());
  return (box_.max())[1];
}

double vsol_box_3d::get_min_z()
{
  assert(!box_.empty());
  return (box_.min())[2];
}

double vsol_box_3d::get_max_z()
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
bool vsol_box_3d::operator< (vsol_box_3d& b)
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

bool vsol_box_3d::near_equal(vsol_box_3d& b, float tolerance)
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
