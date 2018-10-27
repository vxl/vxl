#include <string>
#include <vector>

#include <vgl/vgl_polygon.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_blob.h>
#include <vil/algo/vil_colour_space.h>
#include <brad/brad_spectral_angle_mapper.h>
#include <brad/brad_worldview3_functions.h>
#include <brip/brip_ms_blob_detection.h>

int main(int argc, char * argv[])
{
  if (argc != 9) {
    for (int i = 0; i < argc; i++) {
      std::cout << argv[i] << "\n";
    }
    std::cerr << "Usage: brad_find_blob_app [image_band_dir] [material_mask_file] [valid_mask_file] [lambda0] [lambda1] [neighborhood_radius] [out_file]\n";
    return 1;
  }

  std::cerr << "Loading image and mask\n";
  //get our arguments
  std::string image_band_dir, material_mask_file, valid_mask_file, out_file, out_geo_file;
  float lambda0, lambda1;
  int neighborhood_radius;
  image_band_dir = argv[1];                 // pre-calibrated band images (float), which can be created using functionality in brad_wv3_functions
  material_mask_file = argv[2];             // material to use when computing SAM (each masked pixel will be own sample in library)
  valid_mask_file = argv[3];                // valid pixels i.e. they are in the area of interest
  lambda0 = (float)std::atof(argv[4]);      // determines length of object being searched for
  lambda1 = (float)std::atof(argv[5]);      // determines width of object being searched for
  neighborhood_radius = (int)std::atoi(argv[6]); // radius of neighbor to search for local maxima in
  out_file = argv[7];                       // where our output will be saved
  out_geo_file = argv[8];                   // a geojson output to store the polygon in pixel domain

  // Load the image
  vil_image_view<float> cal_img;
  load_corrected_wv3(image_band_dir, cal_img);
  int ni = cal_img.ni(); int nj = cal_img.nj();

  //load material pixel mask
  vil_image_view<vxl_byte> mask = vil_load(material_mask_file.c_str());
  vil_image_view<bool> mask_b(mask.ni(), mask.nj());
  mask_b.fill(false);
  for (int j = 0; j < mask.nj(); j++) {
    for (int i = 0; i < mask.ni(); i++) {
      if (mask(i, j) > 0) {
        mask(i, j) = 255;
        mask_b(i, j) = true;
      }
    }
  }

  // create or load valid mask
  vil_image_view<bool> valid_mask_b(ni, nj);
  if (valid_mask_file.compare("none") == 0) { // no mask provided, use entire image
    valid_mask_b.fill(true);
  }
  else { //load valid mask
    vil_image_view<vxl_byte> valid_mask = vil_load(valid_mask_file.c_str());
    valid_mask_b.fill(false);
    for (int j = 0; j < valid_mask.nj(); j++) {
      for (int i = 0; i < valid_mask.ni(); i++) {
        if (valid_mask(i, j) > 0) {
          valid_mask(i, j) = 255;
          valid_mask_b(i, j) = true;
        }
      }
    }
  }

  // detect blobs and obtain bounding boxes
  std::cerr << "Calculating bounding boxes\n";
  std::vector<unsigned> i_min;
  std::vector<unsigned> j_min;
  std::vector<unsigned> i_max;
  std::vector<unsigned> j_max;
  std::vector<vgl_polygon<double> > poly;
  std::vector<float> conf;
  brip_blob_local_max_bb(cal_img, mask_b, valid_mask_b, lambda0, lambda1, neighborhood_radius, i_min, j_min, i_max, j_max, poly, conf);

  // create rgb image and overlay bounding boxes
  std::cerr << "Saving bounding box image\n";
  vil_image_view<float> rgb_img(cal_img.ni(), cal_img.nj(), 3);
  for (int y = 0; y < cal_img.nj(); y++)
    for (int x = 0; x < cal_img.ni(); x++) {
      rgb_img(x, y, 0) = cal_img(x, y, 4);
      rgb_img(x, y, 1) = cal_img(x, y, 2);
      rgb_img(x, y, 2) = cal_img(x, y, 1);
    }

  // covert to greyscale image
  vil_image_view<float> grey_img;
  vil_convert_planes_to_grey(rgb_img, grey_img);

  // convert to HSV image so that color scaling based on blob confidence is easier
  vil_image_view<float> hsv_img;
  hsv_img.set_size(ni, nj, 3);
  for (int j = 0; j < nj; j++) {
    for (int i = 0; i < ni; i++) {
      vil_colour_space_RGB_to_HSV(grey_img(i, j), grey_img(i, j), grey_img(i, j), &hsv_img(i, j, 0), &hsv_img(i, j, 1), &hsv_img(i, j, 2));
    }
  }
  // draw bounding boxes in hsv
  float h_color; // changes based on confidence
  float s_color = 1;
  float v_color = 10;
  float min_conf = 100;
  float max_conf = 0;
  for (float i : conf) {
    if (min_conf > i) min_conf = i;
    if (max_conf < i) max_conf = i;
  }
  float color_max = 250;
  float color_min = 100;
  int bb_rad = 4;
  for (int i = 0; i < i_min.size(); i++) {
    // pick a color based on the confidence
    h_color = (color_max - color_min) / (max_conf - min_conf) * conf[i] + color_max - (color_max - color_min) / (max_conf - min_conf) * max_conf;
    // draw horizontal lines
    for (int x = i_min[i]; x <= i_max[i]; x++) {
      for (int bb = 0; bb <= bb_rad; bb++) {
        hsv_img(x, std::max(0, int(j_min[i]) - bb), 0) = h_color;
        hsv_img(x, std::max(0, int(j_min[i]) - bb), 1) = s_color;
        hsv_img(x, std::max(0, int(j_min[i]) - bb), 2) = v_color;
        hsv_img(x, std::min(int(j_max[i]) + bb, nj - 1), 0) = h_color;
        hsv_img(x, std::min(int(j_max[i]) + bb, nj - 1), 1) = s_color;
        hsv_img(x, std::min(int(j_max[i]) + bb, nj - 1), 2) = v_color;
      }
    }
    // draw verticle lines
    for (int y = j_min[i] + 1; y < j_max[i]; y++) {
      for (int bb = 0; bb <= bb_rad; bb++) {
        hsv_img(std::max(int(i_min[i]) - bb, 0), y, 0) = h_color;
        hsv_img(std::max(int(i_min[i]) - bb, 0), y, 1) = s_color;
        hsv_img(std::max(int(i_min[i]) - bb, 0), y, 2) = v_color;
        hsv_img(std::min(int(i_max[i]) + bb, ni - 1), y, 0) = h_color;
        hsv_img(std::min(int(i_max[i]) + bb, ni - 1), y, 1) = s_color;
        hsv_img(std::min(int(i_max[i]) + bb, ni - 1), y, 2) = v_color;
      }
    }
  }

  // print out the vgl_polygon and associated conf values
  std::ofstream ofs;
  ofs.open(out_geo_file.c_str());
  ofs << "{\n"
      << "  \"type\": \"FeatureCollection\",\n"
      << "  \"features\": [\n";
  for (int i = 0; i < i_min.size(); i++) {
    unsigned n_pt = poly[i][0].size();
    ofs << "    {\n"
        << "      \"geometry\": {\n"
        << "        \"type\": \"Polygon\",\n"
        << "        \"coordinates\": [\n"
        << "          [\n";
    for (unsigned p_idx = 0; p_idx < n_pt; p_idx++) {
      ofs << "            [\n"
          << "              " << poly[i][0][p_idx].x() << ",\n"
          << "              " << poly[i][0][p_idx].y() << "\n";
      if (p_idx == (n_pt-1))
        ofs << "            ]\n";
      else
        ofs << "            ],\n";
    }
    ofs << "          ]\n"
        << "        ]\n"
        << "      },\n"
        << "      \"type\": \"Feature\",\n"
        << "      \"properties\": {\n"
        << "        \"confidence\": " << conf[i] << "\n"
        << "      }\n";
    if (i == i_min.size() - 1)
      ofs << "    }\n";
    else
      ofs << "    },\n";
  }
  ofs << "  ]\n"
      << "}\n";

  ofs.close();

  // convert back to rgb image and then convert to vxl_byte for saving
  vil_image_view<float> rgb_img_bb;
  rgb_img_bb.set_size(ni, nj, 3);
  for (int j = 0; j < nj; j++) {
    for (int i = 0; i < ni; i++) {
      vil_colour_space_HSV_to_RGB(hsv_img(i, j,0), hsv_img(i, j,1), hsv_img(i, j,2), &rgb_img_bb(i, j, 0), &rgb_img_bb(i, j, 1), &rgb_img_bb(i, j, 2));
    }
  }
  vil_image_view<vxl_byte> to_save;
  vil_convert_stretch_range_limited(rgb_img_bb, to_save, (float) 0.0, (float) 1.0, (vxl_byte)0, (vxl_byte)255);

  // convert to rgb byte image and save
  if (!vil_save(to_save, out_file.c_str())) {
    std::cerr << "error saving spectral angle map";
    return 1;
  }
};
