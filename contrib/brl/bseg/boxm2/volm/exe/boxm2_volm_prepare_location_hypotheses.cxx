//:
// \file
// \brief an executable to prepare location hypotheses for volumetric matcher
// \author Ozge C. Ozcanli
// \date Oct 11, 2012

#include <vul/vul_arg.h>

#include <boxm2/volm/boxm2_volm_locations.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <volm/volm_io.h>
#include <boxm2/boxm2_scene.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>


int main(int argc,  char** argv)
{
  vul_arg<std::string> scene_file("-scene", "scene xml filename", "");
  vul_arg<std::string> dem_file("-dem", "dem name, assume there is a tfw file in the same folder ", "");
  vul_arg<std::string> out_file("-out", "prefix for names of output binary files", "");
  vul_arg<unsigned> int_i("-int_i", "interval in +x direction (East) in pixels in the output tiles", (unsigned)10);
  vul_arg<unsigned> int_j("-int_j", "interval in +y direction (North) in pixels in the output tiles", (unsigned)10);
  vul_arg<float> alt("-altitude", "altitude (in meters) from above ground to generate the elevation of each hyptohesis, default is 1.6", 1.6f);
  vul_arg_parse(argc, argv);

  std::cout << "argc: " << argc << std::endl;
  if (scene_file().compare("") == 0 || dem_file().compare("") == 0 || out_file().compare("") == 0) {
    std::cerr << "EXE_ARGUMENT_ERROR!\n";
    vul_arg_display_usage_and_exit();
    return 0;
  }
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());

  std::vector<vil_image_view<float> > dems;
  std::vector<vpgl_geo_camera* > cams;
  vil_image_resource_sptr dem_res = vil_load_image_resource(dem_file().c_str());
  vil_image_view<float> dem = dem_res->get_view();
  vpgl_geo_camera* geocam = nullptr;
  vpgl_geo_camera::init_geo_camera(dem_res, lvcs, geocam);

  dems.push_back(dem);
  cams.push_back(geocam);

  std::cout << "generating hypotheses for scene: " << scene_file() << " using " << dem_file() << std::endl;

  //: just generate dummy output
  //std::vector<volm_tile> tiles = volm_tile::generate_p1_tiles();
  std::vector<volm_tile> tiles = volm_tile::generate_p1_wr1_tiles();

  for (auto & tile : tiles) {
    std::string out_name = out_file() + "_volm_" + tile.get_string() + ".kml";
    tile.write_kml(out_name, 1000);
  }

  for (auto & tile : tiles) {
    //boxm2_volm_loc_hypotheses_sptr h = new boxm2_volm_loc_hypotheses(lvcs, tiles[i], int_i(), int_j(), alt(), dems, cams);
    boxm2_volm_loc_hypotheses_sptr h = new boxm2_volm_loc_hypotheses(tile);
    h->add_dems(scene, int_i(), int_j(), alt(), dems, cams);
    std::cout << "constructed: " << h->locs_.size() << " hypotheses for tile: " << tile.get_string() << std::endl;
    std::string out_name = out_file() + "_" + tile.get_string() + ".bin";
    h->write_hypotheses(out_name);

#if 0
    vil_image_view<unsigned int> out(3601, 3601);
    out.fill(volm_io::UNEVALUATED);
    std::vector<float> dummy_scores(h.locs_.size(), 1.0f);
    h.generate_output_tile(dummy_scores, 5*int_i()/12, 5*int_j()/12, 0.5f, out);
    std::string out_name = out_file() + "_volm_" + tiles[i].get_string() + ".tif";
    vil_save(out, out_name.c_str());
#endif
  }
#if 0
  for (unsigned i = 0; i < tiles.size(); i++) {
    std::string out_name = out_file() + "_" + tiles[i].get_string() + ".bin";
    boxm2_volm_loc_hypotheses_sptr h = new boxm2_volm_loc_hypotheses(out_name);
    std::cout << "read: " << h->locs_.size() << " hypotheses for tile: " << tiles[i].get_string() << std::endl;
    std::string out_name_kml = out_file() + "_" + tiles[i].get_string() + ".kml";
    h->write_hypotheses_kml(scene, out_name_kml);
  }
#endif

  std::cout << "returning SUCCESS!\n";
  return volm_io::SUCCESS;
}
