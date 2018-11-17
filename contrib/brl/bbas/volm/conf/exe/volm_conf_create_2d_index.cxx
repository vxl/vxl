// This is contrib/brl/bbas/volm/conf/exe/volm_conf_create_2d_index.cxx
//:
// \file
// \brief  executable to create configurational 2d index database
//
// \author Yi Dong
// \date September 01, 2014
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <volm/volm_io.h>
#include <volm/conf/volm_conf_2d_indexer.h>
#include <volm/conf/volm_conf_indexer.h>

#include <utility>

static void error(std::string log_file, const std::string& msg)
{  std::cerr << msg;  volm_io::write_post_processing_log(std::move(log_file), msg);  }

int main(int argc, char** argv)
{
  vul_arg<unsigned> tile_id("-tile", "ROI tile id", 9999);
  vul_arg<int> max_leaf_id("-max", "maximum leaf id considered", 1000);
  vul_arg<int> min_leaf_id("-min", "minimum leaf id considered", 0);
  vul_arg<double> radius("-radius", "index radius (in meter)", 3000);
  vul_arg<float> buffer_capacity("-buffer", "buffer capacity for index creation (in GByte)", 2.0f);
  vul_arg<std::string> geo_folder("-hypo", "folder to read the geo hypotheses","");
  vul_arg<std::string> land_index_folder("-land", "folder to read land map index", "");
  vul_arg<std::string> out_folder("-out", "folder to put created index","");
  vul_arg_parse(argc, argv);
  std::cout << "argc: " << argc << std::endl;
  // input check
  if (tile_id() == 9999 || geo_folder().compare("") == 0 || land_index_folder().compare("") == 0 || out_folder().compare("") == 0)
  {  vul_arg_display_usage_and_exit();  return volm_io::EXE_ARGUMENT_ERROR;  }

  std::stringstream log_file;
  log_file << out_folder() << "/log_tile_" << tile_id() << "_leaf_" << min_leaf_id() << "_to_" << max_leaf_id() << ".xml";
  std::stringstream log;

  // create index
  std::cout << "-------------- Start to create 2d configurational index for tile " << tile_id() << std::endl;
  volm_conf_indexer_sptr indexer = new volm_conf_2d_indexer(radius(), out_folder(), land_index_folder(), tile_id());
  // load the location hypotheses
  if(!indexer->load_loc_hypos(geo_folder(), tile_id()))
  {
    log <<"ERROR: load location hypotheses failed from folder: " << geo_folder() << "!\n";
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::cout << "\t " << indexer->loc_leaves().size() << " location leaves are loaded from geo location folder: " << geo_folder() << std::endl;
  std::cout << "\t create index from leaf " << min_leaf_id() << " to " << max_leaf_id() << std::endl;

  unsigned num_locs = 0;
  for (auto & i : indexer->loc_leaves())
    num_locs += i->hyps_->locs_.size();

  vul_timer t;
  if (!indexer->index(buffer_capacity(), min_leaf_id(), max_leaf_id()))
  {
    log << "ERROR: creating index failed for tile " << tile_id() << "!\n";
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  log << "indexing " << num_locs << " locations consumes " << t.all() / (1000.0*60.0) << " seconds (" << t.all()/(1000.0*60.0*60.0) << " minutes)" << std::endl;
  volm_io::write_post_processing_log(log_file.str(), log.str());

  return volm_io::SUCCESS;
}
