#include <testlib/testlib_test.h>

#if HAS_OPENCL
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <bbas/volm/volm_query.h>
#include <bbas/volm/volm_query_sptr.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <boxm2/volm/boxm2_volm_matcher_p1.h>
#include <volm/volm_loc_hyp.h>
#include <bbas/volm/volm_io.h>
#include <bbas/volm/volm_tile.h>
#include <boxm2/volm/boxm2_volm_locations.h>
#include <boxm2/volm/boxm2_volm_locations_sptr.h>
#include <bbas/bocl/bocl_manager.h>
#include <bbas/bocl/bocl_device.h>
#include <vcl_set.h>
#include <vcl_iostream.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <vpl/vpl.h>

static void test_volm_matcher_p1()
{

  // input parameters (modify accordingly)
  vcl_string geo_index_folder = "Z:/projects/FINDER/index/geoindex_zone_17_inc_2_nh_100_pa_5/";
  vcl_string         cam_file = "D:/work/find/volm_matcher/test1/local_output/test_weight_pa_5/p1a_test1_40/camera_space.bin";
  vcl_string         dms_file = "Z:/projects/FINDER/test1/p1a_test1_40/p1a_test1_40.vsl";
  vcl_string       out_folder = "D:/work/find/volm_matcher/test1/output/test_weight_pa_5/p1a_test1_40/";
  vcl_string   candidate_file = "Z:/projects/FINDER/test1/candidate_lists/p1a_test1_40/out.txt";
  vcl_string    sph_shell_bin = "Z:/projects/FINDER/index/sph_shell_vsph_ca_180_pa_5_ta_75_ba_75.bin";
  vcl_string      weight_file = "D:/work/find/volm_matcher/test1/local_output/test_weight_pa_5/p1a_test1_40/weight_param.txt";

  unsigned tile_id = 3;
  float buffer_capacity = 1.0f;
  bool is_candidate = true;
  bool is_last_pass = false;
  bool use_orient = true;
  unsigned dev_id = 0;
  float threshold = 0.2;
  unsigned max_cam_per_loc = 30;


  // constrcture the tree
  vcl_stringstream file_name_pre;
  file_name_pre << geo_index_folder << "geo_index_tile_" << tile_id;
  vcl_cout << "constructing: " << file_name_pre.str() << vcl_endl;
  float min_size;
  // read the tree and fill the leaves with hypotheses
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());

  // check the tree before prune
  vcl_vector<volm_geo_index_node_sptr> leaves_bef;
  volm_geo_index::get_leaves(root, leaves_bef);
  for (unsigned li = 0; li < leaves_bef.size(); li++) {
    if (leaves_bef[li]->hyps_) {
      vcl_cout << " li = " << li << " index_file = " << leaves_bef[li]->get_index_name(file_name_pre.str()) << vcl_endl;
    }
  }
  // prune geo_index tree given candidate list
  // read txt format candidiate list
  vgl_polygon<double> cand_poly;
  volm_io::read_polygons(candidate_file, cand_poly);
  vcl_cout << " num of sheets in the candidate list = " << cand_poly.num_sheets() << vcl_endl;
  volm_geo_index::prune_tree(root, cand_poly);

  // create wr3db index based on pruned tree
  vcl_vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);

  // read in the prameter to create container
  boxm2_volm_wr3db_index_params params;
  vcl_string index_file = leaves[0]->get_index_name(file_name_pre.str());
  if (!params.read_params_file(index_file)) {
    vcl_cerr << " ERROR: cannot read params file from " << index_file << vcl_endl;
  }
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);
  
  // read the sph_shell binary
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container;
  vsl_b_ifstream sph_ifs(sph_shell_bin);
  sph_shell->b_read(sph_ifs);
  sph_ifs.close();
  unsigned layer_size = sph_shell->get_container_size();

  // create depth interval;
  vcl_map<double, unsigned char>& depth_interval = sph->get_depth_interval_map();
  vcl_map<double, unsigned char>::iterator iter = depth_interval.begin();
  vcl_vector<float> depth_interval_rev;
  for ( ; iter != depth_interval.end(); ++iter) {
    depth_interval_rev.push_back((float)iter->first);
  }
  
  // read camera space binary
  vsl_b_ifstream ifs_cam(cam_file);
  volm_camera_space_sptr cam_space = new volm_camera_space();
  cam_space->b_read(ifs_cam);
  ifs_cam.close();

  // create query
  volm_query_sptr query = new volm_query(cam_space, dms_file, sph_shell, sph);
  // screen output of query
  unsigned total_size = query->obj_based_query_size_byte();
  vcl_cout << "\n==================================================================================================\n"
           << "\t\t  2. Create query from given camera space and Labelme geometry\n"
           << "\t\t  generate query has " << query->get_cam_num() << " cameras "
           << " and " << (float)total_size/1024 << " Kbyte in total\n"
           << "==================================================================================================\n" << vcl_endl;
  depth_map_scene_sptr dm = query->depth_scene();
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;
  if (dm->sky().size()) {
    vcl_cout << " -------------- SKYs --------------" << vcl_endl;
    for (unsigned i = 0; i < dm->sky().size(); i++)
      vcl_cout << "\t name = " << (dm->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << (int)query->sky_orient()
               << vcl_endl;
  }
  if (dm->ground_plane().size()) {
    vcl_cout << " -------------- GROUND PLANE --------------" << vcl_endl;
    for (unsigned i = 0; i < dm->ground_plane().size(); i++)
      vcl_cout << "\t name = " << dm->ground_plane()[i]->name()
               << ", depth = " << dm->ground_plane()[i]->min_depth()
               << ", orient = " << dm->ground_plane()[i]->orient_type()
               << ", land_id = " << dm->ground_plane()[i]->land_id()
               << ", land_name = " << volm_label_table::land_string(dm->ground_plane()[i]->land_id())
               << vcl_endl;
  }
  if (dm->scene_regions().size()) {
    vcl_cout << " -------------- DEPTH REGIONS --------------" << vcl_endl;
    for (unsigned i = 0; i < dm->scene_regions().size(); i++) {
      vcl_cout << "\t " <<  (dm->scene_regions())[i]->name()  << " region "
               << ",\t min_depth = " << (dm->scene_regions())[i]->min_depth()
               << ",\t max_depth = " << (dm->scene_regions())[i]->max_depth()
               << ",\t order = " << (dm->scene_regions())[i]->order()
               << ",\t orient = " << (dm->scene_regions())[i]->orient_type()
               << ",\t NLCD_id = " << (dm->scene_regions())[i]->land_id()
               << ",\t land_name = " << volm_label_table::land_string((dm->scene_regions())[i]->land_id())
               << vcl_endl;
    }
  }


  // read (or create) weight parameters for depth_map_scene
  vcl_vector<volm_weight> weights;
  volm_weight::read_weight(weights, weight_file);

  // define the device that will be used

  bocl_manager_child_sptr mgr = bocl_manager_child::instance();

  boxm2_volm_matcher_p1 obj_order_matcher(query, leaves, buffer_capacity, geo_index_folder, tile_id,
                                          depth_interval_rev, cand_poly, mgr->gpus_[dev_id], is_candidate, is_last_pass, out_folder,
                                          threshold, max_cam_per_loc, weights);
  bool good = obj_order_matcher.volm_matcher_p1();
 
  // output

  vcl_stringstream out_fname_bin;
  out_fname_bin << out_folder << "pass1_scores_tile_" << tile_id << ".bin";
  vcl_stringstream out_fname_txt;
  out_fname_txt << out_folder << "pass1_scores_tile_" << tile_id << ".txt";

  good = obj_order_matcher.write_matcher_result(out_fname_bin.str(), out_fname_txt.str());

#if 0
  vcl_vector<volm_score_sptr> scores;
  volm_score::read_scores(scores, out_fname_bin.str());  // this file may be too large, make sure it fits to memory!!
  
  vcl_cout << " THE READ IN BINRAY SCORE FILE " << vcl_endl;
  for (unsigned i = 0; i < scores.size(); i++) {
    vcl_cout << scores[i]->leaf_id_ << " " << scores[i]->hypo_id_
             << " " << scores[i]->max_score_ << " " << scores[i]->max_cam_id_ << vcl_endl;
    vcl_cout << " cam_id: \t";
    vcl_vector<unsigned> cam_ids = scores[i]->cam_id_;
    for (unsigned jj = 0; jj < cam_ids.size(); jj++)
      vcl_cout << ' ' << cam_ids[jj];
    vcl_cout << '\n';
  }
#endif

}

#else // HAS_OPENCL
static void test_volm_matcher_p1() {}
#endif // HAS_OPENCL
TESTMAIN(test_volm_matcher_p1);
