//:
// \file
// \brief executable to classify all the pixels in a single lidar image, 
//        finds all the aerial images which overlap this image to create filter responses 
//        run this exe in parallel to classify all lidar images
// \author Ozge C. Ozcanli
// \date Mar 01, 2013

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_loc_hyp.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_convert.h>
#include <vpgl/vpgl_utm.h>
#include <vpgl/vpgl_lvcs.h>
#include <bkml/bkml_parser.h>
#include <vul/vul_timer.h>
#include <vgl/vgl_intersection.h>
#include <bkml/bkml_write.h>
#include <volm/volm_geo_index.h>

#include <sdet/sdet_texture_classifier.h>
#include <sdet/sdet_texture_classifier_params.h>

unsigned int northing = 0;  // WARNING: north hard-coded 

struct img_info {
public:
  bool intersects(vgl_polygon<double> poly) { return vgl_intersection(bbox, poly); }
  bool intersects(vgl_box_2d<double> other) { return vgl_intersection(bbox, other).area() > 0; }
  void save_box_kml(vcl_string out_name) { 
    vcl_ofstream ofs(out_name.c_str());
    bkml_write::open_document(ofs);
    bkml_write::write_box(ofs, name, "", bbox);
    bkml_write::close_document(ofs);
  }

  unsigned ni, nj;
  vpgl_geo_camera* cam;   // in UTM for NAIP imgs
  vgl_box_2d<double> bbox;   // in lon (x), lat (y).  lower left corner is (0,nj), upper right corner is (ni, 0)
  vcl_string name;
  vcl_string img_name;
};

bool read_box(vcl_string bbox_file, vgl_box_2d<double>& bbox) {
  char buffer[1000];
  vcl_ifstream ifs(bbox_file);
  if (!ifs.is_open()) {
    vcl_cerr << " cannot open: " << bbox_file << "!\n";
    return false;
  }

  vcl_string dummy; double top_lat, bottom_lat, left_lon, right_lon;
  for (unsigned kk = 0; kk < 22; kk++)
    ifs.getline(buffer, 1000);
  // top
  ifs.getline(buffer, 1000);
  vcl_stringstream top_edge_line(buffer);
  top_edge_line >> dummy; top_edge_line >> dummy; top_edge_line >> dummy; 
  top_edge_line >> top_lat;
  // bottom
  ifs.getline(buffer, 1000);
  vcl_stringstream bot_edge_line(buffer);
  bot_edge_line >> dummy; bot_edge_line >> dummy; bot_edge_line >> dummy; 
  bot_edge_line >> bottom_lat;
  // left
  ifs.getline(buffer, 1000);
  vcl_stringstream left_edge_line(buffer);
  left_edge_line >> dummy; left_edge_line >> dummy; left_edge_line >> dummy; 
  left_edge_line >> left_lon;
  // right
  ifs.getline(buffer, 1000);
  vcl_stringstream right_edge_line(buffer);
  right_edge_line >> dummy; right_edge_line >> dummy; right_edge_line >> dummy; 
  right_edge_line >> right_lon;

  vgl_point_2d<double> lower_left(left_lon, bottom_lat);
  vgl_point_2d<double> upper_right(right_lon, top_lat);
  bbox = vgl_box_2d<double>(lower_left, upper_right);
  //vcl_cout << "bbox: " << bbox << vcl_endl;
  return true;
}

bool load_naip_img(vcl_string const& img_folder, vcl_string const& name, vpgl_lvcs_sptr& lvcs, img_info& info, bool load_resource = false)
{
  vcl_string filename = img_folder + "\\" + name;
  vcl_string img_name = filename + "\\" + name + ".tif";
  vcl_string tfw_name = filename + "\\" + name + ".tfw";
  if (!vul_file::exists(tfw_name) || !vul_file::exists(img_name)) 
    return false;
    
  info.name = name; info.img_name = img_name;
  vcl_string bbox_file = filename + "\\output_parameters.txt";
  if (!read_box(bbox_file, info.bbox)) {
    vcl_cerr << " cannot find: " << bbox_file << vcl_endl;
    return false;
  }
  //vcl_cout << "NAIP bbox: " << info.bbox << vcl_endl;
  // figure out utm zone
  vpgl_utm utm; int utm_zone, zone_max; double xx, yy;
  utm.transform(info.bbox.min_point().y(), info.bbox.min_point().x(), xx, yy, utm_zone); 
  utm.transform(info.bbox.max_point().y(), info.bbox.max_point().x(), xx, yy, zone_max); 
  if (utm_zone != zone_max) {
    vcl_cout << "!!!!!!!!!!!!!!!!!!!!!!!!!WARNING! img: " << img_name << " has min and max points in different UTM zones, using zone of min point!\n";

  }

  vpgl_geo_camera *cam = 0;
  if (!vpgl_geo_camera::init_geo_camera(tfw_name, lvcs, utm_zone, northing, cam))
    return false;    
  info.cam = cam;

  if (load_resource) {
    vil_image_resource_sptr img = vil_load_image_resource(img_name.c_str());
    vcl_cout << "ni: " << img->ni() <<" nj: " << img->nj() <<vcl_endl;
    info.ni = img->ni(); info.nj = img->nj(); 
  }
  return true;
}

bool load_naip_imgs(vcl_string const& img_folder, vcl_vector<img_info>& imgs, bool load_resource = false) {
 
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs; // just the default, no concept of local coordinate system here, so won't be used

  vcl_string in_dir = img_folder + "*";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    vcl_string filename = fn();
    //vcl_cout << "filename: " << filename << vcl_endl;
    vcl_string file = vul_file::strip_directory(filename);
    
    img_info info;
    if (load_naip_img(img_folder, file, lvcs, info, load_resource))
      imgs.push_back(info);
  }
  return true;
}

int load_lidar_img(vcl_string img_file, img_info& info) {
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs; // just the default, no concept of local coordinate system here, so won't be used
  
  vil_image_view_base_sptr img_sptr = vil_load(img_file.c_str());
  info.ni = img_sptr->ni(); info.nj = img_sptr->nj(); 
  info.name = vul_file::strip_directory(vul_file::strip_extension(img_file)); 
  info.img_name = img_file;

  vpgl_geo_camera *cam;
  vpgl_geo_camera::init_geo_camera(img_file, info.ni, info.nj, lvcs, cam);
  info.cam = cam; 
    
  double lat, lon;
  cam->img_to_global(0.0, info.nj-1, lon, lat);
  vgl_point_2d<double> lower_left(-lon, lat);

  vpgl_utm utm; int utm_zone; double x,y;
  utm.transform(lat, -lon, x, y, utm_zone);
  vcl_cout << " zone of lidar img: " << img_file << ": " << utm_zone << " from lower left corner!\n";

  cam->img_to_global(info.ni-1, 0.0, lon, lat);
  vgl_point_2d<double> upper_right(-lon, lat);
  vgl_box_2d<double> bbox(lower_left, upper_right);
  //vcl_cout << "bbox: " << bbox << vcl_endl;
  info.bbox = bbox;

  return utm_zone;
}
void load_lidar_imgs(vcl_string const& folder, vcl_vector<img_info>& infos) {
  vcl_string in_dir = folder + "*.tif";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    vcl_string filename = fn();
    img_info info;
    load_lidar_img(filename, info);
    infos.push_back(info);
  }
}

// no need to check cause get data checks it anyway
void get_block(int i, int j, int bb, vcl_vector<vcl_pair<int, int> >& pixels)
{
  for (int ii = i-bb; ii < i+bb; ii++)
    for (int jj = j-bb; jj < j+bb; jj++) {
      pixels.push_back(vcl_pair<int, int>(ii, jj));
    }
}

// read the tiles of the region, create a geo index and write the hyps
int main(int argc,  char** argv)
{
  vul_timer t;
  t.mark();

  vul_arg<vcl_string> img_folder("-imgs", "input folder to read image files", "");
  vul_arg<vcl_string> lidar_file("-lidar", "input file to read", "");
  vul_arg<float> lidar_max("-max", "max lidar value to stretch lidar images", 80.0f);
  vul_arg<vcl_string> filter_folder("-filter_dir", "folder to read/write filter responses", "");
  vul_arg<vcl_string> out_folder("-out", "folder to write output to", "");
  vul_arg<int> neighborhood_size("-n", "size of the neighborhood around a pixel to collect training examples, in pixels, e.g. 5", 5);
  vul_arg<vcl_string> test("-test", "load the specified classifier instance and classify pixels in images in imgs folder specified in test_imgs.txt file", "");
  vul_arg<int> block("-block", "size of the block around a pixel to compute its set of filter vectors", 5);
  vul_arg_parse(argc, argv);
  vcl_cout << "argc: " << argc << vcl_endl;

  if (filter_folder().compare("") == 0) {
    vcl_cout << " filter folder is not specified! exiting..\n"; return 0;
  }

  vcl_vector<img_info> imgs;
  load_naip_imgs(img_folder(), imgs);

  img_info lidar_info;
  int utm_zone = load_lidar_img(lidar_file(), lidar_info);
  lidar_info.save_box_kml(out_folder() + lidar_info.name + "_box.kml");

  vpgl_lvcs_sptr lvcs;
  if (utm_zone == 17)
     lvcs = new vpgl_lvcs(30.371923, -81.539398, -8.0, vpgl_lvcs::utm, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  else if (utm_zone == 18)
    lvcs = new vpgl_lvcs(35.289477, -75.570596, -8.0, vpgl_lvcs::utm, vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  // find the images that it intersects
  vcl_vector<img_info> intersection_imgs;
  vcl_cout << "checking intersection of bbox: " << lidar_info.bbox << vcl_endl;
  for (unsigned ii = 0; ii < imgs.size(); ii++) {
    //vcl_cout << " \t with bbox: " << imgs[ii].bbox << vcl_endl;
    if (lidar_info.intersects(imgs[ii].bbox)) {
      img_info info;
      load_naip_img(img_folder(), imgs[ii].name, lvcs, info, true);
      intersection_imgs.push_back(info);
      vcl_cout << "intersects: " << imgs[ii].name << vcl_endl;
    }
  }
  vcl_cout << " !!!!!! lidar intersects: " << intersection_imgs.size() << " imgs!\n";
  
  vcl_string name = vul_file::strip_extension(test()) + "_dict.vsl";
    
  sdet_texture_classifier_params dummy;
  sdet_texture_classifier tc(dummy);
  tc.load_dictionary(name);  // loads the params as well
  unsigned ntextons = tc.get_number_of_textons();
  vcl_cout << " testing using the dictionary: " << name << " with the number of textons: " << ntextons << "\n categories:\n";

  vcl_vector<vcl_string> cats = tc.get_training_categories();
  vcl_map<vcl_string, vil_rgb<vxl_byte> > cat_color_map;
  for (unsigned kk = 0; kk < cats.size(); kk++) {
    unsigned char id = volm_label_table::get_id_closest_name(cats[kk]);
    cat_color_map[cats[kk]] = volm_label_table::land_id[id].color_;
    vcl_cout << "\t\t" << cats[kk] << " closest land type: " << volm_label_table::land_id[id].name_ << " color: " << volm_label_table::land_id[id].color_ << '\n';
    vcl_cout.flush();
  }
    
  // process lidar img
  sdet_texture_classifier lidar_c((sdet_texture_classifier_params)tc);
  if (!lidar_c.compute_filter_bank_float_img(filter_folder(), lidar_file(), lidar_max())) {
    vcl_cerr << " problems in computing filters of: " << lidar_file() << vcl_endl;
    return 0;
  }

  // compute filters of intersection imgs
  vcl_vector<sdet_texture_classifier*> intersection_imgs_naip_c;
  for (unsigned ii = 0; ii < intersection_imgs.size(); ii++) {
    // process naip img
    sdet_texture_classifier* naip_c = new sdet_texture_classifier((sdet_texture_classifier_params)tc);
    if (!naip_c->compute_filter_bank_color_img(filter_folder(), intersection_imgs[ii].img_name)) {
      vcl_cerr << " problems in computing filters of: " << intersection_imgs[ii].img_name << vcl_endl;
      return 0;
    }
    intersection_imgs_naip_c.push_back(naip_c);
  }

  vil_image_view<float> lidar_orig = vil_load(lidar_file().c_str());

  vil_image_view<float> out(lidar_info.ni, lidar_info.nj);
  vil_image_view<vil_rgb<vxl_byte> > out_rgb(out.ni(), out.nj());
  out.fill(0); out_rgb.fill(vil_rgb<vxl_byte>(0,0,0));

  int bb = block();
  unsigned larger_neigh = bb+neighborhood_size();
  double lon, lat, lx, ly, lz, u, v;

  vcl_cout << "lidar ni: " << lidar_info.ni << " nj: " << lidar_info.nj << "\n";

  // find the local coords of each pixel to retrieve corresponding img pixels
  for (int i = bb+1; i < lidar_info.ni-bb; i++) {
    for (int j = bb+1; j < lidar_info.nj-bb; j++) {

      if (lidar_orig(i,j) <= 0)
        continue;

      lidar_info.cam->img_to_global(i, j, lon, lat);

      // generate a block of pixels around lidar
      vcl_vector<vcl_pair<int, int> > l_pixels;
      get_block(i, j, bb, l_pixels);
      vcl_vector<vnl_vector<double> > data;
      lidar_c.compute_data(l_pixels, data);
          
      for (unsigned kk = 0; kk < intersection_imgs.size(); kk++) {
        int img_ni = intersection_imgs[kk].ni; 
        int img_nj = intersection_imgs[kk].nj;
    
        //lvcs->global_to_local(-lon, lat, 0, vpgl_lvcs::wgs84, lx, ly, lz);
        //intersection_imgs[kk].cam->project(lx, ly, 0.0, u, v);
        intersection_imgs[kk].cam->global_to_img(-lon, lat, 0, u, v);
        int uu = (int)vcl_floor(u + 0.5); 
        int vv = (int)vcl_floor(v + 0.5);
        if (uu >= 0 && vv >= 0 && uu < img_ni && vv < img_nj) {  // there is a correspondence
          //vcl_cout << i << ",j "; vcl_cout.flush();
          // now around img
          vcl_vector<vcl_pair<int, int> > img_pixels;
          get_block(uu, vv, larger_neigh, img_pixels); // enlarge the neighborhood to account for misregistration 
            
          // collect data from both images
          vcl_vector<vnl_vector<double> > data2;
          intersection_imgs_naip_c[kk]->compute_data(img_pixels, data2);
            
          // create texton histogram and classify, use the same weight for all samples put into the histogram
          float weight = 1.0f/ (data.size() + data2.size());
          vcl_vector<float> hist(ntextons, 0.0f);
          tc.update_hist(data, weight, hist);
          tc.update_hist(data2, weight, hist);
          vcl_pair<vcl_string, float> hc = tc.highest_prob_class(hist);
          out_rgb(i,j) = cat_color_map[hc.first];  
          out(i,j) = hc.second;
          break;  
        }
      }
      
    }
    //vcl_cout << i << " ";
  }
  vcl_cout << '\n';
  
  vcl_string out_name = out_folder() + lidar_info.name + "_outmap";
  vil_save(out, (out_name + ".tif").c_str());
  vil_save(out_rgb, (out_name + ".png").c_str());

  vcl_cout << "total time: " << t.all()/1000 << " seconds = " << t.all()/(1000*60) << " mins.\n";
  return volm_io::SUCCESS;
}

