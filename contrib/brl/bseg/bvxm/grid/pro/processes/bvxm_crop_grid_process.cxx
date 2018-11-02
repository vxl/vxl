// This is brl/bseg/bvxm/grid/pro/processes/bvxm_crop_grid_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for cropping and bvxm_voxel_grid.
// \author Isabel Restrepo
// \date June 15, 2009
//
// \verbatim
//  Modifications
//   Aug 13 2009 added support for Gaussian grids
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_parameters.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_opinion.h>
#include <vul/vul_file.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_attributes.h>
#include <vnl/vnl_float_4.h>

namespace bvpl_crop_grid_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}


//: set input and output types
bool bvxm_crop_grid_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_crop_grid_process_globals;

  // process takes 2 inputs and has 1 output.
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0]="bvxm_voxel_grid_base_sptr"; //the input grid
  input_types_[1]="vcl_string"; //the path for output grid

  //The output grid
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]="bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: Execute the process
bool bvxm_crop_grid_process(bprb_func_process& pro)
{
  using namespace bvpl_crop_grid_process_globals;
  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << "The number of inputs should be " <<n_inputs_ << std::endl;
    return false;
  }

  unsigned corner_x = 0;
  pro.parameters()->get_value("corner_x", corner_x);
  unsigned corner_y = 0;
  pro.parameters()->get_value("corner_y", corner_y);
  unsigned corner_z = 0;
  pro.parameters()->get_value("corner_z", corner_z);


  unsigned int dimx = 10;
  pro.parameters()->get_value("dim_x", dimx);
  unsigned int dimy = 10;
  pro.parameters()->get_value("dim_y", dimy);
  unsigned int dimz = 10;
  pro.parameters()->get_value("dim_z", dimz);

  unsigned i=0;
  bvxm_voxel_grid_base_sptr input_grid = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  std::string output_path = pro.get_input<std::string>(i++);

  if (vul_file::is_directory(output_path)) {
    std::cerr << "In bvxm_crop_grid_process -- output directory "<< output_path << "is not valid!\n";
    return false;
  }

  //if the output file exits, delete it
  if (vul_file::exists(output_path))
    vul_file::delete_file_glob(output_path);

  vgl_vector_3d<unsigned int> out_grid_dim(dimx, dimy, dimz);
  if (auto * float_input_grid=dynamic_cast<bvxm_voxel_grid<float> *>(input_grid.ptr()))
  {
    bvxm_voxel_grid<float> * grid_out=new bvxm_voxel_grid<float>(output_path, out_grid_dim);

    unsigned slab_idx = corner_z;
    bvxm_voxel_grid<float>::iterator grid_in_it = float_input_grid->slab_iterator(slab_idx);
    bvxm_voxel_grid<float>::iterator grid_out_it = grid_out->slab_iterator(slab_idx - corner_z);

    for (; slab_idx < (corner_z + dimz); ++grid_in_it, ++grid_out_it, ++slab_idx)
    {
      for (unsigned x = corner_x; x < corner_x + dimx; x++)
        for (unsigned y = corner_y; y < corner_y + dimy; y++)
          (*grid_out_it)(x-corner_x, y-corner_y) = (* grid_in_it)(x,y);
    }

    std::cout << "Cropping done." << std::endl;
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    return true;
  }
  else  if (auto * float_input_grid=dynamic_cast<bvxm_voxel_grid<unsigned> *>(input_grid.ptr()))
  {
    bvxm_voxel_grid<unsigned> * grid_out=new bvxm_voxel_grid<unsigned>(output_path, out_grid_dim);

    unsigned slab_idx = corner_z;
    bvxm_voxel_grid<unsigned>::iterator grid_in_it = float_input_grid->slab_iterator(slab_idx);
    bvxm_voxel_grid<unsigned>::iterator grid_out_it = grid_out->slab_iterator(slab_idx - corner_z);

    for (; slab_idx < (corner_z + dimz); ++grid_in_it, ++grid_out_it, ++slab_idx)
    {
      for (unsigned x = corner_x; x < corner_x + dimx; x++)
        for (unsigned y = corner_y; y < corner_y + dimy; y++)
          (*grid_out_it)(x-corner_x, y-corner_y) = (* grid_in_it)(x,y);
    }

    std::cout << "Cropping done." << std::endl;
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    return true;
  }
  else if (auto * opinion_input_grid=dynamic_cast<bvxm_voxel_grid<bvxm_opinion> *>(input_grid.ptr()))
  {
    bvxm_voxel_grid<bvxm_opinion> * grid_out=new bvxm_voxel_grid<bvxm_opinion>(output_path, out_grid_dim);

    unsigned slab_idx = corner_z;
    bvxm_voxel_grid<bvxm_opinion>::iterator grid_in_it = opinion_input_grid->slab_iterator(slab_idx);
    bvxm_voxel_grid<bvxm_opinion>::iterator grid_out_it = grid_out->slab_iterator(slab_idx - corner_z);

    for (; slab_idx < (corner_z + dimz); ++grid_in_it, ++grid_out_it, ++slab_idx)
    {
      for (unsigned x = corner_x; x < corner_x + dimx; x++)
        for (unsigned y = corner_y; y < corner_y + dimy; y++)
          (*grid_out_it)(x-corner_x, y-corner_y) = (* grid_in_it)(x,y);
    }

    std::cout << "Cropping done." << std::endl;
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    return true;
  }
  else if (auto * gauss_input_grid=dynamic_cast<bvxm_voxel_grid<gauss_type> *>(input_grid.ptr()))
  {
    bvxm_voxel_grid<gauss_type> * grid_out=new bvxm_voxel_grid<gauss_type>(output_path, out_grid_dim);

    unsigned slab_idx = corner_z;
    bvxm_voxel_grid<gauss_type>::iterator grid_in_it = gauss_input_grid->slab_iterator(slab_idx);
    bvxm_voxel_grid<gauss_type>::iterator grid_out_it = grid_out->slab_iterator(slab_idx - corner_z);

    for (; slab_idx < (corner_z + dimz); ++grid_in_it, ++grid_out_it, ++slab_idx)
    {
      for (unsigned x = corner_x; x < corner_x + dimx; x++)
        for (unsigned y = corner_y; y < corner_y + dimy; y++)
          (*grid_out_it)(x-corner_x, y-corner_y) = (* grid_in_it)(x,y);
    }

    std::cout << "Cropping done." << std::endl;
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    return true;
  }
  else if (auto * four_input_grid=dynamic_cast<bvxm_voxel_grid<vnl_float_4> *>(input_grid.ptr()))
  {
    bvxm_voxel_grid<vnl_float_4> * grid_out=new bvxm_voxel_grid<vnl_float_4>(output_path, out_grid_dim);
    unsigned slab_idx = corner_z;
    bvxm_voxel_grid<vnl_float_4>::iterator grid_in_it = four_input_grid->slab_iterator(slab_idx);
    bvxm_voxel_grid<vnl_float_4>::iterator grid_out_it = grid_out->slab_iterator(slab_idx - corner_z);

    for (; slab_idx < (corner_z + dimz); ++grid_in_it, ++grid_out_it, ++slab_idx)
    {
      for (unsigned x = corner_x; x < corner_x + dimx; x++)
        for (unsigned y = corner_y; y < corner_y + dimy; y++)
          (*grid_out_it)(x-corner_x, y-corner_y) = (* grid_in_it)(x,y);
    }

    std::cout<<"Cropping done."<<std::endl;
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    return true;
  }
  return true;
}
