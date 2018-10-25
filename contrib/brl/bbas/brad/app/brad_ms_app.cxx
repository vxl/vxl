#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/vil_save.h>
#include <vgl/vgl_polygon.h>
#include <vnl/vnl_math.h>
#include <vul/vul_awk.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_image_atmospherics_est.h>
#include <brad/brad_spectral_angle_mapper.h>
#include <brad/brad_multispectral_functions.h>
#include <brad_worldview3_functions.h>
#include <brad/brad_sam_template_match.h>

int main(int  /*argc*/, char *  /*argv*/[])
{

  // =========================template matching=======================
   // Set arguments
  std::string mul_file(
    "C:/Users/sca0161/Documents/data/sevastopol/2015_08_21_WV03/D_20150821_090655_1040010010A4C400_P002_MUL/15AUG21090655-M1BS-056107310010_01_P002.NTF");
     //"C:/Users/sca0161/Documents/sevastopol2/104001001E0AA000_P005_MUL/16JUL21091539-M1BS-056339611010_01_P005.NTF");
     //"D:/data/core3d/sevastopol2/104001001E0AA000_P005_MUL/16JUL21091539-M1BS-056339611010_01_P005.NTF");
  std::string swir_file(
    "C:/Users/sca0161/Documents/data/sevastopol/2015_08_21_WV03/D_20150821_090654_104A010010A4C400_P001_SWR/15AUG21090654-A1BS-056530600010_01_P001.NTF");
     //"C:/Users/sca0161/Documents/sevastopol2/104A01001E0AA000_P001_SWR/16JUL21091536-A1BS-056339460010_01_P001.NTF");
     //"D:/data/core3d/sevastopol2/104A01001E0AA000_P001_SWR/16JUL21091536-A1BS-056339460010_01_P001.NTF");


   std::cerr << "Composing 16-band image\n";
   vil_image_view<float> comp_img;
   brad_compose_16band_wv3_img(
     mul_file, swir_file, comp_img, 1.0f);

   // Correct for atmospherics
   float mean_albedo = 0.3;
   vil_image_view<float> cal_img;
   brad_estimate_reflectance_image_multi(comp_img, mean_albedo, cal_img);
   std::cerr << "done with image corrections\n";
   save_corrected_wv3(cal_img, "C:/Users/sca0161/Documents/data/sevastopol/2015_08_21_WV03/tif band float P002_MUL P001_SWR/");

   return 0;
   std::string out_dir(
     "C:/Users/sca0161/Documents/sevastopol2/cpp result/template matching/");
 // vil_image_view<float> cal_img;
  load_corrected_wv3("C:/Users/sca0161/Documents/sevastopol2/cpp result/tif band float/", cal_img);

  vil_image_view<float> sam;

   //load boat mask
   std::string dock_mask_file("C:/Users/sca0161/Documents/sevastopol2/masks/mask_ship_pair.png");
   vil_image_resource_sptr mask_rsc = vil_load_image_resource(dock_mask_file.c_str());
   vil_image_view<vxl_uint_8> mask_16 = mask_rsc->get_view();
   vil_image_view<vxl_byte> mask(mask_16.ni(), mask_16.nj());
   mask.fill(0);
   for (int j = 0; j < mask_16.nj(); j++) {
     for (int i = 0; i < mask_16.ni(); i++) {
       if (mask_16(i, j, 0) > 0) mask(i, j) = 255;
     }
   }

   // crop mask/calibrated image down to just the port area
   int crop_x = 2599;//3648;
   int crop_y = 3374;//3747;
   int crop_width = 1510;
   int crop_height = 1585;
   mask = vil_crop(mask, crop_x, crop_width, crop_y, crop_height);
   cal_img = vil_crop(cal_img, crop_x, crop_width, crop_y, crop_height);
   //vil_save(mask, (out_dir + "mask.png").c_str());

   brad_template_bb(mask, crop_x, crop_y, crop_width, crop_height);
   //crop_x = 1049;//3648;
   //crop_y = 373;//3747;
   //crop_width = 14;
   //crop_height = 65;
   vil_image_view<float> chip = vil_crop(cal_img, crop_x, crop_width, crop_y, crop_height);

   //vil_image_view<float> cur_plane;
   //for (int i = 0; i < cal_img.nplanes(); i++) {
   //  cur_plane = vil_plane(cal_img, i);
   //  vil_save(cur_plane, (out_dir + "/band" + std::to_string(i) + ".tif").c_str());
   //  cur_plane = vil_plane(chip, i);
   //  vil_save(cur_plane, (out_dir + "/chip" + std::to_string(i) + ".tif").c_str());
   //}


   vil_image_view<vxl_byte> chip_mask = vil_crop(mask, crop_x, crop_width, crop_y, crop_height);

   vil_save(chip_mask, (out_dir + "chip_mask.png").c_str());
   brad_sam_template_match(cal_img, chip, chip_mask, sam, 360);

   vil_image_view<vxl_byte> vis;
   vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
    vil_save(vis, (out_dir + "sam_cos.png").c_str());
 // // =========================classification========================
 // //---------------------------with SWIR----------------------
 // // Set arguments
 // std::string mul_file(
 //   "C:/Users/sca0161/Documents/sevastopol2/104001001E0AA000_P005_MUL/16JUL21091539-M1BS-056339611010_01_P005.NTF");
 //   //"D:/data/core3d/sevastopol2/104001001E0AA000_P005_MUL/16JUL21091539-M1BS-056339611010_01_P005.NTF");
 // std::string swir_file(
 //   "C:/Users/sca0161/Documents/sevastopol2/104A01001E0AA000_P001_SWR/16JUL21091536-A1BS-056339460010_01_P001.NTF");
 //   //"D:/data/core3d/sevastopol2/104A01001E0AA000_P001_SWR/16JUL21091536-A1BS-056339460010_01_P001.NTF");

 // std::string out_dir(
 //   "C:/Users/sca0161/Documents/sevastopol2/cpp result/");
 //   //"D:/results/ms/");
 //
 // std::cerr << "Composing 16-band image\n";
 // vil_image_view<float> comp_img;
 // brad_compose_16band_wv3_img(
 //   mul_file, swir_file, comp_img, 1.0f);

 // // Correct for atmospherics
 // float mean_albedo = 0.3;
 // vil_image_view<float> cal_img;
 // brad_estimate_reflectance_image_multi(comp_img, mean_albedo, cal_img);
 // std::cerr << "done with image corrections\n";

 // // save image
 // save_corrected_wv3(cal_img, "C:/Users/sca0161/Documents/sevastopol2/cpp result/tif band float");

 // // load image
 //// vil_image_view<float> cal_img;
 // load_corrected_wv3("C:/Users/sca0161/Documents/sevastopol2/cpp result/tif band float", cal_img);
 // return 0;
 // // visible byte image to be used from saving
 // vil_image_view<vxl_byte> vis;

 // //// Create an RGB image for visualization
 // //std::cerr << "Saving RGB image\n";
 // //vil_image_view<float> rgb_img(cal_img.ni(), cal_img.nj(), 3);
 // //for (int y = 0; y < cal_img.nj(); y++)
 // //  for (int x = 0; x < cal_img.ni(); x++) {
 // //    rgb_img(x, y, 0) = cal_img(x, y, 4);
 // //    rgb_img(x, y, 1) = cal_img(x, y, 2);
 // //    rgb_img(x, y, 2) = cal_img(x, y, 1);
 // //  }
 // //vil_convert_stretch_range_limited(
 // //  rgb_img, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "rgb.png").c_str());

 // // Write bands
 // //std::cerr << "Saving band images\n";
 // //for (int p = 0; p < 16; p++) {
 // //  vil_image_view<float> plane = vil_plane(cal_img, p);
 // //  std::stringstream ss;
 // //  ss << out_dir << "cal_" << p << ".png";
 // //  vil_image_view<vxl_byte> vis;
 // //  vil_convert_stretch_range_limited(
 // //    plane, vis, 0.0f, 1.0f);
 // //  vil_save(vis, ss.str().c_str());
 // //}
 // //for (int p = 0; p < 16; p++) {
 // //  vil_image_view<float> plane = vil_plane(comp_img, p);
 // //  std::stringstream ss;
 // //  ss << out_dir << "comp_" << p << ".png";
 // //  vil_image_view<vxl_byte> vis;
 // //  vil_convert_stretch_range_limited(
 // //    plane, vis, 0.0f, 1.0f);
 // //  vil_save(vis, ss.str().c_str());
 // //}

 // std::string aster_dir(
 //   "C:/Users/sca0161/Documents/MATLAB/sevastopol_wv03/170306_MaterialClassification/aster_data/sevastopol/*.spectrum.txt");
 //   //"C:/Users/sca0161/Documents/MATLAB/sevastopol_wv03/170306_MaterialClassification/aster_data/new/*.spectrum.txt");
 //   //"D:/data/core3d/ASTER/data/caras/*.txt");

 // //load dock mask
 // std::string dock_mask_file("C:/Users/sca0161/Documents/sevastopol2/cpp result/mask_dock.png");
 // vil_image_resource_sptr dock_mask_rsc = vil_load_image_resource(dock_mask_file.c_str());
 // vil_image_view<vxl_uint_8> dock_mask_16 = dock_mask_rsc->get_view();
 // vil_image_view<bool> dock_mask(dock_mask_16.ni(), dock_mask_16.nj());
 // dock_mask.fill(false);
 // for (int j = 0; j < dock_mask_16.nj(); j++) {
 //   for (int i = 0; i < dock_mask_16.ni(); i++) {
 //     if (dock_mask_16(i, j, 0) > 0) dock_mask(i, j) = true;
 //   }
 // }

 // //load military mask
 // std::string military_mask_file("C:/Users/sca0161/Documents/sevastopol2/cpp result/mask_military_ship.png");
 // vil_image_resource_sptr military_mask_rsc = vil_load_image_resource(military_mask_file.c_str());
 // vil_image_view<vxl_uint_8> military_mask_16 = military_mask_rsc->get_view();
 // vil_image_view<bool> military_mask(military_mask_16.ni(), military_mask_16.nj());
 // military_mask.fill(false);
 // for (int j = 0; j < military_mask_16.nj(); j++) {
 //   for (int i = 0; i < military_mask_16.ni(); i++) {
 //     if (military_mask_16(i, j, 0) > 0) military_mask(i, j) = true;
 //   }
 // }

 // // Setup WV3 bands
 // std::vector<float> bands_min, bands_max;
 // brad_wv3_bands(bands_min, bands_max);
 // // create aster objects
 // std::cerr << "Loading ASTER directory\n";
 // brad_spectral_angle_mapper aster(bands_min, bands_max);
 // //aster.add_aster_dir(aster_dir);
 // ////aster.add_aster_dir("C:/Users/sca0161/Documents/MATLAB/sevastopol_wv03/170306_MaterialClassification/aster_data/temp metal/*.spectrum.txt");
 // //std::cerr << "Adding material dock to aster\n";
 // //aster.add_material("dock", cal_img, dock_mask);
 // //std::cerr << "Adding material military (ship) to aster\n";
 // //aster.add_material("military", cal_img, military_mask);
 //
 // std::cerr << "Done loading ASTER directory\n";


 // std::cerr << "Classifying materials\n";

 // vil_image_view<int> class_img;
 // vil_image_view<float> conf_img;
 // std::string save_name;
 // std::vector<std::string> mat_list;

 // //mat_list.clear();
 // //mat_list.push_back("paving");
 // //mat_list.push_back("roofing");
 // //mat_list.push_back("soil");
 // //mat_list.push_back("vegetation");
 // //save_name = "";
 // //for (int i = 0; i < mat_list.size(); i++) {
 // //  save_name += ("_" + mat_list[i]).c_str();
 // //}
 // //aster.aster_classify_material(cal_img, mat_list, 0.8, class_img, conf_img);
 // //vil_convert_stretch_range_limited(class_img, vis, -1, int(mat_list.size())-1);
 // //vil_save(vis, (out_dir + "class_img" + save_name + ".png").c_str());
 // //vil_convert_stretch_range_limited(conf_img, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "conf_img" + save_name + ".png").c_str());

 // std::cerr << "Creating spectral angle maps\n";
 // vil_image_view<float> sam;
 // //aster.compute_sam_img(cal_img, "dock", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_dock.png").c_str());
 // //aster.compute_sam_img(cal_img, "asphalt", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_asphalt.png").c_str());
 // //aster.compute_sam_img(cal_img, "building", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_building.png").c_str());
 // //aster.compute_sam_img(cal_img, "concrete", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_concrete.png").c_str());
 // //aster.compute_sam_img(cal_img, "manmade", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_manmade.png").c_str());
 // //aster.compute_sam_img(cal_img, "soil", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_soil.png").c_str());
 // //aster.compute_sam_img(cal_img, "vegetation", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_vegetation.png").c_str());
 // //aster.compute_sam_img(cal_img, "water", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_water.png").c_str());
 // //aster.compute_sam_img(cal_img, "inceptisol", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_inceptisol.png").c_str());
 // //aster.compute_sam_img(cal_img, "paving", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_paving.png").c_str());
 // //aster.compute_sam_img(cal_img, "roofing", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_roofing.png").c_str());
 // //aster.compute_sam_img(cal_img, "trees", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_trees.png").c_str());
 // //aster.compute_sam_img(cal_img, "military", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_military.png").c_str());


 // //aster.clear_library();
 // //std::cerr << "cleared library\n";
 // //aster.add_material("dock", comp_img, dock_mask);
 // //std::cerr << "added material dock (uncalibrated) to aster\n";
 // //aster.compute_sam_img(comp_img, "dock", sam);
 // //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
 // //vil_save(vis, (out_dir + "sam_dock_uncalibrated.png").c_str());

 // //  // Classify using normalized indices
 // //  std::cerr << "Computing normalized index images\n";
 // //  vil_image_view<float> ndwi, ndvi, ndsi, nhfd;
 // //  brad_compute_normalized_index_image(cal_img, 0, 7, ndwi);
 // //  vil_convert_stretch_range_limited(ndwi, vis, -1.0f, 1.0f);
 // //  vil_save(vis, (out_dir + "ndwi_swir.png").c_str());
 // //  brad_compute_normalized_index_image(cal_img, 7, 4, ndvi);
 // //  vil_convert_stretch_range_limited(ndvi, vis, -1.0f, 1.0f);
 // //  vil_save(vis, (out_dir + "ndvi_swir.png").c_str());
 // //  brad_compute_normalized_index_image(cal_img, 3, 2, ndsi);
 // //  vil_convert_stretch_range_limited(ndsi, vis, -1.0f, 1.0f);
 // //  vil_save(vis, (out_dir + "ndsi_swir.png").c_str());
 // //  brad_compute_normalized_index_image(cal_img, 5, 0, nhfd);
 // //  vil_convert_stretch_range_limited(nhfd, vis, -1.0f, 1.0f);
 // //  vil_save(vis, (out_dir + "nhfd_swir.png").c_str());
 // return 0;
};
