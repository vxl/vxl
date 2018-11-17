// This is contrib/brl/bseg/boxm2/volm/conf/exe/boxm2_volm_create_candidate_region.cxx
//:
// \file
// \brief executable to create candidate region kml file from configurational matcher output.  Each candidate region is a
//        circular region and contains a heading direction pointing to some representative query annotation
//
// \author Yi Dong
// \date September 12, 2014
// \verbatim
//   Modifications
//    <none yet>
// \endverbatim
//

#include <iostream>
#include <map>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vgl/io/vgl_io_polygon.h>
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>
#include <volm/volm_tile.h>
#include <volm/volm_io.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_candidate_list.h>
#include <volm/conf/volm_conf_buffer.h>
#include <volm/conf/volm_conf_score.h>
#include <volm/volm_utils.h>

//: check whether the given point is inside the polygon
static bool is_contained(std::vector<vgl_polygon<double> > const& poly_vec, vgl_point_2d<double> const& pt);

//: generate the landmarks (wgs84) from matcher output
static bool generate_landmarks(vgl_point_2d<double> const& pt, std::vector<volm_conf_object> const& land_objs, std::vector<vgl_point_2d<double> >& landmarks);

int main(int argc, char** argv)
{
  vul_arg<std::string>       out_kml("-out", "output kml file", "");
  vul_arg<unsigned>        world_id("-world", "ROI world id of the query", 9999);
  vul_arg<std::string>    geo_folder("-geo", "geo location database", "");
  vul_arg<std::string>    cand_folder("-cand", "candidate list used during matching for search space reduction, if existed","");
  vul_arg<std::string>    index_name("-idx-name", "name of the loaded index", "");
  vul_arg<std::string>  score_folder("-score", "folder to read matcher socre","");
  vul_arg<float>    buffer_capacity("-buffer",   "buffer capacity for loading score binary (in GByte)", 2.0f);
  vul_arg<double>            radius("-radius", "radius of pin-pointed circle (in meter)", 200.0);
  vul_arg<unsigned >   num_top_locs("-num-locs", "number of desired pinning points", 100);
  vul_arg_parse(argc, argv);
  std::cout << "argc: " << argc << std::endl;

  // input check
  if (world_id() == 9999 || geo_folder().compare("") == 0 || score_folder().compare("") == 0 || out_kml().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::string log_file;
  std::stringstream log;
  std::string kml_folder = vul_file::dirname(out_kml());
  log_file = kml_folder + "/log_cand_region.xml";
  // check score folder
  std::string score_bins = score_folder() + "/*" + index_name() + "*.bin";
  unsigned ns = 0;
  for (vul_file_iterator fn = score_bins.c_str(); fn; ++fn) {
    ns++;
  }
  if (ns == 0) {
    log << "ERROR: there is no score binaries in the input score folder: " << score_folder() << "!\n";
    volm_io::write_error_log(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // create volm tiles for given world_id
  std::vector<volm_tile> tiles;
  if (!volm_tile::generate_tiles(world_id(), tiles)) {
    log << "ERROR: unknown world id: " << world_id() << "!\n";
    volm_io::write_error_log(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // loop over each tile to load all scores and sort them
  std::cout << "------------------ Start to create candidate region ---------------------" << std::endl;
  std::cout << tiles.size() << " tiles are created for world: " << world_id() << std::endl;
  std::multimap<float, std::pair<volm_conf_score, vgl_point_2d<double> >, std::greater<float> > score_map;
  for (unsigned t_idx = 0; t_idx < tiles.size(); t_idx++)
  {
    volm_tile tile = tiles[t_idx];
    std::stringstream file_name_pre;
    file_name_pre << geo_folder() << "/geo_index_tile_" << t_idx;
    // no geo location for current tile, skip
    if (!vul_file::exists(file_name_pre.str()+ ".txt"))
      continue;
    // check file size
    if (vul_file::size(file_name_pre.str()+".txt") == 0)
      continue;
    // load the location database for current tile
    float min_size;
    volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
    volm_geo_index::read_hyps(root, file_name_pre.str());
    std::vector<volm_geo_index_node_sptr> loc_leaves;
    loc_leaves.clear();
    volm_geo_index::get_leaves_with_hyps(root, loc_leaves);
    std::cout << "  loading and sorting scores for tile " << t_idx << " from " << loc_leaves.size() << " leaves" << std::endl;
    std::stringstream score_file_pre;
    score_file_pre << score_folder() << "/conf_score_tile_" << t_idx;
    for (const auto& leaf : loc_leaves)
    {
      std::string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name() + ".bin";
      if (!vul_file::exists(score_bin_file))
        continue;

      volm_conf_buffer<volm_conf_score> score_idx(buffer_capacity());
      score_idx.initialize_read(score_bin_file);
      // load the candidate regions for current leaf
      bool is_cand = false;
      // load and check candidate region
      std::string outer_region_file = cand_folder() + "/cand_region_outer_" + leaf->get_string() + ".bin";
      vgl_polygon<double> cand_outer, cand_inner;
      cand_outer.clear();
      cand_inner.clear();
      if (vul_file::exists(outer_region_file)) {  // read the exterior boundaries of candidate region
        is_cand = true;
        vsl_b_ifstream ifs_out(outer_region_file);
        vsl_b_read(ifs_out, cand_outer);
        ifs_out.close();
      }
      std::string inner_region_file = cand_folder() + "/cand_region_inner_" + leaf->get_string() + ".bin";
      if (vul_file::exists(inner_region_file)) {
        vsl_b_ifstream ifs_in(inner_region_file);
        vsl_b_read(ifs_in, cand_inner);
        ifs_in.close();
      }
      // check whether the leaf is entirely covered by the candidate region
      if (cand_inner.num_sheets() == 0) {
        for (unsigned i = 0; (i < cand_outer.num_sheets() && is_cand); i++)
          if (volm_utils::poly_contains(cand_outer[i], leaf->extent_))
            is_cand = false;
      }
      // load the score
      vgl_point_3d<double> h_pt;
      while (leaf->hyps_->get_next(0,1,h_pt)) {
        if (is_cand)
          if (!volm_candidate_list::inside_candidate_region(cand_inner, cand_outer, h_pt.x(), h_pt.y()))
            continue;
        volm_conf_score score_in;
        score_idx.get_next(score_in);
        vgl_point_2d<double> h_pt_2d(h_pt.x(), h_pt.y());
        std::pair<float, std::pair<volm_conf_score, vgl_point_2d<double> > > tmp_pair(score_in.score(), std::pair<volm_conf_score, vgl_point_2d<double> >(score_in, h_pt_2d));
        score_map.insert(tmp_pair);
      }
    } // end of loop over leaves
    std::cout << "  scores in tile " << t_idx << " from " << loc_leaves.size() << " have been sorted successfully" << std::flush << std::endl;
  } // end of loop over tiles

  std::cout << "Start to generate " << num_top_locs() << " top regions from " << score_map.size() << " matched locations..." << std::flush << std::endl;
  // containers to store the circle region and heading direction
  std::vector<vgl_polygon<double> > pin_pt_poly;
  std::vector<float> pin_pt_heading;
  std::vector<vgl_point_2d<double> > pin_pt_center;
  std::vector<std::vector<vgl_point_2d<double> > > pin_pt_landmarks;
  std::vector<std::vector<unsigned char> > pin_pt_landmark_types;
  std::vector<float> pin_pt_max_dist;
  std::vector<float> pin_pt_heading_mid;
  std::vector<float> likelihood;
  auto mit = score_map.begin();
  while (pin_pt_center.size() < num_top_locs() && mit != score_map.end())
  {
    // check whether the location has been in the pin-pointed region
    if (is_contained(pin_pt_poly, mit->second.second)) {
      ++mit;
      continue;
    }
    // generate a pin-point region for current location
    likelihood.push_back(mit->first);
    pin_pt_center.push_back(mit->second.second);
    pin_pt_heading.push_back(mit->second.first.theta());
    std::vector<vgl_point_2d<double> > circle;
    if (!volm_candidate_list::generate_pin_point_circle(mit->second.second, radius(), circle))
    {
      log << "ERROR: generating pin point circle for location " << mit->second.second << " failed!\n";
      volm_io::write_error_log(log_file, log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    pin_pt_poly.emplace_back(circle);
    // find the furthest landmarks
    float max_dist = 0.0;
    for (auto & vit : mit->second.first.landmarks())
      if (vit.dist() > max_dist)
        max_dist = vit.dist();
    if (max_dist == 0.0)
      max_dist = radius()*5.0;
    float heading_mid = 0.0f;
    for (auto & vit : mit->second.first.landmarks())
      heading_mid += vit.theta();
    if (mit->second.first.landmarks().empty())
      heading_mid = mit->second.first.theta();
    else
      heading_mid /= mit->second.first.landmarks().size();
    pin_pt_heading_mid.push_back(heading_mid);
    pin_pt_max_dist.push_back(max_dist);
    // calculate the matched landmarks
    std::vector<vgl_point_2d<double> > landmarks;
    if (!generate_landmarks(mit->second.second, mit->second.first.landmarks(), landmarks)) {
      log << "ERROR: generating landmarks for location " << mit->second.second << " failed\n";
      volm_io::write_error_log(log_file, log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    pin_pt_landmarks.push_back(landmarks);
    std::vector<unsigned char> landmark_types;
    for (auto & i : mit->second.first.landmarks())
      landmark_types.push_back(i.land());
    pin_pt_landmark_types.push_back(landmark_types);

    ++mit;
  }

  std::cout << pin_pt_center.size() << " pin points are created out of " << score_map.size() <<  " matched locations" << std::endl;

  // write the kml file
  std::ofstream ofs_kml(out_kml().c_str());
  std::string kml_name = vul_file::strip_extension(vul_file::strip_directory(out_kml()));
  volm_candidate_list::open_kml_document(ofs_kml, kml_name, (float)num_top_locs());
  unsigned rank = 0;
  for (unsigned i = 0; i < pin_pt_center.size(); i++)
  {
    // create a line to represent heading direction and camera viewing volume
    double right_fov = 35.0*vnl_math::pi_over_180;  // 15 degree
    std::vector<vgl_point_2d<double> > cam_viewing;
    std::vector<vgl_point_2d<double> > heading_line;
    if (!volm_candidate_list::generate_heading_direction(pin_pt_center[i], pin_pt_heading_mid[i], pin_pt_max_dist[i]*1.2f, right_fov, heading_line, cam_viewing)) {
      log << "ERROR: generate heading directional line for rank " << rank << " pin point failed!\n";
      volm_io::write_error_log(log_file, log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    // generate a camera viewing volume, that is, heading direction +/- 15 degree

    // put current candidate region into kml
    volm_candidate_list::write_kml_regions(ofs_kml, pin_pt_poly[i][0], pin_pt_center[i], heading_line, cam_viewing, pin_pt_landmarks[i], pin_pt_landmark_types[i], likelihood[i], rank++);
  }
  volm_candidate_list::close_kml_document(ofs_kml);
  ofs_kml.close();

  std::cout << "FINISH!  candidate region is stored at: " << out_kml() << std::endl;
  return volm_io::SUCCESS;
}

#if 0
int main(int argc, char** argv)
{
  vul_arg<std::string>      out_kml("-out", "output kml file", "");
  vul_arg<unsigned>       world_id("-world", "ROI world id of the query", 9999);
  vul_arg<std::string>   geo_folder("-geo", "geo location database", "");
  vul_arg<std::string>    cand_file("-cand", "candidate list used during matching for search space reduction, if existed","");
  vul_arg<std::string>   index_name("-idx-name", "name of the loaded index", "");
  vul_arg<std::string> score_folder("-score", "folder to read matcher socre","");
  vul_arg<float>   buffer_capacity("-buffer",   "buffer capacity for loading score binary (in GByte)", 2.0f);
  vul_arg<double>           radius("-radius", "radius of pin-pointed circle (in meter)", 200.0);
  vul_arg<unsigned >  num_top_locs("-num-locs", "number of desired pinning points", 100);
  vul_arg_parse(argc, argv);
  std::cout << "argc: " << argc << std::endl;

  // input check
  if (world_id() == 9999 || geo_folder().compare("") == 0 || score_folder().compare("") == 0 || out_kml().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::string log_file;
  std::stringstream log;
  std::string kml_folder = vul_file::dirname(out_kml());
  log_file = kml_folder + "/log_cand_region.xml";
  // check score folder
  std::string score_bins = score_folder() + "/*" + index_name() + "*.bin";
  unsigned ns = 0;
  for (vul_file_iterator fn = score_bins.c_str(); fn; ++fn) {
    ns++;
  }
  if (ns == 0) {
    log << "ERROR: there is no score binaries in the input score folder: " << score_folder() << "!\n";
    volm_io::write_error_log(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // create volm tiles for given world_id
  std::vector<volm_tile> tiles;
  if (!volm_tile::generate_tiles(world_id(), tiles)) {
    log << "ERROR: unknown world id: " << world_id() << "!\n";
    volm_io::write_error_log(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // load candidate region from previous matcher if exists
  // create the candidate polygon if exists
  vgl_polygon<double> cand_out, cand_in;
  bool is_cand = false;
  cand_out.clear();  cand_in.clear();
  if (vul_file::exists(cand_file())) {
    //cand_poly = bkml_parser::parse_polygon(cand_file());
    unsigned n_out, n_in;
    vgl_polygon<double> poly = bkml_parser::parse_polygon_with_inner(cand_file(), cand_out, cand_in, n_out, n_in);
    std::cout << "candidate regions (" << cand_out.num_sheets() << " outer sheet and " << cand_in.num_sheets() << " inner sheet)are loaded from file: "
             << cand_file() << "!!!!!!!!!!" << std::endl;
    is_cand = (cand_out.num_sheets() != 0);
  }

  // loop over each tile to load all scores and sort them
  std::cout << "------------------ Start to create candidate region ---------------------" << std::endl;
  std::cout << tiles.size() << " tiles are created for world: " << world_id() << std::endl;
  std::multimap<float, std::pair<volm_conf_score, vgl_point_2d<double> >, std::greater<float> > score_map;
  for (unsigned t_idx = 0; t_idx < tiles.size(); t_idx++)
  {
    volm_tile tile = tiles[t_idx];
    std::stringstream file_name_pre;
    file_name_pre << geo_folder() << "geo_index_tile_" << t_idx;
    // no geo location for current tile, skip
    if (!vul_file::exists(file_name_pre.str()+ ".txt"))
      continue;
    // check file size
    if (vul_file::size(file_name_pre.str()+".txt") == 0)
      continue;
    // load the location database for current tile
    float min_size;
    volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
    volm_geo_index::read_hyps(root, file_name_pre.str());
    std::vector<volm_geo_index_node_sptr> loc_leaves_all;
    loc_leaves_all.clear();
    volm_geo_index::get_leaves_with_hyps(root, loc_leaves_all);
    // obtain the desired leaf
    std::vector<volm_geo_index_node_sptr> loc_leaves;
    for (unsigned i = 0; i < loc_leaves_all.size(); i++)
      if (is_cand && vgl_intersection(loc_leaves_all[i]->extent_, cand_out))
        loc_leaves.push_back(loc_leaves_all[i]);
      else
        loc_leaves.push_back(loc_leaves_all[i]);
    std::cout << "  loading and sorting scores for tile " << t_idx << " from " << loc_leaves.size() << " leaves" << std::endl;
    std::stringstream score_file_pre;
    score_file_pre << score_folder() << "/conf_score_tile_" << t_idx;
    for (unsigned i = 0; i < loc_leaves.size(); i++)
    {
      volm_geo_index_node_sptr leaf = loc_leaves[i];
      std::string score_bin_file = score_file_pre.str() + "_" + leaf->get_string() + "_" + index_name() + ".bin";
      if (!vul_file::exists(score_bin_file))
        continue;
      volm_conf_buffer<volm_conf_score> score_idx(buffer_capacity());
      score_idx.initialize_read(score_bin_file);
      vgl_point_3d<double> h_pt;
      while (leaf->hyps_->get_next(0, 1, h_pt))
      {
#if 0
        // use both inner and outer boundary
        if (is_cand && !volm_candidate_list::inside_candidate_region(cand_in, cand_out, h_pt.x(), h_pt.y()))
          continue;
#endif
        // use outer boundary only
        if (is_cand && !volm_candidate_list::inside_candidate_region(cand_out, h_pt.x(), h_pt.y()))
          continue;
        volm_conf_score score_in;
        score_idx.get_next(score_in);
        vgl_point_2d<double> h_pt_2d(h_pt.x(), h_pt.y());
        std::pair<float, std::pair<volm_conf_score, vgl_point_2d<double> > > tmp_pair(score_in.score(), std::pair<volm_conf_score, vgl_point_2d<double> >(score_in, h_pt_2d));
        score_map.insert(tmp_pair);
      }
    }
    std::cout << "  socres in tile " << t_idx << " from " << loc_leaves.size() << " have been sorted successfully" << std::flush << std::endl;
  }  // end of loop over tiles

  std::cout << "Start to generate " << num_top_locs() << " top regions from " << score_map.size() << " matched locations..." << std::flush << std::endl;
  // containers to store the circle region and heading direction
  std::vector<vgl_polygon<double> > pin_pt_poly;
  std::vector<float> pin_pt_heading;
  std::vector<vgl_point_2d<double> > pin_pt_center;
  std::vector<std::vector<vgl_point_2d<double> > > pin_pt_landmarks;
  std::vector<std::vector<unsigned char> > pin_pt_landmark_types;
  std::vector<float> pin_pt_max_dist;
  std::vector<float> pin_pt_heading_mid;
  std::vector<float> likelihood;
  std::multimap<float, std::pair<volm_conf_score, vgl_point_2d<double> > >::iterator mit = score_map.begin();
  while (pin_pt_center.size() < num_top_locs() && mit != score_map.end())
  {
    // check whether the location has been in the pin-pointed region
    if (is_contained(pin_pt_poly, mit->second.second)) {
      ++mit;
      continue;
    }
    // generate a pin-point region for current location
    likelihood.push_back(mit->first);
    pin_pt_center.push_back(mit->second.second);
    pin_pt_heading.push_back(mit->second.first.theta());
    std::vector<vgl_point_2d<double> > circle;
    if (!volm_candidate_list::generate_pin_point_circle(mit->second.second, radius(), circle))
    {
      log << "ERROR: generating pin point circle for location " << mit->second.second << " failed!\n";
      volm_io::write_error_log(log_file, log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    pin_pt_poly.push_back(vgl_polygon<double>(circle));
    // find the furthest landmarks
    float max_dist = 0.0;
    for (std::vector<volm_conf_object>::iterator vit = mit->second.first.landmarks().begin(); vit != mit->second.first.landmarks().end(); ++vit)
      if (vit->dist() > max_dist)
        max_dist = vit->dist();
    if (max_dist == 0.0)
      max_dist = radius()*5.0;
    float heading_mid = 0.0f;
    for (std::vector<volm_conf_object>::iterator vit = mit->second.first.landmarks().begin(); vit != mit->second.first.landmarks().end(); ++vit)
      heading_mid += vit->theta();
    if (mit->second.first.landmarks().empty())
      heading_mid = mit->second.first.theta();
    else
      heading_mid /= mit->second.first.landmarks().size();
    pin_pt_heading_mid.push_back(heading_mid);
    pin_pt_max_dist.push_back(max_dist);
    // calculate the matched landmarks
    std::vector<vgl_point_2d<double> > landmarks;
    if (!generate_landmarks(mit->second.second, mit->second.first.landmarks(), landmarks)) {
      log << "ERROR: generating landmarks for location " << mit->second.second << " failed\n";
      volm_io::write_error_log(log_file, log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    pin_pt_landmarks.push_back(landmarks);
    std::vector<unsigned char> landmark_types;
    for (unsigned i = 0; i < mit->second.first.landmarks().size(); i++)
      landmark_types.push_back(mit->second.first.landmarks()[i].land());
    pin_pt_landmark_types.push_back(landmark_types);

    ++mit;
  }

  std::cout << pin_pt_center.size() << " pin points are created out of " << score_map.size() <<  " matched locations" << std::endl;

  // write the kml file
  std::ofstream ofs_kml(out_kml().c_str());
  std::string kml_name = vul_file::strip_extension(vul_file::strip_directory(out_kml()));
  volm_candidate_list::open_kml_document(ofs_kml, kml_name, (float)num_top_locs());
  unsigned rank = 0;
  for (unsigned i = 0; i < pin_pt_center.size(); i++)
  {
    // create a line to represent heading direction and camera viewing volume
    double right_fov = 35.0*vnl_math::pi_over_180;  // 15 degree
    std::vector<vgl_point_2d<double> > cam_viewing;
    std::vector<vgl_point_2d<double> > heading_line;
    if (!volm_candidate_list::generate_heading_direction(pin_pt_center[i], pin_pt_heading_mid[i], pin_pt_max_dist[i]*1.2f, right_fov, heading_line, cam_viewing)) {
      log << "ERROR: generate heading directional line for rank " << rank << " pin point failed!\n";
      volm_io::write_error_log(log_file, log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    // generate a camera viewing volume, that is, heading direction +/- 15 degree

    // put current candidate region into kml
    volm_candidate_list::write_kml_regions(ofs_kml, pin_pt_poly[i][0], pin_pt_center[i], heading_line, cam_viewing, pin_pt_landmarks[i], pin_pt_landmark_types[i], likelihood[i], rank++);
  }
  volm_candidate_list::close_kml_document(ofs_kml);
  ofs_kml.close();

  std::cout << "FINISH!  candidate region is stored at: " << out_kml() << std::endl;
  return volm_io::SUCCESS;
}
#endif

bool is_contained(std::vector<vgl_polygon<double> > const& poly_vec, vgl_point_2d<double> const& pt)
{
  unsigned n_poly = poly_vec.size();
  for (unsigned i = 0; i < n_poly; i++)
  {
    for (unsigned k = 0; k < poly_vec[i].num_sheets(); k++)
    {
      vgl_polygon<double> single_sheet(poly_vec[i][k]);
      if (single_sheet.contains(pt))
        return true;
    }
  }
  return false;
}

bool generate_landmarks(vgl_point_2d<double> const& pt, std::vector<volm_conf_object> const& land_objs, std::vector<vgl_point_2d<double> >& landmarks)
{
  // construct a local lvcs
  double c_lon = pt.x(), c_lat = pt.y();
  vpgl_lvcs lvcs(c_lat, c_lon, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  for (const auto & land_obj : land_objs)
  {
    double lon, lat, gz;
    float lx = land_obj.x();
    float ly = land_obj.y();
    lvcs.local_to_global(lx, ly, 0.0, vpgl_lvcs::wgs84, lon, lat, gz);
    landmarks.emplace_back(lon, lat);
  }
  return true;
}
