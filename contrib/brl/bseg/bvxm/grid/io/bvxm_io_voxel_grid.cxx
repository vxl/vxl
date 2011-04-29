#include "bvxm_io_voxel_grid.h"
//:
// \file
// \brief This file contains template instantiations of functions

#include <vcl_config_compiler.h>

//: Save to Drishti raw file
VCL_DEFINE_SPECIALIZATION
bool bvxm_grid_save_raw<bsta_num_obs<bsta_gauss_sf1> >(bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> > *grid,  vcl_string filename)
{
    vcl_fstream ofs(filename.c_str(),vcl_ios::binary | vcl_ios::out);
    if (!ofs.is_open()) {
      vcl_cerr << "error opening file " << filename << " for write!\n";
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
    DataType *data_array = new DataType[nx*ny*nz];

    //get the range
    bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> >::iterator grid_it = grid->begin();
    float max=vcl_abs(((*grid_it)(0,0)).mean());
    float min=vcl_abs(((*grid_it)(0,0)).mean());
    for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
      for (unsigned i=0; i<(*grid_it).nx(); ++i) {
        for (unsigned j=0; j < (*grid_it).ny(); ++j) {
          if (vcl_abs(((*grid_it)(i,j)).mean())> max)
            max = vcl_abs(((*grid_it)(i,j)).mean());
          if (vcl_abs(((*grid_it)(i,j)).mean())< min)
            min = vcl_abs(((*grid_it)(i,j)).mean());
        }
      }
    }
    vcl_cout << "max =  " << max << " min= " <<min << vcl_endl;

    grid_it = grid->begin();
    for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
      vcl_cout << '.';
      for (unsigned i=0; i<(*grid_it).nx(); ++i) {
        for (unsigned j=0; j < (*grid_it).ny(); ++j) {
          data_array[i*ny*nz + j*nz + k] =vcl_abs(((*grid_it)(i,j)).mean());
        }
      }
    }
    vcl_cout << vcl_endl;
    ofs.write(reinterpret_cast<char*>(data_array),sizeof(DataType)*nx*ny*nz);

    ofs.close();

    delete[] data_array;

    return true;
}

