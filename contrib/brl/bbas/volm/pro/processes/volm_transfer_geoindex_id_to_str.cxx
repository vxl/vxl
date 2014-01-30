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
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}

bool volm_transfer_geoindex_id_to_str_process_cons(bprb_func_process& pro)
{
  using namespace volm_transfer_geoindex_id_to_str_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // geo_index2 tree structure txt
  input_types_[1] = "vcl_string";  // output text
  vcl_vector<vcl_string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool volm_transfer_geoindex_id_to_str_process(bprb_func_process& pro)
{
  using namespace volm_transfer_geoindex_id_to_str_process_globals;

  if (pro.n_inputs() < n_inputs_) {
    vcl_cout << "Error: " << pro.name() << ": the input number should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vcl_string tree_txt = pro.get_input<vcl_string>(i++);
  vcl_string  out_txt = pro.get_input<vcl_string>(i++);

  // load the tree
  if (!vul_file::exists(tree_txt)) {
    vcl_cout << "Error: " << pro.name() << ": can not find tree structure file: " << tree_txt << vcl_endl;
    return false;
  }
  double min_size;
  volm_geo_index2_node_sptr root = volm_geo_index2::read_and_construct<volm_loc_hyp_sptr>(tree_txt, min_size);
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);

  vcl_ofstream ofs(out_txt.c_str());
  ofs << " leaf_id \t leaf_coordinates" << vcl_endl;
  for (unsigned i = 0; i < leaves.size(); i++)
    ofs << i << " \t " << leaves[i]->get_string() << vcl_endl;

  return true;
}

namespace volm_transfer_geoindex_id_to_str_process2_globals
{
  const unsigned n_inputs_  = 3;
  const unsigned n_outputs_ = 0;
}

bool volm_transfer_geoindex_id_to_str_process2_cons(bprb_func_process& pro)
{
  using namespace volm_transfer_geoindex_id_to_str_process2_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "float";       // min_size of geo_index2
  input_types_[1] = "vcl_string";  // geological region of geo_index2
  input_types_[2] = "vcl_string";  // output text
  vcl_vector<vcl_string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_transfer_geoindex_id_to_str_process2(bprb_func_process& pro)
{
  using namespace volm_transfer_geoindex_id_to_str_process2_globals;
  if (pro.n_inputs() < n_inputs_) {
    vcl_cout << "Error: " << pro.name() << ": the input number should be " << n_inputs_ << vcl_endl;
    return false;
  }
  // get the input 
  unsigned i = 0;
  float min_size = pro.get_input<float>(i++);
  vcl_string in_poly = pro.get_input<vcl_string>(i++);
  vcl_string out_txt = pro.get_input<vcl_string>(i++);
  // create the tree
  if (!vul_file::exists(in_poly)) {
    vcl_cout << "Error: " << pro.name() << ": can not find input file: " << in_poly << vcl_endl;
    return false;
  }
  // find the bbox of ROI from the input polygon
  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly);
  vgl_box_2d<double> bbox;
  for (unsigned i = 0; i < poly[0].size(); i++)
    bbox.add(poly[0][i]);
  vcl_cout << "bbox of ROI: " << bbox << vcl_endl;

  // create a geo index and use the leaves as scenes, use template param as volm_loc_hyp_sptr but it won't actually be used
  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(bbox, min_size, poly);
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);

  vcl_ofstream ofs(out_txt.c_str());
  ofs << " leaf_id \t leaf_coordinates" << vcl_endl;
  for (unsigned i = 0; i < leaves.size(); i++)
    ofs << i << " \t " << leaves[i]->get_string() << vcl_endl;

  return true;
}
