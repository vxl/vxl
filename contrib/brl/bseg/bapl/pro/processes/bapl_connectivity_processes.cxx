// This is brl/bseg/bapl/pro/processes/bapl_connectivity_processes.cxx
#include <bapl/bapl_connectivity.h>
//:
// \file
// \brief Processes to find tracks from a connectivity graph of an image set
//
// \author Ozge Can Ozcanli
// \date September 16, 2010
//
// \verbatim
//  Modifications
//    none
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <bapl/bapl_connectivity_sptr.h>
#include <bapl/bapl_keypoint_set.h>

//: Constructor
//  sets up an empty table instance
bool bapl_create_conn_table_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  ok = pro.set_input_types(input_types);
  input_types.push_back("int");  // number of images
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("bapl_conn_table_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_create_conn_table_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 1) {
    vcl_cout << "bapl_create_conn_table_process: The input number should be 1" << vcl_endl;
    return false;
  }
  unsigned i=0;
  int n_images = pro.get_input<int>(i++);

  bapl_conn_table_sptr table = new bapl_conn_table(n_images);
  pro.set_output_val<bapl_conn_table_sptr>(0, table);
  return true;
}

//: Constructor
//  add a match set to the table
bool bapl_add_match_set_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("bapl_conn_table_sptr");
  input_types.push_back("bapl_keypoint_match_set_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_add_match_set_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cout << "bapl_add_match_set_process: The input number should be 2" << vcl_endl;
    return false;
  }
  // get the inputs
  unsigned i=0;
  bapl_conn_table_sptr tab = pro.get_input<bapl_conn_table_sptr>(i++);
  bapl_keypoint_match_set_sptr set = pro.get_input<bapl_keypoint_match_set_sptr>(i++);
  if (!tab->add_sym(set)) {
    vcl_cout << "In  bapl_add_match_set_process() -- cannot add match set for image pair: (" << set->id_left_ << ", " << set->id_right_ << ")!\n";
    return false;
  }
  vcl_cout << *tab << vcl_endl;

  return true;
}

//: Constructor
//  add a match set to the table
bool bapl_add_image_keys_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("bapl_conn_table_sptr");
  input_types.push_back("int");
  input_types.push_back("bapl_keypoint_set_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_add_image_keys_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    vcl_cout << "bapl_add_image_keys_process: The input number should be 3" << vcl_endl;
    return false;
  }
  // get the inputs
  unsigned i=0;
  bapl_conn_table_sptr tab = pro.get_input<bapl_conn_table_sptr>(i++);
  int img_id = pro.get_input<int>(i++);
  bapl_keypoint_set_sptr set = pro.get_input<bapl_keypoint_set_sptr>(i++);
  tab->add_image_data(img_id, set);
  return true;
}

bool bapl_compute_tracks_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("bapl_conn_table_sptr");
  input_types.push_back("vcl_string");  // output file to print correspondences
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_compute_tracks_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cout << "bapl_compute_tracks_process: The input number should be 2" << vcl_endl;
    return false;
  }
  // get the inputs
  unsigned i=0;
  bapl_conn_table_sptr tab = pro.get_input<bapl_conn_table_sptr>(i++);
  vcl_string out_file = pro.get_input<vcl_string>(i++);
  vcl_cout << "match table (should be symmetric):" << vcl_endl;
  tab->print_table();
  vcl_vector<bapl_track_data> tracks;
  tab->compute_tracks(tracks);

  // Print tracks as correspondences in BWM_VIDEO_SITE format for visualization
  vcl_ofstream os(out_file.c_str());
  print_tracks(os, tracks);
  os.close();

  return true;
}


