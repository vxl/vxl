//:
// \file
// \brief executable to evaluate output tiles using the ground truth camera
// \author Ozge C. Ozcanli
// \date Nov 18, 2012

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <bkml/bkml_parser.h>

// generate gt hypos
int main(int argc,  char** argv)
{
  vul_arg<std::string> poly_in("-poly", "region polygon as kml, only the pixels of the output tiles within this polygon will be used for evaluation", "");
  vul_arg<std::string> cam_file("-cam", "ground truth camera", "");
  vul_arg<std::string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<std::string> category_file("-cat", "category file for transferring labelme type to land id", "");
  vul_arg<std::string> out("-out", "folder which has the output tiles", "");
  vul_arg<unsigned> thres("-t", "threshold to count # of pixels less than", 127);
  vul_arg_parse(argc, argv);

  std::cout << "argc: " << argc << std::endl;
  if (out().compare("") == 0 || poly_in().compare("") == 0 || cam_file().compare("") == 0 || label_file().compare("") == 0 || category_file().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  unsigned threshold = thres();

  depth_map_scene_sptr dm = new depth_map_scene;
  std::string img_category;
  if (!volm_io::read_labelme(label_file(), category_file(), dm, img_category)) {
    std::cerr << "problem parsing: " << label_file() << '\n';
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  std::cout << "parsed image category: " << img_category << '\n';
  vgl_polygon<double> poly = bkml_parser::parse_polygon(poly_in());
  std::cout << "ROI poly has: " << poly[0].size() << " vertices!\n";

  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double top_fov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, top_fov, top_fov_dev, altitude, lat, lon)) {
    return volm_io::CAM_FILE_IO_ERROR;
  }
  std::cout << "cam params\nheading: " << heading << " dev: " << heading_dev << "\ntilt: " << tilt << " dev: " << tilt_dev << "\nroll: " << roll << " dev: " << roll_dev << "\ntop_fov: " << top_fov << " dev: " << top_fov_dev << " alt: " << altitude << std::endl;

  // read the output tiles
  unsigned cnt_below = 0;
  unsigned tot_pix_count = 0;
  unsigned tot_pix_unevaluated = 0;
  unsigned within_poly = 0;
  std::vector<volm_tile> tiles;
  if (img_category == "desert")
    tiles = volm_tile::generate_p1_wr1_tiles();
  else
    tiles = volm_tile::generate_p1_wr2_tiles();
  for (auto & i : tiles)
  {
    std::string name = out() + "/" + "ProbMap_" + i.get_string() + ".tif";
    std::cout << "reading " << name << std::endl;
    if (!vul_file::exists(name))
    {
      std::cerr << name << " is missing!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    vil_image_view<vxl_byte> tile = vil_load(name.c_str());
    for (unsigned ii = 0; ii < tile.ni(); ii++)
      for (unsigned jj = 0; jj < tile.nj(); jj++)
      {
        double tlat, tlon;
        i.img_to_global(ii, jj, tlon, tlat);
        if (poly.contains(tlon, tlat))
        {
          within_poly++;
          if (tile(ii, jj) == 0)
            tot_pix_unevaluated++;
          else
          {
            tot_pix_count++;
            if (tile(ii, jj) < threshold)
              cnt_below++;
          }
        }
      }
    unsigned u, v;
    i.global_to_img(lon, lat, u, v);
    if (u < tile.ni() && v < tile.nj())
      std::cout << "GT location: " << lon << ", " << lat << " is at pixel: " << u << ", " << v << " and has value: " << (int)tile(u, v) << std::endl;
  }
  std::cout << "tot pixels within ROI: " << within_poly << '\n'
           << "tot pixels unevaluated: " << tot_pix_unevaluated << '\n'
           << "tot pixels evaluated: " << tot_pix_count << '\n'
           << "tot pixels below threshold " << threshold << ": " << cnt_below << '\n'
           << "so knocked out " << (float)cnt_below/tot_pix_count*100 << " percent of the evaluated ROI!\n";

  return volm_io::SUCCESS;
}
