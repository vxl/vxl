// This is brl/bseg/bvxm/grid/io/bvxm_io_voxel_grid.h
#ifndef bvxm_io_voxel_grid_h
#define bvxm_io_voxel_grid_h
//:
// \file
// \brief set of io functionalities for bvxm_grid
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  June 18, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "../bvxm_voxel_grid.h"
#include "../bvxm_voxel_grid_base.h"
#include <vcl_limits.h>
#include <vcl_iostream.h>

//: Traits for saving grids of different datatypes to Dristhi .raw file
template <class T>
class bvxm_dristhi_traits;

//: Dristhi datypes for header:
// 0 : unsigned byte - 1 byte per voxel
// 1 : signed byte - 1 byte per voxel
// 2 : unsigned short - 2 bytes per voxel
// 3 : signed short - 2 bytes per voxel
// 4 : integer - 4 bytes per voxel
// 8 : float - 4 bytes per voxel

//: Dristhi traits specializations. Please extent as needed

template <>
class bvxm_dristhi_traits<float>
{
 public:
  static unsigned char dristhi_header() {return 8;}
};

template<>
class bvxm_dristhi_traits<unsigned char>
{
 public:
  static unsigned char dristhi_header() {return 0;}
};

template<>
class bvxm_dristhi_traits<char>
{
 public:
  static unsigned char dristhi_header() {return 1;}
};

//: Save to dristi raw file
template<class T>
bool bvxm_grid_save_raw(bvxm_voxel_grid_base_sptr grid_base,  vcl_string filename)
{
    vcl_fstream ofs(filename.c_str(),vcl_ios::binary | vcl_ios::out);
    if (!ofs.is_open()) {
      vcl_cerr << "error opening file " << filename << " for write!\n";
      return false;
    }

    //cast grid
    bvxm_voxel_grid<T> *grid = dynamic_cast<bvxm_voxel_grid<T>* >(grid_base.ptr());
    // write header
    unsigned char data_type = bvxm_dristhi_traits<T>::dristhi_header();

    vxl_uint_32 nx = grid->grid_size().x();
    vxl_uint_32 ny = grid->grid_size().y();
    vxl_uint_32 nz = grid->grid_size().z();

    ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
    ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
    ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
    ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));

    // write data
    // iterate through slabs and fill in memory array
    T *data_array = new T[nx*ny*nz];

    //get the range
    bvxm_voxel_grid<float>::const_iterator grid_it = grid->begin();
    T max = vcl_numeric_limits<T>::min();
    T min = vcl_numeric_limits<T>::max();
    for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
      for (unsigned i=0; i<(*grid_it).nx(); ++i) {
        for (unsigned j=0; j < (*grid_it).ny(); ++j) {
          if ((*grid_it)(i,j)> max)
            max = (*grid_it)(i,j);
          if ((*grid_it)(i,j)< min)
            min = (*grid_it)(i,j);
        }
      }
    }
    vcl_cout << "max =  " << max << " min= " <<min << vcl_endl;

    grid_it = grid->begin();
    for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
      vcl_cout << '.';
      for (unsigned i=0; i<(*grid_it).nx(); ++i) {
        for (unsigned j=0; j < (*grid_it).ny(); ++j) {
          data_array[i*ny*nz + j*nz + k] =(*grid_it)(i,j);
        }
      }
    }
    vcl_cout << vcl_endl;
    ofs.write(reinterpret_cast<char*>(data_array),sizeof(T)*nx*ny*nz);

    ofs.close();

    delete[] data_array;

    return true;
}

#endif
