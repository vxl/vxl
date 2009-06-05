#include "bvpl_kernel_base.h"
#include <vcl_map.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>


bool bvpl_kernel_base::warp_nearest_neighbor()
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
  int max_x =-20;
  int max_y =-20;
  int max_z =-20;
  int min_x = 20;
  int min_y =20;
  int min_z = 20;

  for (; kernel_it != kernel_.end(); ++kernel_it)
  {
    //Rotate, mantain floating point values
    vnl_vector_fixed<double,3> new_coord = R_*vnl_vector_fixed<double,3>(double((*kernel_it).first.x()),
                                                                         double((*kernel_it).first.y()),
                                                                         double((*kernel_it).first.z()));

    // cast to the nearest integer value
    int x0 = (int)vcl_floor(new_coord[0]+0.5f);
    int y0 = (int)vcl_floor(new_coord[1]+0.5f);
    int z0 = (int)vcl_floor(new_coord[2]+0.5f);

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


#if 0
// Converts the kernel_map to a bvpl_kernel_iterator
bvpl_kernel_iterator bvpl_kernel_base::iterator()
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

bool bvpl_kernel_base::save_raw(vcl_string filename)
{
  vcl_cout << "still alive\n";

  vcl_fstream ofs(filename.c_str(), vcl_ios::binary | vcl_ios::out);
  if (!ofs.is_open()) {
    vcl_cerr << "error opening file " << filename << " for write!\n";
    return false;
  }

  // write header
  unsigned char data_type = 1; // 1 means signed byte

  vxl_uint_32 nx = (max3d_.x() - min3d_.x()) + 1;
  vxl_uint_32 ny = (max3d_.y() - min3d_.y()) + 1;
  vxl_uint_32 nz = (max3d_.z() - min3d_.z()) + 1;

  ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));

  // write data
  // iterate through slabs and fill in memory array
  unsigned size = nx*ny*nz;
  char *data_array = new char[size];
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> >::iterator it = kernel_.begin();

  //Since our kernel does not occupy the entire space we need to initialize our data
  for (unsigned i = 0; i < size; i++)
    data_array[i] = 1;

  for (; it !=kernel_.end(); ++it) {
    vgl_point_3d<int> coord =( *it).first;
    int index = (coord.x()-min3d_.x())*ny*nz + (coord.y()-min3d_.y())*nz + (coord.z() - min3d_.z());
    data_array[index] =(char)((*it).second.c_);
  }

  vcl_cout << vcl_endl;
  ofs.write(reinterpret_cast<char*>(data_array),sizeof(char)*nx*ny*nz);

  ofs.close();

  delete[] data_array;

  return true;
}

vgl_vector_3d<int> bvpl_kernel_base::dim()
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

void bvpl_kernel_base::print()
{
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> >::iterator it = kernel_.begin();
  for (; it !=kernel_.end(); ++it) {
    vgl_point_3d<int> coord =( *it).first;
    char c = (char)((*it).second.c_);
    vcl_cout << coord << "  " << c << vcl_endl;
  }
}
