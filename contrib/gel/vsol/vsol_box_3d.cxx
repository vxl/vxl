#include <vsol/vsol_box_3d.h>

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

