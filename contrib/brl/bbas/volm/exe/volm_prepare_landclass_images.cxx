//:
// \file
// \brief an executable to prepare land classification images, add 'forts' 'building_tall' and 'building_short' as additional classes
//        moved the exe from boxm2/volm/exe to here, changed output and made it to accept 1 lidar image and generate a map of the same size and name
// \author Ozge C. Ozcanli
// \date June 19, 2013
//

#include <vul/vul_arg.h>

#include <volm/volm_io.h>
#include <volm/volm_io_tools.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vul/vul_file_iterator.h>
#include <vpgl/vpgl_utm.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_random.h>
#include <vul/vul_file.h>

vil_rgb<vxl_byte> pier_color = vil_rgb<vxl_byte>(0, 105, 101);

int main(int argc,  char** argv)
{
  vul_arg<std::string> builds_file("-builds", "folder with building files", "");
  vul_arg<std::string> nlcd_folder("-nlcd", "folder with nlcd images", "");
  vul_arg<std::string> sme_folder("-sme", "folder with sme object files", "");
  vul_arg<std::string> lidar_file("-lidar", "path to lidar image", "");
  vul_arg<std::string> class_folder("-class", "folder with classification outputs of lidar images", "");
  vul_arg<std::string> out_folder("-out", "prefix for names of output x y z images", "");
  vul_arg<float> pier_prob_threshold("-pier_prob", "threshold for pier probability", 0.0010f);
  vul_arg_parse(argc, argv);

  std::cout << "argc: " << argc << std::endl;
  if (builds_file().compare("") == 0 || out_folder().compare("") == 0 || class_folder().compare("") == 0 ||
    lidar_file().compare("") == 0 || nlcd_folder().compare("") == 0) {
    std::cerr << "EXE_ARGUMENT_ERROR!\n";
    vul_arg_display_usage_and_exit();
    return 0;
  }

  volm_img_info lidar_img_info;
  volm_io_tools::load_lidar_img(lidar_file(), lidar_img_info, true, false);

  float pier_prob_thres = pier_prob_threshold();
  unsigned char dev_id1 = volm_label_table::land_id[volm_label_table::DEVELOPED_OPEN].id_;
  unsigned char dev_id2 = volm_label_table::land_id[volm_label_table::DEVELOPED_LOW].id_;
  unsigned char dev_id3 = volm_label_table::land_id[volm_label_table::DEVELOPED_HIGH].id_;

  // iterate over NLCD images and find the ones that overlap
  std::vector<std::pair<vil_image_view<vxl_byte>, vpgl_geo_camera*> > nlcd_imgs;
  std::vector<volm_img_info> nlcd_img_infos;
  volm_io_tools::load_nlcd_imgs(nlcd_folder(), nlcd_img_infos);
  for (auto & nlcd_img_info : nlcd_img_infos) {
    if (vgl_area(vgl_intersection(lidar_img_info.bbox, nlcd_img_info.bbox)) > 0) {

      if (nlcd_img_info.img_r->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
        std::cout << "NLCD Input image pixel format is not VIL_PIXEL_FORMAT_BYTE!\n";
        return -1;
      }
      vil_image_view<vxl_byte> img(nlcd_img_info.img_r);

      std::pair<vil_image_view<vxl_byte>, vpgl_geo_camera*> pair(img, nlcd_img_info.cam);
      nlcd_imgs.push_back(pair);
    }
  }
  std::cout << "there are " << nlcd_imgs.size() << " nlcd images that intersect the lidar image!\n";

  // load the LIDAR classification images
  vil_image_view<float> lidar_img(lidar_img_info.img_r);

  std::string name = vul_file::strip_directory(lidar_file());
  name = vul_file::strip_extension(name);
  std::string name1 = class_folder() + name + "_outmap.png";
  std::string name2 = class_folder() + name + "_outmap.tif";
  if (!vul_file::exists(name1) || !vul_file::exists(name2)) {
    std::cerr << " cannot load img: " << name1 << " or " << name2 << std::endl;
    return 0;
  }

  vil_image_view<vil_rgb<vxl_byte> > lidar_class_img = vil_load(name1.c_str());
  vil_image_view<float > lidar_prob_img = vil_load(name2.c_str());

  unsigned char water_id = volm_label_table::land_id[volm_label_table::WATER].id_;
  unsigned char sand_id = volm_label_table::land_id[volm_label_table::SAND].id_;
  unsigned char pier_id = volm_label_table::land_id[volm_label_table::PIER].id_;
  unsigned char building_id = volm_label_table::land_id[volm_label_table::BUILDING].id_;
  unsigned char fort_id = volm_label_table::land_id[volm_label_table::FORT].id_;
  unsigned char building_tall_id = volm_label_table::land_id[volm_label_table::BUILDING_TALL].id_;

  vil_rgb<vxl_byte> pier_pixel_color = volm_label_table::land_id[volm_label_table::PIER].color_;
  vil_rgb<vxl_byte> building_pixel_color = volm_label_table::land_id[volm_label_table::BUILDING].color_;
  vil_rgb<vxl_byte> fort_pixel_color = volm_label_table::land_id[volm_label_table::FORT].color_;
  vil_rgb<vxl_byte> building_tall_pixel_color = volm_label_table::land_id[volm_label_table::BUILDING_TALL].color_;

  std::cout << "pier color is: " << pier_color << " water id: " << (int)water_id << " sand id: " << (int)sand_id << " pier_id: " << (int)pier_id << "\n";

   // prepare an image for the finest resolution
  int ni = lidar_img.ni();
  int nj = lidar_img.nj();

  vil_image_view<vxl_byte> out_img_label(ni, nj, 1);  // to be ingested
  vil_image_view<vil_rgb<vxl_byte> > out_class_img(ni, nj, 1);  // visualization for debugging purposes

  out_img_label.fill((vxl_byte)0);
  out_class_img.fill(vil_rgb<vxl_byte>(255, 255, 255));

  // iterate over the image and for each pixel, calculate, xyz in the local coordinate system
  for (int i = 0; i < ni; i++)
    for (int j = 0; j < nj; j++) {

      double lon, lat;
      lidar_img_info.cam->img_to_global(i, j, lon, lat);

      // find pixel in images
      unsigned char label = volm_label_table::INVALID;
      bool nlcd_found = false;
      for (auto & nlcd_img : nlcd_imgs) {
        double u, v;
        nlcd_img.second->global_to_img(lon, lat, 0, u, v);
        auto uu = (unsigned)std::floor(u + 0.5);
        auto vv = (unsigned)std::floor(v + 0.5);
        if (uu > 0 && vv > 0 && uu < nlcd_img.first.ni() && vv < nlcd_img.first.nj()) {
          label = (nlcd_img.first)(uu, vv);
          break;
        }
      }
      // find elev
      float elev = lidar_img(i,j);

      // decide the class
      if (label == volm_label_table::WETLAND || label == volm_label_table::WOODY_WETLAND) {
        if (elev < 0.5f)
          label = volm_label_table::WATER;
      } else {
        if (elev < 1.0f)
          label = volm_label_table::WATER;
      }

      out_class_img(i,j) = volm_label_table::land_id[label].color_;
      out_img_label(i,j) = volm_label_table::land_id[label].id_;
    }


  // read the sme labels
  std::string glob = sme_folder() + "/*.csv";
  std::vector<std::pair<vgl_point_2d<double>, int> > objects;
  std::cout << "\t\t reading: " << glob << std::endl;
  for (vul_file_iterator fit = glob;fit; ++fit)
    volm_io::read_sme_file(fit(), objects);

  std::cout << " read a total of " << objects.size() << " sme objects!\n";

  // mark the forts
  double fort_rad = 500.0; // forts have radius about this much around ground truth location
  for (auto & object : objects) {
    if (object.second == volm_label_table::FORT) {
      if (lidar_img_info.bbox.contains(object.first)) { // the scene contains this bbox
        double lu, lv;
        lidar_img_info.cam->global_to_img(-(object.first.x()), object.first.y(), 0.0, lu, lv); // WARNING: W is hard coded in vpgl_geo_camera so use -lon !!!!!
        int i = (int)std::floor(lu+0.5);
        int j = (int)std::floor(lv+0.5);
        for (int ii = i - fort_rad; ii < i + fort_rad; ii++)
          for (int jj = j - fort_rad; jj < j + fort_rad; jj++) {
            if (ii >= 0 && jj >= 0 && ii < (int)out_img_label.ni() && jj < (int)out_img_label.nj() && out_img_label(ii,jj) != water_id) {
              out_img_label(ii,jj) = fort_id;
              out_class_img(ii,jj) = fort_pixel_color;
            }
          }
      }
    }
  }

  // iterate over the image and for each pixel, to check for water neighborhood to label piers
  for (int i = 0; i < ni; i++)
    for (int j = 0; j < nj; j++) {
      unsigned char pixel_id = out_img_label(i,j);
      vil_rgb<vxl_byte> pixel_color = out_class_img(i,j);
      if (pixel_id != water_id && pixel_id != sand_id)
        continue;

      // find elev
      float elev = lidar_img(i,j);
      vil_rgb<vxl_byte> class_color = lidar_class_img(i,j);
      float class_prob = lidar_prob_img(i,j);

      if (pixel_id == sand_id && class_color == pier_color && elev > 1.0f && class_prob > pier_prob_thres) {

        // enforce water neighborhood
        unsigned cnt = 0;
        for (int ii = i-10; ii < i+10; ii++)  // note it was -20 and +20 for both i and j for 1 m pixel resolution
          for (int jj = j-10; jj < j+10; jj++) {
            if (ii < 0 || jj < 0 || ii >= ni || jj >= nj)
              continue;
            if (out_img_label(ii,jj) == water_id)
              cnt++;
          }
        if (cnt > 200) {//if (cnt > 750) { // note it was 750 for 1 m pixel resolution and +- 20 pixels

          // also enforce some developed region in a region of 200m radius
          unsigned cnt_dev = 0;
          for (int ii = i-100; ii < i+100; ii++)  // note it was -200 and +200 for both i and j and for 1 m pixel resolution
            for (int jj = j-100; jj < j+100; jj++) {
              if (ii < 0 || jj < 0 || ii >= ni || jj >= nj)
                continue;
              if (out_img_label(ii,jj) == dev_id1 || out_img_label(ii,jj) == dev_id2 || out_img_label(ii,jj) == dev_id3)
                cnt_dev++;
            }
          if (cnt_dev > 1) {
            out_class_img(i,j) = pier_pixel_color;
            out_img_label(i,j) = pier_id;
          }
        }
      }

    }

  vil_image_view<bool> grew(ni, nj);
  grew.fill(false);
  // iterate over again to grow the pier region if possible
  for (int i = 0; i < ni; i++)
    for (int j = 0; j < nj; j++) {
      unsigned char pixel_id = out_img_label(i,j);
      if (pixel_id == pier_id && !grew(i,j)) {
        for (int ii = i-23; ii < i+23; ii++) {  // note it was -45 and +45 for both i and j and for 1 m pixel resolution
          for (int jj = j-23; jj < j+23; jj++) {
            if (ii < 0 || jj < 0 || ii >= ni || jj >= nj)
              continue;
            if (out_img_label(ii,jj) == sand_id) {
              out_class_img(ii,jj) = pier_pixel_color;
              out_img_label(ii,jj) = pier_id;
              grew(ii,jj) = true;
            }
          }
        }
      }
    }

  vnl_random rng;
  // read the buildings
  glob = builds_file() + "/*.csv";
  std::vector<std::pair<vgl_polygon<double>, vgl_point_2d<double> > > polys;
  std::vector<double> heights;
  std::cout << "\t\t reading: " << glob << std::endl;
  for (vul_file_iterator fit = glob;fit; ++fit) {
    volm_io::read_building_file(fit(), polys, heights);
  }
  std::cout << " read a total of " << polys.size() << " buildings!\n";

  // mark the buildings in the images
  for (unsigned ii = 0; ii < polys.size(); ii++) {

    int label = volm_label_table::BUILDING;
    unsigned char pixel_id = building_id;
    vil_rgb<vxl_byte> pixel_color = building_pixel_color;

    if (heights[ii] > 20) {  // specify the category
      pixel_id = building_tall_id;
      pixel_color = building_tall_pixel_color;
    }

    //vil_rgb<vxl_byte> pixel_color = vil_rgb<vxl_byte>((unsigned char)(255*rng.drand32()), (unsigned char)(255*rng.drand32()), (unsigned char)(255*rng.drand32()));

     // check if this is one of the sme objects
    for (auto & object : objects) {
      if (polys[ii].first.contains(object.first.x(), object.first.y())) {
        label = object.second;
        pixel_id = volm_label_table::land_id[label].id_;
        pixel_color = volm_label_table::land_id[label].color_;
        break;
      }

    }

    // find local poly
    vgl_polygon<double> img_poly(1);
    for (unsigned jj = 0; jj < polys[ii].first[0].size(); jj++) {
      double i, j;
      lidar_img_info.cam->global_to_img(-(polys[ii].first[0][jj].x()), polys[ii].first[0][jj].y(), 0.0, i, j); // WARNING: W is hard coded in vpgl_geo_camera so use -lon !!!!!

      if (i >= 0 && j >= 0 &&  i < out_class_img.ni() && j < out_class_img.nj())  // change the label
        img_poly[0].push_back(vgl_point_2d<double>(i,j));

    }

    vgl_polygon_scan_iterator<double> psi(img_poly, true);  // last argument true == include boundary

    bool hit_a_pier = false;
    for (psi.reset(); psi.next(); ) {
      int y = psi.scany();
      for (int x = psi.startx(); x <= psi.endx(); ++x) {
        if (x >= 0 && y >= 0 &&  x < (int)out_class_img.ni() && y < (int)out_class_img.nj())  {
          if (out_img_label(x,y) == pier_id) {
            pixel_id = pier_id;
            pixel_color = pier_pixel_color;
            hit_a_pier = true;
            break;
          }
        }
      }
      if (hit_a_pier)
        break;
    }
    for (psi.reset(); psi.next(); ) {
      int y = psi.scany();
      for (int x = psi.startx(); x <= psi.endx(); ++x) {
        for (int yy = y-3; yy < y+3; yy++) // enlarge 5 meters
          for (int xx = x-3; xx < x+3; xx++) {
            if (xx >= 0 && yy >= 0 && xx < (int)out_class_img.ni() && yy < (int)out_class_img.nj()) {
                out_img_label(xx,yy) = pixel_id;
                out_class_img(xx,yy) = pixel_color;
              }
          }
      }
    }
  }
  name = name.substr(name.find_first_of('_')+1, name.size());
  std::stringstream out_prefix; out_prefix << out_folder() << "/class_" << name;
  vil_save(out_class_img, (out_prefix.str() + ".png").c_str());
  vil_save(out_img_label, (out_prefix.str() + ".tiff").c_str());

  std::cout << "returning SUCCESS!\n";
  return volm_io::SUCCESS;
}
