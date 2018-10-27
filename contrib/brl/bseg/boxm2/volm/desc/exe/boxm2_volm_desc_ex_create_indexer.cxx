//
// \file
// \brief executable to create index existence descriptor of locations
// \author Yi Dong
// \date June 05, 2013

#include <volm/desc/volm_desc_ex.h>
#include <boxm2/volm/desc/boxm2_volm_desc_ex_indexer.h>
#include <boxm2/volm/desc/boxm2_volm_desc_ex_land_only_indexer.h>
#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>


int main(int argc, char** argv)
{
  vul_arg<std::string> geo_hypo_folder("-geo", "directory where the hypothesized geolocations are stored", "");
  vul_arg<std::string> index_folder("-index", "directory that contains the created wr3db indices", "");
  vul_arg<std::string> out_index_folder("-out", "directory where create existence indices will be stored", "");
  vul_arg<unsigned> tile_id("-tile", "tile id for which the existence indices will be created", 100);
  vul_arg<std::vector<double> > radius("-radius", "vector of distance interval");
  vul_arg<unsigned> norients("-nori", "number of orientation type considered, default are horizontal, vertical and infinite", 0);
  vul_arg<unsigned> nlands("-nlands", "number of land types considered, default are 35 land types", 0);
  vul_arg<float> buffer_capacity("-buffer", "buffer size used for indices, ensure the system has 2*buffer_capacity memory, unit as GB (default 2.0G)", 2.0f);
  vul_arg<bool> is_land_only("-land", "option to choose land only existence descriptor", false);
  vul_arg<bool> is_log("-log", "option to save log files", false);
  vul_arg<int> max_leaf_id ("-max", "maximum leaf id considered", 1000);
  vul_arg<int> min_leaf_id ("-min", "minimum leaf id considered", -1);
  vul_arg_parse(argc, argv);
  std::cout << " number or arguments = " << argc << std::endl;

  // check argument list
  std::stringstream log;
  std::stringstream log_file;
  log_file << out_index_folder() << "/ex_creation_log_" << tile_id() << ".xml";
  if (out_index_folder().compare("") == 0 || geo_hypo_folder().compare("") == 0 || index_folder().compare("") == 0 ||
      tile_id() == 100) {
    log << " ERROR: input or output folders can not be empty / tile_id or radius is not defined\n";
    std::cerr << log.str();
    vul_arg_display_usage_and_exit();
    if (is_log()) volm_io::write_post_processing_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  if ( (norients() == 0 && nlands() != 0) || (norients() != 0 && nlands() == 0) ){
    log << " ERROR: please define both norients and nlands\n";
    std::cerr << log.str();
    if (is_log()) volm_io::write_post_processing_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  if (radius().size() == 0) {
    std::cout << " NOTE: No distance type considered, the descriptor only consider land type and orientation" << std::endl;
  }
  if (norients() != 0 || nlands() != 0) {
    std::cout << " WARNING: specified number of orientation types and specified number of land type considered" << std::endl;
  }

  // obtain parameters of the wr3db
  std::stringstream params_file;
  params_file << index_folder() << "geo_index_tile_" << tile_id() << "_index.params";
  boxm2_volm_wr3db_index_params params;
  if (!params.read_params_file(params_file.str())) {
      log << " ERROR: parsing parameter file " << params_file.str() << " failed\n";
      std::cerr << log.str();
      if (is_log()) volm_io::write_post_processing_log(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
  }
  unsigned index_layer_size = params.layer_size;

  // create the depth_interval
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);
  std::map<double, unsigned char>& depth_interval_map = sph->get_depth_interval_map();
  auto iter = depth_interval_map.begin();
  std::vector<double> depth_interval;
  for (; iter != depth_interval_map.end(); ++iter)
    depth_interval.push_back((double)iter->first);


  // create the existence indices for given tile
  std::cout << " ============================= Start to create existence indices for tile " << tile_id() << " ===================== " << std::endl;
  volm_desc_indexer_sptr ex_indexer;
  if (is_land_only()) {
    if (norients() == 0 && nlands() == 0)
      ex_indexer = new volm_desc_ex_land_only_indexer(index_folder(), out_index_folder(), radius(), depth_interval, index_layer_size, buffer_capacity());
    else
      ex_indexer = new volm_desc_ex_land_only_indexer(index_folder(), out_index_folder(), radius(), depth_interval, index_layer_size, buffer_capacity(),
                                                      nlands(), (unsigned char)0);
  }
  else {
    if (norients() == 0 && nlands() == 0)
      ex_indexer = new volm_desc_ex_indexer(index_folder(), out_index_folder(), radius(), depth_interval, index_layer_size, buffer_capacity());
    else
      ex_indexer = new volm_desc_ex_indexer(index_folder(), out_index_folder(), radius(), depth_interval, index_layer_size, buffer_capacity(),
                                            norients(), nlands(), (unsigned char)0);
  }

  std::cout << " \t Descriptor used: " << ex_indexer->get_index_type_str() << std::endl;
  std::cout << " \t Following parameters are used" << std::endl;
  std::cout << " \t\t radius = ";
  for (double & vit : radius())
    std::cout << vit << ' ';
  std::cout << '\n';
  if (norients() == 0)
    if (is_land_only())
      std::cout << " \t\t Orientation is not considered, only use land type and distance " << std::endl;
    else
      std::cout << " \t\t number of orientation types: 3" << std::endl;
  else
    std::cout << " \t\t number of orientation types: " << norients() << std::endl;
  if (nlands() == 0)
    std::cout << " \t\t number of land types: " << volm_label_table::compute_number_of_labels() << std::endl;
  else
    std::cout << " \t\t number of land types: " << nlands() << std::endl;
  std::cout << " \t\t size of the descriptor: " << ex_indexer->layer_size();

  // load the hypothesized geolocation
  std::cout << " \t Load the hypothesized geolocation for tile " << tile_id() << "..." << std::endl;
  if (!ex_indexer->load_tile_hypos(geo_hypo_folder(), tile_id())) {
    log << " ERROR: load hypothesized geolocation for tile " << tile_id() << " failed.\n";
    std::cerr << log.str();
    if (is_log()) volm_io::write_post_processing_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  std::cout << " \t Create existence indices for tile " << tile_id() << "..." << std::endl;
  if (!ex_indexer->index(buffer_capacity(), min_leaf_id(), max_leaf_id())) {
    log << " ERROR: creating existence indices for tile " << tile_id() << " failed.\n";
    std::cerr << log.str();
    if (is_log()) volm_io::write_post_processing_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::cout << " \t\n Finished!  All indices are stored in " << out_index_folder() << std::endl;
  std::cout << " ============================= Finished to create existence indices for tile " << tile_id() << " ===================== " << std::endl;
  return volm_io::SUCCESS;
}
