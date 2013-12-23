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
#include <volm/volm_category_io.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_region_finder.h>
#include <vcl_algorithm.h>


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
    unsigned num_pixels = mit->second.second.size();
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

int main(int argc, char** argv)
{
  vul_arg<vcl_string> height_img_name("-height", "height map about to be refined",
                                      "V:/satellite_modeling/wr2/satellite_height_map/scene_11278/height_map_ortho_epoc_2.tif");
  vul_arg<vcl_string> class_img_name("-class", "classification map used to refine", 
                                     "V:/satellite_modeling/wr2/satellite_height_map/scene_11278/crops/crop_5V121005P0012161044B222005301092M_000977701_class_3_scales_ang_int_15_band_3_2_1_0_neigh_7_ids_ortho.tif");
  vul_arg<vcl_string> out("-out", "output folder where height map will be stored", "V:/satellite_modeling/wr2/satellite_height_map/scene_11278/");
  vul_arg<unsigned> dx("-x", "refined window size in pixel unit", 60);
  vul_arg<unsigned> dy("-y", "refined window size in pixel unit", 60);
  vul_arg<unsigned> iteration("-iter","range of the pixel neighbors", 1);
  vul_arg<float> ratio("-ratio", "shape factor to control morphological operation", 0.5);
  vul_arg<float> height_threshold("-thres", "threshold in height difference", 5);
  vul_arg_parse(argc, argv);

  // check input
  if (height_img_name().compare("") == 0 || class_img_name().compare("") == 0 || out().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return false;
  }
  vcl_stringstream log_file;
  vcl_stringstream log;
  log_file << out() << "/log_refine_height_map.xml";

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
  vcl_stringstream str;
  str << out() << "/refined_height_img_median_iter_" << iteration() << "_ratio_" << ratio() << ".tif";
  vcl_string refined_h_img_name = str.str();
  refined_h_img.deep_copy(h_img);

  // save a window image for debugging purpose
  //vil_image_view<vxl_byte> w_img(ni, nj);
  vil_image_view<vil_rgb<vxl_byte> > w_img(ni, nj, 1);
  vcl_string w_img_name = out() + "/w_img.tif";
  w_img.fill(0);

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

# if 0
      // refine the height map by the median height value in each region
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
#endif

      vcl_pair<vcl_pair<unsigned, unsigned>, vcl_vector<vcl_pair<unsigned, unsigned> > > tmp_pair(vcl_pair<unsigned, unsigned>(w_idx_i, w_idx_j), non_buildings);
      non_building_regions.insert(tmp_pair);

      // obtain the ground height from flat region
      vcl_vector<float> flat_heights;
      for (vcl_map<unsigned, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >::iterator mit = flat_regions.begin();
           mit != flat_regions.end(); ++mit)
      {
        unsigned num_pts = mit->second.first.size();
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
  if (window_min_height.empty()) {
    log << "warning: no road/park is available to refine ground plane height, ground refinement ignored\n";
    error(log_file.str(), log.str());
    vil_save(w_img, w_img_name.c_str());
    vil_save(refined_h_img, refined_h_img_name.c_str());
    return volm_io::SUCCESS;
  }

  // ingest ground level height using height median retrieved from road and park region
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
      // only refine the road and parks
      for (unsigned g_idx_i = start_ni; g_idx_i < end_ni; g_idx_i++) {
        for (unsigned g_idx_j = start_nj; g_idx_j < end_nj; g_idx_j++) {
          if (g_idx_i >= 0 && g_idx_i < ni && g_idx_j >= 0 && g_idx_j < nj) {
            if (c_img(g_idx_i, g_idx_j) == volm_osm_category_io::volm_land_table_name["roads"].id_ ||
                c_img(g_idx_i, g_idx_j) == volm_osm_category_io::volm_land_table_name["parks"].id_ ||
                c_img(g_idx_i, g_idx_j) == volm_osm_category_io::volm_land_table_name["palm_tree"].id_ ||
                c_img(g_idx_i, g_idx_j) == volm_osm_category_io::volm_land_table_name["tree"].id_ ||
                c_img(g_idx_i, g_idx_j) == 0)
              refined_h_img(g_idx_i, g_idx_j) = grd_height;
          }
        }
      }
#if 0
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

#if 0
  // refine the building boundary using morphological operation
  vil_image_view<vxl_byte> refined_c_img;
  refined_c_img.deep_copy(c_img);
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

  // refine once again with high ratio and no height threshold
  if(!refine_building(0.6, 1000, h_img, refined_c_img, refined_h_img)) {
    log << "error: refining building failed at ratio " << ratio() << '\n';
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << vcl_endl;
  vcl_string refined_c_img_name = out() + "/refined_buiding_img.tif";
  vil_save(refined_c_img, refined_c_img_name.c_str());
#endif

  // output the image
  //vil_save(w_img, w_img_name.c_str());
  vil_save(refined_h_img, refined_h_img_name.c_str());

  return volm_io::SUCCESS;
}


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
      else if (c_img(i,j) == volm_osm_category_io::volm_land_table_name["roads"].id_ ||
               c_img(i,j) == volm_osm_category_io::volm_land_table_name["parks"].id_)
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
    unsigned num_pts = mit->second.first.size();
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
      if (i == 427 && j == 391) {
        vcl_cout << " --------- " << vcl_endl;
        vcl_cout << "i = " << i << " j = " << j << " height = " << curr_height << " ratio = " << curr_ratio
                 << " origin height = " << h_img(i,j) << vcl_endl;
      }
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