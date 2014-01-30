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

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <volm/volm_io.h>
#include <volm/volm_io_tools.h>
#include <volm/volm_category_io.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_osm_objects.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_region_finder.h>
#include <vcl_algorithm.h>
#include <vul/vul_file_iterator.h>


static void error(vcl_string log_file, vcl_string msg)
{
  vcl_cerr << msg;
  volm_io::write_post_processing_log(log_file, msg);
}

static bool float_equal(float const& a, float const& b)
{
  return ((a-b)*(a-b) < 1E-8);
}

static float average(vcl_vector<float> const& values)
{
  float sum = 0.0f;
  for (unsigned i = 0; i < values.size(); i++)
    sum += values[i];
  return sum/values.size();
}

static bool find_nearest_building(unsigned const& i, unsigned const& j,
                                  int const& radius,
                                  vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > const& buildings,
                                  unsigned & building_id)
{
  bool found = false;
  unsigned num_nbrs = 8;

  vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >::const_iterator mit = buildings.begin();
  for( ; mit != buildings.end(); ++mit)
  {
    vcl_vector<vcl_pair<int, int> > b_pixels;
    unsigned num_pixels = (unsigned)mit->second.second.size();
    for (unsigned p_idx = 0; p_idx < num_pixels; p_idx++)
      b_pixels.push_back(vcl_pair<int,int>(mit->second.first[p_idx], mit->second.second[p_idx]));

    for (int r_idx = 1; r_idx < radius; r_idx++)
    {
      int nbrs8_delta[8][2] = { { r_idx, 0}, { r_idx,-r_idx}, { 0,-r_idx}, {-r_idx,-r_idx},
                                {-r_idx, 0}, {-r_idx, r_idx}, { 0, r_idx}, { r_idx, r_idx} };
      for (unsigned c = 0; c < num_nbrs; c++) {
        int nbr_i = i + nbrs8_delta[c][0];
        int nbr_j = j + nbrs8_delta[c][1];
        if (vcl_find(b_pixels.begin(), b_pixels.end(), vcl_pair<int,int>(nbr_i, nbr_j)) != b_pixels.end())
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

static float standard_devation(vcl_vector<float> const& values, float const& mean)
{
  float dev_sum = 0.0f;
  for (unsigned i = 0; i <values.size(); i++)
    dev_sum += (values[i] - mean) * (values[i] - mean);
  return vcl_sqrt(dev_sum/values.size());
}

//: obtain the building pixels inside a window from the classification image (forward declaration)
static bool obtain_buildings(vil_image_view<vxl_byte> const& c_img,
                             unsigned const& start_ni, unsigned const& start_nj,
                             unsigned const& end_ni,   unsigned const& end_nj,
                             vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >& buildings,
                             vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >& flat_regions,
                             vcl_vector<vcl_pair<unsigned, unsigned> >& non_buildings);

static bool refine_building_by_median(vil_image_view<float> const& h_img,
                                      vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > const& buildings,
                                      vcl_map<unsigned, vcl_vector<float> >& building_height,
                                      vil_image_view<float>& refined_img);

//static bool refine_building_by_height(vil_image_view<float> const& h_img,
//                                      vil_image_view<vxl_byte> const& c_img,
//                                      int const& radius,
//                                      vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > const& buildings,
//                                      vcl_map<unsigned, vcl_vector<float> > const& building_height,
//                                      vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >& flat_regions,
//                                      vcl_vector<vcl_pair<unsigned, unsigned> >& non_buildings,
//                                      vil_image_view<float>& refined_img);

static bool refine_building(float const& ratio,
                            float const& height_threshold,
                            vil_image_view<float> const& h_img,
                            vil_image_view<vxl_byte> & refined_c_img,
                            vil_image_view<float>& refined_h_img);

static float neighbor_height(vcl_map<vcl_pair<unsigned, unsigned>, float> const& window_height,
                             unsigned const& w_idx_i,
                             unsigned const& w_idx_j,
                             unsigned const& w_size);


#if 1
// refine satellite height map using building footprint obtained both from satellite classification map and OSM buildings/roads
// the ground height here is defined by road obtained from osm
int main(int argc, char** argv)
{
  vul_arg<vcl_string> height_img_name("-height", "height map about to be refined", "");
  vul_arg<vcl_string> height_cam_name("-height-cam", "tfw camera for height map", "");
  vul_arg<vcl_string> out_height("-out-height", "output filename of the refined height map", "");
  vul_arg<vcl_string> out_class("-out-class", "output filename of the refined class map", "");
  vul_arg<unsigned> dx("-x", "refined window size in pixel unit", 60);
  vul_arg<unsigned> dy("-y", "refined window size in pixel unit", 60);
  vul_arg<unsigned> iteration("-iter","number of refinement iteration", 100);
  vul_arg<float> ratio("-ratio", "shape factor to control morphological operation", 0.5);
  vul_arg<float> height_threshold("-thres", "threshold in height difference", 5);
  vul_arg<float> tall_building_thres("-tall","height threshold for tall building category",1000);
  vul_arg<vcl_string> class_map_folder("-2d-class-root", "folder where all generated 2d map stored","");
  vul_arg<vcl_string> out_class_color("-out-class-color","output color class map","");
  vul_arg_parse(argc, argv);


  if (height_img_name().compare("") == 0 || height_cam_name().compare("") == 0 ||
      out_height().compare("") == 0 || out_class().compare("") == 0 || class_map_folder().compare("") == 0)
  {
    vul_arg_display_usage_and_exit();
    return false;
  }
  vcl_stringstream log_file;
  vcl_stringstream log;
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

  vcl_cout << "Start refined the height map..." << vcl_endl;
  vcl_cout << "  img size: " << ni << 'x' << nj << vcl_endl;
  vcl_cout << "  window size: " << dx() << 'x' << dy() << vcl_endl;
  vcl_cout << "  window num: " << num_w_i << 'x' << num_w_j << vcl_endl;

  // read all the geo_index tree for class map
  double class_map_min_size;
  vcl_vector<vcl_string> class_map_texts;
  vcl_string file_glob = class_map_folder() + "/p1b_wr*_tile*.txt";
  vcl_vector<volm_geo_index2_node_sptr> class_roots;
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn) {
    volm_geo_index2_node_sptr class_root = volm_geo_index2::read_and_construct<volm_osm_object_ids_sptr>(fn(), class_map_min_size);
    class_roots.push_back(class_root);
    vcl_cout << " geo index is created from file " << fn() << vcl_endl;
  }

  // find the overlap resources
  vcl_vector<volm_geo_index2_node_sptr> class_leaves;
  double h_lowerleft_lon, h_lowerleft_lat;
  h_cam->img_to_global(0, nj-1, h_lowerleft_lon, h_lowerleft_lat);
  double h_upperrght_lon, h_upperrght_lat;
  h_cam->img_to_global(ni-1, 0, h_upperrght_lon, h_upperrght_lat);
  vgl_box_2d<double> h_bbox(h_lowerleft_lon, h_upperrght_lon, h_lowerleft_lat, h_upperrght_lat);
  for (unsigned r_idx = 0; r_idx < class_roots.size(); r_idx++) {
    vcl_vector<volm_geo_index2_node_sptr> leaves;
    leaves.clear();
    volm_geo_index2::get_leaves(class_roots[r_idx], leaves, h_bbox);
    for (unsigned li = 0; li < leaves.size(); li++)
      class_leaves.push_back(leaves[li]);
  }

  if (class_leaves.empty()) {
    log << "ERROR: can not find any overlapped 2d class map for current height map: " << height_img_name() << " -- " << h_bbox << '\n';
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  
  unsigned num_leaves = class_leaves.size();
  vcl_cout << "there are " << num_leaves << " class maps intersect with height map " << h_bbox << vcl_endl;
  for (unsigned l_idx = 0; l_idx < num_leaves; l_idx++)
    vcl_cout <<"\t " << class_leaves[l_idx]->get_string() << " --- " << class_leaves[l_idx]->extent_ << vcl_endl;

  // load the associated image resource
  vcl_map<vcl_string, vil_image_view<vxl_byte> > class_img_map;
  vcl_map<vcl_string, vpgl_geo_camera*> class_cam_map;
  for (unsigned l_idx = 0; l_idx < num_leaves; l_idx++) {
    double lon_min, lat_min, lon_max, lat_max;
    volm_geo_index2_node_sptr leaf = class_leaves[l_idx];
    lon_min = leaf->extent_.min_x();  lat_min = leaf->extent_.min_y();
    lon_max = leaf->extent_.max_x();  lat_max = leaf->extent_.max_y();
    double scale_x = lon_max-lon_min;
    double scale_y = lat_max-lat_min;
    vcl_string hemisphere, direction;
    if (lon_min < 0)  direction  = "W";
    else              direction  = "E";
    if (lat_min < 0)  hemisphere = "S";
    else              hemisphere = "N";
    vcl_stringstream img_name;
    img_name << class_map_folder() << "/Sat2dMap_" << hemisphere << vcl_setprecision(12) << lat_min
                                   << direction << vcl_setprecision(12) << lon_min
                                   << "_S" << scale_x << 'x' << scale_y << ".tif";
    vcl_stringstream cam_name;
    cam_name << class_map_folder() << "/Sat2dMap_" << hemisphere << vcl_setprecision(12) << lat_min
                                   << direction << vcl_setprecision(12) << lon_min
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
    vil_image_view<vxl_byte>* img_src = dynamic_cast<vil_image_view<vxl_byte>*>(info.img_r.ptr());
    class_img_map.insert(vcl_pair<vcl_string, vil_image_view<vxl_byte> >(leaf->get_string(), *img_src));
    class_cam_map.insert(vcl_pair<vcl_string, vpgl_geo_camera*>(leaf->get_string(), info.cam));
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
        unsigned uu = (unsigned)vcl_floor(u+0.5);
        unsigned vv = (unsigned)vcl_floor(v+0.5);
        if (uu > 0 && vv > 0 && uu < class_img_map[class_leaves[l_idx]->get_string()].ni() && vv < class_img_map[class_leaves[l_idx]->get_string()].nj())
          c_img(i,j) = (class_img_map[class_leaves[l_idx]->get_string()])(uu,vv);
      }
    }
  }

  // start to refine the building
  vil_image_view<vxl_byte> refined_c_img;
  refined_c_img.deep_copy(c_img);
  vcl_string refined_c_img_name = out_class();
  vil_image_view<float> refined_h_img(ni, nj);
  refined_h_img.deep_copy(h_img);
  vcl_string refined_h_img_name = out_height();

#if 0
  vcl_string c_img_before_refine = vul_file::strip_extension(refined_c_img_name) + "_cropped.tif";
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
  vcl_map<vcl_pair<unsigned, unsigned>, float> window_min_height;
  window_min_height.clear();

  // map to record the non-building pixels of each window
  vcl_map<vcl_pair<unsigned, unsigned>, vcl_vector<vcl_pair<unsigned, unsigned> > > non_building_regions;

  // record all the building median heights in the height image
  vcl_vector<float> all_buidling_heights;

  vcl_cout << "Start to refine buildings";
  for (unsigned w_idx_i = 0; w_idx_i < num_w_i; w_idx_i++)
  {
    unsigned start_ni, end_ni;
    start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
    for (unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++)
    {
      vcl_cout << '.';
      vcl_cout.flush();
      vcl_pair<unsigned,unsigned> window_key(w_idx_i, w_idx_j);
      // obtain window pixels
      unsigned start_nj, end_nj;
      start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
      start_nj = w_idx_j*dy(); end_nj = (w_idx_j+1)*dy();
      // obtain building pixels, road pixels and non-building pixels (key is a unique value from upper left of the pixel)
      vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > buildings;
      vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > flat_regions;
      vcl_vector<vcl_pair<unsigned, unsigned> > non_buildings;
      if (!obtain_buildings(c_img, start_ni, start_nj, end_ni, end_nj, buildings, flat_regions, non_buildings)) {
        log << "error: building extraction failed for window (" << start_ni << 'x' << start_nj << ") (to " << end_ni << 'x' << end_nj << ")\n";
        error(log_file.str(), log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }

      // flatten the building height by their median
      // map for storing building_height info for each building (first is median, second is mean, third is standard deviation)
      vcl_map<unsigned, vcl_vector<float> > building_height;
      if (!refine_building_by_median(h_img, buildings, building_height, refined_h_img))
      {
        log << "error: refine height image using median height failed\n";
        error(log_file.str(), log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }
      // store the building height for current window
      for (vcl_map<unsigned, vcl_vector<float> >::iterator bh_mit = building_height.begin(); bh_mit != building_height.end(); ++bh_mit)
        all_buidling_heights.push_back(bh_mit->second[0]);

      vcl_pair<vcl_pair<unsigned, unsigned>, vcl_vector<vcl_pair<unsigned, unsigned> > > tmp_pair(vcl_pair<unsigned, unsigned>(w_idx_i, w_idx_j), non_buildings);
      non_building_regions.insert(tmp_pair);

      // obtain the ground height from flat region -- using the minimum height for current window
      vcl_vector<float> flat_heights;
      for (vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >::iterator mit = flat_regions.begin();
           mit != flat_regions.end(); ++mit)
      {
        unsigned num_pts = (unsigned)mit->second.first.size();
        for (unsigned ri = 0; ri < num_pts; ri++)
          flat_heights.push_back(h_img(mit->second.first[ri], mit->second.second[ri]));
      }
      if (flat_heights.size() != 0) {
        vcl_sort(flat_heights.begin(), flat_heights.end());
        //float height_median = flat_heights[flat_heights.size()/2];
        float height_median = flat_heights[0];
        window_min_height.insert(vcl_pair<vcl_pair<unsigned, unsigned>, float>(vcl_pair<unsigned, unsigned>(w_idx_i, w_idx_j), height_median));
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
  vcl_cout << "\nStart to refine ground elevation";
  for (unsigned w_idx_i = 0; w_idx_i < num_w_i; w_idx_i++)
  {
    unsigned start_ni, end_ni;
    start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
    for (unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++)
    {
      vcl_cout << '.';
      vcl_cout.flush();
      // obtain window pixels
      unsigned start_nj, end_nj;
      start_nj = w_idx_j*dy(); end_nj = (w_idx_j+1)*dy();

      // obtain the window height
      float grd_height = 0.0f;
      vcl_map<vcl_pair<unsigned, unsigned>, float>::iterator mit = window_min_height.find(vcl_pair<unsigned,unsigned>(w_idx_i, w_idx_j));
      if (mit == window_min_height.end())
        // search neighbors to obtain height
        grd_height = neighbor_height(window_min_height, w_idx_i, w_idx_j, window_size);
      else
        grd_height = mit->second;
      // refine the ground height
      vcl_map<vcl_pair<unsigned,unsigned>, vcl_vector<vcl_pair<unsigned, unsigned> > >::iterator r_mit = non_building_regions.find(vcl_pair<unsigned,unsigned>(w_idx_i, w_idx_j));
      if (r_mit != non_building_regions.end()) {
        vcl_vector<vcl_pair<unsigned, unsigned> > region_pixels = r_mit ->second;
        for (unsigned pidx = 0; pidx < region_pixels.size(); pidx++) {
          refined_h_img(region_pixels[pidx].first,region_pixels[pidx].second) = grd_height;
        }
      }
    }
  }

  // refine the building boundary using morphological operation (class map will be updated here to accept new building pixels)
  // height threshold used to divide neighbor buildings having different height
  vcl_cout << "\nStart to refine building boundary";
  for (unsigned i = 0; i < iteration(); i++) {
    vcl_cout << '.';
    vcl_cout.flush();
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
  vcl_cout << vcl_endl;

  // re-check class image
  for (unsigned w_idx_i = 0; w_idx_i < num_w_i; w_idx_i++)
  {
    unsigned start_ni, end_ni;
    start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
    for (unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++)
    {
      vcl_cout << '.';
      vcl_cout.flush();
      vcl_pair<unsigned,unsigned> window_key(w_idx_i, w_idx_j);
      // obtain window pixels
      unsigned start_nj, end_nj;
      start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
      start_nj = w_idx_j*dy(); end_nj = (w_idx_j+1)*dy();
      // obtain the ground height for current window
      float grd_height = 0.0f;
      vcl_map<vcl_pair<unsigned, unsigned>, float>::iterator mit = window_min_height.find(vcl_pair<unsigned,unsigned>(w_idx_i, w_idx_j));
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
    start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
    for (unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++)
    {
      vcl_cout << '.';
      vcl_cout.flush();
      vcl_pair<unsigned,unsigned> window_key(w_idx_i, w_idx_j);
      // obtain window pixels
      unsigned start_nj, end_nj;
      start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
      start_nj = w_idx_j*dy(); end_nj = (w_idx_j+1)*dy();
      // obtain the ground height for current window
      float grd_height = 0.0f;
      vcl_map<vcl_pair<unsigned, unsigned>, float>::iterator mit = window_min_height.find(vcl_pair<unsigned,unsigned>(w_idx_i, w_idx_j));
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
  vcl_string out_folder = vul_file::dirname(refined_c_img_name);
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


#if 0
// refine satellite height map using original satellite classification map
int main(int argc, char** argv)
{
  vul_arg<vcl_string> height_img_name("-height", "height map about to be refined", "");
  vul_arg<vcl_string> class_img_name("-class", "classification map used to refine", "");
  vul_arg<vcl_string> out_height("-out-height", "output filename of the refined height map", "");
  vul_arg<vcl_string> out_class("-out-class", "output filename of the refined class map", "");
  vul_arg<unsigned> dx("-x", "refined window size in pixel unit", 60);
  vul_arg<unsigned> dy("-y", "refined window size in pixel unit", 60);
  vul_arg<unsigned> iteration("-iter","number of refinement iteration", 100);
  vul_arg<float> ratio("-ratio", "shape factor to control morphological operation", 0.5);
  vul_arg<float> height_threshold("-thres", "threshold in height difference", 5);
  vul_arg_parse(argc, argv);

  // check input
  if (height_img_name().compare("") == 0 || class_img_name().compare("") == 0 || out_height().compare("") == 0 || out_class().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return false;
  }
  vcl_stringstream log_file;
  vcl_stringstream log;
  log_file << out_height() << "/log_refine_height_map_scene.xml";

  // load height map image (float format)
  if (!vul_file::exists(height_img_name())) {
    log << "error: can not find height map image: " << height_img_name() << '\n';
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vil_image_view<float> h_img = vil_load(height_img_name().c_str());
  // load classification map (unsigned char)
  if (!vul_file::exists(class_img_name())) {
    log << "error: can not find classification image: " << class_img_name() << '\n';
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vil_image_view<vxl_byte> c_img = vil_load(class_img_name().c_str());
  // image size needs to be same
  if (h_img.ni() != c_img.ni() || h_img.nj() != c_img.nj()) {
    log << "error: height map and classification map have different size" << '\n';
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  unsigned ni = h_img.ni();
  unsigned nj = h_img.nj();
  unsigned num_w_i = ni/dx() + 1;
  unsigned num_w_j = nj/dx() + 1;

  vcl_cout << "Start refined the height map..." << vcl_endl;
  vcl_cout << "  img size: " << ni << 'x' << nj << vcl_endl;
  vcl_cout << "  window size: " << dx() << 'x' << dy() << vcl_endl;
  vcl_cout << "  window num: " << num_w_i << 'x' << num_w_j << vcl_endl;

  // start to refine the height map on each window
  // refined height map
  vil_image_view<float> refined_h_img(ni, nj);
  refined_h_img.deep_copy(h_img);
  vcl_string refined_h_img_name = out_height();

  vil_image_view<vxl_byte> refined_c_img;
  refined_c_img.deep_copy(c_img);
  vcl_string refined_c_img_name = out_class();

#if 0
  // save a window image for debugging purpose
  vil_image_view<vil_rgb<vxl_byte> > w_img(ni, nj, 1);
  vcl_string w_img_name = out_height() + "/w_img.tif";
  w_img.fill(0);
#endif

  // maps to record the ground height of each window (key is its window id)
  vcl_map<vcl_pair<unsigned, unsigned>, float> window_min_height;
  window_min_height.clear();

  // map to record the non-building pixels of each window
  vcl_map<vcl_pair<unsigned, unsigned>, vcl_vector<vcl_pair<unsigned, unsigned> > > non_building_regions;

  // record all the building median heights in the height image
  vcl_vector<float> all_buidling_heights;

  vcl_cout << "Start to refine buildings";
  for (unsigned w_idx_i = 0; w_idx_i < num_w_i; w_idx_i++)
  {
    unsigned start_ni, end_ni;
    start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
    for (unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++)
    {
      vcl_cout << '.';
      vcl_cout.flush();
      vcl_pair<unsigned,unsigned> window_key(w_idx_i, w_idx_j);
      // obtain window pixels
      unsigned start_nj, end_nj;
      start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
      start_nj = w_idx_j*dy(); end_nj = (w_idx_j+1)*dy();
      // obtain building pixels, road pixels and non-building pixels (key is a unique value from upper left of the pixel)
      vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > buildings;
      vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > flat_regions;
      vcl_vector<vcl_pair<unsigned, unsigned> > non_buildings;
      if (!obtain_buildings(c_img, start_ni, start_nj, end_ni, end_nj, buildings, flat_regions, non_buildings)) {
        log << "error: building extraction failed for window (" << start_ni << 'x' << start_nj << ") (to " << end_ni << 'x' << end_nj << ")\n";
        error(log_file.str(), log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }

      // flatten the building height by their median
      // map for storing building_height info for each building (first is median, second is mean, third is standard deviation)
      vcl_map<unsigned, vcl_vector<float> > building_height;
      if (!refine_building_by_median(h_img, buildings, building_height, refined_h_img))
      {
        log << "error: refine height image using median height failed\n";
        error(log_file.str(), log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }
      // store the building height for current window
      for (vcl_map<unsigned, vcl_vector<float> >::iterator bh_mit = building_height.begin(); bh_mit != building_height.end(); ++bh_mit)
        all_buidling_heights.push_back(bh_mit->second[0]);

      vcl_pair<vcl_pair<unsigned, unsigned>, vcl_vector<vcl_pair<unsigned, unsigned> > > tmp_pair(vcl_pair<unsigned, unsigned>(w_idx_i, w_idx_j), non_buildings);
      non_building_regions.insert(tmp_pair);

      // obtain the ground height from flat region -- using the minimum height for current window
      vcl_vector<float> flat_heights;
      for (vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >::iterator mit = flat_regions.begin();
           mit != flat_regions.end(); ++mit)
      {
        unsigned num_pts = (unsigned)mit->second.first.size();
        for (unsigned ri = 0; ri < num_pts; ri++)
          flat_heights.push_back(h_img(mit->second.first[ri], mit->second.second[ri]));
      }
      if (flat_heights.size() != 0) {
        vcl_sort(flat_heights.begin(), flat_heights.end());
        //float height_median = flat_heights[flat_heights.size()/2];
        float height_median = flat_heights[0];
        window_min_height.insert(vcl_pair<vcl_pair<unsigned, unsigned>, float>(vcl_pair<unsigned, unsigned>(w_idx_i, w_idx_j), height_median));
      }

#if 0
      // create a window image for debugging purpose
      for (vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >::iterator mit = buildings.begin();
           mit != buildings.end(); ++mit)
      {
        if (mit->second.first.size() < 10)
          continue;
        for (unsigned b_idx = 0; b_idx < mit->second.first.size(); b_idx++) {
          unsigned w_tmp_i = mit->second.first[b_idx];
          unsigned w_tmp_j = mit->second.second[b_idx];
          w_img(mit->second.first[b_idx], mit->second.second[b_idx]) = vil_rgb<vxl_byte>(255,0,0);
        }
      }
      for (vcl_vector<vcl_pair<unsigned, unsigned> >::iterator vit = non_buildings.begin();  vit != non_buildings.end(); ++vit)
      {
        w_img(vit->first, vit->second) = vil_rgb<vxl_byte>(0,255,0);
      }
      for (vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >::iterator mit = flat_regions.begin();
           mit != flat_regions.end(); ++mit)
      {
        if (mit->second.first.size() < 10)
          continue;
        for (unsigned b_idx = 0; b_idx < mit->second.first.size(); b_idx++) {
          unsigned w_tmp_i = mit->second.first[b_idx];
          unsigned w_tmp_j = mit->second.second[b_idx];
          w_img(mit->second.first[b_idx], mit->second.second[b_idx]) = vil_rgb<vxl_byte>(0,0,255);
        }
      }
      
#endif
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
  vcl_cout << "\nStart to refine ground elevation";
  for (unsigned w_idx_i = 0; w_idx_i < num_w_i; w_idx_i++)
  {
    unsigned start_ni, end_ni;
    start_ni = w_idx_i*dx(); end_ni = (w_idx_i+1)*dx();
    for (unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++)
    {
      vcl_cout << '.';
      vcl_cout.flush();
      // obtain window pixels
      unsigned start_nj, end_nj;
      start_nj = w_idx_j*dy(); end_nj = (w_idx_j+1)*dy();

      // obtain the window height
      float grd_height = 0.0f;
      vcl_map<vcl_pair<unsigned, unsigned>, float>::iterator mit = window_min_height.find(vcl_pair<unsigned,unsigned>(w_idx_i, w_idx_j));
      if (mit == window_min_height.end())
        // search neighbors to obtain height
        grd_height = neighbor_height(window_min_height, w_idx_i, w_idx_j, window_size);
      else
        grd_height = mit->second;
      // refine the ground height
#if 0
      for (unsigned g_idx_i = start_ni; g_idx_i < end_ni; g_idx_i++) {
        for (unsigned g_idx_j = start_nj; g_idx_j < end_nj; g_idx_j++) {
          if (g_idx_i >= 0 && g_idx_i < ni && g_idx_j >= 0 && g_idx_j < nj) {

            if (g_idx_i == 147 && g_idx_j == 73) {
              vcl_cout << " pixel (" << g_idx_i << "x" << g_idx_j << ") = "
                << volm_osm_category_io::volm_land_table[c_img(g_idx_i, g_idx_j)].name_ << " "
                << c_img(g_idx_i, g_idx_j) << " ground height = " << grd_height << vcl_endl;
            }

            if (c_img(g_idx_i, g_idx_j) == volm_osm_category_io::volm_land_table_name["roads"].id_ ||
                c_img(g_idx_i, g_idx_j) == volm_osm_category_io::volm_land_table_name["parks"].id_ ||
                c_img(g_idx_i, g_idx_j) == volm_osm_category_io::volm_land_table_name["parking"].id_ ||
                c_img(g_idx_i, g_idx_j) == volm_osm_category_io::volm_land_table_name["palm_tree"].id_ ||
                c_img(g_idx_i, g_idx_j) == volm_osm_category_io::volm_land_table_name["tree"].id_ ||
                c_img(g_idx_i, g_idx_j) == 0)
              refined_h_img(g_idx_i, g_idx_j) = grd_height;
          }
        }
      }
#endif
#if 1
      vcl_map<vcl_pair<unsigned,unsigned>, vcl_vector<vcl_pair<unsigned, unsigned> > >::iterator r_mit = non_building_regions.find(vcl_pair<unsigned,unsigned>(w_idx_i, w_idx_j));
      if (r_mit != non_building_regions.end()) {
        vcl_vector<vcl_pair<unsigned, unsigned> > region_pixels = r_mit ->second;
        for (unsigned pidx = 0; pidx < region_pixels.size(); pidx++) {
          refined_h_img(region_pixels[pidx].first,region_pixels[pidx].second) = grd_height;
        }
      }
#endif
    }
  }

  // refine the building boundary using morphological operation (class map will be updated here to accept new building pixels)
  // height threshold used to divide neighbor buildings having different height
  vcl_cout << "\nStart to refine building boundary";
  for (unsigned i = 0; i < iteration(); i++) {
    vcl_cout << '.';
    vcl_cout.flush();
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
  vcl_cout << vcl_endl;
  vil_save(refined_c_img, refined_c_img_name.c_str());

  // output the image
  vil_save(refined_h_img, refined_h_img_name.c_str());

  return volm_io::SUCCESS;
}
#endif

bool obtain_buildings(vil_image_view<vxl_byte> const& c_img,
                      unsigned const& start_ni, unsigned const& start_nj,
                      unsigned const& end_ni,   unsigned const& end_nj,
                      vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >& buildings,
                      vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >& flat_regions,
                      vcl_vector<vcl_pair<unsigned, unsigned> >& non_buildings)
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
      unsigned img_cat = c_img(i,j);
      if (c_img(i,j) == volm_osm_category_io::volm_land_table_name["building"].id_)
      {
        vcl_vector<unsigned> ri;  vcl_vector<unsigned> rj;
        ri.resize(0);  rj.resize(0);
        region_finder.same_int_region(i, j, ri, rj);
        //ignore regions that are smaller than 10 pixels
        if (ri.size() >= 10) {
          unsigned key = (i+j)*(i+j+1)/2 + j;
          vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > tmp(ri, rj);
          buildings.insert(vcl_pair<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >(key, tmp));
        }
      }
      else if (c_img(i,j) == volm_osm_category_io::volm_land_table_name["roads"].id_)
      {
        vcl_vector<unsigned> ri;  vcl_vector<unsigned> rj;
        ri.resize(0);  rj.resize(0);
        region_finder.same_int_region(i, j, ri, rj);
        //ignore regions that are smaller than 10 pixels
        if (ri.size()) {
          unsigned key = (i+j)*(i+j+1)/2 + j;
          vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > tmp(ri, rj);
          flat_regions.insert(vcl_pair<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >(key, tmp));
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
      for (vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >::iterator mit = buildings.begin();
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
        non_buildings.push_back(vcl_pair<unsigned, unsigned>(i,j));
    }
  }
  return true;
}

bool refine_building_by_median(vil_image_view<float> const& h_img,
                               vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > const& buildings,
                               vcl_map<unsigned, vcl_vector<float> >& building_height,
                               vil_image_view<float>& refined_img)
{
  // refine each building with its median height
  for (vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >::const_iterator mit = buildings.begin();
       mit != buildings.end(); ++mit)
  {
    if (mit->second.first.size() < 10)
      continue;
    unsigned building_key = mit->first;
    unsigned num_pts = (unsigned)mit->second.first.size();
    // find the median
    vcl_vector<float> height_values;
    for (unsigned p_idx = 0; p_idx < num_pts; p_idx++)
      height_values.push_back(h_img(mit->second.first[p_idx], mit->second.second[p_idx]));
    vcl_sort(height_values.begin(), height_values.end());
    float median = height_values[height_values.size()/2];

    // refine the height using median
    for (unsigned p_idx = 0; p_idx < num_pts; p_idx++) {
      unsigned img_i = mit->second.first[p_idx];
      unsigned img_j = mit->second.second[p_idx];
      refined_img(mit->second.first[p_idx], mit->second.second[p_idx]) = median;
    }

    // compute the mean height and the std
    float mean = average(height_values);
    float variance = standard_devation(height_values, mean);
    vcl_vector<float> height_data;
    height_data.push_back(median);
    height_data.push_back(mean);
    height_data.push_back(variance);
    building_height.insert(vcl_pair<unsigned, vcl_vector<float> >(building_key, height_data)); 
  }

#if 0
  // refine ground using road median
  // find the road median
  vcl_vector<float> road_heights;
  for (vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >::const_iterator mit = roads.begin();
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
  vcl_sort(road_heights.begin(), road_heights.end());
  float road_median = road_heights[road_heights.size()/2];
  // fill the road median
  unsigned num_grd_pixel = non_buildings.size();
  for (unsigned gi = 0; gi < num_grd_pixel; gi++) {
    refined_img(non_buildings[gi].first, non_buildings[gi].second) = road_median;
  }
#endif

  return true;
}


float neighbor_height(vcl_map<vcl_pair<unsigned, unsigned>, float> const& window_height, unsigned const& w_idx_i, unsigned const& w_idx_j, unsigned const& w_size)
{
  bool found_neigh_height = false;
  
  unsigned num_nbrs = 8;
  vcl_vector<float> neigh_heights;
  for (int radius = 1; (radius < (int)w_size && !found_neigh_height); radius++)
  {
    neigh_heights.clear();
    int nbrs8_delta[8][2] = { { radius, 0}, { radius,-radius}, { 0,-radius}, {-radius,-radius},
                              {-radius, 0}, {-radius, radius}, { 0, radius}, { radius, radius} };
    for (unsigned c = 0; c < num_nbrs; c++)
    {
      int nbr_i = (int)w_idx_i + nbrs8_delta[c][0];
      int nbr_j = (int)w_idx_j + nbrs8_delta[c][1];
      vcl_map<vcl_pair<unsigned, unsigned>, float>::const_iterator mit = window_height.find(vcl_pair<int,int>(nbr_i, nbr_j));
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
  for (unsigned i = 0; i < neigh_heights.size(); i++)
    grd_height += neigh_heights[i];

  return grd_height/neigh_heights.size();

}

bool refine_building(float const& ratio,
                     float const& height_threshold,
                     vil_image_view<float> const& h_img,
                     vil_image_view<vxl_byte> & refined_c_img,
                     vil_image_view<float>& refined_h_img)
{
  // refine the building by checking its first nearest neighbor
  int ni = refined_h_img.ni();
  int nj = refined_h_img.nj();
  if (ni != refined_c_img.ni() || nj != refined_c_img.nj())
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
      if (refined_c_img(i,j) == volm_osm_category_io::volm_land_table_name["building"].id_)
        continue;
      vcl_vector<float> neigh_heights;
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
        vcl_sort(neigh_heights.begin(), neigh_heights.end());
        vcl_vector<float> repeat;
        vcl_vector<unsigned> occurance;
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
          vcl_map<unsigned, float> occ_h_map;
          for (unsigned r_idx = 0; r_idx < repeat.size(); r_idx++)
            occ_h_map.insert(vcl_pair<unsigned, float>(occurance[r_idx], repeat[r_idx]));
          vcl_map<unsigned, float>::iterator mit = occ_h_map.end();
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
        vcl_cout << " --------- " << vcl_endl;
        vcl_cout << "i = " << i << " j = " << j << " height = " << curr_height << " ratio = " << curr_ratio
                 << " origin height = " << h_img(i,j) << vcl_endl;
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