//:
// \file
// \brief executable to generate hypothesis locations using DEMs or LIDAR
// \author Ozge C. Ozcanli
// \date Nov 15, 2012

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_loc_hyp.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_utm.h>
#include <vpgl/vpgl_lvcs.h>
#include <bkml/bkml_parser.h>
#include <vul/vul_timer.h>

inline float next_mult_2(float val)
{
  return (float)(2.0f*vcl_ceil(val*0.5f));
}

#if 0  // region 1
// generate gt hypos
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<unsigned> inc_i("-inc_i", "increments in pixels in the DEM image", 1);
  vul_arg<unsigned> inc_j("-inc_j", "increments in pixels in the DEM image", 1);
  vul_arg<vcl_string> out_pre("-out_pre", "output file prefix", "");
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  if (out_pre().compare("") == 0 || in_poly().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << "will use increments " << inc_i() << " pixels along i and " << inc_j() << " pixels along j in the DEM image!\n";
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(38.074326, -117.654869, 1200.0, vpgl_lvcs::utm, 0.0, 0.0, vpgl_lvcs::DEG, vpgl_lvcs::METERS, 0.0, 0.0, 0.0);


  //vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());

  double x[] = {-118.21f, -117.102f, -117.102f, -118.21f, -118.21f};
  double y[] = {38.553f, 38.553f, 37.623f, 37.623f, 38.553f};
  vgl_polygon<double> poly(x, y, 5);

  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs();

  volm_loc_hyp hyp;

  // add region 1 gt positions
  hyp.add(37.6763153076, -117.978889465, 1891.40698242);
  hyp.add(37.6763076782, -117.978881836, 1891.40698242);
  hyp.add(37.9629554749, -117.964019775, 1670.87597656);
  hyp.add(37.9623947144, -117.962753296, 1643.76293945);
  hyp.add(38.5223121643, -118.147850037, 1382.84594727);
  hyp.add(38.5202674866, -118.147697449, 1378.42004395);
  hyp.add(38.0597877502, -117.936508179, 1376.12097168);
  hyp.add(38.0598144531, -117.936523438, 1376.12097168);
  hyp.add(37.7081604004, -117.416557312, 1860.39599609);
  hyp.add(37.7084999084, -117.416664124, 1863.23205566);
  hyp.add(37.7082748413, -117.416511536, 1860.99194336);

  //vcl_string data_folder = "I:/";
  vcl_string data_folder = "/home/ozge/mnt/fileserver/projects/FINDER/";
  vcl_string dem_file = data_folder + "P-1A/Finder_Program_Only/ReferenceData/Desert/NAMDTM/NAM_DEM.tif";
  vil_image_resource_sptr dem_res = vil_load_image_resource(dem_file.c_str());
  vil_image_view<float> dem = dem_res->get_view();
  vpgl_geo_camera* geocam = 0;
  vpgl_geo_camera::init_geo_camera(dem_res, lvcs, geocam);

  hyp.add(poly, dem, geocam, inc_i(), inc_j());

  vcl_cout << "after addition hyp has: " << hyp.size() << " elements\n";

  vcl_stringstream str;
  str << out_pre() << '_' << inc_i() << '_' << inc_j() << ".bin";
  hyp.write_hypotheses(str.str());

  return volm_io::SUCCESS;
}
#endif

#if 0
// region 2 gt only
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<vcl_string> out_pre("-out_pre", "output file prefix", "");
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  if (out_pre().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs();

  volm_loc_hyp hyp;

  // add region 2 gt positions
  // note to self, the order here is not necessarily the order given by get_next() method of hyp
  // increase elev by 1 meter
  hyp.add(33.912397,-78.281430,next_mult_2(1.60));  // p1a_res12_trashcan-GROUNDTRUTH
  hyp.add(33.912068,-78.282300,next_mult_2(1.60)); // p1a_res13_shakypier-GROUNDTRUTH
  hyp.add(34.370855,-77.627093,next_mult_2(1.60)); // p1a_res14_boat-GROUNDTRUTH.kml
  hyp.add(34.368807,-77.624855,next_mult_2(3.61)); // p1a_res15_walkway-GROUNDTRUTH.kml
  hyp.add(34.370836,-77.627113,next_mult_2(1.60)); // p1a_res16_whitehouse-GROUNDTRUTH
  hyp.add(32.650674,-79.948996,next_mult_2(3.00)); // p1a_res17_beachgrass-GROUNDTRUTH.kml
  hyp.add(32.648747,-79.952434,next_mult_2(1.40)); // p1a_res18_rocks-GROUNDTRUTH.kml
  hyp.add(36.116348,-75.741750,next_mult_2(1.60)); // p1a_res19_channelfront-GROUNDTRUTH.kml
  hyp.add(36.126512,-75.743331,next_mult_2(1.60)); // p1a_res20_drivingbyboats-GROUNDTRUTH.kml
  hyp.add(32.375492,-80.436054,next_mult_2(1.60)); // p1a_res21_360spin-GROUNDTRUTH.kml
  hyp.add(32.375502,-80.437492,next_mult_2(40.30)); // p1a_res22_lookingsouth-GROUNDTRUTH.kml

  vcl_stringstream str;
  str << out_pre() << ".bin";
  hyp.write_hypotheses(str.str());

  return volm_io::SUCCESS;
}
#endif

# if 0 // adding region 2 using LIDAR, go over each lidar tile and for each pixel add a hypo
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> in_folder("-in", "input folder to read files as .tif", "");
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<unsigned> inc_i("-inc_i", "increments in pixels in the DEM image", 1);
  vul_arg<unsigned> inc_j("-inc_j", "increments in pixels in the DEM image", 1);
  vul_arg<vcl_string> out_pre("-out_pre", "output file prefix", "");
  vul_arg<bool> add_gt("-addgt", "add known gt locations?", false); // if no -addgt argument then the value is false, if there is one then the value is true
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  if (in_folder().compare("") == 0 || out_pre().compare("") == 0 || in_poly().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << "will use increments " << inc_i() << " pixels along i and " << inc_j() << " pixels along j in the DEM image!\n";

  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs();

  volm_loc_hyp hyp;

  vcl_string file_glob = in_folder() + "/*.tif";
  for (vul_file_iterator fn=file_glob; fn; ++fn) {
    vcl_string tiff_fname = fn();
    vcl_cout << tiff_fname << vcl_endl;

    vil_image_view_base_sptr img_sptr = vil_load(tiff_fname.c_str());
    vil_image_view<float> img(img_sptr);
    unsigned ni = img.ni(); unsigned nj = img.nj();
    vcl_cout << " tile size: "<< ni << " x " << nj << vcl_endl;

    // determine the translation matrix from the image file name and construct a geo camera
    vcl_string name = vul_file::strip_directory(tiff_fname);
    name = name.substr(name.find_first_of('_')+1, name.size());
    vcl_cout << "will determine transformation matrix from the file name: " << name << vcl_endl;
    vcl_string n = name.substr(name.find_first_of('N')+1, name.find_first_of('W'));
    float lon, lat, scale;
    vcl_stringstream str(n); str >> lat;
    n = name.substr(name.find_first_of('W')+1, name.find_first_of('_'));
    vcl_stringstream str2(n); str2 >> lon;
    n = name.substr(name.find_first_of('x')+1, name.find_last_of('.'));
    vcl_stringstream str3(n); str3 >> scale;
    vcl_cout << " lat: " << lat << " lon: " << lon << " WARNING: using same scale for both ni and nj: scale:" << scale << vcl_endl;

    // determine the upper left corner to use a vpgl_geo_cam, subtract from lat
    vcl_cout << "upper left corner in the image is: " << lat+scale << " N " << lon << " W\n"
             << "lower right corner in the image is: " << lat << " N " << lon-scale << " W" << vcl_endl;
    vnl_matrix<double> trans_matrix(4,4,0.0);
    trans_matrix[0][0] = -scale/ni; trans_matrix[1][1] = -scale/nj;
    trans_matrix[0][3] = lon; trans_matrix[1][3] = lat+scale;
    vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, dummy_lvcs);
    cam->set_scale_format(true);
    double lon2, lat2;
    cam->img_to_global(ni, nj, lon2, lat2);
    vpgl_utm utm; double x, y; int zone; utm.transform(lat2, -lon2, x, y, zone);
    vcl_cout << "lower right corner in the image given by geocam is: " << lat2 << " N " << lon2 << " W " << " zone: " << zone << vcl_endl;

    hyp.add(poly, img, cam, inc_i(), inc_j(), true, 'N', 'W');
    delete cam;
    vcl_cout << "after addition hyp has: " << hyp.size() << " elements\n";
  }
  if (add_gt()) {
    //: add any gt positions if any
    hyp.add(32.759063, -79.857689, 1.60);  // p1a_test1_06-GROUNDTRUTH
    hyp.add(32.775959, -79.813014, 4.41);  // p1a_test1_28-GROUNDTRUTH
    hyp.add(33.911997, -78.282153, 1.60);  // p1a_test1_08-GROUNDTRUTH
    hyp.add(33.912397, -78.281430, 1.60);  // p1a_test1_46-GROUNDTRUTH
    hyp.add(30.720336, -81.550366, 1.60);  // p1a_test1_18-GROUNDTRUTH
    hyp.add(32.648980, -79.951930, 1.60);  // p1a_test1_38-GROUNDTRUTH
    hyp.add(33.365799, -79.268871, 1.60);  // p1a_test1_34-GROUNDTRUTH

    vcl_cout << "after addition of some gt points hyp has: " << hyp.size() << " elements\n";
  }
  vcl_stringstream str;
  str << out_pre() << '_' << inc_i() << '_' << inc_j() << ".bin";
  hyp.write_hypotheses(str.str());

  return volm_io::SUCCESS;
}

#endif

#include <volm/volm_geo_index.h>

// read the tiles of the region, create a geo index and write the hyps
int main(int argc,  char** argv)
{
  vul_timer t;
  t.mark();

  vul_arg<vcl_string> in_folder("-in", "input folder to read files as .tif", "");
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<float> inc("-inc", "increments in arcseconds, e.g. 0.1 for ~3m increments", 0.1f);
  vul_arg<unsigned> nh("-nh", "number of hyps in each direction in one leaf tile, e.g. 100 so that each tile has 100x100 hyps", 100);
  vul_arg<vcl_string> out_pre("-out_pre", "output file folder with file separator at the end", "");
  vul_arg<vcl_string> add_gt("-addgt", "add known gt locations? pass the name of txt file containing gt locations", ""); // if no -addgt argument then the value is false, if there is one then the value is true
  vul_arg<bool> only_gt("-onlygt", "add only known gt locations", false);
  vul_arg<unsigned> tile_id("-tile", "id of the tile", 0);
  vul_arg<unsigned> utm_zone("-zone", "utm zone to fill", 17);
  vul_arg<bool> read("-read", "if passed only read the index in the out_pre() folder and report some statistics", false);
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;

  // change to wr1 tiles for desert
  vcl_vector<volm_tile> tiles = volm_tile::generate_p1_wr2_tiles();
  vcl_cout << " number of tiles: " << tiles.size() << vcl_endl;
  unsigned i = tile_id();
  if (i >= tiles.size()) {
    vcl_cerr << "tile id: " << i << " is greater than number of tiles: " << tiles.size() << "!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  if (read()) {
    if (in_folder().compare("") == 0) {
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    float min_s;
    vcl_stringstream file_name; file_name << out_pre() << "geo_index_tile_" << i;
    volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name.str() + ".txt", min_s);
    vcl_vector<volm_geo_index_node_sptr> leaves2;
    volm_geo_index::get_leaves(root, leaves2);
    vcl_cout << "\t number of leaves: " << leaves2.size() << vcl_endl;
    volm_geo_index::read_hyps(root, file_name.str());
    vcl_cout << " read hyps!\n";
    vcl_vector<volm_geo_index_node_sptr> leaves;
    volm_geo_index::get_leaves_with_hyps(root, leaves);
    vcl_cout << "Geo index for tile: " << i << " stored in: " << file_name.str() << '\n'
             << "\t number of leaves with hyps: " << leaves.size() << vcl_endl;
    unsigned size = volm_geo_index::hypo_size(root);
    vcl_cout << "\t total number of hypos: " << size << vcl_endl;
    return volm_io::SUCCESS;
  }

  if (in_folder().compare("") == 0 || out_pre().compare("") == 0 || in_poly().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << "will use increments " << inc() << " arcseconds along north and east directions!\n";

  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  // determine depth of the geo index depending on inc, if we want to have 100x100 = 10K hyps in each leaf
  double arcsec_to_sec = 1.0f/3600.0f;
  double inc_in_sec = inc()*arcsec_to_sec;
  double inc_in_sec_rad = 3.0*inc_in_sec/4.0; // radius to search for existence of before adding a new one
  double size = nh()*inc_in_sec; // inc() is given in arcseconds, convert it to seconds;
  vcl_cout << " each leaf has size: " << size << " seconds in geographic coords..\n"
           << " increments in seconds: " << inc_in_sec << '\n'
           << " increments in meters: " << (inc_in_sec*21/0.000202) << '\n'
           << " only putting hyps in UTM zone: " << utm_zone() << '\n';


  volm_geo_index_node_sptr root = volm_geo_index::construct_tree(tiles[i], (float)size, poly);

  // write the geo index and the hyps
  vcl_stringstream file_name; file_name << out_pre() << "geo_index_tile_" << i << ".txt";
  volm_geo_index::write(root, file_name.str(), (float)size);
  unsigned depth = volm_geo_index::depth(root);
  vcl_stringstream file_name2; file_name2 << out_pre() << "geo_index_tile_" << i << ".kml";
  volm_geo_index::write_to_kml(root, 0, file_name2.str());
  vcl_stringstream file_name3; file_name3 << out_pre() << "geo_index_tile_" << i << "_depth_" << depth << ".kml";
  volm_geo_index::write_to_kml(root, depth, file_name3.str());

  // prune the out of zone leaves
  if (!volm_geo_index::prune_by_zone(root, utm_zone())) {
    vcl_cout << " root " << i << " is not in zone: " << utm_zone() << "! no hypotheses in its leaves!\n";
    return 0;
  }
  if (!only_gt()) {
  vcl_string file_glob = in_folder() + "/*.tif";
  unsigned cnt = 0;
  for (vul_file_iterator fn=file_glob; fn; ++fn, ++cnt) {
    vcl_string tiff_fname = fn();

    vil_image_view_base_sptr img_sptr = vil_load(tiff_fname.c_str());
    vil_image_view<float> img(img_sptr);
    unsigned ni = img.ni(); unsigned nj = img.nj();
    volm_tile t(tiff_fname, ni, nj);
    //t.write_kml(out_pre() + t.get_string() + ".kml", 0);

    // write the geo index and the hyps
    vcl_vector<volm_geo_index_node_sptr> leaves;
    vgl_box_2d<float> leaf_box = t.bbox();
    volm_geo_index::get_leaves(root, leaves, leaf_box);
    if (!leaves.size())
      continue;
    float size_leaf = leaves[0]->extent_.width();
#if 0
    volm_index_node_sptr dummy_root = new volm_index_node(t.bbox());
    dummy_root->children_ = leaves;
    vcl_stringstream file_name; file_name << out_pre() << "geo_index_tile_" << i << "_intersection_" << cnt << ".kml";
    volm_index::write_to_kml(dummy_root, 1, file_name.str());
#endif
    // generate the hyps and find heights from LIDAR
    for (unsigned j = 0; j < leaves.size(); ++j) {
      if (!leaves[j]->hyps_)
        leaves[j]->hyps_ = new volm_loc_hyp();
      float lower_left_lon = leaves[j]->extent_.min_point().x();
      float lower_left_lat = leaves[j]->extent_.min_point().y();
      unsigned nhi = (unsigned)vcl_ceil(size_leaf/inc_in_sec);
      for (unsigned hi = 0; hi < nhi; ++hi) {
        double lon = lower_left_lon + hi*inc_in_sec;
        for (unsigned hj = 0; hj < nhi; ++hj) {
          double lat = lower_left_lat + hj*inc_in_sec;
          vpgl_utm u; int zone;  double x, y;
          u.transform(lat, lon, x, y, zone);
          if (zone != (int)utm_zone())
            continue;
          unsigned ii, jj;
          bool contains = t.global_to_img(lon, lat, ii, jj);
          if (contains) {
            float z = img(ii, jj);
            unsigned id;
            if (z > 0  && !(leaves[j]->hyps_->exist(lat, lon, inc_in_sec_rad, id)))
              leaves[j]->hyps_->add(lat, lon, z);
          }
        }
      }
    }

    //if (cnt > 0)
    //  break;
  }
  }
  unsigned r_cnt = volm_geo_index::hypo_size(root) ;
  vcl_cout << " root " << i << " has total " << r_cnt << " hypotheses in its leaves!\n";
 
  if (add_gt().compare("") != 0) {  // user passed the path to a text file with the gt locations

    // load the images
    vcl_string file_glob = in_folder() + "/*.tif";
    vcl_vector<vcl_pair<vil_image_view_base_sptr,  volm_tile > > tiles;
    for (vul_file_iterator fn=file_glob; fn; ++fn) {
      vcl_string tiff_fname = fn();

      vil_image_view_base_sptr img_sptr = vil_load(tiff_fname.c_str());
      unsigned ni = img_sptr->ni(); unsigned nj = img_sptr->nj();
      volm_tile t(tiff_fname, ni, nj);
      tiles.push_back(vcl_pair<vil_image_view_base_sptr, volm_tile>(img_sptr, t));
    }


#if 0    //: add any gt positions if any
    if (volm_geo_index::add_hypothesis(root, -79.857689, 32.759063, 1.60))
      vcl_cout << " added p1a_test1_06-GROUNDTRUTH\n";

    if (volm_geo_index::add_hypothesis(root, -79.813014, 32.775959, 4.41))
      vcl_cout << " added p1a_test1_28-GROUNDTRUTH\n";
    if (volm_geo_index::add_hypothesis(root, -78.282153, 33.911997, 1.60))
      vcl_cout << " added p1a_test1_08-GROUNDTRUTH\n";
    if (volm_geo_index::add_hypothesis(root, -78.281430, 33.912397, 1.60))
      vcl_cout << " added p1a_test1_46-GROUNDTRUTH\n";
    if (volm_geo_index::add_hypothesis(root, -81.550366, 30.720336, 1.60))
      vcl_cout << " added p1a_test1_18-GROUNDTRUTH\n";
    if (volm_geo_index::add_hypothesis(root, -79.951930, 32.648980, 1.60))
      vcl_cout << " added p1a_test1_38-GROUNDTRUTH\n";
    if (volm_geo_index::add_hypothesis(root, -79.268871, 33.365799, 1.60))
      vcl_cout << " added p1a_test1_34-GROUNDTRUTH\n";
#endif

    vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_pair<vcl_string, int>, vcl_string> > > samples;
    int cnt = volm_io::read_gt_file(add_gt(), samples);
    vcl_cout << " adding " << cnt <<" gt locs!\n";
    for (int j = 0; j < cnt; ++j) {
      vpgl_utm u; int zone;  double x, y;
      u.transform(samples[j].first.y(), samples[j].first.x(), x, y, zone);
      if (zone != (int)utm_zone()) {
        vcl_cout << samples[j].second.first.first << " is in zone: " << zone <<" not in " << utm_zone() << " skipping!\n";
        continue;
      }
      vcl_cout << samples[j].second.first.first << " adding.. " << samples[j].first.y() << ", " << samples[j].first.x() << ' ';

      // find which box contains it
      bool added = false;
      for (unsigned kk = 0; kk < tiles.size(); kk++) {
         unsigned ii, jj;
         bool contains = tiles[kk].second.global_to_img(samples[j].first.x(), samples[j].first.y(), ii, jj);
         if (contains) {
          vil_image_view<float> img(tiles[kk].first);
          float z = img(ii, jj);
          // check the neighborhood
          for (int ii2 = ii - 1; ii2 <= ii+1; ii2++)
            for (int jj2 = jj - 1; jj2 <= jj+1; jj2++) {
              if (ii2 >= 0 && jj2 >= 0 && ii2 < img.ni() && jj2 < img.nj()) {
                if (z < img(ii2, jj2)) z = img(ii2, jj2);
              }
            }
          if (z > 0.0f) {
            vcl_cout << " corrected height from: " << samples[j].first.z() << " to: " << z+1.6 << '\n';
            added = volm_geo_index::add_hypothesis(root, samples[j].first.x(), samples[j].first.y(), z+1.6);
          } else {
            vcl_cout << " height from LIDAR is: " << z << " writing original height: " << samples[j].first.z() << '\n';
            added = volm_geo_index::add_hypothesis(root, samples[j].first.x(), samples[j].first.y(), samples[j].first.z());
          }
          break;
         }
      }
      //bool added = volm_geo_index::add_hypothesis(root, samples[j].first.x(), samples[j].first.y(), samples[j].first.z());
      if (added) vcl_cout << " success!\n";
      else       vcl_cout <<" not found in tree of tile: " << tile_id() << "!\n";
    }

    unsigned r_cnt = volm_geo_index::hypo_size(root) ;
    vcl_cout << " after addition of gt locs, root " << i << " has total " << r_cnt << " hypotheses in its leaves!\n";
  }

  // write the hypos
  vcl_stringstream file_name4; file_name4 << out_pre() << "geo_index_tile_" << i;
  vcl_cout << "writing hyps to: " << file_name4.str() << vcl_endl;
  volm_geo_index::write_hyps(root, file_name4.str());
//#if DEBUG
  vcl_vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);
  vcl_stringstream file_name5; file_name5 << out_pre() << "geo_index_tile_" << i << "_hyps.kml";
  leaves[0]->hyps_->write_to_kml(file_name5.str(), inc_in_sec);
//#endif

  vcl_cout << "total time: " << t.all()/1000 << " seconds = " << t.all()/(1000*60) << " mins.\n";
  return volm_io::SUCCESS;
}

