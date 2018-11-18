//This is brl/bseg/bvxm/pro/processes/bvxm_ocp_compare_process.cxx
#include <iostream>
#include <string>
#include "bvxm_ocp_compare_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_process.h>

#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

//: set input and output types
bool bvxm_ocp_compare_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_ocp_compare_process_gloabals;

  // This process has 4 inputs:
  std::vector<std::string> input_types_(n_inputs_);
  int i=0;
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world for LIDAR ONLY update
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world for IMAGE ONLY update
  input_types_[i++] = "unsigned";                 // search neighb. size
  input_types_[i++] = "unsigned";                 // scale
  if (!pro.set_input_types(input_types_))
    return false;

  // This process has 1 output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]= "double";  // the sum of ocp prob product
  return pro.set_output_types(output_types_);
}


bool bvxm_ocp_compare_process(bprb_func_process& pro)
{
  using namespace bvxm_ocp_compare_process_gloabals;
  // This process has 2 inputs:
  //input[0]: The voxel world for LIDAR ONLY update
  //input[1]: The voxel world for IMAGE ONLY update
  //input[2]: Search neighborhood size
  //input[3]: Scale of the voxel world
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs:
  unsigned i = 0;
  bvxm_voxel_world_sptr voxel_world1 = pro.get_input<bvxm_voxel_world_sptr>(i++);
  //voxel_world2
  bvxm_voxel_world_sptr voxel_world2 = pro.get_input<bvxm_voxel_world_sptr>(i++);
  auto n = pro.get_input<unsigned>(i++);
  //scale
  auto scale =pro.get_input<unsigned>(i++);

   //check inputs validity
  if (!voxel_world1) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }
  if (!voxel_world2) {
    std::cout << pro.name() <<" :--  Input 1  is not valid!\n";
    return false;
  }

  double val = compare(voxel_world1, voxel_world2, n,scale);

  //store output
  pro.set_output_val<double>(0, val);

  return true;
}


bool bvxm_ocp_compare_process_gloabals::save_raw(char *ocp_array, int x, int y, int z, const std::string& filename)
{
  std::fstream ofs(filename.c_str(),std::ios::binary | std::ios::out);
  if (!ofs.is_open()) {
    std::cerr << "error opening file " << filename << " for write!\n";
    return false;
  }
  // write header
  unsigned char data_type = 0; // 0 means unsigned byte

  vxl_uint_32 nx = x;
  vxl_uint_32 ny = y;
  vxl_uint_32 nz = z;

  ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));
  ofs.write(reinterpret_cast<char*>(ocp_array),sizeof(unsigned char)*nx*ny*nz);

  ofs.close();

  delete[] ocp_array;

  return true;
}

double bvxm_ocp_compare_process_gloabals::compare(const bvxm_voxel_world_sptr& w1,
                                                  const bvxm_voxel_world_sptr& w2,
                                                  unsigned n, unsigned scale)
{
  typedef bvxm_voxel_traits<LIDAR>::voxel_datatype lidar_datatype;
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // get occupancy probability grids
  bvxm_voxel_grid_base_sptr ocp_grid_base1 = w1->get_grid<OCCUPANCY>(0,scale);
  auto *ocp_grid1  = static_cast<bvxm_voxel_grid<lidar_datatype>*>(ocp_grid_base1.ptr());
  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it1 = ocp_grid1->begin();

  bvxm_voxel_grid_base_sptr ocp_grid_base2 = w2->get_grid<OCCUPANCY>(0,scale);
  auto *ocp_grid2  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base2.ptr());
  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it2 = ocp_grid2->begin();

  vgl_vector_3d<unsigned int> grid_size = w1->get_params()->num_voxels(scale);

  int m = int(n); // using this m instead of n will avoid compiler warnings
  double maxN=0;
  int dim=2*m+1, imax=0, jmax=0, kmax=0;
  char *comp_array = new char[dim*dim*dim];
  for (int k=-m; k<=m; ++k)
  {
    std::cout << k << std::endl;
    for (int j1=-m; j1<=m; ++j1)
    {
      for (int i1=-m; i1<=m; ++i1)
      {
        double N=0;
        int num=0,
        size_x=grid_size.x(), size_y=grid_size.y(), size_z=grid_size.z();
        ocp_slab_it1 = ocp_grid1->begin();
        for (int kdx=0; kdx<size_z; ++kdx, ++ocp_slab_it1)
        {
          //std::cout << kdx;
          if (kdx >= k && kdx < k + size_z) {
            bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it2 = ocp_grid2->slab_iterator(kdx);
            bvxm_voxel_slab<float> slab1 = *ocp_slab_it1;
            bvxm_voxel_slab<float> slab2 = *ocp_slab_it2;
            for (int i=0; i<size_x; ++i) {
              for (int j=0; j<size_y; ++j) {
                //std::cout << '[' << i << ',' << j << ']' << std::endl;
                int idx = i+i1;
                int jdx = j+j1;
                if (idx<size_x && idx>=0 &&
                    jdx<size_y && jdx>=0) {
                  N += slab1(i,j)*slab2(idx,jdx);
                  num++;
                  if (N>maxN) {
                    maxN = N;
                    kmax = k;
                    imax = i1;
                    jmax = j1;
                  }
                    }
              }
            }
          }
        }
        N = (N/num)*size_x*size_y*size_z;
        std::cout << "k=" << k << "  j=" << j1 << "  i=" << i1 << "-->" << N << std::endl;
        comp_array[(i1+m)*dim*dim + (j1+m)*dim + k+m] = char((N/14927.35)*255);
      }
    }
  }
  std::cout << "Maximum -->\n"
           << "k=" << kmax << "  j=" << jmax << "  i=" << imax << "-->" << maxN << std::endl;
  save_raw(comp_array,dim,dim,dim,"data_comp.raw");
  return maxN;
}
