// This is brl/bbas/volm/pro/processes/volm_transfer_geoindex_id_to_str.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//          Transfer the leaf id in geoindex to their coordinate string
//
//
#include <bprb/bprb_parameters.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <vul/vul_file.h>
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>

namespace volm_transfer_geoindex_id_to_str_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}

bool volm_transfer_geoindex_id_to_str_process_cons(bprb_func_process& pro)
{
  using namespace volm_transfer_geoindex_id_to_str_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // geo_index2 tree structure txt
  input_types_[1] = "vcl_string";  // output text
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool volm_transfer_geoindex_id_to_str_process(bprb_func_process& pro)
{
  using namespace volm_transfer_geoindex_id_to_str_process_globals;

  if (pro.n_inputs() < n_inputs_) {
    std::cout << "Error: " << pro.name() << ": the input number should be " << n_inputs_ << std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  std::string tree_txt = pro.get_input<std::string>(i++);
  std::string  out_txt = pro.get_input<std::string>(i++);

  // load the tree
  if (!vul_file::exists(tree_txt)) {
    std::cout << "Error: " << pro.name() << ": can not find tree structure file: " << tree_txt << std::endl;
    return false;
  }
  double min_size;
  volm_geo_index2_node_sptr root = volm_geo_index2::read_and_construct<volm_loc_hyp_sptr>(tree_txt, min_size);
  std::vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);

  std::ofstream ofs(out_txt.c_str());
  ofs << " leaf_id \t leaf_coordinates" << std::endl;
  for (unsigned i = 0; i < leaves.size(); i++)
    ofs << i << " \t " << leaves[i]->get_string() << std::endl;

  return true;
}

namespace volm_transfer_geoindex_id_to_str_process2_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool volm_transfer_geoindex_id_to_str_process2_cons(bprb_func_process& pro)
{
  using namespace volm_transfer_geoindex_id_to_str_process2_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "float";       // min_size of geo_index2
  input_types_[1] = "vcl_string";  // geological region of geo_index2
  input_types_[2] = "vcl_string";  // output text
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_transfer_geoindex_id_to_str_process2(bprb_func_process& pro)
{
  using namespace volm_transfer_geoindex_id_to_str_process2_globals;
  if (pro.n_inputs() < n_inputs_) {
    std::cout << "Error: " << pro.name() << ": the input number should be " << n_inputs_ << std::endl;
    return false;
  }
  // get the input
  unsigned i = 0;
  auto min_size = pro.get_input<float>(i++);
  std::string in_poly = pro.get_input<std::string>(i++);
  std::string out_txt = pro.get_input<std::string>(i++);
  // create the tree
  if (!vul_file::exists(in_poly)) {
    std::cout << "Error: " << pro.name() << ": can not find input file: " << in_poly << std::endl;
    return false;
  }
  // find the bbox of ROI from the input polygon
  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly);
  vgl_box_2d<double> bbox;
  for (auto i : poly[0])
    bbox.add(i);
  std::cout << "bbox of ROI: " << bbox << std::endl;

  // create a geo index and use the leaves as scenes, use template param as volm_loc_hyp_sptr but it won't actually be used
  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(bbox, min_size, poly);
  std::vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);

  std::ofstream ofs(out_txt.c_str());
  ofs << " leaf_id \t leaf_coordinates" << std::endl;
  for (unsigned i = 0; i < leaves.size(); i++)
    ofs << i << " \t " << leaves[i]->get_string() << std::endl;

  return true;
}
