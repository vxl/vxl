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
#include <bvrml/bvrml_write.h>

#include <vul/vul_file.h>
#include <vpl/vpl.h>
#include <vil/algo/vil_colour_space.h>

namespace bvpl_visualize_corner_pairs_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

//:
// Inputs:
// * input[0]: The vector of corner pairs
// * input[1]: The index of lines to be displayed
// * input[2]: The path for output vrml file
// * input[3]: A flag to write vrml header and delete previous file or add info
// * input[4]: Hue value for the lines -a float in [0 1]


bool bvpl_visualize_corner_pairs_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_visualize_corner_pairs_process_globals;

  //process takes 5 inputs but has no outputs
  std::vector<std::string> input_types_(n_inputs_);
  std::vector<std::string> output_types_(n_outputs_);
  input_types_[0] = "bvpl_corner_pairs_sptr";
  input_types_[1] = "unsigned";
  input_types_[2] = "vcl_string";
  input_types_[3] = "bool";
  input_types_[4] = "float";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_visualize_corner_pairs_process(bprb_func_process& pro)
{
  using namespace bvpl_visualize_corner_pairs_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  bvpl_corner_pairs_sptr pairs =pro.get_input<bvpl_corner_pairs_sptr>(i++);
#if 0
  unsigned lines_id =
#endif
                         pro.get_input<unsigned>(i++);
  std::string vrml_path = pro.get_input<std::string>(i++);
  bool write_header = pro.get_input<bool>(i++);
  float hue = pro.get_input<float>(i++)*360.0f;

  std::ofstream os;

  if (write_header) {
    if (vul_file::exists(vrml_path.c_str()))
      vpl_unlink(vrml_path.c_str());
    os.open(vrml_path.c_str(), std::ios::out);
    bvrml_write::write_vrml_header(os);
  }
  else {
    os.open(vrml_path.c_str(), std::ios::app);
  }
   float r, g, b;
   vil_colour_space_HSV_to_RGB<float>(hue,1.0f,255.0f,&r,&g,&b);
#if 0
   for (unsigned j=0; j<pairs->pairs_[lines_id].size(); ++j)
     bvrml_write::write_vrml_line_segment(os, pairs->pairs_[lines_id][j],r/255.0,g/255.0,b/255.0,0);

   for (unsigned j=0; j<pairs->boxes_[lines_id].size(); ++j)
     bvxm_vrml_voxel_grid::write_vrml_box(os, pairs->boxes_[lines_id][j],r/255.0,g/255.0,b/255.0,0.9);
#endif // 0

  return true;
}
