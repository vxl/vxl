#include "bvpl_kernel_factory.h"
//:
// \file
#include <vcl_map.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_limits.h>
#include <vcl_cmath.h>

//: Saves the kernel to Dristhi .raw data format. 
// The kernel does not occupy the entire volume, so the empty voxels at set to 0.
// The size of the box is max(x,y,z) * max(x,y,z) * max(x,y,z) 
bool bvpl_kernel::save_raw(vcl_string filename)
{

  
  vcl_fstream ofs(filename.c_str(), vcl_ios::binary | vcl_ios::out);
  if (!ofs.is_open()) {
    vcl_cerr << "error opening file " << filename << " for write!\n";
    return false;
  }

  // write header
  unsigned char data_type = 1; // 1 means signed byte
  
  vxl_uint_32 nx = (max_.x() - min_.x()) + 1;
  vxl_uint_32 ny = (max_.y() - min_.y()) + 1;
  vxl_uint_32 nz = (max_.z() - min_.z()) + 1;
  
  
  ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));

  // write data
  // iterate through slabs and fill in memory array
  unsigned size = nx*ny*nz;
  char *data_array = new char[size];

  
  //get the range of the grid
  char max = char(-1) * vcl_numeric_limits<char>::infinity();
  char min = vcl_numeric_limits<char>::infinity();

  kernel_.begin();
  while (!kernel_.isDone()){ 
    if ((*kernel_).c_> max)
      max = (*kernel_).c_;
    if ((*kernel_).c_< min)
      min =(*kernel_).c_;
    ++kernel_; 
  }

  
  vcl_cout << "max: " << int(max) <<vcl_endl;
  vcl_cout << "min: " << int(min) <<vcl_endl;
  
  //Since our kernel does not occupy the entire space we need to initialize our data
  for (unsigned i = 0; i < size; i++)
    data_array[i] = min;

  kernel_.begin();
  while (!kernel_.isDone()){
    vgl_point_3d<int> coord = kernel_.index();
    int index = (coord.x()-min_.x())*ny*nz + (coord.y()-min_.y())*nz + (coord.z() - min_.z());
    data_array[index] =(char)((*kernel_).c_);
    ++kernel_;
  }

  vcl_cout << vcl_endl;
  ofs.write(reinterpret_cast<char*>(data_array),sizeof(char)*nx*ny*nz);

  ofs.close();

  delete[] data_array;

  return true;
}

//: Returns a kernel along the local rotation_axis_ and rotated around this axis an amount angle_
//  The user can modified the axis and angle using set_rotation_axis() and set_angle()
bvpl_kernel
bvpl_kernel_factory::create()
{
  bvpl_kernel_iterator iter = interpolate(rotate(angle_)); 
  bvpl_kernel kernel(iter, dim(), max3d_, min3d_);

  return kernel;
}

//: Returns a kernel along the rotation_axis and rotated around this axis an amount angle
bvpl_kernel
bvpl_kernel_factory::create(vnl_vector_fixed<float,3> rotation_axis, float angle)
{
  this->set_rotation_axis(rotation_axis);
  return bvpl_kernel(interpolate(rotate(angle)), dim(), max3d_, min3d_);
}

//: Rounds coordinates of kernel to the nearest integer
bvpl_kernel_iterator
bvpl_kernel_factory::interpolate(kernel_type const& kernel)
{
  kernel_type::const_iterator kernel_it = kernel.begin();
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> >  kernel_out;

    //Kernels shouldn't get any bigger than this, so this initial values work
  int max_x =-100;
  int max_y =-100;
  int max_z =-100;
  int min_x = 100;
  int min_y =100;
  int min_z = 100;

  for (; kernel_it != kernel.end(); ++kernel_it)
  {
      int x0 = (int)vcl_floor((kernel_it->first).x()+0.5f);
      int y0 = (int)vcl_floor((kernel_it->first).y()+0.5f);
      int z0 = (int)vcl_floor((kernel_it->first).z()+0.5f);
      kernel_out.push_back(vcl_make_pair(vgl_point_3d<int>(x0,y0,z0), (kernel_it->second)));


    if ( x0 > max_x) max_x =  x0;
    if ( y0 > max_y) max_y =  y0;
    if ( z0 > max_z) max_z =  z0;

    if ( x0 < min_x) min_x =  x0;
    if ( y0 < min_y) min_y =  y0;
    if ( z0 < min_z) min_z =  z0;
  }

  max3d_.set(max_x,max_y,max_z);
  min3d_.set(min_x,min_y,min_z);

  return kernel_out;
}

//: Aligns the edge along direction of "axis"
void bvpl_kernel_factory::set_rotation_axis( vnl_vector_fixed<float,3> rotation_axis)
{
  // rotation axis should be unit vector
  float mag =rotation_axis.magnitude();
  if (mag > vcl_numeric_limits<float>::epsilon())
    rotation_axis /= mag;
  else {
    vcl_cout << "Rotation axis magnitude is zero, returning withount modifycation of kernel\n";
    return ;
  }

  //set axis of rotation
  rotation_axis_ = rotation_axis;

  //spherical coordinates of the rotation axis.

  float radius = 1.0,
        /* theta = vcl_atan2(rotation_axis[1],rotation_axis[0]), //azimuth, unused */
        phi = vcl_acos(rotation_axis[2]/radius); //elevation


  //construct a rotation to rotate vector a to vector b
  vgl_rotation_3d<float> r_align(canonical_rotation_axis_, rotation_axis);
  kernel_= rotate(r_align);

  //rotate the parallel axis. 
  vnl_vector_fixed<float,3> parallerl_axis = r_align.as_matrix() * canonical_parallel_axis_;

  //spherical coordinates of the parallel axis.

  float /* theta_p = vcl_atan2(parallerl_axis[1],parallerl_axis[0]), //azimuth, unused */
        phi_p = vcl_acos(parallerl_axis[2]/radius); //elevation


  //parallel axis needs to be rotated to have same polar angle and rotation axis

  //If the rotation axis is pointing to the poles, there is an exception
  if ((rotation_axis_ != vnl_vector_fixed<float, 3>(0.0f, 0.0f, 1.0f))&&
      (rotation_axis_ != vnl_vector_fixed<float, 3>(0.0f, 0.0f, -1.0f))){
    float correction_phi = phi_p - phi; 
    vgl_rotation_3d<float> r_correct(vnl_quaternion<float>(rotation_axis, correction_phi));
     //rotate correction_phi around new axis of rotation. This position is the 0-rotation.
    kernel_ = rotate(r_correct);
  }
  else { //make sure parallel axis is aligned with x-axis
    if ((phi_p - float(vnl_math::pi_over_2))>vcl_numeric_limits<float>::epsilon()){
      vcl_cerr << "Error when aligning rotation axis to the z axis\n" ;
      return;
    }
  }

  return;
}

//: Rotates "class-kernel_" around "class-rotation_axis_"  by an "angle"
bvpl_kernel_factory::kernel_type
bvpl_kernel_factory::rotate(float angle)
{
  //construct a quternion to represent the rotation
  float mag =rotation_axis_.magnitude();
  if (mag > double(0)){
    vnl_quaternion<float> q(rotation_axis_/mag,angle);
    return this->rotate(vgl_rotation_3d<float>(q));
  }
  else {// identity rotation is a special case
    vcl_cout << "magnitude of rotation axis is zero, returning withount modifying kernel\n";
    return kernel_;
  }
}

//: Rotates "class-kernel_" using the given rotation matrix
bvpl_kernel_factory::kernel_type
bvpl_kernel_factory::rotate(vgl_rotation_3d<float> R)
{

  vcl_cout << R.as_matrix() << vcl_endl;
  vcl_cout << R.as_rodrigues() << vcl_endl;

  vcl_vector<vcl_pair<vgl_point_3d<float>, bvpl_kernel_dispatch> >::iterator kernel_it =this->kernel_.begin();
  vcl_vector<vcl_pair<vgl_point_3d<float>, bvpl_kernel_dispatch> > kernel;


  //for efficiency and accuracy, get the rotation matrix of R and use the matrix for multiplicatiom
  vnl_matrix_fixed<float,3,3> R_as_matrix = R.as_matrix() ;


  for (; kernel_it != kernel_.end(); ++kernel_it)
  {
    //Rotate, mantaing floating point values
    vnl_vector_fixed<float,3> new_coord = R_as_matrix* vnl_vector_fixed<float,3>(float((*kernel_it).first.x()),
                                                                           float((*kernel_it).first.y()),
                                                                           float((*kernel_it).first.z()));

    kernel.push_back(vcl_make_pair(vgl_point_3d<float>(new_coord[0],new_coord[1],new_coord[2]), (kernel_it->second)));

    // As is is implemented now, if many point to round a sigle integer, then that integer is used multiple times
    // this may be a good solution, and avoids the problem of getting unequal number of symbols
  }

  return kernel;
}

#if 0
bool bvpl_kernel_factory::warp_nearest_neighbor(vgl_rotation_3d<float> R_)
{
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> >::iterator kernel_it =this->kernel_.begin();
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> > new_map;
  vcl_map< char, unsigned int> new_symbols_map;

  //fill new_symbols_map to keep a new count for sympols
#if 0
  vcl_map< char, unsigned int>::iterator syms_it = symbols_map_.begin();

  for (; syms_it != symbols_map_.end(); ++syms_it)
   new_symbols_map.insert(vcl_make_pair( syms_it->first, 0));
#endif

  //Kernels shouldn't get any bigger than this, so this initial values work
  int max_x =-50;
  int max_y =-50;
  int max_z =-50;
  int min_x = 50;
  int min_y =50;
  int min_z = 50;

  for (; kernel_it != kernel_.end(); ++kernel_it)
  {
    //Rotate, mantain floating point values
    vgl_point_3d<float> new_coord = R_* vgl_point_3d<float>(float((*kernel_it).first.x()),
                                                             float((*kernel_it).first.y()),
                                                             float((*kernel_it).first.z()));

    // cast to the nearest integer value
    int x0 = (int)vcl_floor(new_coord.x()+0.5f);
    int y0 = (int)vcl_floor(new_coord.y()+0.5f);
    int z0 = (int)vcl_floor(new_coord.z()+0.5f);

    if (x0> max_x) max_x = x0;
    if (y0> max_y) max_y = y0;
    if (z0> max_z) max_z = z0;

    if (x0< min_x) min_x = x0;
    if (y0< min_y) min_y = y0;
    if (z0< min_z) min_z = z0;

    new_map.push_back(vcl_make_pair(vgl_point_3d<int>(x0,y0,z0), (kernel_it->second)));

    // As is is implemented now, if many point to round a sigle integer, then that integer is used multiple times
    // this may be a good solution, and avoids the problem of getting unequal number of symbols
  }
  //set the dimension of the 3-d grid
  max3d_.set(max_x,max_y,max_z);
  min3d_.set(min_x,min_y,min_z);


  //TO DO: check the number of symbols is consistent
  //vcl_map< vgl_point_3d<int>, char >::iterator new_map_it = new_map.begin();

  //swap maps
  kernel_.swap(new_map);
  return true;
}
#endif

#if 0

//: Calculates standard rotation matrix obtained as the product of rotation matrices R_z*R_y*R_x
bool bvpl_kernel_factory::set_rotation(vnl_vector_fixed<double,3> const& rotations)
{
  double cos_x = vcl_cos(rotations[0]);
  double cos_y = vcl_cos(rotations[1]);
  double cos_z = vcl_cos(rotations[2]);

  double sin_x = vcl_sin(rotations[0]);
  double sin_y = vcl_sin(rotations[1]);
  double sin_z = vcl_sin(rotations[2]);

  R_[0][0] = cos_y*cos_z;
  R_[0][1] = (sin_x*sin_y*cos_z) - (cos_x*sin_z);
  R_[0][2] = (cos_x*sin_y*cos_z) + (sin_x*sin_z);
  R_[1][0] = cos_y*sin_z;
  R_[1][1] = (sin_x*sin_y*sin_z) + (cos_x*cos_z);
  R_[2][2] = (cos_x*sin_y*sin_z) - (sin_x*cos_z);
  R_[2][0] = -1.0 *sin_y;
  R_[2][1] = sin_x*cos_y;
  R_[2][2] = cos_x*cos_y;
}
#endif

#if 0
// Converts the kernel_map to a bvpl_kernel_iterator

bvpl_kernel_iterator bvpl_kernel_factory::iterator()
{
  //the iterator
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> > kernel_vals;
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> >::iterator k_it= kernel_.begin();

  for (; k_it!=kernel_.end(); ++k_it)
  {
    kernel_vals.push_back(vcl_make_pair<vgl_point_3d<int>, bvpl_kernel_dispatch>
                          `(vgl_point_3d<int>(k_it->first[0],k_it->first[1],k_it->first[2]), bvpl_kernel_dispatch(k_it->second)));
  }
  return kernel_vals;
}
#endif


vgl_vector_3d<int> bvpl_kernel_factory::dim()
{
  int x,y,z;

  if (vcl_abs(max3d_.x()) > vcl_abs(min3d_.x()))
    x = vcl_abs(max3d_.x())*2+1;
  else
    x = vcl_abs(min3d_.x())*2+1;

  if (vcl_abs(max3d_.y()) > vcl_abs(min3d_.y()))
    y = vcl_abs(max3d_.y())*2+1;
  else
    y = vcl_abs(min3d_.y())*2+1;

  if (vcl_abs(max3d_.z()) > vcl_abs(min3d_.z()))
    z = vcl_abs(max3d_.z())*2+1;
  else
    z = vcl_abs(min3d_.z())*2+1;

  return vgl_vector_3d<int>(x,y,z);
}

