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

int main(int argc, char * argv[])
{

  //---------------------------with SWIR----------------------
  // Set arguments
  std::string mul_file(
    "C:/Users/sca0161/Documents/sevastopol2/104001001E0AA000_P005_MUL/16JUL21091539-M1BS-056339611010_01_P005.NTF");
    //"D:/data/core3d/sevastopol2/104001001E0AA000_P005_MUL/16JUL21091539-M1BS-056339611010_01_P005.NTF");
  std::string swir_file(
    "C:/Users/sca0161/Documents/sevastopol2/104A01001E0AA000_P001_SWR/16JUL21091536-A1BS-056339460010_01_P001.NTF");
    //"D:/data/core3d/sevastopol2/104A01001E0AA000_P001_SWR/16JUL21091536-A1BS-056339460010_01_P001.NTF");

  std::string out_dir(
    "C:/Users/sca0161/Documents/sevastopol2/cpp result/");
    //"D:/results/ms/");
  
  std::cerr << "Composing 16-band image\n";
  vil_image_view<float> comp_img;
  brad_compose_16band_wv3_img(
    mul_file, swir_file, comp_img, 1.0f);

  // Correct for atmospherics
  float mean_albedo = 0.3;
  vil_image_view<float> cal_img;
  brad_estimate_reflectance_image_multi(comp_img, mean_albedo, cal_img);
  std::cerr << "done with image corrections\n";

  // visible byte image to be used from saving
  vil_image_view<vxl_byte> vis;

  //// Create an RGB image for visualization
  //std::cerr << "Saving RGB image\n";
  //vil_image_view<float> rgb_img(cal_img.ni(), cal_img.nj(), 3);
  //for (int y = 0; y < cal_img.nj(); y++)
  //  for (int x = 0; x < cal_img.ni(); x++) {
  //    rgb_img(x, y, 0) = cal_img(x, y, 4);
  //    rgb_img(x, y, 1) = cal_img(x, y, 2);
  //    rgb_img(x, y, 2) = cal_img(x, y, 1);
  //  }
  //vil_convert_stretch_range_limited(
  //  rgb_img, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "rgb.png").c_str());

  ////Write bands
  //std::cerr << "Saving band images\n";
  //for (int p = 0; p < 16; p++) {
  //  vil_image_view<float> plane = vil_plane(comp_img, p);
  //  std::stringstream ss;
  //  ss << out_dir << p << ".png";
  //  vil_image_view<vxl_byte> vis;
  //  vil_convert_stretch_range_limited(
  //    plane, vis, 0.0f, 1.0f);
  //  vil_save(vis, ss.str().c_str());
  //}

  std::string aster_dir(
    "C:/Users/sca0161/Documents/MATLAB/sevastopol_wv03/170306_MaterialClassification/aster_data/sevastopol/*.spectrum.txt");
    //"C:/Users/sca0161/Documents/MATLAB/sevastopol_wv03/170306_MaterialClassification/aster_data/new/*.spectrum.txt");
    //"D:/data/core3d/ASTER/data/caras/*.txt");

  ////load dock mask
  //std::string dock_mask_file("C:/Users/sca0161/Documents/sevastopol2/cpp result/mask_dock.png");
  //vil_image_resource_sptr dock_mask_rsc = vil_load_image_resource(dock_mask_file.c_str());
  //vil_image_view<vxl_uint_8> dock_mask_16 = dock_mask_rsc->get_view();
  //vil_image_view<bool> dock_mask(dock_mask_16.ni(), dock_mask_16.nj());
  //dock_mask.fill(false);
  //for (int j = 0; j < dock_mask_16.nj(); j++) {
  //  for (int i = 0; i < dock_mask_16.ni(); i++) {
  //    if (dock_mask_16(i, j, 0) > 0) dock_mask(i, j) = true;
  //  }
  //}

  // Setup WV3 bands
  std::vector<float> bands_min, bands_max;
  brad_wv3_bands(bands_min, bands_max);
  // create aster objects
  std::cerr << "Loading ASTER directory\n";
  brad_spectral_angle_mapper aster(bands_min, bands_max);
  aster.add_aster_dir(aster_dir);
  //aster.add_aster_dir("C:/Users/sca0161/Documents/MATLAB/sevastopol_wv03/170306_MaterialClassification/aster_data/jhu.becknic.soil/*.spectrum.txt");
  //std::cerr << "Adding material dock to aster\n";
  //aster.add_material("dock", cal_img, dock_mask);
  
  
  std::cerr << "Classifying materials\n";
  vil_image_view<int> class_img;
  vil_image_view<float> conf_img;
  //aster.aster_classify_material(cal_img, { "asphalt", "building", "concrete", "vegetation" }, 0.8, class_img, conf_img);
  //vil_convert_stretch_range_limited(class_img, vis, -1, 4);
  //vil_save(vis, (out_dir + "class_img_asphalt_building_concrete_vegetation.png").c_str());
  //vil_convert_stretch_range_limited(conf_img, vis, -0.0f, 1.0f);
  //vil_save(vis, (out_dir + "conf_img_asphalt_building_concrete_vegetation.png").c_str());

  //aster.aster_classify_material(cal_img, { "asphalt", "building", "concrete" }, 0.8, class_img, conf_img);
  //vil_convert_stretch_range_limited(class_img, vis, -1, 4);
  //vil_save(vis, (out_dir + "class_img_asphalt_building_concrete.png").c_str());
  //vil_convert_stretch_range_limited(conf_img, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "conf_img_asphalt_building_concrete.png").c_str());

  //aster.aster_classify_material(cal_img, { "asphalt", "building", "concrete", "vegetation", "water" }, 0.8, class_img, conf_img);
  //vil_convert_stretch_range_limited(class_img, vis, -1, 4);
  //vil_save(vis, (out_dir + "class_img_asphalt_building_concrete_vegetation_water.png").c_str());
  //vil_convert_stretch_range_limited(conf_img, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "conf_img_asphalt_building_concrete_vegetation_water.png").c_str());

  aster.aster_classify_material(cal_img, { "manmade", "soil", "vegetation", "water" }, 0.8, class_img, conf_img);
  vil_convert_stretch_range_limited(class_img, vis, -1, 4);
  vil_save(vis, (out_dir + "class_img_manmade_soil_vegetation_water.png").c_str());
  vil_convert_stretch_range_limited(conf_img, vis, 0.0f, 1.0f);
  vil_save(vis, (out_dir + "conf_img_manmade_soil_vegetation_water.png").c_str());

  //aster.aster_classify_material(cal_img, { "alfisol", "aridisol", "entisol", "inceptisol", "lunar", "mollisol", "spodosol", "utilsol", "vertisol" }, 0.8, class_img, conf_img);
  //vil_convert_stretch_range_limited(class_img, vis, -1, 8);
  //vil_save(vis, (out_dir + "class_img_soil_subclasses.png").c_str());
  //vil_convert_stretch_range_limited(conf_img, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "conf_img_soil_subclasses.png").c_str());

  std::cerr << "Creating spectral angle maps\n";
  vil_image_view<float> sam;
  //aster.compute_sam_img(cal_img, "dock", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_dock.png").c_str());
  //aster.compute_sam_img(cal_img, "asphalt", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_asphalt.png").c_str());
  //aster.compute_sam_img(cal_img, "building", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_building.png").c_str());
  //aster.compute_sam_img(cal_img, "concrete", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_concrete.png").c_str());
  //aster.compute_sam_img(cal_img, "manmade", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_manmade.png").c_str());
  //aster.compute_sam_img(cal_img, "soil", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_soil.png").c_str());
  //aster.compute_sam_img(cal_img, "vegetation", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_vegetation.png").c_str());
  //aster.compute_sam_img(cal_img, "water", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_water.png").c_str());

  //// soil subclass sam
  //aster.compute_sam_img(cal_img, "alfisol", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_alfisol.png").c_str());
  //aster.compute_sam_img(cal_img, "aridisol", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_aridisol.png").c_str());
  //aster.compute_sam_img(cal_img, "entisol", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_entisol.png").c_str());
  //aster.compute_sam_img(cal_img, "inceptisol", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_inceptisol.png").c_str());
  //aster.compute_sam_img(cal_img, "lunar", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_lunar.png").c_str());
  //aster.compute_sam_img(cal_img, "mollisol", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_mollisol.png").c_str());
  //aster.compute_sam_img(cal_img, "spodosol", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_spodosol.png").c_str());
  //aster.compute_sam_img(cal_img, "utilisol", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_utilisol.png").c_str());
  //aster.compute_sam_img(cal_img, "vertisol", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_vertisol.png").c_str());

  //aster.clear_library();
  //std::cerr << "cleared library\n";
  //aster.add_material("dock", comp_img, dock_mask);
  //std::cerr << "added material dock (uncalibrated) to aster\n";
  //aster.compute_sam_img(comp_img, "dock", sam);
  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
  //vil_save(vis, (out_dir + "sam_dock_uncalibrated.png").c_str());

  //  // Classify using normalized indices
  //  std::cerr << "Computing normalized index images\n";
  //  vil_image_view<float> ndwi, ndvi, ndsi, nhfd;
  //  brad_compute_normalized_index_image(cal_img, 0, 7, ndwi);
  //  vil_convert_stretch_range_limited(ndwi, vis, -1.0f, 1.0f);
  //  vil_save(vis, (out_dir + "ndwi_swir.png").c_str());
  //  brad_compute_normalized_index_image(cal_img, 7, 4, ndvi);
  //  vil_convert_stretch_range_limited(ndvi, vis, -1.0f, 1.0f);
  //  vil_save(vis, (out_dir + "ndvi_swir.png").c_str());
  //  brad_compute_normalized_index_image(cal_img, 3, 2, ndsi);
  //  vil_convert_stretch_range_limited(ndsi, vis, -1.0f, 1.0f);
  //  vil_save(vis, (out_dir + "ndsi_swir.png").c_str());
  //  brad_compute_normalized_index_image(cal_img, 5, 0, nhfd);
  //  vil_convert_stretch_range_limited(nhfd, vis, -1.0f, 1.0f);
  //  vil_save(vis, (out_dir + "nhfd_swir.png").c_str());
  return 0;


  // ----------------------just MUL---------------------------------
//  // Set arguments
//  std::string aster_dir(
//    "C:/Users/sca0161/Documents/MATLAB/sevastopol_wv03/170306_MaterialClassification/aster_data/new/*.spectrum.txt");
//  //"C:/Users/thomas.pollard/Desktop/170306_MaterialClassification/aster_data/new/*.txt");
//  std::string mul_file(
//    //"D:/data/core3d/sevastopol_wv03/2017_01_10_WV03/056247234010_01_003/056247234010_01/056247234010_01_P001_MUL/17JAN10091703-M1BS-056247234010_01_P001.NTF");
//    //"D:/data/core3d/sevastopol_wv03/2016_08_15_WV03/056247235010_01_003/056247235010_01/056247235010_01_P001_MUL/16AUG15092138-M1BS-056247235010_01_P001.NTF");
//    "C:/Users/sca0161/Documents/MATLAB/sevastopol_wv03/2016_08_15_WV03/056247235010_01_003/056247235010_01/056247235010_01_P001_MUL/16AUG15092138-M1BS-056247235010_01_P001.NTF");
//    //"C:/Users/sca0161/Documents/sevastopol2/104001001E0AA000_P005_MUL/16JUL21091539-M1BS-056339611010_01_P005.NTF");
//  //"D:/data/core3d/sevastopol_wv03/2017_01_10_WV03/056247234010_01_003/056247234010_01/056247234010_01_P001_MUL/17JAN10091703-M1BS-056247234010_01_P001.TIF");
//  //"D:/data/core3d/sevastopol_wv03/2016_08_15_WV03/056247235010_01_003/056247235010_01/056247235010_01_P001_MUL/16AUG15092138-M1BS-056247235010_01_P001.TIF");
////float mean_albedo = 0.15f;
//
////load dock mask
//std::string dock_mask_file("C:/Users/sca0161/Documents/multispectral results/cpp/port classification full image/mask_dock.png");
//vil_image_resource_sptr dock_mask_rsc = vil_load_image_resource(dock_mask_file.c_str());
//
//std::cerr << "Loading images\n";
//vil_image_view<vxl_uint_8> dock_mask_16 = dock_mask_rsc->get_view();
//vil_image_view<bool> dock_mask(dock_mask_16.ni(), dock_mask_16.nj());
//dock_mask.fill(false);
//for (int j = 0; j < dock_mask_16.nj(); j++) {
//  for (int i = 0; i < dock_mask_16.ni(); i++) {
//    if (dock_mask_16(i, j, 0) > 0) dock_mask(i, j) = true;
//  }
//}
//
//
//  //std::string out_dir("C:/Users/sca0161/Documents/multispectral results/cpp/");
//  //std::string out_dir("C:/Users/sca0161/Documents/sevastopol2/cpp result/just mul/");
//  std::string out_dir("C:/Users/sca0161/Documents/multispectral results/cpp/port classification full image/");
//  // Setup WV3 bands
//  std::vector<float> bands_min, bands_max;
//  brad_wv3_bands(bands_min, bands_max, 8);  
//
//  // create aster objects
//  std::cerr << "Loading ASTER directory\n";
//  brad_spectral_angle_mapper aster(bands_min, bands_max);
//  aster.add_aster_dir(aster_dir);
//  std::cerr << "done with aster initialization\n";
//
//  // Load the images
//  //vil_image_resource_sptr pan_img = vil_load_image_resource(pan_file.c_str());
//  //std::cerr << pan_img->nplanes() << ' ' << pan_img->ni() << ' ' << pan_img->nj() << '\n';
//  vil_image_resource_sptr mul_img = vil_load_image_resource(mul_file.c_str());
//  std::cerr << "Loaded a " << mul_img->ni() << 'x' << mul_img->nj() 
//    << " image with " << mul_img->nplanes() << " channels\n";
//  unsigned crop_x = 0, crop_y = 0, crop_w = mul_img->ni(), crop_h = mul_img->nj();
//  //unsigned crop_x = 3500, crop_y = 500, crop_w = 3500, crop_h = 2000;
//  // Get a cropped view and convert to float
//  vil_image_view<vxl_uint_16> mul_crop = 
//    vil_crop( mul_img, crop_x, crop_w, crop_y, crop_h )->get_view();
//  vil_image_view<float> mul_f;
//  vil_convert_cast(mul_crop, mul_f);
//
//  // Load metadata
//  brad_image_metadata meta(vul_file::strip_extension(mul_file)+".IMD");
//
//
//  int num_bands = mul_f.nplanes();
//  // Calibrate the image, lifted from 
//  // brad_nitf_abs_radiometric_calibration_process
//  for (int b = 0; b < num_bands; b++) {
//    vil_image_view<float> band = vil_plane(mul_f, b); 
//    vil_math_scale_and_offset_values(
//      band, meta.gains_[b + 1].first, meta.gains_[b + 1].second);
//  }
//
//  // Correct for atmospherics
//  float mean_albedo = 0.3;
//  vil_image_view<float> cal_img;
//  brad_estimate_reflectance_image_no_meta(mul_f, mean_albedo, cal_img);
//  std::cerr << "done with image corrections\n";
//
//  // visible byte image to be used from saving
//  vil_image_view<vxl_byte> vis;
//
// /* std::cerr << "Classifying materials\n";
//  vil_image_view<int> class_img;
//  vil_image_view<float> conf_img;
//  aster.aster_classify_material(cal_img, { "asphalt", "building", "concrete", "vegetation" }, 0.95, class_img, conf_img);
//  vil_convert_stretch_range_limited(class_img , vis, -1, 3);
//  vil_save(vis, (out_dir + "class_img_asphalt_building_concrete_vegetation.png").c_str());
//  vil_convert_stretch_range_limited(conf_img, vis, -0.0f, 1.0f);
//  vil_save(vis, (out_dir + "conf_img_asphalt_building_concrete_vegetation.png").c_str());
//
//  aster.aster_classify_material(cal_img, { "asphalt", "building", "concrete" }, 0.95, class_img, conf_img);
//  vil_convert_stretch_range_limited(class_img, vis, -1, 3);
//  vil_save(vis, (out_dir + "class_img_asphalt_building_concrete.png").c_str());
//  vil_convert_stretch_range_limited(conf_img, vis, 0.0f, 1.0f);
//  vil_save(vis, (out_dir + "conf_img_asphalt_building_concrete.png").c_str());*/
//
//  aster.add_material("dock", cal_img, dock_mask);
//  vil_image_view<float> sam;
//  aster.compute_sam_img(cal_img, "dock", sam);
//  vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
//  vil_save(vis, (out_dir + "sam_dock.png").c_str());
//  //aster.compute_sam_img(cal_img, "asphalt", sam);
//  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
//  //vil_save(vis, (out_dir + "sam_asphalt.png").c_str());
//  //aster.compute_sam_img(cal_img, "building", sam);
//  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
//  //vil_save(vis, (out_dir + "sam_building.png").c_str());
//  //aster.compute_sam_img(cal_img, "concrete", sam);
//  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
//  //vil_save(vis, (out_dir + "sam_concrete.png").c_str());
//  //aster.compute_sam_img(cal_img, "vegetation", sam);
//  //vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
//  //vil_save(vis, (out_dir + "sam_vegetation.png").c_str());
//
//  aster.clear_library();
//  aster.add_material("dock", mul_f, dock_mask);
//  std::cerr << "added material dock (uncalibrated) to aster\n";
//  aster.compute_sam_img(mul_f, "dock", sam);
//  vil_convert_stretch_range_limited(sam, vis, 0.0f, 1.0f);
//  vil_save(vis, (out_dir + "sam_dock_uncalibrated.png").c_str());
//
//
//  //// Create an RGB image for visualization
//  //std::cerr << "Saving RGB image\n";
//  //vil_image_view<float> rgb_img(cal_img.ni(), cal_img.nj(), 3);
//  //for (int y = 0; y < cal_img.nj(); y++)
//  //  for (int x = 0; x < cal_img.ni(); x++) {
//  //    rgb_img(x, y, 0) = cal_img(x, y, 4);
//  //    rgb_img(x, y, 1) = cal_img(x, y, 2);
//  //    rgb_img(x, y, 2) = cal_img(x, y, 1);
//  //  }
//
//  //vil_convert_stretch_range_limited(
//  //  rgb_img, vis, 0.0f, 1.0f);
//  //vil_save(vis, (out_dir + "rgb.png").c_str());
//
//  //// Classify using normalized indices
//  //std::cerr << "Computing normalized index images\n";
//  //vil_image_view<float> ndwi, ndvi, ndsi, nhfd;
//  //brad_compute_normalized_index_image(cal_img, 0, 7, ndwi);
//  //vil_convert_stretch_range_limited(ndwi, vis, -1.0f, 1.0f);
//  //vil_save(vis, (out_dir + "ndwi.png").c_str());
//  //brad_compute_normalized_index_image(cal_img, 7, 4, ndvi);
//  //vil_convert_stretch_range_limited(ndvi, vis, -1.0f, 1.0f);
//  //vil_save(vis, (out_dir + "ndvi.png").c_str());
//  //brad_compute_normalized_index_image(cal_img, 3, 2, ndsi);
//  //vil_convert_stretch_range_limited(ndsi, vis, -1.0f, 1.0f);
//  //vil_save(vis, (out_dir + "ndsi.png").c_str());
//  //brad_compute_normalized_index_image(cal_img, 5, 0, nhfd);
//  //vil_convert_stretch_range_limited(nhfd, vis, -1.0f, 1.0f);
//  //vil_save(vis, (out_dir + "nhfd.png").c_str());
//  return 0;
};
