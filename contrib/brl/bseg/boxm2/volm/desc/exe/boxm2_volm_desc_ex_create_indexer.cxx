//
// \file
// \brief executable to create index existance descriptor of locations
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
  vul_arg<vcl_string> geo_hypo_folder("-geo", "directory where the hypothesized geolocations are stored", "");
  vul_arg<vcl_string> index_folder("-index", "directory that contains the created wr3db indices", "");
  vul_arg<vcl_string> out_index_folder("-out", "directory where create existance indices will be stored", "");
  vul_arg<unsigned> tile_id("-tile", "tile id for which the existance indices will be created", 100);
  vul_arg<vcl_vector<double> > radius("-radius", "vector of distance interval");
  vul_arg<unsigned> norients("-nori", "number of orientation type considered, default are horizontal, vertial and infinite", 0);
  vul_arg<unsigned> nlands("-nlands", "number of land types considered, default are 35 land types", 0);
  vul_arg<float> buffer_capacity("-buffer", "buffer size used for indices, ensure the system has 2*buffer_capacity memory, unit as GB (default 2.0G)", 2.0f);
  vul_arg<bool> is_land_only("-land", "option to choose land only existance descriptor", false);
  vul_arg<bool> is_log("-log", "option to save log files", false);
  vul_arg<int> max_leaf_id ("-max", "maximum leaf id considered", 1000);
  vul_arg<int> min_leaf_id ("-min", "minimum leaf id considered", -1);
  vul_arg_parse(argc, argv);
  vcl_cout << " number or arguments = " << argc << vcl_endl;

  // check argument list
  vcl_stringstream log;
  vcl_stringstream log_file;
  log_file << out_index_folder() << "/ex_creation_log_" << tile_id() << ".xml";
  if (out_index_folder().compare("") == 0 || geo_hypo_folder().compare("") == 0 || index_folder().compare("") == 0 ||
      tile_id() == 100) {
    log << " ERROR: input or output folders can not be empty / tile_id or radius is not defined\n";
    vcl_cerr << log.str();
    vul_arg_display_usage_and_exit();
    if (is_log()) volm_io::write_post_processing_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  if ( (norients() == 0 && nlands() != 0) || (norients() != 0 && nlands() == 0) ){
    log << " ERROR: please define both norients and nlands\n";
    vcl_cerr << log.str();
    if (is_log()) volm_io::write_post_processing_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  if (radius().size() == 0) {
    vcl_cout << " NOTE: No distance type considered, the discriptor only consider land type and orientation" << vcl_endl;
  }
  if (norients() != 0 || nlands() != 0) {
    vcl_cout << " WARNING: specified number of orientation types and specified number of land type considered" << vcl_endl;
  }

  // obtain parameters of the wr3db 
  vcl_stringstream params_file;
  params_file << index_folder() << "geo_index_tile_" << tile_id() << "_index.params";
  boxm2_volm_wr3db_index_params params;
  if (!params.read_params_file(params_file.str())) {
      log << " ERROR: parsing parameter file " << params_file.str() << " failed\n";
      vcl_cerr << log.str();
      if (is_log()) volm_io::write_post_processing_log(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
  }
  unsigned index_layer_size = params.layer_size;

  // create the depth_interval
  volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle, params.vmin, params.dmax);
  vcl_map<double, unsigned char>& depth_interval_map = sph->get_depth_interval_map();
  vcl_map<double, unsigned char>::iterator iter = depth_interval_map.begin();
  vcl_vector<double> depth_interval;
  for (; iter != depth_interval_map.end(); ++iter)
    depth_interval.push_back((double)iter->first);


  // create the existance indices for given tile
  vcl_cout << " ============================= Start to create existance indices for tile " << tile_id() << " ===================== " << vcl_endl;
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
  
  vcl_cout << " \t Descriptor used: " << ex_indexer->get_index_type_str() << vcl_endl;
  vcl_cout << " \t Following parameters are used" << vcl_endl;
  vcl_cout << " \t\t radius = ";
  for (vcl_vector<double>::iterator vit = radius().begin(); vit != radius().end(); ++vit)
    vcl_cout << *vit << ' ';
  vcl_cout << '\n';
  if (norients() == 0)
    if (is_land_only())
      vcl_cout << " \t\t Orientation is not considered, only use land type and distance " << vcl_endl;
    else
      vcl_cout << " \t\t number of orientation types: 3" << vcl_endl;
  else
    vcl_cout << " \t\t number of orientation types: " << norients() << vcl_endl;
  if (nlands() == 0)
    vcl_cout << " \t\t number of land types: " << volm_label_table::compute_number_of_labels() << vcl_endl;
  else
    vcl_cout << " \t\t number of land types: " << nlands() << vcl_endl;
  vcl_cout << " \t\t size of the descriptor: " << ex_indexer->layer_size();

  // load the hypothesized geolocation
  vcl_cout << " \t Load the hypothesized geolocation for tile " << tile_id() << "..." << vcl_endl;
  if (!ex_indexer->load_tile_hypos(geo_hypo_folder(), tile_id())) {
    log << " ERROR: load hypothesized geolocation for tile " << tile_id() << " failed.\n";
    vcl_cerr << log.str();
    if (is_log()) volm_io::write_post_processing_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  vcl_cout << " \t Create existance indices for tile " << tile_id() << "..." << vcl_endl;
  if (!ex_indexer->index(buffer_capacity(), min_leaf_id(), max_leaf_id())) {
    log << " ERROR: creating existance indices for tile " << tile_id() << " failed.\n";
    vcl_cerr << log.str();
    if (is_log()) volm_io::write_post_processing_log(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << " \t\n Finsihed!  All indices are stored in " << out_index_folder() << vcl_endl;
  vcl_cout << " ============================= Finished to create existance indices for tile " << tile_id() << " ===================== " << vcl_endl;
  return volm_io::SUCCESS;
}