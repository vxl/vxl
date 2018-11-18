#include "bvxm_io_voxel_grid.h"
//:
// \file
// \brief This file contains template instantiations of functions

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Save to Drishti raw file
template <>
bool bvxm_grid_save_raw<bsta_num_obs<bsta_gauss_sf1> >(bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> > *grid,  std::string filename)
{
    std::fstream ofs(filename.c_str(),std::ios::binary | std::ios::out);
    if (!ofs.is_open()) {
      std::cerr << "error opening file " << filename << " for write!\n";
      return false;
    }

    // write header
    unsigned char data_type = bvxm_dristhi_traits<bsta_num_obs<bsta_gauss_sf1> >::dristhi_header();

    vxl_uint_32 nx = grid->grid_size().x();
    vxl_uint_32 ny = grid->grid_size().y();
    vxl_uint_32 nz = grid->grid_size().z();

    ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
    ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
    ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
    ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));

    // write data
    // iterate through slabs and fill in memory array
    typedef  bvxm_dristhi_traits<bsta_num_obs<bsta_gauss_sf1> >::datatype DataType;
    auto *data_array = new DataType[nx*ny*nz];

    //get the range
    bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> >::iterator grid_it = grid->begin();
    float max=std::abs(((*grid_it)(0,0)).mean());
    float min=std::abs(((*grid_it)(0,0)).mean());
    for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
      for (unsigned i=0; i<(*grid_it).nx(); ++i) {
        for (unsigned j=0; j < (*grid_it).ny(); ++j) {
          if (std::abs(((*grid_it)(i,j)).mean())> max)
            max = std::abs(((*grid_it)(i,j)).mean());
          if (std::abs(((*grid_it)(i,j)).mean())< min)
            min = std::abs(((*grid_it)(i,j)).mean());
        }
      }
    }
    std::cout << "max =  " << max << " min= " <<min << std::endl;

    grid_it = grid->begin();
    for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
      std::cout << '.';
      for (unsigned i=0; i<(*grid_it).nx(); ++i) {
        for (unsigned j=0; j < (*grid_it).ny(); ++j) {
          data_array[i*ny*nz + j*nz + k] =std::abs(((*grid_it)(i,j)).mean());
        }
      }
    }
    std::cout << std::endl;
    ofs.write(reinterpret_cast<char*>(data_array),sizeof(DataType)*nx*ny*nz);

    ofs.close();

    delete[] data_array;

    return true;
}
