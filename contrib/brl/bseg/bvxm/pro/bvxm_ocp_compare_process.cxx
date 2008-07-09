#include "bvxm_ocp_compare_process.h"
//#include "bvxm_voxel_world.h"

#include <bprb/bprb_parameters.h>
#include <vgl/vgl_vector_3d.h>

bvxm_ocp_compare_process::bvxm_ocp_compare_process()
{
  // This process has 2 inputs:
  //input[0]: The voxel world
  //input[1]: The voxel world
  input_data_.resize(4, brdb_value_sptr(0));
  input_types_.resize(4);

  int i=0;
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world for LIDAR ONLY update
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world for IMAGE ONLY update
  input_types_[i++] = "unsigned";                 // search neighb. size
  input_types_[i++] = "unsigned";                 // scale

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  int j=0;
  output_types_[j++]= "double";  // the sum of ocp prob product
}

bool bvxm_ocp_compare_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:

  //voxel_world1
  brdb_value_t<bvxm_voxel_world_sptr >* input0 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr >* >(input_data_[0].ptr());
  bvxm_voxel_world_sptr voxel_world1 = input0->value();

  //voxel_world2
  brdb_value_t<bvxm_voxel_world_sptr >* input1 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr >* >(input_data_[1].ptr());
  bvxm_voxel_world_sptr voxel_world2 = input1->value();

  brdb_value_t<unsigned >* input2 =
    static_cast<brdb_value_t<unsigned >* >(input_data_[2].ptr());
  unsigned n = input2->value();
  //scale 
  brdb_value_t<unsigned >* input3 =
    static_cast<brdb_value_t<unsigned >* >(input_data_[3].ptr());
  unsigned scale = input3->value();

  double val = compare(voxel_world1, voxel_world2, n,scale);

  //store output
  brdb_value_sptr output0 =
    new brdb_value_t<double>(val);
  output_data_[0] = output0;

  return true;
}

double bvxm_ocp_compare_process::compare(bvxm_voxel_world_sptr w1,
                                         bvxm_voxel_world_sptr w2,
                                         unsigned n, unsigned scale)
{
  typedef bvxm_voxel_traits<LIDAR>::voxel_datatype lidar_datatype;
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // get ocuppancy probability grids
  bvxm_voxel_grid_base_sptr ocp_grid_base1 = w1->get_grid<OCCUPANCY>(0,scale);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid1  = static_cast<bvxm_voxel_grid<lidar_datatype>*>(ocp_grid_base1.ptr());
  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it1 = ocp_grid1->begin();

  bvxm_voxel_grid_base_sptr ocp_grid_base2 = w2->get_grid<OCCUPANCY>(0,scale);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid2  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base2.ptr());
  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it2 = ocp_grid2->begin();

  vgl_vector_3d<unsigned int> grid_size = w1->get_params()->num_voxels(scale);

  int m = int(n); // using this m instead of n will avoid compiler warnings
  double maxN=0;
  int dim=2*m+1, imax=0, jmax=0, kmax=0;
  char *comp_array = new char[dim*dim*dim];
  for (int k=-m; k<=m; ++k)
  {
    vcl_cout << k << vcl_endl;
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
          //vcl_cout << kdx;
          if (kdx >= k && kdx < k + size_z) {
            bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it2 = ocp_grid2->slab_iterator(kdx);
            bvxm_voxel_slab<float> slab1 = *ocp_slab_it1;
            bvxm_voxel_slab<float> slab2 = *ocp_slab_it2;
            for (int i=0; i<size_x; ++i) {
              for (int j=0; j<size_y; ++j) {
                //vcl_cout << '[' << i << ',' << j << ']' << vcl_endl;
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
        vcl_cout << "k=" << k << "  j=" << j1 << "  i=" << i1 << "-->" << N << vcl_endl;
        comp_array[(i1+m)*dim*dim + (j1+m)*dim + k+m] = char((N/14927.35)*255);
      }
    }
  }
  vcl_cout << "Maximum -->\n"
           << "k=" << kmax << "  j=" << jmax << "  i=" << imax << "-->" << maxN << vcl_endl;
  save_raw(comp_array,dim,dim,dim,"data_comp.raw");
  return maxN;
}

bool bvxm_ocp_compare_process::save_raw(char *ocp_array,
                                        int x, int y, int z,
                                        vcl_string filename)
{
  vcl_fstream ofs(filename.c_str(),vcl_ios::binary | vcl_ios::out);
  if (!ofs.is_open()) {
    vcl_cerr << "error opening file " << filename << " for write!\n";
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
