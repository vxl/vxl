// This is gel/vsol/vsol_box_3d.cxx
#include "vsol_box_3d.h"
//:
// \file

vsol_box_3d::vsol_box_3d()
{
  // vcl_cout << "making a boxology object\n";
}

vsol_box_3d::vsol_box_3d(vsol_box_3d const &b){
  _box = b._box;
}

vsol_box_3d::~vsol_box_3d()
{
  // vcl_cout << "deleting a boxology object\n";
}

double vsol_box_3d::get_min_x()
{
  return (_box.min())[0];
}

double vsol_box_3d::get_max_x()
{
  return (_box.max())[0];
}

double vsol_box_3d::get_min_y()
{
  return (_box.min())[1];
}

double vsol_box_3d::get_max_y()
{
  return (_box.max())[1];
}

double vsol_box_3d::get_min_z()
{
  return (_box.min())[2];
}

double vsol_box_3d::get_max_z()
{
  return (_box.max())[2];
}


void vsol_box_3d::set_min_x(const double& v)
{
  (_box.min())[0]=v;
}

void vsol_box_3d::set_max_x(const double& v)
{
  (_box.max())[0]=v;
}

void vsol_box_3d::set_min_y(const double& v)
{
  (_box.min())[1]=v;
}

void vsol_box_3d::set_max_y(const double& v)
{
  (_box.max())[1]=v;
}

void vsol_box_3d::set_min_z(const double& v)
{
  (_box.min())[2]=v;
}

void vsol_box_3d::set_max_z(const double& v)
{
  (_box.max())[2]=v;
}


//: compare mins and maxs between this and the comp_box, grow to the bounding box
void vsol_box_3d::grow_minmax_bounds(vsol_box_3d & comp_box)
{
  if (comp_box.get_min_x() < get_min_x()) set_min_x(comp_box.get_min_x());
  if (comp_box.get_max_x() > get_max_x()) set_max_x(comp_box.get_max_x());
  if (comp_box.get_min_y() < get_min_y()) set_min_y(comp_box.get_min_y());
  if (comp_box.get_max_y() > get_max_y()) set_max_y(comp_box.get_max_y());
  if (comp_box.get_min_z() < get_min_z()) set_min_z(comp_box.get_min_z());
  if (comp_box.get_max_z() > get_max_z()) set_max_z(comp_box.get_max_z());

  return;
}

double vsol_box_3d::width()
{
  return get_max_x() - get_min_x();
}

double vsol_box_3d::height()
{
  return get_max_y() - get_min_y();
}


//-------------------------------------------------------------------
//:   Determines if *this is inside the input IUBox.
//    That is, all boundaries of *this must be on or
//    inside the boundaries of box.
bool vsol_box_3d::operator< (vsol_box_3d& b)
{
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
  return 
    near_same(this->get_min_x(), b.get_min_x(), tolerance) &&
    near_same(this->get_min_y(), b.get_min_y(), tolerance) &&
    near_same(this->get_min_z(), b.get_min_z(), tolerance) &&
    near_same(this->get_max_x(), b.get_max_x(), tolerance) &&
    near_same(this->get_max_y(), b.get_max_y(), tolerance) &&
    near_same(this->get_max_z(), b.get_max_z(), tolerance);
}
