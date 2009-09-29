// This is brl/bseg/bvpl/pro/processes/util/bvpl_visualize_corner_pairs_process.cxx

//:
// \file
// \brief A process to find corner pairs that are 90-rotated
// \author Isabel Restrepo mir@lems.brown.edu
// \date  9/27/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>

#include <bvpl/util/bvpl_corner_pair_finder.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/io/bvxm_vrml_voxel_grid.h>

#include <vul/vul_file.h>
#include <vpl/vpl.h>

namespace bvpl_find_corner_pairs_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}

//:
// Inputs:
// * input[0]: The vector of corner pairs
// * input[1]: The index of lines to be displayed
// * input[2]: The path for output vrml file
// * input[3]: A flag to write vrml header and delete previous file or add info
// * input[4]: Hue value for the lines -a float in [0 1]
// Outputs:
// * output[0]: A vector containing the lines connecting corners found

bool bvpl_visualize_corner_pairs_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_find_corner_pairs_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvpl_corner_pairs_sptr";
  input_types_[1] = "unsigned";
  input_types_[2] = "vcl_string";
  input_types_[3] = "bool";
  input_types_[4] = "float";

  vcl_vector<vcl_string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_visualize_corner_pairs_process(bprb_func_process& pro)
{
  using namespace bvpl_find_corner_pairs_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  bvpl_corner_pairs_sptr pairs =pro.get_input<bvpl_corner_pairs_sptr>(i++);
  unsigned lines_id = pro.get_input<unsigned>(i++);
  vcl_string vrml_path =pro.get_input<vcl_string>(i++);
  bool write_header = pro.get_input<bool>(i++);
  float hue = pro.get_input<float>(i++);

  vcl_ofstream os;

  if (write_header) {
    if (vul_file::exists(vrml_path.c_str()))
      vpl_unlink(vrml_path.c_str());
    os.open(vrml_path.c_str(), vcl_ios::out);
    bvxm_vrml_voxel_grid::write_vrml_header(os);
  }
  else {
    os.open(vrml_path.c_str(), vcl_ios::app);
  }

   for (unsigned j=0; j<pairs->pairs_[lines_id].size(); ++j)
     bvxm_vrml_voxel_grid::write_vrml_line_segment(os, pairs->pairs_[lines_id][j]);

  return true;
}
