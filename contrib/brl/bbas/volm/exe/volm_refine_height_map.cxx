// This is contrib/brl/bbas/volm/volm_refine_height_map.cxx
//:
// \file
// \brief executable to refine height map using land classification information
//        Note in this executable, 'road' and 'parks' are used to evaluate the ground level height
// \author Yi Dong
// \date Dec 05, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <algorithm>
#include <iostream>
#include <utility>
#include <vil/algo/vil_region_finder.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <volm/volm_category_io.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_io.h>
#include <volm/volm_io_tools.h>
#include <volm/volm_osm_objects.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file_iterator.h>


static void error(std::string log_file, const std::string& msg)
{
  std::cerr << msg;
  volm_io::write_post_processing_log(std::move(log_file), msg);
}

static bool float_equal(float const& a, float const& b)
{
  return ((a-b)*(a-b) < 1E-8);
}

static float average(std::vector<float> const& values)
{
  float sum = 0.0f;
  for (float value : values)
    sum += value;
  return sum/values.size();
}

static bool find_nearest_building(unsigned const& i, unsigned const& j,
                                  int const& radius,
                                  std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > > const& buildings,
                                  unsigned & building_id)
{
  unsigned num_nbrs = 8;

  auto mit = buildings.begin();
  for( ; mit != buildings.end(); ++mit)
  {
    std::vector<std::pair<int, int> > b_pixels;
    auto num_pixels = (unsigned)mit->second.second.size();
    for (unsigned p_idx = 0; p_idx < num_pixels; p_idx++)
      b_pixels.emplace_back(mit->second.first[p_idx], mit->second.second[p_idx]);

    for (int r_idx = 1; r_idx < radius; r_idx++)
    {
      int nbrs8_delta[8][2] = { { r_idx, 0}, { r_idx,-r_idx}, { 0,-r_idx}, {-r_idx,-r_idx},
                                {-r_idx, 0}, {-r_idx, r_idx}, { 0, r_idx}, { r_idx, r_idx} };
      for (unsigned c = 0; c < num_nbrs; c++) {
        int nbr_i = i + nbrs8_delta[c][0];
        int nbr_j = j + nbrs8_delta[c][1];
        if (std::find(b_pixels.begin(), b_pixels.end(), std::pair<int,int>(nbr_i, nbr_j)) != b_pixels.end())
        {
          building_id = mit->first;
          return true;
        }
      }
    }
  }
  building_id = 0;
  return false;
}

static float standard_devation(std::vector<float> const& values, float const& mean)
{
  float dev_sum = 0.0f;
  for (float value : values)
    dev_sum += (value - mean) * (value - mean);
  return std::sqrt(dev_sum/values.size());
}

//: obtain the building pixels inside a window from the classification image (forward declaration)
static bool obtain_buildings(vil_image_view<vxl_byte> const& c_img,
                             unsigned const& start_ni, unsigned const& start_nj,
                             unsigned const& end_ni,   unsigned const& end_nj,
                             std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > >& buildings,
                             std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > >& flat_regions,
                             std::vector<std::pair<unsigned, unsigned> >& non_buildings);

static bool refine_building_by_median(vil_image_view<float> const& h_img,
                                      std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > > const& buildings,
                                      vil_image_view<float>& refined_img);

static bool refine_building_by_median_divided(vil_image_view<float> const& h_img,
                                              vil_image_view<vxl_byte> const& c_img,
                                              std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > > const& buildings,
                                              unsigned const& size,
                                              vil_image_view<float>& refined_img);

static bool refine_building(float const& ratio,
                            float const& height_threshold,
                            vil_image_view<float> const& h_img,
                            vil_image_view<vxl_byte> & refined_c_img,
                            vil_image_view<float>& refined_h_img);

static float neighbor_height(std::map<std::pair<unsigned, unsigned>, float> const& window_height,
                             unsigned const& w_idx_i,
                             unsigned const& w_idx_j,
                             unsigned const& w_size);


#if 1
// refine satellite height map using building footprint obtained both from satellite classification map and OSM buildings/roads
// the ground height here is defined by road obtained from osm
int main(int argc, char** argv)
{
  vul_arg<std::string> height_img_name("-height", "height map about to be refined", "");
  vul_arg<std::string> height_cam_name("-height-cam", "tfw camera for height map", "");
  vul_arg<std::string> out_height("-out-height", "output filename of the refined height map", "");
  vul_arg<std::string> out_class("-out-class", "output filename of the refined class map", "");
  vul_arg<unsigned> dx("-x", "refined window size in pixel unit", 60);
  vul_arg<unsigned> dy("-y", "refined window size in pixel unit", 60);
  vul_arg<unsigned> iteration("-iter","number of refinement iteration", 100);
  vul_arg<float> ratio("-ratio", "shape factor to control morphological operation", 0.5);
  vul_arg<float> height_threshold("-thres", "threshold in height difference", 5);
  vul_arg<float> tall_building_thres("-tall","height threshold for tall building category",60);
  vul_arg<std::string> class_map_folder("-2d-class-root", "folder where all generated 2d map stored","");
  vul_arg<std::string> out_class_color("-out-class-color","output color class map","");
  vul_arg_parse(argc, argv);


  if (height_img_name().compare("") == 0 || height_cam_name().compare("") == 0 ||
      out_height().compare("") == 0 || out_class().compare("") == 0 || class_map_folder().compare("") == 0)
  {
    vul_arg_display_usage_and_exit();
    return false;
  }
  std::stringstream log_file;
  std::stringstream log;
  log_file << out_height() << "/log_refine_height_map_scene.xml";

  // load height map image (float format)
  if (!vul_file::exists(height_img_name())) {
    log << "error: can not find height map image: " << height_img_name() << '\n';
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vil_image_view<float> h_img = vil_load(height_img_name().c_str());

  // image size needs to be same
  unsigned ni = h_img.ni();
  unsigned nj = h_img.nj();
  unsigned num_w_i = ni/dx() + 1;
  unsigned num_w_j = nj/dx() + 1;

  // load height map camera
  if (!vul_file::exists(height_cam_name())) {
    log << "ERROR: can not find tfw camera file for height map: " << height_cam_name() << '\n';
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs;
  vpgl_geo_camera* h_cam;
  vpgl_geo_camera::init_geo_camera(height_cam_name(), dummy_lvcs, 0, 0, h_cam);

  std::cout << "Start refined the height map..." << std::endl;
  std::cout << "  img size: " << ni << 'x' << nj << std::endl;
  std::cout << "  window size: " << dx() << 'x' << dy() << std::endl;
  std::cout << "  window num: " << num_w_i << 'x' << num_w_j << std::endl;

  // read all the geo_index tree for class map
  double class_map_min_size;
  std::vector<std::string> class_map_texts;
  std::string file_glob = class_map_folder() + "/p1b_wr*_tile*.txt";
  std::vector<volm_geo_index2_node_sptr> class_roots;
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn) {
    volm_geo_index2_node_sptr class_root = volm_geo_index2::read_and_construct<volm_osm_object_ids_sptr>(fn(), class_map_min_size);
    class_roots.push_back(class_root);
    std::cout << " geo index is created from file " << fn() << std::endl;
  }

  // find the overlap resources
  std::vector<volm_geo_index2_node_sptr> class_leaves;
  double h_lowerleft_lon, h_lowerleft_lat;
  h_cam->img_to_global(0, nj-1, h_lowerleft_lon, h_lowerleft_lat);
  double h_upperrght_lon, h_upperrght_lat;
  h_cam->img_to_global(ni-1, 0, h_upperrght_lon, h_upperrght_lat);
  vgl_box_2d<double> h_bbox(h_lowerleft_lon, h_upperrght_lon, h_lowerleft_lat, h_upperrght_lat);
  for (const auto & class_root : class_roots) {
    std::vector<volm_geo_index2_node_sptr> leaves;
    leaves.clear();
    volm_geo_index2::get_leaves(class_root, leaves, h_bbox);
    for (const auto & leave : leaves)
      class_leaves.push_back(leave);
  }

  if (class_leaves.empty()) {
    log << "ERROR: can not find any overlapped 2d class map for current height map: " << height_img_name() << " -- " << h_bbox << '\n';
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  unsigned num_leaves = class_leaves.size();
  std::cout << "there are " << num_leaves << " class maps intersect with height map " << h_bbox << std::endl;
  for (unsigned l_idx = 0; l_idx < num_leaves; l_idx++)
    std::cout <<"\t " << class_leaves[l_idx]->get_string() << " --- " << class_leaves[l_idx]->extent_ << std::endl;

  // load the associated image resource
  std::map<std::string, vil_image_view<vxl_byte> > class_img_map;
  std::map<std::string, vpgl_geo_camera*> class_cam_map;
  for (unsigned l_idx = 0; l_idx < num_leaves; l_idx++) {
    double lon_min, lat_min, lon_max, lat_max;
    volm_geo_index2_node_sptr leaf = class_leaves[l_idx];
    lon_min = leaf->extent_.min_x();  lat_min = leaf->extent_.min_y();
    lon_max = leaf->extent_.max_x();  lat_max = leaf->extent_.max_y();
    double scale_x = lon_max-lon_min;
    double scale_y = lat_max-lat_min;
    std::string hemisphere, direction;
    if (lon_min < 0)  direction  = "W";
    else              direction  = "E";
    if (lat_min < 0)  hemisphere = "S";
    else              hemisphere = "N";
    std::stringstream img_name;
    img_name << class_map_folder() << "/Sat2dMap_" << hemisphere << std::setprecision(12) << lat_min
                                   << direction << std::setprecision(12) << lon_min
                                   << "_S" << scale_x << 'x' << scale_y << ".tif";
    std::stringstream cam_name;
    cam_name << class_map_folder() << "/Sat2dMap_" << hemisphere << std::setprecision(12) << lat_min
                                   << direction << std::setprecision(12) << lon_min
                                   << "_S" << scale_x << 'x' << scale_y << ".tfw";
    if (!vul_file::exists(img_name.str())) {
      log << "ERROR: can not find class 2d map image resource: " << img_name.str() << '\n';  error(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    if (!vul_file::exists(cam_name.str())) {
      log << "ERROR: can not find class 2d map camera tfw file: " << cam_name.str() << '\n';  error(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    volm_img_info info;
    volm_io_tools::load_lidar_img(img_name.str(), info, true, false, true, cam_name.str());
    //volm_io_tools::load_geotiff_image(img_name.str(), info, true);
    auto* img_src = dynamic_cast<vil_image_view<vxl_byte>*>(info.img_r.ptr());
    class_img_map.insert(std::pair<std::string, vil_image_view<vxl_byte> >(leaf->get_string(), *img_src));
    class_cam_map.insert(std::pair<std::string, vpgl_geo_camera*>(leaf->get_string(), info.cam));
  }

  // create a class image from overlapped class map
  vil_image_view<vxl_byte> c_img(ni, nj);
  c_img.fill(0);
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      double lon, lat;
      h_cam->img_to_global(i,j,lon,lat);
      for (unsigned l_idx = 0; l_idx < num_leaves; l_idx++) {
        //if (!class_leaves[l_idx]->extent_.contains(lon, lat))
        //  continue;
        double u, v;
        class_cam_map[class_leaves[l_idx]->get_string()]->global_to_img(lon, lat, 0.0, u, v);
        auto uu = (unsigned)std::floor(u+0.5);
        auto vv = (unsigned)std::floor(v+0.5);
        if (uu > 0 && vv > 0 && uu < class_img_map[class_leaves[l_idx]->get_string()].ni() && vv < class_img_map[class_leaves[l_idx]->get_string()].nj())
          c_img(i,j) = (class_img_map[class_leaves[l_idx]->get_string()])(uu,vv);
      }
    }
  }

  // start to refine the building
  vil_image_view<vxl_byte> refined_c_img;
  refined_c_img.deep_copy(c_img);
  std::string refined_c_img_name = out_class();
  vil_image_view<float> refined_h_img(ni, nj);
  refined_h_img.deep_copy(h_img);
  std::string refined_h_img_name = out_height();

#if 0
  std::string c_img_before_refine = vul_file::strip_extension(refined_c_img_name) + "_cropped.tif";
  vil_image_view<vil_rgb<vxl_byte> > out_class_img(ni, nj, 1);
  out_class_img.fill(volm_osm_category_io::volm_land_table[0].color_);
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      out_class_img(i,j) = volm_osm_category_io::volm_land_table[c_img(i,j)].color_;
    }
  }
  vil_save(out_class_img, c_img_before_refine.c_str());
#endif

  // maps to record the ground height of each window (key is its window id)
  std::map<std::pair<unsigned, unsigned>, float> window_min_height;
  window_min_height.clear();

  // map to record the non-building pixels of each window
  std::map<std::pair<unsigned, unsigned>, std::vector<std::pair<unsigned, unsigned> > > non_building_regions;

  // record all the building median heights in the height image
  std::vector<float> all_buidling_heights;

  std::cout << "Start to refine buildings";
  for (unsigned w_idx_i = 0; w_idx_i < num_w_i; w_idx_i++)
  {
    unsigned start_ni, end_ni;
    start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
    for (unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++)
    {
      std::cout << '.';
      std::cout.flush();
      std::pair<unsigned,unsigned> window_key(w_idx_i, w_idx_j);
      // obtain window pixels
      unsigned start_nj, end_nj;
      start_nj = w_idx_j*dy(); end_nj = (w_idx_j+1)*dy();
      // obtain building pixels, road pixels and non-building pixels (key is a unique value from upper left of the pixel)
      std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > > buildings;
      std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > > flat_regions;
      std::vector<std::pair<unsigned, unsigned> > non_buildings;
      if (!obtain_buildings(c_img, start_ni, start_nj, end_ni, end_nj, buildings, flat_regions, non_buildings)) {
        log << "error: building extraction failed for window (" << start_ni << 'x' << start_nj << ") (to " << end_ni << 'x' << end_nj << ")\n";
        error(log_file.str(), log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }

      // flatten the building height by their median
      // flat building super pixels values (use one of the refinement scheme below)
#if 0
      // refine the whole building superpixel by a constant median value
      if (!refine_building_by_median(h_img, buildings, refined_h_img))
      {
        log << "error: refine height image using median height failed\n";
        error(log_file.str(), log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }
#endif
#if 1
      // refine the building roof by dividing building super pixels into small region
      unsigned sub_pixel_size = 10;  // a 5x5 pixel window to split the building super pixels
      if (!refine_building_by_median_divided(h_img, c_img, buildings, sub_pixel_size, refined_h_img)) {
        log << "error: refine height image using median height failed\n";
        error(log_file.str(), log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }

#endif

      std::pair<std::pair<unsigned, unsigned>, std::vector<std::pair<unsigned, unsigned> > > tmp_pair(std::pair<unsigned, unsigned>(w_idx_i, w_idx_j), non_buildings);
      non_building_regions.insert(tmp_pair);

      // obtain the ground height from flat region -- using the minimum height for current window
      std::vector<float> flat_heights;
      for (auto & flat_region : flat_regions)
      {
        auto num_pts = (unsigned)flat_region.second.first.size();
        for (unsigned ri = 0; ri < num_pts; ri++)
          flat_heights.push_back(h_img(flat_region.second.first[ri], flat_region.second.second[ri]));
      }
      if (flat_heights.size() != 0) {
        std::sort(flat_heights.begin(), flat_heights.end());
        //float height_median = flat_heights[flat_heights.size()/2];
        float height_median = flat_heights[0];
        window_min_height.insert(std::pair<std::pair<unsigned, unsigned>, float>(std::pair<unsigned, unsigned>(w_idx_i, w_idx_j), height_median));
      }
    } // end of loop over each window
  } // end of loop over each window

   // if there is no region can be used to define ground plane height, do not refine ground region
  // at this stage, only building roof being flattened, class map remain same, building boundary remain as before
  if (window_min_height.empty()) {
    log << "warning: no road/park is available to refine ground plane height, ground refinement ignored\n";
    error(log_file.str(), log.str());
    vil_save(refined_h_img, refined_h_img_name.c_str());
    vil_save(refined_c_img, refined_c_img_name.c_str());
    return volm_io::SUCCESS;
  }

  // ingest ground level height using height median retrieved from road and park region (class map doesn't change
  unsigned window_size = num_w_i;
  if (window_size > num_w_j) window_size = num_w_j;
  std::cout << "\nStart to refine ground elevation";
  for (unsigned w_idx_i = 0; w_idx_i < num_w_i; w_idx_i++)
  {
    unsigned start_ni, end_ni;
    for (unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++)
    {
      std::cout << '.';
      std::cout.flush();

      // obtain the window height
      float grd_height = 0.0f;
      auto mit = window_min_height.find(std::pair<unsigned,unsigned>(w_idx_i, w_idx_j));
      if (mit == window_min_height.end())
        // search neighbors to obtain height
        grd_height = neighbor_height(window_min_height, w_idx_i, w_idx_j, window_size);
      else
        grd_height = mit->second;
      // refine the ground height
      auto r_mit = non_building_regions.find(std::pair<unsigned,unsigned>(w_idx_i, w_idx_j));
      if (r_mit != non_building_regions.end()) {
        std::vector<std::pair<unsigned, unsigned> > region_pixels = r_mit ->second;
        for (auto & region_pixel : region_pixels) {
          refined_h_img(region_pixel.first,region_pixel.second) = grd_height;
        }
      }
    }
  }

  // refine the building boundary using morphological operation (class map will be updated here to accept new building pixels)
  // height threshold used to divide neighbor buildings having different height
  std::cout << "\nStart to refine building boundary";
  for (unsigned i = 0; i < iteration(); i++) {
    std::cout << '.';
    std::cout.flush();
    if(!refine_building(ratio(), height_threshold(), h_img, refined_c_img, refined_h_img)) {
      log << "error: refining building failed at ratio " << ratio() << '\n';
      error(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
  }

  // refine once again with high ratio and no height threshold to get rid of white noise (spikes)
  if(!refine_building(0.6f, 1000, h_img, refined_c_img, refined_h_img)) {
    log << "error: refining building failed at ratio " << ratio() << '\n';
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::cout << std::endl;

  // re-check class image
  for (unsigned w_idx_i = 0; w_idx_i < num_w_i; w_idx_i++)
  {
    unsigned start_ni, end_ni;
    for (unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++)
    {
      std::cout << '.';
      std::cout.flush();
      std::pair<unsigned,unsigned> window_key(w_idx_i, w_idx_j);
      // obtain window pixels
      unsigned start_nj, end_nj;
      start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
      start_nj = w_idx_j*dy(); end_nj = (w_idx_j+1)*dy();
      // obtain the ground height for current window
      float grd_height = 0.0f;
      auto mit = window_min_height.find(std::pair<unsigned,unsigned>(w_idx_i, w_idx_j));
      if (mit == window_min_height.end())
        // search neighbors to obtain height
        grd_height = neighbor_height(window_min_height, w_idx_i, w_idx_j, window_size);
      else
        grd_height = mit->second;

      // re-label building as tall building
      for (unsigned i = start_ni; i < end_ni; i++)
      {
        for (unsigned j = start_nj; j < end_nj; j++)
        {
          if ( i >= ni || j >= nj)
            continue;
          if ((refined_h_img(i,j) - grd_height) > 0)
            refined_c_img(i,j) = volm_osm_category_io::volm_land_table_name["building"].id_;
        }
      }
    }
  }

  // re-label to get tall building the tall building
  // for building with height larger than given tall_building_thres, they are relabeled as tall building
  for (unsigned w_idx_i = 0; w_idx_i < num_w_i; w_idx_i++)
  {
    unsigned start_ni, end_ni;
    for (unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++)
    {
      std::cout << '.';
      std::cout.flush();
      std::pair<unsigned,unsigned> window_key(w_idx_i, w_idx_j);
      // obtain window pixels
      unsigned start_nj, end_nj;
      start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
      start_nj = w_idx_j*dy(); end_nj = (w_idx_j+1)*dy();
      // obtain the ground height for current window
      float grd_height = 0.0f;
      auto mit = window_min_height.find(std::pair<unsigned,unsigned>(w_idx_i, w_idx_j));
      if (mit == window_min_height.end())
        // search neighbors to obtain height
        grd_height = neighbor_height(window_min_height, w_idx_i, w_idx_j, window_size);
      else
        grd_height = mit->second;

      // re-label building as tall building
      for (unsigned i = start_ni; i < end_ni; i++)
      {
        for (unsigned j = start_nj; j < end_nj; j++)
        {
          if ( i >= ni || j >= nj)
            continue;
          if ((refined_h_img(i,j) - grd_height) > tall_building_thres() &&  refined_c_img(i,j) == volm_osm_category_io::volm_land_table_name["building"].id_)
            refined_c_img(i,j) = volm_osm_category_io::volm_land_table_name["tall_building"].id_;
        }
      }
    }
  }

  //output the result
  vil_save(refined_c_img, refined_c_img_name.c_str());
  vil_save(refined_h_img, refined_h_img_name.c_str());

#if 1
  // define a color image for debugging purpose
  std::string out_folder = vul_file::dirname(refined_c_img_name);
  vil_image_view<vil_rgb<vxl_byte> > out_color_img(ni, nj, 1);
  out_color_img.fill(volm_osm_category_io::volm_land_table[0].color_);
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      out_color_img(i,j) = volm_osm_category_io::volm_land_table[refined_c_img(i,j)].color_;
    }
  }
  vil_save(out_color_img, out_class_color().c_str());
#endif

  return volm_io::SUCCESS;
}
#endif

static bool obtain_buildings(vil_image_view<vxl_byte> const& c_img,
                             unsigned const& start_ni, unsigned const& start_nj,
                             unsigned const& end_ni,   unsigned const& end_nj,
                             std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > >& buildings,
                             std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > >& flat_regions,
                             std::vector<std::pair<unsigned, unsigned> >& non_buildings)
{
  buildings.clear();
  flat_regions.clear();
  non_buildings.clear();
  unsigned ni = c_img.ni();  unsigned nj = c_img.nj();
  vil_region_finder<vxl_byte> region_finder(c_img, vil_region_finder_8_conn);
  for (unsigned i = start_ni; i < end_ni; i++)
  {
    for (unsigned j = start_nj; j < end_nj; j++)
    {
      if ( i >= ni || j >= nj)
        continue;
      if (c_img(i,j) == volm_osm_category_io::volm_land_table_name["building"].id_)
      {
        std::vector<unsigned> ri;  std::vector<unsigned> rj;
        ri.resize(0);  rj.resize(0);
        region_finder.same_int_region(i, j, ri, rj);
        //ignore regions that are smaller than 10 pixels
        if (ri.size() >= 10) {
          unsigned key = (i+j)*(i+j+1)/2 + j;
          std::pair<std::vector<unsigned>, std::vector<unsigned> > tmp(ri, rj);
          buildings.insert(std::pair<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > >(key, tmp));
        }
      }
      else if (c_img(i,j) == volm_osm_category_io::volm_land_table_name["roads"].id_)
      {
        std::vector<unsigned> ri;  std::vector<unsigned> rj;
        ri.resize(0);  rj.resize(0);
        region_finder.same_int_region(i, j, ri, rj);
        //ignore regions that are smaller than 10 pixels
        if (ri.size()) {
          unsigned key = (i+j)*(i+j+1)/2 + j;
          std::pair<std::vector<unsigned>, std::vector<unsigned> > tmp(ri, rj);
          flat_regions.insert(std::pair<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > >(key, tmp));
        }
      }
      else
        continue;
    }
  }
  // obtain non-building region
  for (unsigned i = start_ni; i < end_ni; i++) {
    for (unsigned j = start_nj; j < end_nj; j++)
    {
      if ( i >= ni || j >= nj)
        continue;
      // check whether pixel (i,j) is inside any building
      bool is_inside_building = false;
      for (auto mit = buildings.begin();
          (mit != buildings.end() && !is_inside_building); ++mit) {
        if (mit->second.first.size() != mit->second.second.size())
          return false;
        for (unsigned b_idx = 0; (b_idx < mit->second.first.size() && !is_inside_building); b_idx++) {
          if ( i == mit->second.first[b_idx] && j == mit->second.second[b_idx] )
            is_inside_building = true;
        }
      }
      // add non_building pixels
      if (!is_inside_building)
        non_buildings.emplace_back(i,j);
    }
  }
  return true;
}

static bool refine_building_by_median(vil_image_view<float> const& h_img,
                                      std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > > const& buildings,
                                      vil_image_view<float>& refined_img)
{
  // refine each building with its median height
  for (const auto & building : buildings)
  {
    if (building.second.first.size() < 10)
      continue;
    auto num_pts = (unsigned)building.second.first.size();
    // find the median
    std::vector<float> height_values;
    for (unsigned p_idx = 0; p_idx < num_pts; p_idx++)
      height_values.push_back(h_img(building.second.first[p_idx], building.second.second[p_idx]));
    std::sort(height_values.begin(), height_values.end());
    float median = height_values[height_values.size()/2];

    // refine the height using median
    for (unsigned p_idx = 0; p_idx < num_pts; p_idx++) {
      unsigned int img_i = building.second.first[p_idx];
      unsigned int img_j = building.second.second[p_idx];
      refined_img(img_i, img_j) = median;
    }

    // compute the mean height and the std
    float mean = average(height_values);
    float variance = standard_devation(height_values, mean);
    std::vector<float> height_data;
    height_data.push_back(median);
    height_data.push_back(mean);
    height_data.push_back(variance);
  }

#if 0
  // refine ground using road median
  // find the road median
  std::vector<float> road_heights;
  for (std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > >::const_iterator mit = roads.begin();
       mit != roads.end(); ++mit)
  {
    //if (mit->second.first.size() < 10)
    //  continue;
    unsigned num_pts = mit->second.first.size();
    for (unsigned ri = 0; ri < num_pts; ri++)
      road_heights.push_back(h_img(mit->second.first[ri], mit->second.second[ri]));
  }
  if (road_heights.size() == 0)
    return false;
  std::sort(road_heights.begin(), road_heights.end());
  float road_median = road_heights[road_heights.size()/2];
  // fill the road median
  unsigned num_grd_pixel = non_buildings.size();
  for (unsigned gi = 0; gi < num_grd_pixel; gi++) {
    refined_img(non_buildings[gi].first, non_buildings[gi].second) = road_median;
  }
#endif

  return true;
}

static bool refine_building_by_median_divided(vil_image_view<float> const& h_img,
                                              vil_image_view<vxl_byte> const& c_img,
                                              std::map<unsigned, std::pair<std::vector<unsigned>, std::vector<unsigned> > > const& buildings,
                                              unsigned const& w_size,
                                              vil_image_view<float>& refined_img)
{
  // loop over each super pixels and divide them with a given window size
  for (const auto & building : buildings)
  {
    if (building.second.first.size() < 10)  // ignore super pixel with less than 10 pixels
      continue;
    std::vector<std::pair<unsigned, unsigned> > super_pixel;
    auto num_pts = (unsigned)building.second.first.size();
    for (unsigned p_idx = 0; p_idx < num_pts; p_idx++)
      super_pixel.emplace_back(building.second.first[p_idx], building.second.second[p_idx]);
    // form the bbox of super pixels
    vgl_box_2d<unsigned> bbox;
    for (unsigned p_idx = 0; p_idx < num_pts; p_idx++)
      bbox.add(vgl_point_2d<unsigned>(super_pixel[p_idx].first, super_pixel[p_idx].second));
    // refine the each window inside the bbox
    unsigned ni = bbox.max_x() - bbox.min_x();
    unsigned nj = bbox.max_y() - bbox.min_y();
    unsigned num_w_i = ni/w_size + 1;
    unsigned num_w_j = nj/w_size + 1;
    // obtain the building height for each window
    for (unsigned int w_idx_i = 0; w_idx_i < num_w_i; ++w_idx_i) {
      for (unsigned int w_idx_j = 0; w_idx_j < num_w_j; ++w_idx_j) {
        unsigned int s_ni = bbox.min_x() + w_idx_i*w_size;
        unsigned int e_ni = bbox.min_x() + (w_idx_i+1)*w_size;
        unsigned int s_nj = bbox.min_y() + w_idx_j*w_size;
        unsigned int e_nj = bbox.min_y() + (w_idx_j+1)*w_size;
        std::vector<float> h_values;
        for (unsigned int i = s_ni; i < e_ni; ++i) {
          for (unsigned int j = s_nj; j < e_nj; ++j) {
            if ( i > bbox.max_x() && j > bbox.max_y() )  // outside the bounding box
              continue;
            if ( c_img(i,j) == volm_osm_category_io::volm_land_table_name["building"].id_)
              h_values.push_back(h_img(i,j));
          }
        }
        // ignore the window without any building pixel inside
        if (h_values.empty())
          continue;
        std::sort(h_values.begin(), h_values.end());
        float median = h_values[h_values.size()/2];
        // refine the window using median height
        for (unsigned i = s_ni; i < e_ni; i++) {
          for (unsigned j = s_nj; j < e_nj; j++) {
            if ( i > bbox.max_x() && j > bbox.max_y() )  continue;
            if ( c_img(i,j) == volm_osm_category_io::volm_land_table_name["building"].id_)
              refined_img(i,j) = median;
          }
        }
      }
    }
  }
  return true;
}

static float neighbor_height(std::map<std::pair<unsigned, unsigned>, float> const& window_height, unsigned const& w_idx_i, unsigned const& w_idx_j, unsigned const& w_size)
{
  bool found_neigh_height = false;

  unsigned num_nbrs = 8;
  std::vector<float> neigh_heights;
  for (int radius = 1; (radius < (int)w_size && !found_neigh_height); radius++)
  {
    neigh_heights.clear();
    int nbrs8_delta[8][2] = { { radius, 0}, { radius,-radius}, { 0,-radius}, {-radius,-radius},
                              {-radius, 0}, {-radius, radius}, { 0, radius}, { radius, radius} };
    for (unsigned c = 0; c < num_nbrs; c++)
    {
      int nbr_i = (int)w_idx_i + nbrs8_delta[c][0];
      int nbr_j = (int)w_idx_j + nbrs8_delta[c][1];
      auto mit = window_height.find(std::pair<int,int>(nbr_i, nbr_j));
      if (mit != window_height.end()) {
        neigh_heights.push_back(mit->second);
      }
    }
    if (neigh_heights.size() != 0)
      found_neigh_height = true;
  }

  if (neigh_heights.empty())
    return 0.0f;

  float grd_height = 0.0f;
  for (float neigh_height : neigh_heights)
    grd_height += neigh_height;

  return grd_height/neigh_heights.size();

}

static bool refine_building(float const& ratio,
                            float const& height_threshold,
                            vil_image_view<float> const& h_img,
                            vil_image_view<vxl_byte> & refined_c_img,
                            vil_image_view<float>& refined_h_img)
{
  // refine the building by checking its first nearest neighbor
  int ni = refined_h_img.ni();
  int nj = refined_h_img.nj();
  if (ni != (int)refined_c_img.ni() || nj != (int)refined_c_img.nj())
    return false;
  // define the first nearest neighbor list
  int nbrs8_delta[8][2] = { { 1, 0}, { 1,-1}, { 0,-1}, {-1,-1},
                            {-1, 0}, {-1, 1}, { 0, 1}, { 1, 1} };
  unsigned num_nbrs = 8;
  for (int i = 0; i < ni; i++) {
    for (int j = 0; j < nj; j++) {
      // ignore the tree and palm tree pixels
      if (refined_c_img(i,j) == volm_osm_category_io::volm_land_table_name["palm_tree"].id_ || refined_c_img(i,j) == volm_osm_category_io::volm_land_table_name["tree"].id_)
        continue;
      // no need to correct building pixel
      if (refined_c_img(i,j) == volm_osm_category_io::volm_land_table_name["building"].id_)
        continue;
      std::vector<float> neigh_heights;
      // obtain the height values of its neighbor pixel which has building land class from refined land class map
      for (unsigned c = 0; c < num_nbrs; c++) {
        int nbr_i = i + nbrs8_delta[c][0];
        int nbr_j = j + nbrs8_delta[c][1];
        if (nbr_i >= 0 && nbr_j >=0 && nbr_i < ni && nbr_j < nj)
          if (refined_c_img(nbr_i,nbr_j) == volm_osm_category_io::volm_land_table_name["building"].id_)
            neigh_heights.push_back(refined_h_img(nbr_i,nbr_j));
      }
      if (neigh_heights.empty())
        continue;

      // obtain the most duplicated height and number of its occurrence
      float curr_height;
      float curr_ratio;
      if (neigh_heights.size() == 1) {
        curr_height = neigh_heights[0];
        curr_ratio = 1.0f/(float)(num_nbrs);
      }
      else {
        std::sort(neigh_heights.begin(), neigh_heights.end());
        std::vector<float> repeat;
        std::vector<unsigned> occurance;
        for (unsigned n_idx = 0; n_idx < neigh_heights.size()-1; n_idx++) {
          if (float_equal(neigh_heights[n_idx], neigh_heights[n_idx+1]))
          {
            unsigned cnt = 1;
            repeat.push_back(neigh_heights[n_idx]);
            while ( (n_idx < neigh_heights.size()-1) && float_equal(neigh_heights[n_idx], neigh_heights[n_idx+1]) ){
              n_idx++;  cnt++;
            }
            occurance.push_back(cnt);
          }
        }
        if (repeat.size() != 0) {
          std::map<unsigned, float> occ_h_map;
          for (unsigned r_idx = 0; r_idx < repeat.size(); r_idx++)
            occ_h_map.insert(std::pair<unsigned, float>(occurance[r_idx], repeat[r_idx]));
          auto mit = occ_h_map.end();
          mit--;
          curr_height = mit->second;
          curr_ratio  = (float)mit->first / (float)(num_nbrs);
        }
        else {
          curr_height = 0.0f;
          curr_ratio  = 0.0f;
        }
      }
#if 0
      if (i == 427 && j == 391) {
        std::cout << " --------- " << std::endl;
        std::cout << "i = " << i << " j = " << j << " height = " << curr_height << " ratio = " << curr_ratio
                 << " origin height = " << h_img(i,j) << std::endl;
      }
#endif
      // update the pixel by checking whether there are enough building neighbor pixels having same height
      if (curr_ratio > ratio) {
        if ( (curr_height-h_img(i,j))*(curr_height-h_img(i,j)) < height_threshold*height_threshold ) {
          refined_h_img(i,j) = curr_height;
          refined_c_img(i,j) = volm_osm_category_io::volm_land_table_name["building"].id_;
        }
      }

    }
  }
  return true;
}
