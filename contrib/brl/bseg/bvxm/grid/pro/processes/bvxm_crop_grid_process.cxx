// This is brl/bseg/bvxm/grid/pro/processes/bvxm_crop_grid_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for cropping and bvxm_voxel_grid.
// \author Isabel Restrepo
// \date June 15, 2009
//
// \verbatim
//  Modifications
//  <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_parameters.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_opinion.h>
#include <vul/vul_file.h>

namespace bvpl_neighborhood_operator_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}


//: set input and output types
bool bvxm_crop_grid_process_cons(bprb_func_process& pro)
{
    using namespace bvpl_neighborhood_operator_process_globals;
  //This process has no inputs nor outputs only parameters
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0]="bvxm_voxel_grid_base_sptr";

  // No outputs to the database. The resulting grid is stored on disk
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0]="bvxm_voxel_grid_base_sptr";
  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool bvxm_crop_grid_process(bprb_func_process& pro)
{
  // check number of inputs
  if (pro.n_inputs() != 1)
  {
    vcl_cout << pro.name() << "The number of inputs should be " <<0 << vcl_endl;
    return false;
  }

  //vcl_string input_path;
  //pro.parameters()->get_value("input_path", input_path);
  //if (vul_file::is_directory(input_path) || !vul_file::exists(input_path)) {
  //  vcl_cerr << "In bvxm_crop_grid_process -- input directory "<< vul_file::get_cwd() << '/' << input_path<< "is not valid!\n";
  //  return false;
  //}
  //vcl_cout << "In bvxm_crop_grid_process( -- input directory is: " <<  vul_file::get_cwd() << input_path << vcl_endl;

  vcl_string output_path;
  pro.parameters()->get_value("output_path", output_path);

  if (vul_file::is_directory(output_path)) {
    vcl_cerr << "In bvxm_crop_grid_process -- output directory "<< vul_file::get_cwd() << '/' << output_path<< "is not valid!\n";
    return false;
  }
//   if the output file exits, delete it
  if (vul_file::exists(output_path))
    vul_file::delete_file_glob(output_path);


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

  vgl_vector_3d<unsigned int> out_grid_dim(dimx, dimy, dimz);
  if (bvxm_voxel_grid<float> * float_input_grid=dynamic_cast<bvxm_voxel_grid<float> *>(input_grid.ptr()))
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

    vcl_cout<<"Cropping done."<<vcl_endl;
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
  }
  else if(bvxm_voxel_grid<bvxm_opinion> * opinion_input_grid=dynamic_cast<bvxm_voxel_grid<bvxm_opinion> *>(input_grid.ptr()))
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

    vcl_cout<<"Cropping done."<<vcl_endl;
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
  }
  return true;
}
