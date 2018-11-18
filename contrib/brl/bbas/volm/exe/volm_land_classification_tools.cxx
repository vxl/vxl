//:
// \file
// \brief executable to run training and classification tools of sdet
// \author Ozge C. Ozcanli
// \date Feb 20, 2013

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
  bool intersects(const vgl_polygon<double>& poly) { return vgl_intersection(bbox, poly); }
  void save_box_kml(const std::string& out_name) {
    std::ofstream ofs(out_name.c_str());
    bkml_write::open_document(ofs);
    bkml_write::write_box(ofs, name, "", bbox);
    bkml_write::close_document(ofs);
  }

  unsigned ni, nj;
  vpgl_geo_camera* cam;   // in UTM for NAIP imgs
  vgl_box_2d<double> bbox;   // in lon (x), lat (y).  lower left corner is (0,nj), upper right corner is (ni, 0)
  std::string name;
  std::string img_name;
};

void load_naip_imgs(const std::string& img_folder, std::vector<img_info>& imgs, int utm_zone) {

  vpgl_lvcs_sptr lvcs = new vpgl_lvcs; // just the default, no concept of local coordinate system here, so won't be used

  std::string in_dir = img_folder + "*";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    std::string filename = fn();
    std::cout << "filename: " << filename << std::endl;
    std::string file = vul_file::strip_directory(filename);
    std::string img_name = filename + "\\" + file + ".tif";
    //std::cout << img_name << "\n" << file << "\n"; std::cout.flush();

    std::string tfw_name = filename + "\\" + file + ".tfw";
    //std::cout << tfw_name << std::endl; std::cout.flush();
    if (!vul_file::exists(tfw_name) || !vul_file::exists(img_name))
      continue;
    vpgl_geo_camera *cam = nullptr;
    if (!vpgl_geo_camera::init_geo_camera(tfw_name, lvcs, utm_zone, northing, cam))
      continue;
    vil_image_resource_sptr img = vil_load_image_resource(img_name.c_str());
    //std::cout << "ni: " << img->ni() <<" nj: " << img->nj() <<std::endl;
    img_info info; info.ni = img->ni(); info.nj = img->nj(); info.cam = cam; info.name = file; info.img_name = img_name;

    double lat, lon;
    cam->img_to_global(0.0, img->nj()-1, lon, lat);
    vgl_point_2d<double> lower_left(lon, lat);
    cam->img_to_global(img->ni()-1, 0.0, lon, lat);
    vgl_point_2d<double> upper_right(lon, lat);
    vgl_box_2d<double> bbox(lower_left, upper_right);
    //std::cout << "bbox: " << bbox << std::endl;
    info.bbox = bbox;
    imgs.push_back(info);
  }
}

void load_lidar_imgs(const std::string& img_folder, std::vector<img_info>& imgs) {
  std::string glob = img_folder + "/*.tif";
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs; // just the default, no concept of local coordinate system here, so won't be used

  for (vul_file_iterator fit = glob;fit; ++fit) {

    vil_image_view_base_sptr img_sptr = vil_load(fit());
    img_info info;
    info.ni = img_sptr->ni(); info.nj = img_sptr->nj();
    info.name = vul_file::strip_directory(vul_file::strip_extension(fit()));
    info.img_name = fit();

    vpgl_geo_camera *cam;
    vpgl_geo_camera::init_geo_camera(fit(), info.ni, info.nj, lvcs, cam);
    info.cam = cam;

    double lat, lon;
    cam->img_to_global(0.0, info.nj-1, lon, lat);
    vgl_point_2d<double> lower_left(-lon, lat);
    cam->img_to_global(info.ni-1, 0.0, lon, lat);
    vgl_point_2d<double> upper_right(-lon, lat);
    vgl_box_2d<double> bbox(lower_left, upper_right);
    //std::cout << "bbox: " << bbox << std::endl;
    info.bbox = bbox;
    imgs.push_back(info);
  }
}

void prepare_site_file(std::string const& site_file, std::vector<std::string >& lidar_full, std::vector<std::string>& lidar_name, std::vector<std::string>& img_full, std::vector<std::string>& img_name) {
  std::string path = vul_file::dirname(site_file);
  std::ofstream ofs(site_file.c_str());
  ofs << "<BWM_SITE name=\"test\">\n"
      << "<home>\n" << path << "\n</home>\n<PyramidExePath>\n</PyramidExePath>\n<Tableaus>\n";
      for (unsigned i = 0; i < lidar_full.size(); i++) {
        ofs << "<ImageTableau name=\"" << lidar_name[i] << "\" status=\"active\">\n"
            << "<imagePath>\n" << lidar_full[i] << "</imagePath>\n</ImageTableau>\n";
      }
      for (unsigned i = 0; i < img_full.size(); i++) {
        ofs << "<ImageTableau name=\"" << img_name[i] << "\" status=\"active\">\n"
            << "<imagePath>\n" << img_full[i] << "</imagePath>\n</ImageTableau>\n";
      }

  ofs << "</Tableaus><Objects></Objects></BWM_SITE>\n";
  ofs.close();
}

void collect_lidar_pixels(const vpgl_lvcs_sptr& lvcs, std::string const& mask_name, std::string const& img_name, std::vector<std::pair<int, int> >& pixels, std::vector<std::pair<double, double> >& local_coords)
{
  vil_image_view<vxl_byte> mask = vil_load(mask_name.c_str());
  vpgl_geo_camera *cam;
  vpgl_geo_camera::init_geo_camera(img_name, mask.ni(), mask.nj(), lvcs, cam);
  double lat, lon;
  cam->img_to_global(0.0, mask.nj()-1, lon, lat);
  vgl_point_2d<double> lower_left(-lon, lat);
  cam->img_to_global(mask.ni()-1, 0.0, lon, lat);
  vgl_point_2d<double> upper_right(-lon, lat);
  vgl_box_2d<double> bbox(lower_left, upper_right);
  std::cout << "for img: " << img_name << " bbox: " << bbox << std::endl;

  for (unsigned i = 0; i < mask.ni(); i++)
    for (unsigned j = 0; j < mask.nj(); j++) {
      if (mask(i,j) == 255) {
        pixels.emplace_back(std::pair<unsigned, unsigned>(i,j));
        double lon, lat, lx, ly, lz;
        cam->img_to_global(i, j, lon, lat);
        lvcs->global_to_local(-lon, lat, 0, vpgl_lvcs::wgs84, lx, ly, lz);
        local_coords.emplace_back(lx, ly);
      }
    }
}

void enlarge_neighborhood(std::vector<std::pair<int, int> >& img_pixels, int ni, int nj, int n_size) {
  std::map<std::pair<int, int>, bool> pixel_map;
  std::map<std::pair<int, int>, bool>::iterator iter;
  for (const auto & img_pixel : img_pixels) {
    // make the previously existing pixels false, so the additional pixels can be added to the end of img_pixels
    pixel_map[img_pixel] = false;
  }
  // add the neighborhood pixels trying to avoid repeats
  for (auto & img_pixel : img_pixels) {
    int ii = img_pixel.first;
    int jj = img_pixel.second;
    // now try adding the neighborhood
    for (int i = ii-n_size; i <= ii + n_size; ++i)
      for (int j = jj - n_size; j <= jj + n_size; ++j) {
        if (i < 0 || j < 0 || i >= ni || j >= nj)
          continue;
        std::pair<int, int> current_p(i, j);
        iter = pixel_map.find(current_p);
        if (iter == pixel_map.end())
          pixel_map[current_p] = true;
      }
  }

  for (iter = pixel_map.begin(); iter != pixel_map.end(); iter++) {
    if (iter->second)
      img_pixels.push_back(iter->first);
  }

}

// use local coords given by lidar pixels to check if there is a corresponding masked img pixel.
bool collect_img_pixels(const vpgl_lvcs_sptr& lvcs, int utm_zone, std::string const& mask_name, std::string const& img_tfw_name, std::vector<std::pair<double, double> >& local_coords, std::vector<std::pair<int, int> >& img_pixels, int n_size)
{
  vil_image_view<vxl_byte> mask = vil_load(mask_name.c_str());

  vpgl_geo_camera *cam = nullptr;
  if (!vpgl_geo_camera::init_geo_camera(img_tfw_name, lvcs, utm_zone, northing, cam))
    return false;
  int ni = mask.ni(); int nj = mask.nj();
  double u,v;
  for (auto & local_coord : local_coords) {
    cam->project(local_coord.first, local_coord.second, 0.0, u, v);
    int uu = (int)std::floor(u + 0.5);
    int vv = (int)std::floor(v + 0.5);
    if (uu >= 0 && vv >= 0 && uu < ni && vv < nj && mask(uu,vv) == 255) {
      img_pixels.emplace_back(uu,vv);
    }
  }
  std::cout << " collected: " << img_pixels.size() << " pixels from aerial image!\n";
  enlarge_neighborhood(img_pixels, ni, nj, n_size);
  std::cout << " collected: " << img_pixels.size() << " pixels from aerial image after neighborhood enlargement by: " << n_size << " x " << n_size << '\n';
  return true;
}

int get_zone(double lon, double lat) {
  vpgl_utm utm; double x, y; int zone;
  utm.transform(lat, lon, x, y, zone);
  return zone;
}
bool in_zone(vgl_polygon<double> poly, int zone) {
  if (get_zone(poly[0][0].x(), poly[0][0].y()) == zone)
    return true;
  else
    return false;
}

bool read_training_img(const std::string& txt_file, std::vector<std::string>& names, std::vector<std::pair<std::string, std::string> >& lidar_imgs, std::vector<std::pair<std::string, std::string> >& imgs)
{
  std::ifstream ifs(txt_file.c_str());
  if (!vul_file::exists(txt_file) || !ifs.is_open()) {

  }
  // there are 5 lines per object in this file
  while (!ifs.eof()) {
    char buffer[10000];
    ifs.getline(buffer, 10000);  // object name  (e.g. there will be <name>.kml in the training folder)
    if (ifs.eof())
      break;
    std::string name(buffer);
    //std::cout << name << std::endl;
    names.push_back(name);
    ifs.getline(buffer, 10000);  // lidar mask name
    std::string mask(buffer);
    //std::cout << mask << std::endl;
    ifs.getline(buffer, 10000);  // lidar name
    std::string iname(buffer);
    //std::cout << iname << std::endl;
    lidar_imgs.emplace_back(iname, mask);
    ifs.getline(buffer, 10000);  // mask name
    std::string imask(buffer);
    //std::cout << imask << std::endl;
    ifs.getline(buffer, 10000);  // name
    std::string iiname(buffer);
    //std::cout << iiname << std::endl;
    imgs.emplace_back(iiname, imask);
  }
  return true;
}

// no need to check cause get data checks it anyway
void get_block(int i, int j, int bb, std::vector<std::pair<int, int> >& pixels)
{
  for (int ii = i-bb; ii < i+bb; ii++)
    for (int jj = j-bb; jj < j+bb; jj++) {
      pixels.emplace_back(ii, jj);
    }
}

// read the tiles of the region, create a geo index and write the hyps
int main(int argc,  char** argv)
{
  vul_timer t;
  t.mark();

  vul_arg<std::string> img_folder("-imgs", "input folder to read image files", "");
  vul_arg<std::string> lidar_folder("-lidar", "input folder to read lidar files", "");
  vul_arg<float> lidar_max("-max", "max lidar value to stretch lidar images", 80.0f);
  vul_arg<std::string> in_poly("-poly", "a folder for a set of polys for the category in question as kml files, to be used for testing or training", "");
  vul_arg<bool> find_imgs("-find_imgs", "find the images which contain polygons", false);
  vul_arg<std::string> out_folder("-out", "folder to write output to", "");
  vul_arg<std::string> filter_folder("-filter_dir", "folder to read/write filter responses", "");
  vul_arg<int> utm_zone("-zone", "utm zone of imagery", -1);
  vul_arg<std::string> class_name("-class", "name of the class, polygons are in kmls named as <name>*.kml", "");
  vul_arg<std::string> train("-train", "load the specified sdet classifier instance and add to its training data using the info in img_folder()", "");
  vul_arg<int> neighborhood_size("-n", "size of the neighborhood around a pixel to collect training examples, in pixels, e.g. 5", 5);
  vul_arg<std::string> compute("-compute", "load the specified classifier instance and compute textons and save the dictionary file", "");
  vul_arg<std::string> test("-test", "load the specified classifier instance and classify pixels in images in imgs folder specified in test_imgs.txt file", "");
  vul_arg<int> block("-block", "size of the block around a pixel to compute its set of filter vectors", 5);
  vul_arg<unsigned> k("-k", "k for k-means algorithm to compute textons, number of textons per category", 100);
  vul_arg_parse(argc, argv);
  std::cout << "argc: " << argc << std::endl;

  vpgl_lvcs_sptr lvcs;
  if (train().compare("") != 0 || test().compare("") != 0) {
    if (utm_zone() == 17)
      lvcs = new vpgl_lvcs(30.371923, -81.539398, -8.0, vpgl_lvcs::utm, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    else if (utm_zone() == 18)
      lvcs = new vpgl_lvcs(35.289477, -75.570596, -8.0, vpgl_lvcs::utm, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    else {
      std::cerr << "unknown utm zone: " << utm_zone() << " exiting!..\n";
      return -1;
    }
  }

  if (find_imgs()) {

    if (class_name().compare("") == 0) {
      std::cout << " class name is not specified! exiting..\n"; return 0;
    }

    std::string in_dir = in_poly() + class_name() + "*.kml";
    std::vector<std::pair<vgl_polygon<double>, std::string> > polys;
    for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
      std::string name = vul_file::strip_directory(fn());
      name = vul_file::strip_extension(name);
      vgl_polygon<double> poly = bkml_parser::parse_polygon(fn());
      // check zone
      if (!in_zone(poly, utm_zone())) {
        std::cout << fn() << " is not in zone: " << utm_zone() << " skipping..!\n";
        continue;
      } else
        std::cout << fn() << " is in zone: " << utm_zone() << " training..!\n";

      for (unsigned j = 0; j < poly.num_sheets(); j++) {
        if (!poly[j].size())
          continue;
        vgl_polygon<double> p(1);
        for (unsigned k = 0; k < poly[j].size(); k++)
          p[0].push_back(poly[j][k]);
        polys.emplace_back(p, name);
      }
    }
    std::cout << " read " << polys.size() << " polys from the folder: " << in_poly() << std::endl;
    std::cout << " first point: " << polys[0].first[0][0].x() << " " << polys[0].first[0][0].y() << " # of points: " << polys[0].first[0].size() << std::endl;

    std::vector<std::string> tmp;
    std::vector<std::vector<std::string> > lidar_full(polys.size(), tmp), lidar_name(polys.size(), tmp);
    std::vector<std::vector<std::string> > img_full(polys.size(), tmp), img_name(polys.size(), tmp);
    // figure out which images contain a polygon
    if (img_folder().compare("") != 0) {
      std::vector<img_info> imgs;
      load_naip_imgs(img_folder(), imgs, utm_zone());
      std::cout << " loaded: " << imgs.size() << std::endl;
      for (auto & img : imgs) {
        for (unsigned j = 0; j < polys.size(); j++) {
          if (img.intersects(polys[j].first)) {
            std::cout << img.name <<" intersects: " << polys[j].second << " it's bbox: " << img.bbox << std::endl;
            std::stringstream file; file << out_folder() << "\\bbox_" << polys[j].second << "_img_" << img.name << ".kml";
            std::cout << " writing: " << file.str() << std::endl;
            img.save_box_kml(file.str());
            img_full[j].push_back(img.img_name);
            img_name[j].push_back(img.name);
            std::cout << " img full has " << img_full[j].size() << " NAIP img!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
          }
        }
      }
    }

    if (lidar_folder().compare("") != 0) {
      std::vector<img_info> lidar_imgs;
      load_lidar_imgs(lidar_folder(), lidar_imgs);
      std::cout << " loaded: " << lidar_imgs.size() << std::endl;
      for (auto & lidar_img : lidar_imgs) {
        for (unsigned j = 0; j < polys.size(); j++) {
          if (lidar_img.intersects(polys[j].first)) {
            std::cout << lidar_img.name <<" intersects: " << polys[j].second << " it's bbox: " << lidar_img.bbox << std::endl;
            std::stringstream file; file << out_folder() << "\\bbox_" << polys[j].second << "_lidar_img_" << lidar_img.name << ".kml";
            lidar_img.save_box_kml(file.str());
            lidar_full[j].push_back(lidar_img.img_name);
            lidar_name[j].push_back(lidar_img.name);
          }
        }
      }
    }

    for (unsigned j = 0; j < polys.size(); j++) {
      std::stringstream site_file; site_file << in_poly() << "site_" << polys[j].second << "_zone_" << utm_zone() << ".xml";
      prepare_site_file(site_file.str(), lidar_full[j], lidar_name[j], img_full[j], img_name[j]);
    }
  }

  if (train().compare("") != 0) {

    if (class_name().compare("") == 0) {
      std::cout << " class name is not specified! exiting..\n"; return 0;
    }
    if (filter_folder().compare("") == 0) {
      std::cout << "specify a folder to read/write filter responses from/to!\n"; return -1;
    } else
      std::cout << "will read responses from: " << filter_folder() << " if available!\n"; std::cout.flush();

    sdet_texture_classifier_params dummy;
    sdet_texture_classifier tc(dummy);
    tc.load_data(train());  // loads the real params from the file
    std::cout << " loaded classifier with params: " << tc << std::endl;
    std::cout << " current # of training data in class " << class_name() << " is: " << tc.data_size(class_name()) << '\n';

    // read the txt file to access the training images and their masks (created manually)
    std::string txt_file = in_poly() + "train_imgs.txt";
    std::vector<std::pair<std::string, std::string> > lidar_imgs, imgs; std::vector<std::string> names;
    if (!read_training_img(txt_file, names, lidar_imgs, imgs)) {
      std::cerr << "cannot find: " << txt_file << "! exiting..\n";
      return -1;
    }
    std::cout << "read: " << names.size() << " training img-mask pairs from file: " << txt_file << std::endl;
    for (unsigned ii = 0; ii < names.size(); ii++) {
      std::cout << "-------------------------------------------------------------\n";
      vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly() + names[ii] + ".kml");
      // check zone
      if (!in_zone(poly, utm_zone())) {
        std::cout << names[ii] << " is not in zone: " << utm_zone() << " skipping..!\n";
        continue;
      } else
        std::cout << names[ii] << " is in zone: " << utm_zone() << " training..!\n";

      // use LIDAR image to go to local coords, from local coords go to NAIP image
      std::vector<std::pair<int, int> > lidar_pixels;
      std::vector<std::pair<double, double> > local_coords;
      collect_lidar_pixels(lvcs, in_poly() + lidar_imgs[ii].second, lidar_imgs[ii].first, lidar_pixels, local_coords);
      std::cout << "  \t collected: " << lidar_pixels.size() << " pixels from lidar image!\n";
      std::string tfw_name = img_folder() + imgs[ii].first + "/" + imgs[ii].first + ".tfw";
      std::cout << " will determine aerial geo camera by reading: " << tfw_name << std::endl;
      std::vector<std::pair<int, int> > img_pixels;
      if (!collect_img_pixels(lvcs, utm_zone(), in_poly() + imgs[ii].second, tfw_name, local_coords, img_pixels, neighborhood_size())) {
        std::cerr << " problems collecting pixels from: " << imgs[ii].first << '\n';
        return -1;
      }

      // now extract training samples in a neighborhood
      sdet_texture_classifier lidar_c((sdet_texture_classifier_params)tc);
      if (!lidar_c.compute_filter_bank_float_img(filter_folder(), lidar_folder() + lidar_imgs[ii].first + ".tif", lidar_max()))
        return 0;
      lidar_c.compute_training_data(class_name(), lidar_pixels);
      std::vector<vnl_vector<double> > data;
      lidar_c.get_training_data(class_name(), data);
      tc.add_training_data(class_name(), data);

      // extract training samples from imgs
      sdet_texture_classifier naip_c((sdet_texture_classifier_params)tc);
      if (!naip_c.compute_filter_bank_color_img(filter_folder(), img_folder() + imgs[ii].first + "/" + imgs[ii].first + ".tif"))
        return 0;
      naip_c.compute_training_data(class_name(), img_pixels);
      std::vector<vnl_vector<double> > data2;
      naip_c.get_training_data(class_name(), data2);
      tc.add_training_data(class_name(), data2);

      std::cout << " current # of training data in class " << class_name() << " is: " << tc.data_size(class_name()) << '\n';
      std::cout << "-------------------------------------------------------------\n";
    }
    tc.save_data(train());
  }

  if (compute().compare("") != 0) {
    sdet_texture_classifier_params dummy;
    sdet_texture_classifier tc(dummy);
    tc.load_data(compute());  // loads the real params from the file
    tc.k_ = k();
    std::cout << " loaded classifier with params: " << tc << std::endl;
    tc.compute_textons_all();
    tc.compute_category_histograms();
    std::string name = vul_file::strip_extension(compute());
    tc.save_dictionary(name + "_dict.vsl");  // saves the params as well
    unsigned ntextons = tc.get_number_of_textons();
    std::cout << " for the dictionary: " << name << ", the number of textons is: " << ntextons << '\n';
  }

  std::cout << "total time: " << t.all()/1000 << " seconds = " << t.all()/(1000*60) << " mins.\n";
  return volm_io::SUCCESS;
}
