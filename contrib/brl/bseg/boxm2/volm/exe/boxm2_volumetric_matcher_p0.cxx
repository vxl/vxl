//:
// \file
// \brief executable to match a given volumetric query and a camera estimate to an indexed reference volume using different matcher
// \author Yi Dong
// \date Jan 19, 2013

#include <volm/volm_io.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>

#include <volm/volm_camera_space.h>
#include <volm/volm_loc_hyp.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <boxm2/volm/boxm2_volm_matcher_p0.h>
#include <bbas/bocl/bocl_manager.h>
#include <bbas/bocl/bocl_device.h>
#include <bkml/bkml_parser.h>
#include <vil/vil_save.h>

int main(int argc, char** argv)
{
  // input
  vul_arg<vcl_string> local_folder("-loc_out", "local output folder where the intermediate files are stored", ""); // camera space will be read from here
  vul_arg<vcl_string> params_file("-params", "text file with the params to construct camera space, spherical containers and query", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");                // query labelme xml
  vul_arg<vcl_string> geo_index_folder("-geo", "folder to read the geo index and the hypo", ""); // folder to read the geo_index and hypos for each leaf
  vul_arg<vcl_string> candidate_list("-cand", "candidate list for given query (txt file)", "");  // candidate list file containing polygons
  vul_arg<float>      buffer_capacity("-buff", "index buffer capacity (GB)", 1.0f);
  vul_arg<unsigned>   tile_id("-tile", "ID of the tile that current matcher consider",30);
  vul_arg<vcl_string> out_folder("-out", "output folder where the results are stored", "");
  vul_arg<float>      threshold("-thres", "only the cameras that are above this threshold are saved to binary file to be used in the next pass", 0.9f);
  vul_arg<unsigned>   max_cam_per_loc("-max_cam", "max number of cameras to be saved per location", 200);
  vul_arg<bool>       logger("-logger", "designate one of the exes as logger", false); // if -logger exists as a command argument then this one is logger exe is to do logging and generate the status.xml file
  vul_arg<bool>       generate_prob("-prob", "generate an output prob map for the tile", false);
  vul_arg<float>      score_scale_threshold("-scale_thres", "threshold to scale output scores to 1-255 in the output prob maps", 0.8f);
  vul_arg_parse(argc, argv);

  vcl_stringstream log;
  bool do_log = false;
  if (logger())
    do_log = true;
  vcl_cout << "argc: " << argc << vcl_endl;
  // check the input parameters
  if ( label_file().compare("") == 0 || geo_index_folder().compare("") == 0 || out_folder().compare("") == 0 || local_folder().compare("") == 0 || params_file().compare("") == 0) {
    vcl_cerr << " ERROR: input file/folders can not be empty!\n";
    log << "EXE_ARGUMENT_ERROR!\n";
    if (do_log) volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // check the query input files
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  vcl_cout << label_file() << vcl_endl;
  if (!volm_io::read_labelme(label_file(), dm, img_category)) {
    log << "problem parsing: " << label_file() << vcl_endl;
    vcl_cerr << log.str() << vcl_endl;
    return volm_io::LABELME_FILE_IO_ERROR;
  }

  vcl_vector<volm_tile> tiles;
  if (img_category == "desert")
    tiles = volm_tile::generate_p1_wr1_tiles();
  else if (img_category == "coast")
    tiles = volm_tile::generate_p1_wr2_tiles();
  else {
    vcl_string error_msg = " ERROR: unknown image category " + img_category + " (should be desert or coast), generating tile for output failed";
    vcl_cerr << error_msg << '\n';
    if (do_log) volm_io::write_status(out_folder(), volm_io::LABELME_FILE_IO_ERROR, 100, error_msg);
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  if (tile_id() >= tiles.size()) {
    vcl_string error_msg = " ERROR: unknown image category " + img_category + " (should be desert or coast), generating tile for output failed";
    vcl_cerr << error_msg << '\n';
    if (do_log) volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR, 100, error_msg);
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // read the params
  volm_io_expt_params params; params.read_params(params_file());

  vsl_b_ifstream ifs(local_folder() + "camera_space.bin");
  volm_camera_space_sptr camera_space = new volm_camera_space;
  camera_space->b_read(ifs);
  ifs.close();
  vcl_cout << " READ camera space, number of cams: " << camera_space->valid_indices().size() << vcl_endl;

  vsl_b_ifstream ifs2(local_folder() + "sph_shell_container.bin");
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container;
  sph_shell->b_read(ifs2);
  ifs2.close();
  vcl_cout << " READ sph_shell, layer size: " << sph_shell->get_container_size() << vcl_endl;

  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);

  // read the geo_index, create geo_index tree, read in parameters
  vcl_stringstream file_name_pre;
  file_name_pre << geo_index_folder() << "geo_index_tile_" << tile_id();
  if (!vul_file::exists(file_name_pre.str() + ".txt")) {
    if (do_log) { volm_io::write_status(out_folder(), volm_io::GEO_INDEX_FILE_MISSING); volm_io::write_log(out_folder(), log.str()); }
    vcl_cerr << log.str() << vcl_endl;
    return volm_io::GEO_INDEX_FILE_MISSING;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);

  // check whether we have candidate list for this query
  vgl_polygon<double> cand_poly;
  vcl_cout << " candidate list = " <<  candidate_list() << vcl_endl;

  if ( candidate_list().compare("") != 0 ) {
    if (vul_file::extension(candidate_list()).compare(".txt") == 0)
      volm_io::read_polygons(candidate_list(), cand_poly);
    else
      cand_poly = bkml_parser::parse_polygon(candidate_list());
  }
  else
  {
    if (do_log) { volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR); volm_io::write_log(out_folder(), log.str()); }
    log << " ERROR: no candidate list or candidate list exist with wrong format: " << candidate_list() << '\n';
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // prune the leaves which do not intersect or contained by the candidate region polygons
  volm_geo_index::prune_tree(root, cand_poly);

  volm_geo_index::read_hyps(root, file_name_pre.str());

  vcl_vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);


  vcl_cout << "\n==================================================================================================\n"
           << "\t\t  2. Create query from given camera space and Labelme geometry\n"
           << "==================================================================================================\n" << vcl_endl;

  // create query
  volm_query_sptr query = new volm_query(camera_space, label_file(), sph, sph_shell);
  // screen output of query
  unsigned total_size = query->obj_based_query_size_byte();
  vcl_cout << "\t\t  generate query has " << query->get_cam_num() << " cameras "
           << " and " << (float)total_size/1024 << " Kbyte in total\n"

           << "\n==================================================================================================\n"
           << "\t\t  4. Start volumetric matching pass 0\n"
           << "==================================================================================================\n" << vcl_endl;

  boxm2_volm_matcher_p0 matcher(camera_space, query, threshold(), max_cam_per_loc());
  vcl_vector<volm_score_sptr> scores;
  for (unsigned i = 0; i < leaves.size(); i++) {
    boxm2_volm_wr3db_index_params params;
    vcl_string index_file = leaves[i]->get_index_name(file_name_pre.str());
    vcl_string label_index_file = leaves[i]->get_label_index_name(file_name_pre.str(), "orientation");
    if (!vul_file::exists(label_index_file) || !vul_file::exists(index_file)) {
      log << " cannot find index files: " << label_index_file << " or " << index_file << " exiting..!\n";
      if (do_log) {
         volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FAILED);
         volm_io::write_log(out_folder(), log.str());
      }
      vcl_cerr << log.str() << vcl_endl;
      return volm_io::MATCHER_EXE_FAILED;
    }

    if (!params.read_params_file(index_file)) {
      log << " ERROR: cannot read params file from " << index_file << '\n';
      if (do_log) {
         volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FAILED);
         volm_io::write_log(out_folder(), log.str());
      }
      vcl_cerr << log.str() << vcl_endl;
      return volm_io::MATCHER_EXE_FAILED;
    }

    boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(params.layer_size, buffer_capacity());
    boxm2_volm_wr3db_index_sptr ind_o = new boxm2_volm_wr3db_index(params.layer_size, buffer_capacity());

    if (!ind->initialize_read(index_file) || !ind_o->initialize_read(label_index_file)) {
      log << " ERROR: cannot load index from " << index_file << " or from: " << label_index_file << '\n';
      if (do_log) {
        volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
        volm_io::write_log(out_folder(), log.str());
      }
      vcl_cerr << log.str();
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    vgl_point_3d<double> h_pt;
    vcl_vector<unsigned char> values(params.layer_size);
    for (unsigned j = 0; j < leaves[i]->hyps_->locs_.size(); j++) {
      ind->get_next(values);
      vgl_point_3d<double> h_pt = leaves[i]->hyps_->locs_[j];
      if (cand_poly.contains(h_pt.x(), h_pt.y())) {  // match
        volm_score_sptr score = new volm_score(i, j);
        if (matcher.match(h_pt, values, score))
          scores.push_back(score);
      }
    }
  }
  // write the scores out
  vcl_stringstream score_file_name; score_file_name << local_folder() << "pass0_scores_tile_" << tile_id() << ".bin";
  volm_score::write_scores(scores, score_file_name.str());

  if (generate_prob()) {
    // output will be a probability map
    vcl_string out_name = out_folder() + "/ProbMap_" + tiles[tile_id()].get_string() + ".tif";
    volm_tile t = tiles[tile_id()];
    vcl_cout << "\n===============================================================================================================\n"
             << "\t  5. Generate probability map for tile " << tile_id() << " of category: " << img_category << " and store it in\n"
             << "\t     " << out_name
             << "\n===============================================================================================================\n" << vcl_endl;

    vil_image_view<vxl_byte> out(3601, 3601);
    out.fill(volm_io::UNEVALUATED);
    for (unsigned i = 0; i < scores.size(); i++) {
      // get hyp
      vgl_point_3d<double> h_pt = leaves[scores[i]->leaf_id_]->hyps_->locs_[scores[i]->hypo_id_];
      // figure out which pixel
      unsigned u,v;
      if (t.global_to_img(h_pt.x() , h_pt.y(), u, v)) {
        unsigned char current_score = volm_io::scale_score_to_1_255(score_scale_threshold(), scores[i]->max_score_);
        if (out(u,v) < current_score)
          out(u,v) = current_score;
      }
    }
    vil_save(out, out_name.c_str());
  }

  return volm_io::SUCCESS;
}
