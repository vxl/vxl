#include <testlib/testlib_test.h>

#if HAS_OPENCL
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <bbas/volm/volm_query.h>
#include <bbas/volm/volm_query_sptr.h>
#include <boxm2/volm/boxm2_volm_obj_based_matcher.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <boxm2/volm/boxm2_volm_matcher_p1.h>
#include <volm/volm_loc_hyp.h>
#include <vul/vul_timer.h>
#include <bbas/volm/volm_io.h>
#include <bbas/volm/volm_tile.h>
#include <vil/vil_save.h>
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
  vcl_string geo_index_folder = "D:\\work\\Dropbox\\FINDER\\index\\geoindex_zone_17_inc_2_nh_100\\";
  vcl_string cam_file = "D:\\work\\find\\volm_matcher\\finderuploads\\coast\\job_174\\Camera_test.kml";
  vcl_string label_file = "D:\\work\\find\\volm_matcher\\finderuploads\\coast\\job_174\\Result_own_calibration_ori.xml";
  vcl_string out_folder = "D:\\work\\find\\volm_matcher\\finderuploads\\coast\\job_174\\integration_matcher_test\\";
  vcl_string candidate_file = "D:\\work\\find\\volm_matcher\\candidate_lists\\p1a_test1_36\\out.txt";


  unsigned tile_id = 3;
  float buffer_capacity = 1.0f;
  bool is_candidate = true;
  bool is_last_pass = false;
  unsigned gpu_id = 0;

  // check the query input files
  depth_map_scene_sptr dms = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_file, dms, img_category)) {
    vcl_cerr << "problem parsing: " << label_file << vcl_endl;
  }
  // generate depth_map_scene binary
#if 0
  vcl_string dm_binary = out_folder + "dms_bindary.vsl";
  // write
  vsl_b_ofstream sos(dm_binary);
  vsl_b_write(sos, dms);
  sos.close();
  // read
  vsl_b_ifstream sis(dm_binary);
  depth_map_scene_sptr s_in;
  vsl_b_read(sis, s_in);

  if (dms->sky().size()) {
    vcl_cout << " -------------- SKYs -------------- " << vcl_endl;
    for (unsigned i = 0; i < dms->sky().size(); i++)
      vcl_cout << "\t name = " << (dms->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << 100
               << vcl_endl;
  }
  if (dms->ground_plane().size()) {
    vcl_cout << " -------------- GROUND PLANE -------------- " << vcl_endl;
    for (unsigned i = 0; i < dms->ground_plane().size(); i++)
      vcl_cout << "\t name = " << dms->ground_plane()[i]->name()
               << ", depth = " << dms->ground_plane()[i]->min_depth()
               << ", orient = " << dms->ground_plane()[i]->orient_type()
               << ", NLCD_id = " << dms->ground_plane()[i]->nlcd_id() 
               << " ---> " << (int)volm_nlcd_table::land_id[dms->ground_plane()[i]->nlcd_id()]
               << vcl_endl;
  }
  if (dms->scene_regions().size()) {
    vcl_cout << " -------------- DEPTH REGIONS -------------- " << vcl_endl;
    for (unsigned i = 0; i < dms->scene_regions().size(); i++) {
      vcl_cout << "\t " <<  (dms->scene_regions())[i]->name()  << " region "
               << ",\t min_depth = " << (dms->scene_regions())[i]->min_depth()
               << ",\t max_depth = " << (dms->scene_regions())[i]->max_depth()
               << ",\t order = " << (dms->scene_regions())[i]->order()
               << ",\t orient = " << (dms->scene_regions())[i]->orient_type()
               << ",\t NLCD_id = " << (dms->scene_regions())[i]->nlcd_id()
               << " ---> " << (int)volm_nlcd_table::land_id[dms->scene_regions()[i]->nlcd_id()]
               << vcl_endl;
    }
  }


  vcl_cout << " READED FROM BINARY --- CHECK " << vcl_endl;

  if (s_in->sky().size()) {
  vcl_cout << " -------------- SKYs -------------- " << vcl_endl;
  for (unsigned i = 0; i < s_in->sky().size(); i++)
      vcl_cout << "\t name = " << (s_in->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << 100
               << vcl_endl;
  }
  if (s_in->ground_plane().size()) {
    vcl_cout << " -------------- GROUND PLANE -------------- " << vcl_endl;
    for (unsigned i = 0; i < s_in->ground_plane().size(); i++)
      vcl_cout << "\t name = " << s_in->ground_plane()[i]->name()
               << ", depth = " << s_in->ground_plane()[i]->min_depth()
               << ", orient = " << s_in->ground_plane()[i]->orient_type()
               << ", NLCD_id = " << s_in->ground_plane()[i]->nlcd_id() 
               << " ---> " << (int)volm_nlcd_table::land_id[s_in->ground_plane()[i]->nlcd_id()]
               << vcl_endl;
  }
  if (s_in->scene_regions().size()) {
    vcl_cout << " -------------- DEPTH REGIONS -------------- " << vcl_endl;
    for (unsigned i = 0; i < s_in->scene_regions().size(); i++) {
      vcl_cout << "\t " <<  (s_in->scene_regions())[i]->name()  << " region "
               << ",\t min_depth = " << (s_in->scene_regions())[i]->min_depth()
               << ",\t max_depth = " << (s_in->scene_regions())[i]->max_depth()
               << ",\t order = " << (s_in->scene_regions())[i]->order()
               << ",\t orient = " << (s_in->scene_regions())[i]->orient_type()
               << ",\t NLCD_id = " << (s_in->scene_regions())[i]->nlcd_id()
               << " ---> " << (int)volm_nlcd_table::land_id[s_in->scene_regions()[i]->nlcd_id()]
               << vcl_endl;
    }
  }
#endif


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
    if (leaves_bef[li]->hyps_){
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
  vcl_vector<volm_geo_index_node_sptr> leaves_prune;
  volm_geo_index::get_leaves(root, leaves_prune);

  // prune tree again to get rid of leaves without any hypo in it
  vcl_vector<volm_geo_index_node_sptr> leaves;
  for (unsigned li = 0; li < leaves_prune.size(); li++)
    if (leaves_prune[li]->hyps_)
      leaves.push_back(leaves_prune[li]);

  // read in the prameter
  boxm2_volm_wr3db_index_params params;
  vcl_string index_file = leaves[0]->get_index_name(file_name_pre.str());
  if (!params.read_params_file(index_file)) {
    vcl_cerr << " ERROR: cannot read params file from " << index_file << vcl_endl;
  }
  vcl_cout << " point_angle = " << params.point_angle << ",  top_angle = " << params.top_angle << ",  bottom_angle = " << params.bottom_angle << vcl_endl;
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, params.cap_angle, params.point_angle, params.top_angle, params.bottom_angle);
  unsigned layer_size = sph_shell->get_container_size();

  // create depth interval;
  vcl_map<double, unsigned char>& depth_interval = sph->get_depth_interval_map();
  vcl_map<double, unsigned char>::iterator iter = depth_interval.begin();
  vcl_vector<float> depth_interval_rev;
  for ( ; iter != depth_interval.end(); ++iter) {
    vcl_cout << " depth = " << iter->first << " --- interval = " << (int)iter->second << vcl_endl;
    depth_interval_rev.push_back((float)iter->first);
  }
  vcl_cout << " check the table " << vcl_endl;
  for ( unsigned i = 0; i < depth_interval_rev.size(); i++) {
    vcl_cout << " depth = " << depth_interval_rev[i] << " --- interval = " << i << vcl_endl;
  }


  // load the indices for all leaves
  vcl_vector<boxm2_volm_wr3db_index_sptr> ind_vec;
  for (unsigned i = 0; i < leaves.size(); i++) {
    if(!leaves[i]->hyps_)
      continue;
    boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index((unsigned)(sph_shell->get_container_size()), buffer_capacity);
    vcl_string index_file = leaves[i]->get_index_name(file_name_pre.str());
    if(!ind->initialize_read(index_file))
      vcl_cerr << "ERROR: can not load index for leaf " << i << ", file name = " << index_file << vcl_endl;
    ind_vec.push_back(ind);
  }
  

  if(ind_vec.size() != leaves.size())
    vcl_cerr << " ERROR: the index vector NOT equal to leaves size" << vcl_endl;
  else
    vcl_cout << ind_vec.size() << " index files are loaded for " << leaves.size() << " leaves" << vcl_endl;


  // create query
  volm_query_sptr query = new volm_query(cam_file, label_file, sph, sph_shell, false);


  // screen output of query
  unsigned total_size = query->obj_based_query_size_byte();
  vcl_cout << "\n==================================================================================================\n";
  vcl_cout << "\t\t  2. Create query from given camera space and Labelme geometry\n";
  vcl_cout << "\t\t  generate query has " << query->get_cam_num() << " cameras "
           << " and " << (float)total_size/1024 << " Kbyte in total\n";
  vcl_cout << "==================================================================================================\n " << vcl_endl;
  depth_map_scene_sptr dm = query->depth_scene();
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;
  if (dm->sky().size()) {
    vcl_cout << " -------------- SKYs -------------- " << vcl_endl;
    for (unsigned i = 0; i < dm->sky().size(); i++)
      vcl_cout << "\t name = " << (dm->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << (int)query->sky_orient()
               << vcl_endl;
  }
  if (dm->ground_plane().size()) {
    vcl_cout << " -------------- GROUND PLANE -------------- " << vcl_endl;
    for (unsigned i = 0; i < dm->ground_plane().size(); i++)
      vcl_cout << "\t name = " << dm->ground_plane()[i]->name()
               << ", depth = " << dm->ground_plane()[i]->min_depth()
               << ", orient = " << dm->ground_plane()[i]->orient_type()
               << ", NLCD_id = " << dm->ground_plane()[i]->nlcd_id() 
               << " ---> " << (int)volm_nlcd_table::land_id[dm->ground_plane()[i]->nlcd_id()].first
               << vcl_endl;
  }
  if (dm->scene_regions().size()) {
    vcl_cout << " -------------- DEPTH REGIONS -------------- " << vcl_endl;
    for (unsigned i = 0; i < dm->scene_regions().size(); i++) {
      vcl_cout << "\t " <<  (dm->scene_regions())[i]->name()  << " region "
               << ",\t min_depth = " << (dm->scene_regions())[i]->min_depth()
               << ",\t max_depth = " << (dm->scene_regions())[i]->max_depth()
               << ",\t order = " << (dm->scene_regions())[i]->order()
               << ",\t orient = " << (dm->scene_regions())[i]->orient_type()
               << ",\t NLCD_id = " << (dm->scene_regions())[i]->nlcd_id()
               << " ---> " << (int)volm_nlcd_table::land_id[dm->scene_regions()[i]->nlcd_id()].first
               << vcl_endl;
    }
  }
  
  // define the device that will be used
  
  bocl_manager_child_sptr mgr = bocl_manager_child::instance();

  boxm2_volm_matcher_p1 obj_order_matcher(query, leaves, buffer_capacity, geo_index_folder, tile_id, depth_interval_rev, cand_poly, mgr->gpus_[0], is_candidate, is_last_pass, out_folder);
  bool good = obj_order_matcher.volm_matcher_p1();
  // FINALLY, clean all indice created
  for (unsigned i = 0; i < ind_vec.size(); i++) {
    ind_vec[i]->finalize();
  }

}

#else // HAS_OPENCL
static void test_volm_matcher_p1() {}
#endif // HAS_OPENCL
TESTMAIN(test_volm_matcher_p1);