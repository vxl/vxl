// This is gel/vsol/vsol_orient_box_3d.cxx
#include <vbl/io/vbl_io_bounding_box.h>
#include "vsol_orient_box_3d.h"
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_box_3d.h>
//:
// \file
#include <vcl_cassert.h>

vsol_orient_box_3d::vsol_orient_box_3d(vgl_orient_box_3d<double> const& orient_box) 
:orient_box_(orient_box) {
  //create a bounding box from the 8 corner points of the oriented box
  vcl_vector<vgl_point_3d<double> > corners = orient_box_.corners();
  assert (corners.size() == 8);

  for (unsigned int i=0; i < corners.size(); i++) {
    vgl_point_3d<double> corner = corners[i];
    box_.update(corner.x(), corner.y(), corner.z());
  }
  this->set_bounding_box(new vsol_box_3d(box_));
}

double vsol_orient_box_3d::get_min_x() const
{
  assert(!box_.empty());
  return (box_.min())[0];
}

double vsol_orient_box_3d::get_max_x() const
{
  assert(!box_.empty());
  return (box_.max())[0];
}

double vsol_orient_box_3d::get_min_y() const
{
  assert(!box_.empty());
  return (box_.min())[1];
}

double vsol_orient_box_3d::get_max_y() const
{
  assert(!box_.empty());
  return (box_.max())[1];
}

double vsol_orient_box_3d::get_min_z() const
{
  assert(!box_.empty());
  return (box_.min())[2];
}

double vsol_orient_box_3d::get_max_z() const
{
  assert(!box_.empty());
  return (box_.max())[2];
}

bool vsol_orient_box_3d::in(vsol_point_3d_sptr const& p) const
{
  // first test if the point inside the bounding box
  if (box_.inside(p->x(), p->y(), p->z())) {
    // now check if the rotated box contains the point
    if (orient_box_.contains(p->x(), p->y(), p->z()))
      return true;
  } 

  return false;
}

void vsol_orient_box_3d::add_point(double x, double y, double z)
{
  box_.update(x, y, z);
}
