#include <vsol/vsol_box_3d.h>

vsol_box_3d::vsol_box_3d()
{
  // cout << "making a boxology object" << endl;
}

vsol_box_3d::vsol_box_3d(vsol_box_3d const &b){
  _box = b._box;
}
vsol_box_3d::~vsol_box_3d()
{
  // cout << "deleting a boxology object" << endl;
  
}




double vsol_box_3d::get_min_x()
{
  return (_box.get_min())[0]; 
}

double vsol_box_3d::get_max_x()
{
  return (_box.get_max())[0]; 
}

double vsol_box_3d::get_min_y()
{
  return (_box.get_min())[1]; 
}

double vsol_box_3d::get_max_y()
{
  return (_box.get_max())[1]; 
}

double vsol_box_3d::get_min_z()
{
  return (_box.get_min())[2]; 
}

double vsol_box_3d::get_max_z()
{
  return (_box.get_max())[2]; 
}


void vsol_box_3d::set_min_x(const double& v)
{
  (_box.get_min())[0]=v;
}

void vsol_box_3d::set_max_x(const double& v)
{
  (_box.get_max())[0]=v;
}

void vsol_box_3d::set_min_y(const double& v)
{
  (_box.get_min())[1]=v;
}

void vsol_box_3d::set_max_y(const double& v)
{
  (_box.get_max())[1]=v;
}

void vsol_box_3d::set_min_z(const double& v)
{
  (_box.get_min())[2]=v;
}

void vsol_box_3d::set_max_z(const double& v)
{
  (_box.get_max())[2]=v;
}

