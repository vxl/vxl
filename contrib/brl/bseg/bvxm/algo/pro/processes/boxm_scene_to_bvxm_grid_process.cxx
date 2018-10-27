// This is brl/bseg/bvxm/algo/pro/processes/boxm_scene_to_bvxm_grid_process.cxx
#include <bvxm/algo/bvxm_boxm_convert.h>
//:
// \file
// \brief  Process to convert a boxm_scene to a bvxm_voxel_grid
// \author Isabel Restrepo mir@lems.brown.edu
// \date  Jan 19, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>


#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_gauss_if3.h>


namespace boxm_scene_to_bvxm_grid_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;
}

//: process takes 4 inputs and 1 output.
// input[0]: The scene
// input[1]: Path to output grid
// input[2]: Resolution level
// input[3]: Bool: Enforce only cells at the resolution leve?
//           If true, there is no interpolation
// output[0]: The output grid

bool boxm_scene_to_bvxm_grid_process_cons(bprb_func_process& pro)
{
  using namespace boxm_scene_to_bvxm_grid_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "bool";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_scene_to_bvxm_grid_process(bprb_func_process& pro)
{
  using namespace boxm_scene_to_bvxm_grid_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cerr << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs:
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  std::string filepath = pro.get_input<std::string>(1);
  unsigned resolution_level = pro.get_input<short>(2);
  bool enforce_level = pro.get_input<bool>(3);

  static const unsigned int n_gaussian_modes_ = 3;
  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type_sf1;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type_sf1, n_gaussian_modes_> > mix_gauss_sf1_type;

  //check input's validity
  if (!scene_base.ptr())
  {
    std::cerr << pro.name() << ": -- Input scene is not valid!\n";
    return false;
  }

  if ( auto *scene= dynamic_cast<boxm_scene< boct_tree<short, float > > * >(scene_base.as_pointer()))
  {
    bvxm_voxel_grid<float> *grid = boxm_scene_to_bvxm_grid(*scene, filepath, resolution_level,enforce_level);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if ( auto *scene= dynamic_cast<boxm_scene< boct_tree<short, bsta_num_obs<bsta_gauss_sf1> > > * >(scene_base.as_pointer()))
  {
    bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> > *grid = boxm_scene_to_bvxm_grid(*scene, filepath, resolution_level, enforce_level);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if ( auto *scene =
            dynamic_cast<boxm_scene< boct_tree<short, mix_gauss_sf1_type > > * >(scene_base.as_pointer()))
  {
    bvxm_voxel_grid<mix_gauss_sf1_type > *grid = boxm_scene_to_bvxm_grid(*scene, filepath, resolution_level, enforce_level);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else
  {
    std::cerr << "It's not possible to convert input scene to a bvxm grid\n";
    return false;
  }
}

#include <boxm/boxm_scene.hxx>
#include <boxm/boxm_block.hxx>
#include <boct/boct_tree.hxx>
#include <boct/boct_tree_cell.hxx>
#include <vbl/vbl_array_3d.hxx>

typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
typedef bsta_num_obs<bsta_mixture_fixed<gauss_type,3> > mixture_type;
typedef boct_tree<short,mixture_type> tree_type;
typedef boxm_block<tree_type> block_type;

BOCT_TREE_INSTANTIATE(short,mixture_type);
BOCT_TREE_CELL_INSTANTIATE(short,mixture_type);
BOXM_SCENE_INSTANTIATE(tree_type);
BOXM_BLOCK_INSTANTIATE(tree_type);
VBL_ARRAY_3D_INSTANTIATE(block_type);

void vsl_b_write(vsl_b_ostream&, bsta_mixture_fixed<bsta_num_obs<bsta_gaussian_sphere<float,1> >,3>&) {}
void vsl_b_read(vsl_b_istream&, bsta_mixture_fixed<bsta_num_obs<bsta_gaussian_sphere<float,1> >,3>&) {}
void vsl_b_read(vsl_b_istream&, bsta_gaussian_sphere<float,1>&) {}
//bool operator==(block_type const&, block_type const&) { return false; }
