//:
// \file
// \brief executable to index land type of locations and match queries based on land type of camera location
// \author Ozge C. Ozcanli
// \date Sept 30, 2013

#include <volm/desc/volm_desc_land.h>
#include <volm/desc/volm_desc_land_indexer.h>
#include <volm/desc/volm_desc_ex_2d_indexer.h>
#include <volm/desc/volm_desc_matcher.h>
#include <volm/desc/volm_desc_land_matcher.h>
#include <volm/volm_tile.h>
#include <volm/volm_io.h>
#include <vul/vul_arg.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>


int main(int argc,  char** argv)
{
  vul_arg<bool> match("-match", "if exists run the matcher, otherwise run the indexer", false);
  vul_arg<bool> random("-random", "if exists create output prob maps randomly, otherwise run the land type matcher");
  vul_arg<vcl_string> category_file("-cat", "category file that contains one line for the land type of the camera for the query ", "");
  vul_arg<vcl_string> category_gt_file("-cat_gt", "category gt file of multiple images, use image id to get gt loc of this one ", "");
  vul_arg<int> img_id("-id", "query image id in the category gt file, starts from 0", 1000);
  vul_arg<unsigned> world_id("-world", "world id of different ROI",100);
  vul_arg<vcl_string> out_folder("-out", "output folder to save index or if matching results", "");
  vul_arg<vcl_string> desc_index_folder("-desc", "folder to read the descriptor index of the tile", "");
  vul_arg<bool> save_images("-save", "save out images or not", false);
  
  // PARAMS for indexing
  vul_arg<vcl_string> lidar_folder("-lidar", "lidar folder to use for indexing", "");
  vul_arg<vcl_string> NLCD_folder("-nlcd", "NLCD folder to use for indexing", "");
  vul_arg<vcl_string> geo_hypo_folder("-hypo", "folder to read the geo hypotheses", "");                      // index -- folder to read the hypos for each leaf
  vul_arg<int> tile_id("-tile", "id of the tile to be indexed", -1);
  vul_arg<bool> index_2d("-ex_2d", "if exists index ex_2d using the classification maps that are passed", false);
  vul_arg<vcl_string> maps_folder("-maps", "the classification map images where each pixel has the land id (type is unsigned char)", "");
  vul_arg<vcl_string> radii_string("-rad", "e.g. pass 100_500_1000 for radii of 100, 500 and 1000 meter to construct descriptors at each location", "");
  vul_arg<int> max_leaf_id ("-max", "maximum leaf id considered", 1000);
  vul_arg<int> min_leaf_id ("-min", "minimum leaf id considered", 0);
  
  vul_arg_parse(argc, argv);
  vcl_cout << "argc: " << argc << vcl_endl;

  if (match()) {  // run the matcher
    
    vcl_vector<volm_tile> tiles;
    if (world_id() == 1)      tiles = volm_tile::generate_p1b_wr1_tiles();
    else if (world_id() == 2) tiles = volm_tile::generate_p1b_wr2_tiles();
    else if (world_id() == 3) tiles = volm_tile::generate_p1b_wr3_tiles();
    else if (world_id() == 4) tiles = volm_tile::generate_p1b_wr4_tiles();
    else if (world_id() == 5) tiles = volm_tile::generate_p1b_wr5_tiles();
    else {
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    if (random()) {
        if (geo_hypo_folder().compare("") == 0 || out_folder().compare("") == 0 || tile_id() < 0 || img_id() < 0) {
          vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
          volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
          vul_arg_display_usage_and_exit();
          return volm_io::EXE_ARGUMENT_ERROR;
        }

        volm_desc_matcher_sptr m = new volm_desc_land_matcher(); // need a deriving class just to call a base method
        vnl_random rng;
        m->create_random_prob_map(rng, geo_hypo_folder(), out_folder(), tile_id(), tiles[tile_id()]);
        double thres = 0.5;
        m->create_scaled_prob_map(out_folder(), tiles[tile_id()], tile_id(), 10, 127, thres); 

        volm_io::write_status(out_folder(), volm_io::SUCCESS);
        vcl_cout << "returning SUCCESS!\n";
        return volm_io::SUCCESS;
    }

    if (category_gt_file().compare("") == 0 || out_folder().compare("") == 0 || geo_hypo_folder().compare("") == 0 || tile_id() < 0 || img_id() < 0 || desc_index_folder().compare("") == 0 || NLCD_folder().compare("") == 0) {
      vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_pair<vcl_string, int>, vcl_string> > > query_img_info;
    volm_io::read_gt_file(category_gt_file(), query_img_info);

    int img_info_id = -1;
    for (unsigned kk = 0; kk < query_img_info.size(); kk++) {
      if (query_img_info[kk].second.first.second == img_id()) {
        img_info_id = kk;
        break;
      }
    }
    //if (query_img_info.size() <= img_id()) {
    if (img_info_id < 0) {
      vcl_cerr << "query image id: " << img_id() << " cannot be found in the gt loc file: " << category_gt_file() << "!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    vcl_cout << "will use the gt loc of the image " << img_id() << " which is: " << query_img_info[img_info_id].first.x() << " " << query_img_info[img_info_id].first.y() << " " << query_img_info[img_info_id].first.z() << "\n";
  
    
    // create the query descriptor    
    volm_desc_matcher_sptr m = new volm_desc_land_matcher(NLCD_folder(), query_img_info[img_info_id].first);
    volm_desc_sptr query = m->create_query_desc();  // land matcher overwrites this method and uses the gt location to fetch the land type from NLCD images
    query->print();

    double thres = 0.5;
    

    if (tile_id() == 10) { // we know this one is empty : TODO: remove hard code
      m->create_empty_prob_map(out_folder(), tile_id(), tiles[tile_id()]);
      m->create_scaled_prob_map(out_folder(), tiles[tile_id()], tile_id(), 10, 200, thres); 
      volm_io::write_status(out_folder(), volm_io::SUCCESS);
      vcl_cout << "returning SUCCESS!\n";
      return volm_io::SUCCESS;
    }

    m->matcher(query, geo_hypo_folder(), desc_index_folder(), 1.0, tile_id());
    m->write_out(out_folder(), tile_id());
    
    float gt_score;
    m->create_prob_map(geo_hypo_folder(), out_folder(), tile_id(), tiles[tile_id()], query_img_info[img_info_id].first, gt_score);
    m->create_scaled_prob_map(out_folder(), tiles[tile_id()], tile_id(), 10, 200, thres); 

    volm_io::write_status(out_folder(), volm_io::SUCCESS);
    vcl_cout << "returning SUCCESS!\n";
    return volm_io::SUCCESS;

  } else {  // run the indexer 
    volm_desc_indexer_sptr indexer;

    if (index_2d()) {
      if (radii_string().compare("") == 0 || maps_folder().compare("") == 0 || out_folder().compare("") == 0) {
        vcl_cerr << "EXE_ARGUMENT_ERROR for -index_2d, need a radii_string, maps_folder and out_folder!\n";
        vul_arg_display_usage_and_exit();
        return volm_io::EXE_ARGUMENT_ERROR;
      }

      vcl_vector<double> radii;
      
      char buf[1000];
      for (unsigned kk = 0; kk < radii_string().size(); kk++)
        buf[kk] = radii_string()[kk];

      char *tok = vcl_strtok(buf, "_");;
      while (tok != 0) {
        vcl_stringstream tokstr(tok);
        double radius;
        tokstr >> radius;
        radii.push_back(radius);
        tok = vcl_strtok(NULL, "_"); // tokenize the remaining string
      }
      vcl_cout << " will index 2d maps using radii: ";
      for (unsigned i = 0; i < radii.size(); i++) {
        vcl_cout << radii[i] << " ";
      }
      vcl_cout << '\n';

      indexer = new volm_desc_ex_2d_indexer(maps_folder(), out_folder(), radii);  
    } else {
      indexer = new volm_desc_land_indexer(NLCD_folder(), out_folder());
    }

    if (!indexer->load_tile_hypos(geo_hypo_folder(), tile_id()))
      return volm_io::EXE_ARGUMENT_ERROR;
    indexer->index(1.0f, min_leaf_id(), max_leaf_id()); // buffer capacity is 1 gigabyte // saves the leaves of the tile at the end
  
    return volm_io::SUCCESS;
  }

  
}
