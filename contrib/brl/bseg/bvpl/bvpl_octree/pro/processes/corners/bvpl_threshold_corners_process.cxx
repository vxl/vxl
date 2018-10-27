//:
// \file
// \brief A process to compute corner measure (Harris)
// \author Isabel Restrepo
// \date 20-Jul-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_global_corners.h>

//:global variables
namespace bvpl_threshold_corners_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_threshold_corners_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_threshold_corners_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bvpl_global_taylor_sptr" ;
  input_types_[i++] = "bvpl_global_corners_sptr" ;
  input_types_[i++] = "int";  //scene id
  input_types_[i++] = "float"; //threshold
  input_types_[i++] = "vcl_string"; //output path for thresholded scenes

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_threshold_corners_process(bprb_func_process& pro)
{
  using namespace bvpl_threshold_corners_process_globals;
  typedef vbl_smart_ptr<bvpl_global_taylor<double, 10> > bvpl_global_taylor_sptr;

  //get inputs
  unsigned i = 0;
  bvpl_global_taylor_sptr global_taylor = pro.get_input<bvpl_global_taylor_sptr>(i++);
  bvpl_global_corners_sptr global_corners = pro.get_input<bvpl_global_corners_sptr>(i++);
  int scene_id = pro.get_input<int>(i++);
  auto corner_thresh = pro.get_input<float>(i++);
  std::string output_path = pro.get_input<std::string>(i++);

  if (!(global_taylor && global_corners))
    return false;

  global_corners->threshold_laptev_corners(global_taylor, scene_id, corner_thresh, output_path);

  return true;
}
