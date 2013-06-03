//:
// \file
// \brief executable to index land type of locations and match queries based on land type of camera location
// \author Ozge C. Ozcanli
// \date Sept 30, 2013

#include <volm/desc/volm_desc_land.h>
#include <volm/desc/volm_desc_land_indexer.h>
#include <volm/volm_tile.h>
#include <volm/volm_io.h>
#include <vul/vul_arg.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>


int main(int argc,  char** argv)
{
  vul_arg<bool> match("-match", "if exists run the matcher, otherwise run the indexer");
  vul_arg<vcl_string> category_file("-cat", "category file that contains one line for the land type of the camera for the query ", "");
  vul_arg<vcl_string> out_folder("-out", "output folder to save index or if matching results", "");
  vul_arg<bool> save_images("-save", "save out images or not", false);
  
  // PARAMS for indexing
  vul_arg<vcl_string> lidar_folder("-lidar", "lidar folder to use for indexing", "");
  vul_arg<vcl_string> NLCD_folder("-nlcd", "NLCD folder to use for indexing", "");
  vul_arg<vcl_string> geo_hypo_folder("-hypo", "folder to read the geo hypotheses", "");                      // index -- folder to read the hypos for each leaf
  vul_arg<int> tile_id("-tile", "id of the tile to be indexed", 0);
  
  
  vul_arg_parse(argc, argv);
  vcl_cout << "argc: " << argc << vcl_endl;

  if (match()) {  // run the matcher
    if (category_file().compare("") == 0 || out_folder().compare("") == 0) {
      vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
      volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    // create the query descriptor
    volm_desc_sptr query = new volm_desc_land(category_file());
    query->print();
  
  
    // just generate empty output -- we know that we only run on coast queries
    vcl_vector<volm_tile> tiles = volm_tile::generate_p1_wr2_tiles();
    for (unsigned i = 0; i < tiles.size(); ++i) {
      vil_image_view<unsigned int> out(3601, 3601);
      out.fill(volm_io::UNKNOWN);

      vcl_string out_name = out_folder() + "/ProbMap_" + tiles[i].get_string() + "_S1x1.tif";
      if (save_images()) {
        vcl_cout << "will write image to: " << out_name << vcl_endl;
        vil_save(out, out_name.c_str());
      }
      else {
        vcl_cout << "would write image to: " << out_name << vcl_endl;
      }
    }

    volm_io::write_status(out_folder(), volm_io::SUCCESS);
    vcl_cout << "returning SUCCESS!\n";
    return volm_io::SUCCESS;

  } else {  // run the indexer 

    volm_desc_indexer_sptr indexer = new volm_desc_land_indexer(NLCD_folder(), out_folder());
    if (!indexer->load_tile_hypos(geo_hypo_folder(), tile_id()))
      return volm_io::EXE_ARGUMENT_ERROR;

    indexer->index(1.0f); // buffer capacity is 1 gigabyte // saves the leaves of the tile at the end
  
    return volm_io::SUCCESS;
  }

  
}
