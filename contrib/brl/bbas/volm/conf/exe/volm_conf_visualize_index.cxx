// This is contrib/brl/bbas/volm/conf/exe/volm_conf_visualize_index.cxx
//:
// \file
// \brief  exectuable to query and visualize created configurational index
//
// \author Yi Dong
// \date September 03, 2014
// \verbatim
//   Modifications
//    <none yet>
// \endverbatim
//
#include <iostream>
#include <iomanip>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_geo_index.h>
#include <volm/volm_loc_hyp.h>
#include <volm/conf/volm_conf_object.h>
#include <volm/conf/volm_conf_buffer.h>

int main(int argc, char** argv)
{
  vul_arg<std::string> geo_hypo_folder("-geo", "geo hypo location folder", "");
  vul_arg<std::string>    index_folder("-idx", "configuraional index folder", "");
  vul_arg<unsigned>           tile_id("-tile","tile id where the query location resides", 9999);
  vul_arg<double>             loc_lon("-lon", "location longitude", 0.00);
  vul_arg<double>             loc_lat("-lat", "location latitude",  0.00);
  vul_arg<float>      buffer_capacity("-buffer", "buffer capacity for index creation (in GByte)", 2.0f);
  vul_arg_parse(argc, argv);

  // input check
  if (geo_hypo_folder().compare("") == 0 || index_folder().compare("") == 0 || tile_id() == 9999)
  {
    vul_arg_display_usage_and_exit();
    return 1;
  }
  // load the geo location index
  std::stringstream loc_file_pre;
  loc_file_pre << geo_hypo_folder() << "/geo_index_tile_" << tile_id();
  if (!vul_file::exists(loc_file_pre.str()+".txt")) {
    std::cerr << "ERROR: can not find geo location index for tile " << tile_id() << " -- " << loc_file_pre.str() << ".txt\n";
    return 1;
  }
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(loc_file_pre.str()+".txt", min_size);
  volm_geo_index::read_hyps(root, loc_file_pre.str());

  unsigned num_locs = volm_geo_index::hypo_size(root);
  if (!num_locs) {
    std::cerr << "ERROR: there is no geo locations in this tile!.. returning!\n";
    return 1;
  }
  std::cout << num_locs << " geo locations are in this tile" << std::endl;

  // locate the desire locations
  unsigned hyp_id;
  volm_geo_index_node_sptr leaf = volm_geo_index::get_closest(root, loc_lat(), loc_lon(), hyp_id);
  if (!leaf) {
    std::cerr << "ERROR: the geo index: " << geo_hypo_folder() << " does not contain any hyp close to " << loc_lon() << ", " << loc_lat() << '\n';
    return 1;
  }
  std::cout << "location " << loc_lon() << ", " << loc_lat() << " is in leaf " << leaf->extent_
           << " and closest location is: " << leaf->hyps_->locs_[hyp_id].x() << ", " << leaf->hyps_->locs_[hyp_id].y() << std::endl;

  // load associated index
  std::stringstream index_file_pre;
  index_file_pre << index_folder() << "/conf_index_tile_" << tile_id();
  volm_conf_buffer<volm_conf_object> ind(buffer_capacity());
  std::string bin_file_name;
  bin_file_name = index_file_pre.str() + "_" + leaf->get_string() + "_conf_2d.bin";
  //if (!vul_file::exists(bin_file_name)) {
  //  std::cerr << "ERROR: can not find index binary file: " << bin_file_name << '\n';
  //  return 1;
  //}
  if (!ind.initialize_read(bin_file_name)) {
    std::cerr << "ERROR: initialize reading binary file failed -- " << bin_file_name << '\n';
    return 1;
  }
  // obtain the index
  std::cout << leaf->hyps_->locs_.size() << " location in current leaf" << std::endl;
  std::vector<volm_conf_object> values;
  for (unsigned i = 0; i < hyp_id; ++i) {
    ind.get_next(values);
  }
  ind.get_next(values);

  // write out the kml file
  std::stringstream kml_file;
  kml_file << vul_file::strip_extension(bin_file_name) << '_' << std::setprecision(6) << std::fixed << leaf->hyps_->locs_[hyp_id].x()
                                                       << '_' << std::setprecision(6) << std::fixed << leaf->hyps_->locs_[hyp_id].y() << ".kml";
  volm_conf_object::write_to_kml(leaf->hyps_->locs_[hyp_id].x(), leaf->hyps_->locs_[hyp_id].y(), values, kml_file.str());

  std::cout << "FINISH: the index can be visualized in: " << kml_file.str() << std::endl;
  return 0;
}
