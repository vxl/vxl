//:
// \file
#include <vsol/vsol_box_3d.h>
#include <vnl/vnl_math.h>

vsol_box_3d::vsol_box_3d()
{
  // vcl_cout << "making a boxology object" << vcl_endl;
}

vsol_box_3d::vsol_box_3d(vsol_box_3d const &b){
  _box = b._box;
}
vsol_box_3d::~vsol_box_3d()
{
  // vcl_cout << "deleting a boxology object" << vcl_endl;
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
bool vsol_box_3d::operator< (vsol_box_3d& box)

{
  float xmin_box = box.get_min_x(), ymin_box = box.get_min_y(),
    zmin_box = box.get_min_z();

  float xmax_box = box.get_max_x(), ymax_box = box.get_max_y(),
    zmax_box = box.get_max_z();

  float xmin = this->get_min_x(),ymin = this->get_min_y(),zmin = this->get_min_z();

  float xmax = this->get_max_x(),ymax = this->get_max_y(),zmax = this->get_max_z();

  if(!(xmin >= xmin_box)) return false;
  if(!(ymin >= ymin_box)) return false;
  if(!(zmin >= zmin_box)) return false;

  if(!(xmax >= xmin_box)) return false;
  if(!(ymax >= ymin_box)) return false;
  if(!(zmax >= zmin_box)) return false;

  if(!(xmin <= xmax_box)) return false;
  if(!(ymin <= ymax_box)) return false;
  if(!(zmin <= zmax_box)) return false;

  if(!(xmax <= xmax_box)) return false;
  if(!(ymax <= ymax_box)) return false;
  if(!(zmax <= zmax_box)) return false;

  return true;
}


static bool near_same(float f1, float f2, const float& tolerance)
{
  return vcl_fabs(f1-f2)<tolerance;
}


bool vsol_box_3d:: near_equal(vsol_box_3d& box, const float& tolerance)
{
  float xmin_box = box.get_min_x(), ymin_box = box.get_min_y(),
    zmin_box = box.get_min_z();

  float xmax_box = box.get_max_x(), ymax_box = box.get_max_y(),
    zmax_box = box.get_max_z();

  float xmin = this->get_min_x(),ymin = this->get_min_y(),zmin = this->get_min_z();

  float xmax = this->get_max_x(),ymax = this->get_max_y(),zmax = this->get_max_z();

  if(!near_same(xmin, xmin_box, tolerance)) return false;
  if(!near_same(ymin, ymin_box, tolerance)) return false;
  if(!near_same(zmin, zmin_box, tolerance)) return false;

  if(!near_same(xmax, xmax_box, tolerance)) return false;
  if(!near_same(ymax, ymax_box, tolerance)) return false;
  if(!near_same(zmax, zmax_box, tolerance)) return false;
  return true;
}
