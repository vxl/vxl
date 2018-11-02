//This is brl/bseg/bvxm/grid/pro/processes/bvxm_create_grid_process.cxx

//:
// \file
// \brief A process for creating a new grid as specified by a path and outputs a bvxm_voxel_grid_base_sptr to be store on the database
// \author Vishal Jain
// \date Aug 4, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_opinion.h>
#include <vul/vul_file.h>

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gaussian_indep.h>
namespace bvxm_create_grid_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
}


//: set input and output types
bool bvxm_create_grid_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_create_grid_process_globals;

  // process takes 5 inputs and has 1 output.
  std::vector<std::string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++]="vcl_string"; //the input path
  input_types_[i++]="vcl_string";//the type e.g. "float","double"...
  input_types_[i++]="unsigned";//dim_x
  input_types_[i++]="unsigned";//dim_y
  input_types_[i++]="unsigned";//dim_z

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]="bvxm_voxel_grid_base_sptr";  // The resulting grid

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: Execute the process
bool bvxm_create_grid_process(bprb_func_process& pro)
{
  // check number of inputs
  if (pro.input_types().size() != 5)
  {
    std::cout << pro.name() << "The number of inputs should be " << 5 << std::endl;
    return false;
  }

  std::string input_path = pro.get_input<std::string>(0);
  std::string datatype =  pro.get_input<std::string>(1);
  auto dim_x =  pro.get_input<unsigned>(2);
  auto dim_y =  pro.get_input<unsigned>(3);
  auto dim_z =  pro.get_input<unsigned>(4);


  if (vul_file::is_directory(input_path) ) {
    std::cerr << "In bvxm_create_grid_process -- input path " << input_path<< "is not valid!\n";
    return false;
  }
  std::cout << "In bvxm_create_grid_process( -- input file is: " <<  input_path << std::endl;

  //This is temporary. What should happen is that we can read the type from the file header.
  //Also the header should be such that we can check if the file is not currupt
  if (datatype == "float"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<float>(input_path,vgl_vector_3d<unsigned>(dim_x,dim_y,dim_z));
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "bool"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<bool>(input_path,vgl_vector_3d<unsigned>(dim_x,dim_y,dim_z));
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "opinion"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<bvxm_opinion>(input_path,vgl_vector_3d<unsigned>(dim_x,dim_y,dim_z));
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "bsta_gaussian_mixture_float_3"){
    //Only floating point are supported for now and mixtures with 3 components.
    typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
    typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
    typedef bsta_num_obs<mix_gauss> mix_gauss_type;
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<mix_gauss_type>(input_path,vgl_vector_3d<unsigned>(dim_x,dim_y,dim_z));
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "vnl_vector_fixed_float_3"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<vnl_vector_fixed<float, 3> >(input_path,vgl_vector_3d<unsigned>(dim_x,dim_y,dim_z));
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "vnl_vector_fixed_float_4"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<vnl_vector_fixed<float,4> >(input_path,vgl_vector_3d<unsigned>(dim_x,dim_y,dim_z));
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else {
    std::cerr << "datatype not supported\n";
  }

  return false;
}
