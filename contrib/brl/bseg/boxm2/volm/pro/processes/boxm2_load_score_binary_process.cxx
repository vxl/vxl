// This is brl/bseg/boxm2/volm/pro/process/boxm2_load_score_binary_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to read score binary file from volm matcher and generate a text file to list the score at each location hypotheses
// NOTE: the output text can be very large
//
// \author Yi Dong
// \date June 08, 2014
#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_file.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <bkml/bkml_parser.h>
#include <vcl_iomanip.h>

namespace boxm2_load_score_binary_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 0;
}

bool boxm2_load_score_binary_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_load_score_binary_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // geo index folder
  input_types_[1] = "vcl_string";  // candidate list if exists
  input_types_[2] = "vcl_string";  // score file
  input_types_[3] = "vcl_string";  // output text file
  input_types_[4] = "unsigned";    // tile id

  vcl_vector<vcl_string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_load_score_binary_process(bprb_func_process& pro)
{
  using namespace boxm2_load_score_binary_process_globals;

  // check input
  if (!pro.verify_inputs()) {
    vcl_cerr << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }
  // get input
  unsigned in_i = 0;
  vcl_string geo_hypo_folder = pro.get_input<vcl_string>(in_i++);
  vcl_string candidate_list  = pro.get_input<vcl_string>(in_i++);
  vcl_string score_file      = pro.get_input<vcl_string>(in_i++);
  vcl_string out_text        = pro.get_input<vcl_string>(in_i++);
  unsigned tile_id           = pro.get_input<unsigned>(in_i++);

  // load the candidate list if exists
  bool is_candidate = false;
  vgl_polygon<double> cand_poly;
  vcl_cout << " candidate list = " <<  candidate_list << vcl_endl;
  if ( candidate_list.compare("") != 0) {
    if (!vul_file::exists(candidate_list)) {
      vcl_cout << pro.name() << ": can not find candidate list file: " << candidate_list << '\n';
      return false;
    }
    else {
      // parse polygon from kml
      is_candidate = true;
      cand_poly = bkml_parser::parse_polygon(candidate_list);
      vcl_cout << " candidate list is parsed from file: " << candidate_list << vcl_endl;
      vcl_cout << " number of sheet in the candidate poly " << cand_poly.num_sheets() << vcl_endl;
    }
  }
  else {
    vcl_cout << " NO candidate list for this query image, full index space is considered" << vcl_endl;
    is_candidate = false;
  }

  // load the geo index to retrieve the location data
  vcl_stringstream file_name_pre;
  file_name_pre << geo_hypo_folder << "geo_index_tile_" << tile_id;
  if (!vul_file::exists(file_name_pre.str() + ".txt")) {
    vcl_cout << pro.name() << ": can not find geo index text file: " << file_name_pre.str() << '\n';
    return false;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  if (is_candidate) {
    volm_geo_index::prune_tree(root, cand_poly);
  }
  vcl_vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);

  // load score binary
  if (!vul_file::exists(score_file)) {
    vcl_cout << pro.name() << ": can not find score binary file " << score_file << '\n';
    return false;
  }
  vcl_vector<volm_score_sptr> scores;
  volm_score::read_scores(scores, score_file);

  // construct the location-score data
  // 1st element: lon,  2nd element: lat,  3rd element: scores
  vcl_vector<vcl_vector<double> > loc_scores;
  unsigned total_ind = scores.size();
  for (unsigned i = 0; i < total_ind; i++) {
    vgl_point_3d<double> loc_pt = leaves[scores[i]->leaf_id_]->hyps_->locs_[scores[i]->hypo_id_];
    vcl_vector<double> tmp;
    tmp.push_back(loc_pt.x());  tmp.push_back(loc_pt.y());  tmp.push_back(scores[i]->max_score_);
    loc_scores.push_back(tmp);
  }

  // screen output
  vcl_cout << scores.size() << " location scores are loaded from score file: " << score_file << vcl_endl;

  // write to text file
  vcl_ofstream fout(out_text.c_str());
  fout << "lon      lat      score\n";
  for (unsigned i = 0; i < loc_scores.size(); i++)
    fout << vcl_setprecision(6) << vcl_fixed << loc_scores[i][0] << ' '
         << vcl_setprecision(6) << vcl_fixed << loc_scores[i][1] << ' '
         << vcl_setprecision(6) << vcl_fixed << loc_scores[i][2] << '\n';
  fout.close();
  return true;
}
