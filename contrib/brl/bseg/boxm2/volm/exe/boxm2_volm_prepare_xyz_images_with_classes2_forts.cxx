//:
// \file
// \brief an executable to prepare xyz images to be ingested into voxel world, add 'forts' 'building_tall' and 'building_short' as additional classes
// \author Ozge C. Ozcanli
// \date Feb 7, 2013
//

#include <vul/vul_arg.h>

#include <boxm2/volm/boxm2_volm_locations.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <volm/volm_io.h>
#include <boxm2/boxm2_scene.h>
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

vil_rgb<vxl_byte> pier_color = vil_rgb<vxl_byte>(0, 105, 101);

int main(int argc,  char** argv)
{
  vul_arg<std::string> scene_file("-scene", "scene xml filename prefix", "");
  vul_arg<int> scene_id("-id", "scene id", -1);
  vul_arg<std::string> builds_file("-builds", "folder with files", "");
  vul_arg<std::string> nlcd_folder("-nlcd", "folder with nlcd images", "");
  vul_arg<std::string> sme_folder("-sme", "folder with sme object files", "");
  vul_arg<std::string> lidar_folder("-lidar", "folder with lidar images", "");
  vul_arg<std::string> class_folder("-class", "folder with classification outputs of lidar images", "");
  vul_arg<std::string> out_folder("-out", "prefix for names of output x y z images", "");
  vul_arg<float> pier_prob_threshold("-pier_prob", "threshold for pier probability", 0.0010f);
  vul_arg_parse(argc, argv);

  std::cout << "argc: " << argc << std::endl;
  if (scene_file().compare("") == 0 || builds_file().compare("") == 0 || out_folder().compare("") == 0 || class_folder().compare("") == 0) {
    std::cerr << "EXE_ARGUMENT_ERROR!\n";
    vul_arg_display_usage_and_exit();
    return 0;
  }

  std::stringstream scene_name;
  scene_name << scene_file();
  if (scene_id() < 0)
    scene_name << ".xml";
  else
    scene_name << scene_id() << ".xml";

  boxm2_scene_sptr scene = new boxm2_scene(scene_name.str());
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());

  float pier_prob_thres = pier_prob_threshold();
  unsigned char dev_id1 = volm_label_table::land_id[volm_label_table::DEVELOPED_OPEN].id_;
  unsigned char dev_id2 = volm_label_table::land_id[volm_label_table::DEVELOPED_LOW].id_;
  unsigned char dev_id3 = volm_label_table::land_id[volm_label_table::DEVELOPED_HIGH].id_;

  // determine number and size of tiles
  std::vector<boxm2_block_id> blks = scene->get_block_ids();
  boxm2_scene_info* info = scene->get_blk_metadata(blks[0]);
  float sb_length = info->block_len;
  float vox_length = sb_length/8.0f;
  std::cout << "scene voxel length: " << vox_length << std::endl;
  vgl_box_3d<double> scene_bbox = scene->bounding_box();
  std::cout << "scene bbox: " << scene_bbox << std::endl;

  // find scene bbox to see if it intersects with the image boxes -- WARNING: assumes that these boxes are small enough (both image and scene are small in area) so that Euclidean distances approximate the geodesic distances in geographic coordinates
  double min_lon, min_lat, gz, max_lon, max_lat;
  lvcs->local_to_global(scene_bbox.min_point().x(), scene_bbox.min_point().y(), 0, vpgl_lvcs::wgs84, min_lon, min_lat, gz);
  lvcs->local_to_global(scene_bbox.max_point().x(), scene_bbox.max_point().y(), 0, vpgl_lvcs::wgs84, max_lon, max_lat, gz);
  vgl_box_2d<double> sbbox(min_lon, max_lon, min_lat, max_lat);
  std::cout << " scene bbox in geo coords: " << sbbox << std::endl;

  // iterate over NLCD images and find the ones that overlap
  std::vector<std::pair<vil_image_view<vxl_byte>, vpgl_geo_camera*> > nlcd_imgs;

  std::string glob = nlcd_folder() + "/*.tif";
  std::cout << "\t\t reading: " << glob << std::endl;
  for (vul_file_iterator fit = glob;fit; ++fit) {

    volm_tile t(fit(), 0, 0); // pass ni, nj as 0 cause just need to parse the name string
    vgl_box_2d<double> bbox = t.bbox_double();

    if (vgl_area(vgl_intersection(bbox, sbbox)) <= 0)
      continue;

    vil_image_view_base_sptr img_sptr = vil_load(fit());
    if (img_sptr->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
      std::cout << "Input image pixel format is not VIL_PIXEL_FORMAT_BYTE!\n";
      return -1;
    }
    vil_image_view<vxl_byte> img(img_sptr);
    unsigned nii = img.ni(); unsigned nji = img.nj();

    vpgl_geo_camera *cam;
    vpgl_geo_camera::init_geo_camera(fit(), nii, nji, lvcs, cam);
    double lon2, lat2;
    cam->img_to_global(nii, nji, lon2, lat2);
    vpgl_utm utm; double x, y; int zone; utm.transform(lat2, -lon2, x, y, zone);

    std::pair<vil_image_view<vxl_byte>, vpgl_geo_camera*> pair(img, cam);
    nlcd_imgs.push_back(pair);
    std::cout << fit() << std::endl;

  }
  std::cout << "there are " << nlcd_imgs.size() << " nlcd images that intersect the scene!\n";

  // read the LIDAR tiles that intersect the scene
  std::vector<std::pair<vil_image_view<float>, vpgl_geo_camera*> > lidar_imgs;
  std::vector< vil_image_view<vil_rgb<vxl_byte> > > lidar_class_imgs;
  std::vector< vil_image_view<float > > lidar_prob_imgs;

  glob = lidar_folder() + "/*.tif";
  std::cout << "\t\t reading: " << glob << std::endl;
  for (vul_file_iterator fit = glob;fit; ++fit) {

    volm_tile t(fit(), 0, 0); // pass ni, nj as 0 cause just need to parse the name string
    vgl_box_2d<double> bbox = t.bbox_double();

    if (vgl_area(vgl_intersection(bbox, sbbox)) <= 0)
      continue;

    vil_image_view_base_sptr img_sptr = vil_load(fit());
    if (img_sptr->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
      std::cout << "Input image pixel format is not VIL_PIXEL_FORMAT_BYTE!\n";
      return -1;
    }
    vil_image_view<float> img(img_sptr);
    unsigned nii = img.ni(); unsigned nji = img.nj();

    vpgl_geo_camera *cam;
    vpgl_geo_camera::init_geo_camera(fit(), nii, nji, lvcs, cam);
    double lon2, lat2;
    cam->img_to_global(nii, nji, lon2, lat2);
    vpgl_utm utm; double x, y; int zone; utm.transform(lat2, -lon2, x, y, zone);

    std::pair<vil_image_view<float>, vpgl_geo_camera*> pair(img, cam);
    lidar_imgs.push_back(pair);

    std::string name = vul_file::strip_directory(fit());
    name = vul_file::strip_extension(name);
    std::string name1 = class_folder() + name + "_outmap.png";
    std::string name2 = class_folder() + name + "_outmap.tif";
    if (!vul_file::exists(name1) || !vul_file::exists(name2)) {
      std::cerr << " cannot load img: " << name1 << " or " << name2 << std::endl;
      return 0;
    }
    vil_image_view<vil_rgb<vxl_byte> > class_img = vil_load(name1.c_str());
    vil_image_view<float > class_prob_img = vil_load(name2.c_str());
    lidar_class_imgs.push_back(class_img);
    lidar_prob_imgs.push_back(class_prob_img);
  }
  std::cout << "there are " << lidar_imgs.size() << " lidar images that intersect the scene!\n";

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
  int ni = (int)std::ceil((scene_bbox.max_x()-scene_bbox.min_x()+1.0)/vox_length);
  int nj = (int)std::ceil((scene_bbox.max_y()-scene_bbox.min_y()+1.0)/vox_length);
  std::cout <<"image size needs ni: " << ni << " nj: " << nj << " to support voxel res: " << vox_length << std::endl;

  // create x y z images
  vil_image_view<float> out_img_x(ni, nj, 1);
  vil_image_view<float> out_img_y(ni, nj, 1);
  vil_image_view<float> out_img_z(ni, nj, 1);
  vil_image_view<vxl_byte> out_img_label(ni, nj, 1);  // to be ingested
  vil_image_view<vil_rgb<vxl_byte> > out_class_img(ni, nj, 1);  // visualization for debugging purposes

  out_img_x.fill(0.0f); out_img_y.fill(0.0f);
  out_img_z.fill((float)(scene_bbox.max_z()+100.0));  // local coord system min z, initialize to constant
  out_img_label.fill((vxl_byte)0);
  out_class_img.fill(vil_rgb<vxl_byte>(255, 255, 255));

  // iterate over the image and for each pixel, calculate, xyz in the local coordinate system
  for (int i = 0; i < ni; i++)
    for (int j = 0; j < nj; j++) {
      auto local_x = (float)(i*vox_length+scene_bbox.min_x()+vox_length/2.0);
      auto local_y = (float)(scene_bbox.max_y()-j*vox_length+vox_length/2.0);
      out_img_x(i,j) = local_x;
      out_img_y(i,j) = local_y;

      double lon, lat, gz;
      lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);

      // find pixel in images
      unsigned char label = volm_label_table::INVALID;
      bool nlcd_found = false;
      for (auto & nlcd_img : nlcd_imgs) {
        double u, v;
        nlcd_img.second->global_to_img(-lon, lat, gz, u, v);
        auto uu = (unsigned)std::floor(u + 0.5);
        auto vv = (unsigned)std::floor(v + 0.5);
        if (uu > 0 && vv > 0 && uu < nlcd_img.first.ni() && vv < nlcd_img.first.nj()) {
          label = (nlcd_img.first)(uu, vv);
          break;
        }
      }

      // find elev
      float elev = 0.0f;
      bool found = false;
      for (auto & lidar_img : lidar_imgs) {
        double u, v;
        lidar_img.second->global_to_img(-lon, lat, gz, u, v);
        auto uu = (unsigned)std::floor(u + 0.5);
        auto vv = (unsigned)std::floor(v + 0.5);
        if (uu > 0 && vv > 0 && uu < lidar_img.first.ni() && vv < lidar_img.first.nj()) {
          elev = (lidar_img.first)(uu, vv);
          break;
        }
      }

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
  glob = sme_folder() + "/*.csv";
  std::vector<std::pair<vgl_point_2d<double>, int> > objects;
  std::cout << "\t\t reading: " << glob << std::endl;
  for (vul_file_iterator fit = glob;fit; ++fit)
    volm_io::read_sme_file(fit(), objects);

  std::cout << " read a total of " << objects.size() << " sme objects!\n";

  // mark the forts
  double fort_rad = 500.0; // forts have radius about this much around ground truth location
  for (auto & object : objects) {
    if (object.second == volm_label_table::FORT) {
      if (sbbox.contains(object.first)) { // the scene contains this bbox
        double lx, ly, lz;
        lvcs->global_to_local(object.first.x(), object.first.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
        int i = (int)std::floor((lx - scene_bbox.min_x())/vox_length+0.5);
        int j = (int)std::floor((scene_bbox.max_y() - ly)/vox_length+0.f);

        std::cout << " contained!: in local: " << i << ", " << ly << ", " << lz << std::endl;
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
      auto local_x = (float)(i*vox_length+scene_bbox.min_x()+vox_length/2.0);
      auto local_y = (float)(scene_bbox.max_y()-j*vox_length+vox_length/2.0);

      double lon, lat, gz;
      lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);

      // find elev
      float elev = 0.0f;
      bool found = false;
      vil_rgb<vxl_byte> class_color;
      float class_prob = 0.0f;
      unsigned lidar_img_id = 0;
      for (unsigned k = 0; k < lidar_imgs.size(); k++) {
        double u, v;
        lidar_imgs[k].second->global_to_img(-lon, lat, gz, u, v);
        auto uu = (unsigned)std::floor(u + 0.5);
        auto vv = (unsigned)std::floor(v + 0.5);
        if (uu > 0 && vv > 0 && uu < lidar_imgs[k].first.ni() && vv < lidar_imgs[k].first.nj()) {
          elev = (lidar_imgs[k].first)(uu, vv);
          class_color = lidar_class_imgs[k](uu,vv);
          class_prob = lidar_prob_imgs[k](uu,vv);
          break;
        }
      }

      if (pixel_id == sand_id && class_color == pier_color && elev > 1.0f && class_prob > pier_prob_thres) {

        // enforce water neighborhood
        unsigned cnt = 0;
        for (int ii = i-20; ii < i+20; ii++)
          for (int jj = j-20; jj < j+20; jj++) {
            if (ii < 0 || jj < 0 || ii >= ni || jj >= nj)
              continue;
            if (out_img_label(ii,jj) == water_id)
              cnt++;
          }
        if (cnt > 750) { // water on at least 1 side

          // also enforce some developed region in a region of 200m radius
          unsigned cnt_dev = 0;
          for (int ii = i-200; ii < i+200; ii++)
            for (int jj = j-200; jj < j+200; jj++) {
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
        for (int ii = i-45; ii < i+45; ii++) {
          for (int jj = j-45; jj < j+45; jj++) {
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
      double lx, ly, lz;
      lvcs->global_to_local(polys[ii].first[0][jj].x(), polys[ii].first[0][jj].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
      double i = (lx - scene_bbox.min_x())/vox_length;
      double j = (scene_bbox.max_y() - ly)/vox_length;

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
        for (int yy = y-5; yy < y+5; yy++) // enlarge 5 meters
          for (int xx = x-5; xx < x+5; xx++) {
            if (xx >= 0 && yy >= 0 && xx < (int)out_class_img.ni() && yy < (int)out_class_img.nj()) {
                out_img_label(xx,yy) = pixel_id;
                out_class_img(xx,yy) = pixel_color;
              }
          }
      }
    }
  }

  std::stringstream out_prefix; out_prefix << out_folder() << "/";
  if (scene_id() < 0)
    out_prefix << "scene_out_img_";
  else
    out_prefix << "scene" << scene_id() << "_out_img_";

  vil_save(out_img_x, (out_prefix.str() + "x.tif").c_str());
  vil_save(out_img_y, (out_prefix.str() + "y.tif").c_str());
  vil_save(out_img_z, (out_prefix.str() + "z.tif").c_str());
  vil_save(out_class_img, (out_prefix.str() + "_class.png").c_str());
  vil_save(out_img_label, (out_prefix.str() + "_class.tiff").c_str());

  std::cout << "returning SUCCESS!\n";
  return volm_io::SUCCESS;
}
