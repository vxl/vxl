#include <vsol/vsol_box_2d.h>

vsol_box_2d::vsol_box_2d()
{
  // vcl_cout << "making a boxology object" << vcl_endl;
}

vsol_box_2d::vsol_box_2d(vsol_box_2d const &b){
  _box = b._box;
}
vsol_box_2d::~vsol_box_2d()
{
  // vcl_cout << "deleting a boxology object" << vcl_endl;
  
}




double vsol_box_2d::get_min_x()
{
  return (_box.min())[0]; 
}

double vsol_box_2d::get_max_x()
{
  return (_box.max())[0]; 
}

double vsol_box_2d::get_min_y()
{
  return (_box.min())[1]; 
}

double vsol_box_2d::get_max_y()
{
  return (_box.max())[1]; 
}



void vsol_box_2d::set_min_x(const double& v)
{
  (_box.min())[0]=v;
}

void vsol_box_2d::set_max_x(const double& v)
{
  (_box.max())[0]=v;
}

void vsol_box_2d::set_min_y(const double& v)
{
  (_box.min())[1]=v;
}

void vsol_box_2d::set_max_y(const double& v)
{
  (_box.max())[1]=v;
}

// compare mins and maxs between this and the comp_box, grow to the bounding box
void vsol_box_2d::grow_minmax_bounds(vsol_box_2d & comp_box)
{
  if (comp_box.get_min_x() < get_min_x()) set_min_x(comp_box.get_min_x());
  if (comp_box.get_max_x() > get_max_x()) set_max_x(comp_box.get_max_x());
  if (comp_box.get_min_y() < get_min_y()) set_min_y(comp_box.get_min_y());
  if (comp_box.get_max_y() > get_max_y()) set_max_y(comp_box.get_max_y());
}

