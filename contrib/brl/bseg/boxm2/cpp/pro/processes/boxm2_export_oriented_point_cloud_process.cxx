// This is brl/bseg/boxm2/pro/processes/boxm2_export_oriented_point_cloud_process.cxx
#include <iostream>
#include <fstream>
#include <sstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting the scene as a point cloud in XYZ or PLY format. The process expects datatypes BOXM2_POINT and BOXM2_NORMAL.
//         The process can take as input a bounding box, specified as two points in a ply file. In addition to points and normals, the process can
//         output quantities such as probabilities, visibility score(BOXM2_VIS_SCORE), as well as normal
//         magnitude(currently stored in BOXM2_NORMAL[4] for convenience).
//
// \author Ali Osman Ulusoy
// \date Oct 25, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_export_oriented_point_cloud.h>
#include <vgl/vgl_intersection.h>


namespace boxm2_export_oriented_point_cloud_process_globals
{
  constexpr unsigned n_inputs_ = 9;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_export_oriented_point_cloud_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_export_oriented_point_cloud_process_globals;

  //process takes 8 inputs (3 required ones), no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string"; //filename
  input_types_[3] = "bool"; //output additional info (prob, vis score, normal magnitude)
  input_types_[4] = "float"; //visibility score threshold
  input_types_[5] = "float"; //normal magnitude threshold
  input_types_[6] = "float"; //prob. threshold
  input_types_[7] = "float"; //exp. threshold
  input_types_[8] = "vcl_string"; //bounding box filename

  brdb_value_sptr output_prob = new brdb_value_t<bool>(false);
  pro.set_input(3, output_prob);

  brdb_value_sptr vis_t = new brdb_value_t<float>(0);
  pro.set_input(4, vis_t);

  brdb_value_sptr nmag_t = new brdb_value_t<float>(0);
  pro.set_input(5, nmag_t);

  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(6, prob_t);

  brdb_value_sptr exp_t = new brdb_value_t<float>(0.0);
  pro.set_input(7, exp_t);

  brdb_value_sptr bb_filename = new brdb_value_t<std::string>("");
  pro.set_input(8, bb_filename);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool boxm2_export_oriented_point_cloud_process (bprb_func_process& pro)
{
  using namespace boxm2_export_oriented_point_cloud_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  std::string output_filename = pro.get_input<std::string>(i++);
  bool output_aux = pro.get_input<bool>(i++);
  auto vis_t = pro.get_input<float>(i++);
  auto nmag_t = pro.get_input<float>(i++);
  auto prob_t = pro.get_input<float>(i++);
  auto exp_t = pro.get_input<float>(i++);
  std::string bb_filename = pro.get_input<std::string>(i++);

  return boxm2_export_oriented_point_cloud::export_oriented_point_cloud(scene, cache, output_filename,
                                                                        output_aux, vis_t, nmag_t, prob_t, exp_t,
                                                                        bb_filename);

}
