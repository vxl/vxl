//This is brl/bseg/bvxm/grid/pro/processes/bvxm_load_grid_process.cxx

//:
// \file
// \brief A process for creating an instance of a grid as specified by a path and outputs a bvxm_voxel_grid_base_sptr to be store on the database
// \author Isabel Restrepo
// \date June 15, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
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
#include <vnl/vnl_float_3.h>
#include <vnl/vnl_float_4.h>
namespace bvxm_load_grid_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}


//: set input and output types
bool bvxm_load_grid_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_load_grid_process_globals;

  // process takes 2 inputs and has 1 output.
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++]="vcl_string"; //the input path
  input_types_[i++]="vcl_string";//the type e.g. "float","double"...

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0]="bvxm_voxel_grid_base_sptr";  // The resulting grid

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: Execute the process
bool bvxm_load_grid_process(bprb_func_process& pro)
{
  // check number of inputs
  if (pro.input_types().size() != 2)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << 2 << vcl_endl;
    return false;
  }

  vcl_string input_path = pro.get_input<vcl_string>(0);
  vcl_string datatype =  pro.get_input<vcl_string>(1);

  if (vul_file::is_directory(input_path) || !vul_file::exists(input_path)) {
    vcl_cerr << "In bvxm_load_grid_process -- input path " << input_path<< "is not valid!\n";
    return false;
  }
  vcl_cout << "In bvxm_load_grid_process( -- input file is: " <<  input_path << vcl_endl;

  //This is temporary. What should happen is that we can read the type from the file header.
  //Also the header should be such that we can check if the file is not currupt
  if (datatype == "float"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<float>(input_path);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
   if (datatype == "unsigned"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<unsigned>(input_path);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  if (datatype == "int"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<int>(input_path);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "bool"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<bool>(input_path);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "ocp_opinion"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<bvxm_opinion>(input_path);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "bsta_gaussian_mixture_float_3"){
    //Only floating point are supported for now and mixtures with 3 components.
    typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
    typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
    typedef bsta_num_obs<mix_gauss> mix_gauss_type;
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<mix_gauss_type>(input_path);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "bsta_gauss_f1"){
    typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<gauss_type>(input_path);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "vnl_vector_fixed_float_3"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<vnl_vector_fixed<float, 3> >(input_path);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "vnl_float_3"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<vnl_float_3>(input_path);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "vnl_float_4"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<vnl_float_4>(input_path);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if (datatype == "vnl_vector_fixed_float_4"){
    bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<vnl_vector_fixed<float,4> >(input_path);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else {
    vcl_cerr << "datatype not supported\n";
  }

  return false;
}
