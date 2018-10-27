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
  vul_arg<std::string> category_file("-cat", "category file that contains one line for the land type of the camera for the query ", "");
  vul_arg<std::string> category_gt_file("-cat_gt", "category gt file of multiple images, use image id to get gt loc of this one ", "");
  vul_arg<int> img_id("-id", "query image id in the category gt file, starts from 0", 1000);
  vul_arg<std::string> world_str("-world", "world name, can be desert, coast, Chile, India, Jordan, Philippine, Taiwan", "");
  vul_arg<std::string> out_folder("-out", "output folder to save index or if matching results", "");
  vul_arg<std::string> desc_index_folder("-desc", "folder to read the descriptor index of the tile", "");
  vul_arg<std::string> land_name("-land", "land id for test query image", "");
  vul_arg<bool> save_images("-save", "save out images or not", false);

  // PARAMS for indexing
  vul_arg<std::string> lidar_folder("-lidar", "lidar folder to use for indexing", "");
  vul_arg<std::string> NLCD_folder("-nlcd", "NLCD folder to use for indexing", "");
  vul_arg<std::string> geo_hypo_folder("-hypo", "folder to read the geo hypotheses", "");                      // index -- folder to read the hypos for each leaf
  vul_arg<int> tile_id("-tile", "id of the tile to be indexed", -1);
  vul_arg<bool> index_2d("-ex_2d", "if exists index ex_2d using the classification maps that are passed", false);
  vul_arg<std::string> maps_folder("-maps", "the classification map images where each pixel has the land id (type is unsigned char)", "");
  vul_arg<std::string> radii_string("-rad", "e.g. pass 100_500_1000 for radii of 100, 500 and 1000 meter to construct descriptors at each location", "");
  vul_arg<int> max_leaf_id ("-max", "maximum leaf id considered", 1000);
  vul_arg<int> min_leaf_id ("-min", "minimum leaf id considered", 0);

  vul_arg_parse(argc, argv);
  std::cout << "argc: " << argc << std::endl;

  if (match()) {  // run the matcher

    std::vector<volm_tile> tiles;

    if (world_str().compare("Chile")==0)             tiles = volm_tile::generate_p1b_wr1_tiles();
    else if (world_str().compare("India")==0)        tiles = volm_tile::generate_p1b_wr2_tiles();
    else if (world_str().compare("Jordan")==0)       tiles = volm_tile::generate_p1b_wr3_tiles();
    else if (world_str().compare("Philippines")==0)  tiles = volm_tile::generate_p1b_wr4_tiles();
    else if (world_str().compare("Taiwan")== 0)      tiles = volm_tile::generate_p1b_wr5_tiles();
    else if (world_str().compare("Coast")== 0)       tiles = volm_tile::generate_p1_wr2_tiles();
    else if (world_str().compare("Desert")== 0)      tiles = volm_tile::generate_p1_wr1_tiles();
    else {
      std::cout << "ERROR: unknown ROI region: " << world_str() << ".  Available regions are: Coast, Desert, Chile, India, Jordan, Philippines, Taiwan\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    if (tile_id() >= tiles.size()) {
      std::cout << "ERROR: unknown tile id " << tile_id() << " for ROI region: " << world_str() << "!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    if (random()) {
        if (geo_hypo_folder().compare("") == 0 || out_folder().compare("") == 0 || tile_id() < 0 || img_id() < 0) {
          std::cerr << "EXE_ARGUMENT_ERROR!\n";
          volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
          vul_arg_display_usage_and_exit();
          return volm_io::EXE_ARGUMENT_ERROR;
        }

        volm_desc_matcher_sptr m = new volm_desc_land_matcher(); // need a deriving class just to call a base method
        vnl_random rng;
        m->create_random_prob_map(rng, geo_hypo_folder(), out_folder(), tile_id(), tiles[tile_id()]);
        float thres = 0.5f;
        m->create_scaled_prob_map(out_folder(), tiles[tile_id()], tile_id(), 10.0f, 127.0f, thres);

        volm_io::write_status(out_folder(), volm_io::SUCCESS);
        std::cout << "returning SUCCESS!\n";
        return volm_io::SUCCESS;
    }

    if (category_gt_file().compare("") == 0 || out_folder().compare("") == 0 || geo_hypo_folder().compare("") == 0 || tile_id() < 0 || img_id() < 0 || desc_index_folder().compare("") == 0 || NLCD_folder().compare("") == 0) {
      std::cerr << "EXE_ARGUMENT_ERROR!\n";
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    std::vector<std::pair<vgl_point_3d<double>, std::pair<std::pair<std::string, int>, std::string> > > query_img_info;
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
      std::cerr << "query image id: " << img_id() << " cannot be found in the gt loc file: " << category_gt_file() << "!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    volm_desc_matcher_sptr m = new volm_desc_land_matcher(NLCD_folder(), query_img_info[img_info_id].first);
    // create the query descriptor
    volm_desc_sptr query;
    if (land_name().compare("") != 0 && volm_osm_category_io::volm_land_table_name.find(land_name()) == volm_osm_category_io::volm_land_table_name.end()) {
      // create query descriptor based on NLCD
      query = m->create_query_desc();
      std::cout << "will use the gt loc of the image " << img_id()
               << " which is: " << query_img_info[img_info_id].first.x()
               << " " << query_img_info[img_info_id].first.y() << " " << query_img_info[img_info_id].first.z() << "\n";
    }
    else {
      // create the query descriptor based on given land category
      unsigned char land_id = volm_osm_category_io::volm_land_table_name[land_name()].id_;
      query = new volm_desc_land((int)land_id, "volm");
    }
    std::cout <<"create query:\n";
    query->print();
    double thres = 0.5;

    m->matcher(query, geo_hypo_folder(), desc_index_folder(), 1.0, tile_id());
    m->write_out(out_folder(), tile_id());

    float gt_score;
    m->create_prob_map(geo_hypo_folder(), out_folder(), tile_id(), tiles[tile_id()], query_img_info[img_info_id].first, gt_score);
    m->create_scaled_prob_map(out_folder(), tiles[tile_id()], tile_id(), 10, 200, thres);

    volm_io::write_status(out_folder(), volm_io::SUCCESS);
    std::cout << "returning SUCCESS!\n";
    return volm_io::SUCCESS;

  } else {  // run the indexer
    volm_desc_indexer_sptr indexer;

    if (index_2d()) {
      if (radii_string().compare("") == 0 || maps_folder().compare("") == 0 || out_folder().compare("") == 0) {
        std::cerr << "EXE_ARGUMENT_ERROR for -index_2d, need a radii_string, maps_folder and out_folder!\n";
        vul_arg_display_usage_and_exit();
        return volm_io::EXE_ARGUMENT_ERROR;
      }

      std::vector<double> radii;

      char buf[1000];
      for (unsigned kk = 0; kk < radii_string().size(); kk++)
        buf[kk] = radii_string()[kk];

      char *tok = std::strtok(buf, "_");;
      while (tok != nullptr) {
        std::stringstream tokstr(tok);
        double radius;
        tokstr >> radius;
        radii.push_back(radius);
        tok = std::strtok(nullptr, "_"); // tokenize the remaining string
      }
      std::cout << " will index 2d maps using radii: ";
      for (double i : radii) {
        std::cout << i << " ";
      }
      std::cout << '\n';

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
