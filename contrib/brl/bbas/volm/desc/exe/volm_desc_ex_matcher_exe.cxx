//:
// \file
// \brief executable to match queries based on the existance of distinguishable objects
// \author Yi Dong
// \date June 04, 2013

#include <volm/desc/volm_desc_ex_matcher.h>
#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_buffered_index.h>
#include <vul/vul_arg.h>

int main(int argc, char** argv)
{
  // query input
  vul_arg<vcl_string> depth_scene("-dms", "depth map scene file", "");
  // index and geolocation
  vul_arg<vcl_string> geo_hypo_a("-geoa", "folder where geo hypotheses for utm zone 17 are","");
  vul_arg<vcl_string> geo_hypo_b("-geob", "folder where geo hypotheses for utm zone 18 are","");
  vul_arg<vcl_string> desc_index_folder("-index", "directory that contains the created wr3db indices", "");
  vul_arg<vcl_string> out_folder("-out", "output folder for the query image", "");
  vul_arg<float> buffer_capacity("-buff", "buffer size used for loading indices", 2.0f);
  // post processing related
  vul_arg<vcl_string> gt_file("-gt", "ground truth files", "");
  vul_arg<float> kl("-kl", "parameter for nonlinear score scaling", 200.0f);
  vul_arg<float> ku("-ku", "parameter for nonlinear score scaling", 10.0f);
  vul_arg<double> thres("-thres", "threshold ratio for generating prob map", 0.5);
  vul_arg<unsigned> test_id("-testid", "phase 1 test id", 0);
  vul_arg<unsigned> id("-imgid", "query image id", 101);
  vul_arg<unsigned> thresc("-thres", "threshold that used to create candidate list", 0);
  vul_arg<unsigned> top_size("-top", "desired top list for each candidate list", 1);
  // error log
  vul_arg<bool> logger("-logger", "option to write status.xml", false);
  vul_arg_parse(argc, argv);

  // check the argument 
  if (depth_scene().compare("") == 0 ||
      geo_hypo_a().compare("") == 0 || geo_hypo_b().compare("") == 0 ||
      desc_index_folder().compare("") == 0 || out_folder().compare("") == 0 ||
      gt_file().compare("") == 0 || id() > 100 || test_id() == 0)
  {
    vcl_cerr << " ERROR: arguemnts error, check the input " << vcl_endl;
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // load the depth map scene
  depth_map_scene_sptr dms = new depth_map_scene;
  vsl_b_ifstream dis(depth_scene().c_str());
  dms->b_read(dis);
  dis.close();

  // Start the matcher
  vcl_vector<volm_tile> tiles = volm_tile::generate_p1_wr2_tiles();
  for (unsigned tile_id = 0; tile_id < tiles.size(); tile_id++) {
    if (tile_id == 10)
      continue;
    vcl_string geo_hypo_folder;
    if (tile_id < 8 && tile_id != 5)
      geo_hypo_folder = geo_hypo_a();
    else
      geo_hypo_folder = geo_hypo_b();
    vcl_stringstream params_file;
    params_file << desc_index_folder() << "/desc_index_tile_" << tile_id << ".params";
    volm_buffered_index_params params;
    params.read_ex_param_file(params_file.str());

    volm_desc_matcher_sptr ex_matcher = new volm_desc_ex_matcher(dms, params.radius, params.norients, params.nlands, 0);

    // create query
    volm_desc_sptr query = new volm_desc_ex();
    query = ex_matcher->create_query_desc();

    // ... to be continue.. 

  }
  
  // generate float probability map

  // generate scaled probability map

  // generate candidate lists

  return volm_io::SUCCESS;
}
