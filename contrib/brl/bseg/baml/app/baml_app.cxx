#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_convert.h>
#include <vil/vil_crop.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_gauss_filter.h>

#include <vul/vul_file.h>

#include <vgl/vgl_intersection.h>

#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_rational_camera.h>

#include <bil/bil_convert_to_grey.h>

#include <baml/baml_warp.h>
#include <baml/baml_detect_change.h>
#include "baml_utilities.h"


int main(int argc,char * argv[])
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // CHANGE DETECTION
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // small portion of runway hamadam
 // std::string tar_name("20160619_064846_0c72");

  // std::string ref_name("20160623_050936_0c64");
  // std::string ref_name("20160701_091757_0c64");
  // std::string ref_name("20160701_091758_0c64");
  // std::string ref_name("20160702_103254_0c59");
  // std::string ref_name("20160705_092219_0c81");
  // std::string ref_name("20160717_043904_0c19");
  // std::string ref_name("20160817_135113_0c68");
  // std::string ref_name("20160820_073052_0c76");
  // std::string ref_name("20160821_063825_0e20");
  // std::string ref_name("20160821_063826_0e20");
  // std::string ref_name("20160822_064308_0c1b");
  // std::string ref_name("20160827_120833_0c13");
  // std::string ref_name("20160831_063745_0e0d");
  // std::string ref_name("20160902_094643_0c19");
  // std::string ref_name("20160911_063924_0e20");
  // std::string ref_name("20160911_063925_0e20");
  // std::string ref_name("20160915_044028_1_0c46");
  // std::string ref_name("20160917_063903_0e30");
  // std::string ref_name("20160926_064007_0e0f");
  // std::string ref_name("20160926_064008_0e0f");
  // std::string ref_name("20161007_064035_0e20");
  // std::string ref_name("20161011_064057_0e16");
  // std::string ref_name("20161014_092852_0c65");
  // std::string ref_name("20161016_083937_0c22");
 //  std::string ref_name("20161024_052928_0c19");

 // vpgl_lvcs lvcs(35.2080385626545, 48.70485305698821, 1675);
 // std::string target_img_file("C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/" + tar_name + ".tif");
 // std::cout << "target: " << target_img_file << "\n";
 // std::string target_cam_file("C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/" + tar_name + "_RPC.txt");
 // std::string ref_img_file("C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/" + ref_name + ".tif");
 // std::cout << "target: " << target_img_file << "\n";

 // std::string ref_cam_file("C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/" + ref_name + "_RPC.txt");
 //// std::string output_namebase("C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/output");
 // vgl_box_2d<int> region(2922, 3037, 456, 551);
  //double z_ground = 0.0;

  //-----------------------------------------------------------------------------------------------------------
  //// This is for the port in Iran
  //std::string tar_name("20160625_031318_0c68"); // image with the fewest boats so we'll use it as the target
  //std::string ref_name("20160609_094252_0c72");
  //vpgl_lvcs lvcs(27.104640, 56.068069, 0);
  //std::string target_img_file("C:/Users/sca0161/Documents/iran port/" + tar_name + ".tif");
  //std::string target_cam_file("C:/Users/sca0161/Documents/iran port/Camera Matrices/" + tar_name + "_RPC.TXT");
  //std::string ref_img_file("C:/Users/sca0161/Documents/iran port/" + ref_name + ".tif");
  //std::string ref_cam_file("C:/Users/sca0161/Documents/iran port/Camera Matrices/" + ref_name + "_RPC.TXT");
  //std::string output_namebase("C:/Users/sca0161/Documents/iran port/output/out");
  //vgl_box_2d<int> region(36, 421, 2660, 2885); 
  //double z_ground = 0.0;

  //-----------------------------------------------------------------------------------------------------------
  //// This is for the port in Iran
  //std::string tar_name("20160625_031318_0c68"); // image with the fewest boats so we'll use it as the target
  //std::string ref_name;
  //vpgl_lvcs lvcs(27.104640, 56.068069, 0);
  //std::string img_dir = "C:/Users/sca0161/Documents/iran port/";
  //std::string cam_dir = "C:/Users/sca0161/Documents/iran port/Camera Matrices/";
  //std::string target_img_file(img_dir + tar_name + ".tif");
  //std::string target_cam_file(cam_dir + tar_name + "_RPC.TXT");
  //std::string ref_img_file;
  //std::string ref_cam_file;
  //std::string output_namebase("C:/Users/sca0161/Documents/iran port/output/");
  //vgl_box_2d<int> region(36, 421, 2660, 2885);
  //double z_ground = 0.0;

  ////-----------------------------------------------------------------------------------------------------------
  //// wide area of hamadan city
  //std::string tar_name("20160619_064848_0c72");
  //std::string ref_name;
  //vpgl_lvcs lvcs(35.2080385626545, 48.70485305698821, 1675); 
  //std::string img_dir = "C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/containCity/";
  //std::string cam_dir = "C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/Camera Matrices/";
  //std::string target_img_file(img_dir + tar_name + ".tif");
  //std::string target_cam_file(cam_dir + tar_name + "_RPC.TXT");
  //std::string ref_img_file;
  //std::string ref_cam_file;
  //std::string output_namebase("C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/output/");
  //vgl_box_2d<int> region(2693, 3423, 2909, 3789);
  //double z_ground = 0.0;

  
  ////-----------------------------------------------------------------------------------------------------------
  //// wide area of hamadan fields
  //std::string tar_name("20160701_091757_0c64");
  //std::string ref_name;
  //vpgl_lvcs lvcs(35.2080385626545, 48.70485305698821, 1675);
  //std::string img_dir = "C:/Users/sca0161/Documents/data/hamadan/updated images 2016.11.4/";
  //std::string cam_dir = "C:/Users/sca0161/Documents/data/hamadan/updated images 2016.11.4/Camera Matrices/";
  //std::string target_img_file(img_dir + tar_name + ".tif");
  //std::string target_cam_file(cam_dir + tar_name + "_RPC.TXT");
  //std::string ref_img_file;
  //std::string ref_cam_file;
  //std::string output_namebase("C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/output/");
  //vgl_box_2d<int> region(1382, 2482, 1652, 2402);
  //double z_ground = 0.0;

  //-----------------------------------------------------------------------------------------------------------
  // mischief reef
  std::string tar_name("20161110_015817_0e0d_1B_Analytic_DN");
  std::string ref_name;
  vpgl_lvcs lvcs(9.926373, 115.539668, 35);
  std::string img_dir = "C:/Users/sca0161/Documents/data/mischief reef/bw images/";
  std::string cam_dir = "C:/Users/sca0161/Documents/data/mischief reef/original/";
  std::string target_img_file(img_dir + tar_name + ".tif");
  std::string target_cam_file(cam_dir + tar_name + "_RPC.TXT");
  std::string ref_img_file;
  std::string ref_cam_file;
  std::string output_namebase("C:/Users/sca0161/Documents/data/mischief reef/output/");
  vgl_box_2d<int> region(655, 2905, 606, 2136);
  double z_ground = 0.0;


  for (vul_file_iterator fn = "C:/Users/sca0161/Documents/data/mischief reef/bw images/*.tif"; fn; ++fn) {
    ref_img_file = fn();
    std::string ref_name = vul_file::basename(vul_file::strip_directory(ref_img_file), ".tif");
    ref_cam_file = (cam_dir + ref_name + "_RPC.TXT").c_str();
    ref_img_file = (img_dir + ref_name + ".tif").c_str();

    if (ref_name.compare(tar_name) == 0) {
      continue;
    }

    // Load images
    vil_image_resource_sptr target_imgr =
      vil_load_image_resource(target_img_file.c_str());
    vil_image_resource_sptr ref_imgr =
      vil_load_image_resource(ref_img_file.c_str());

    // Convert to grey using special bil function
    bil_convert_resource_to_grey cnv;
    vil_image_resource_sptr target_img_grey, ref_img_grey;
    cnv(target_imgr, target_img_grey, true);
    cnv(ref_imgr, ref_img_grey, true);
    vil_image_view<vxl_uint_16> target_img = target_img_grey->get_view();
    vil_image_view<vxl_uint_16> ref_img = ref_img_grey->get_view();

    //vil_image_view<vxl_uint_16> target_img = //vil_load( target_img_file.c_str() );
    //  vil_convert_to_grey_using_rgb_weighting( vil_load( target_img_file.c_str() ) );
    //vil_image_view<vxl_uint_16> ref_img = //vil_load( ref_img_file.c_str() );
    //  vil_convert_to_grey_using_rgb_weighting( vil_load( ref_img_file.c_str() ) );

    // Check images
    if (!(target_img.ni() > 0 && ref_img.ni() > 0)) {
      std::cerr << "Failed to load images\n";
      return 1;
    }

    // Load cameras
    vpgl_rational_camera<double>* target_rpc =
      read_rational_camera_from_txt<double>(target_cam_file);
    vpgl_rational_camera<double>* ref_rpc =
      read_rational_camera_from_txt<double>(ref_cam_file);

    // Check cameras
    if (target_rpc == NULL || ref_rpc == NULL) {
      std::cerr << "Failed to load cameras\n";
      return 1;
    }

    // Convert cameras 
    vpgl_local_rational_camera<double> target_cam(lvcs, *target_rpc);
    vpgl_local_rational_camera<double> ref_cam(lvcs, *ref_rpc);
    delete target_rpc; delete ref_rpc;

    // Crop the target image and blur slightly to compensate for resampling
    // in the reference image
    vil_image_view<vxl_uint_16> tar_cropped = vil_crop(target_img,
      region.min_x(), region.width(), region.min_y(), region.height());
    vil_image_view<vxl_uint_16> tar_blur;
    vil_gauss_filter_2d(tar_cropped, tar_blur, 1.0, 2, vil_convolve_reflect_extend);

    // Warp the reference image
    vil_image_view<vxl_uint_16> ref_warped;
    vil_image_view<bool> ref_valid;
    baml_warp_via_ground_plane(
      ref_img, ref_cam, region,
      target_cam, z_ground, ref_warped);
    // Make valid image
    ref_valid.set_size(region.width(), region.height());
    ref_valid.fill(true);
    for (int y = 0; y < region.height(); y++) {
      for (int x = 0; x < region.width(); x++) {
        if (tar_cropped(x, y) == 0 || ref_warped(x, y) == 0) ref_valid(x, y) = false;
      }
    }

    // Output the cropped images
    vil_save(tar_blur, (output_namebase + "tar_" + tar_name + ".tif").c_str());
    vil_save(ref_warped, (output_namebase + ref_name + "_ref.tif").c_str());

    //// Detect changes
    //vil_image_view<float> tar_prob;
    //baml_change_detection_params params;
    //baml_change_detection cd(params);
    //cd.detect(tar_blur, ref_warped, ref_valid, tar_prob);

    //float tar_max = 0;
    //// Visualize and save
    //for (int x = 0; x < tar_prob.ni(); x++) { 
    //  for (int y = 0; y < tar_prob.nj(); y++) {
    //    //std::cout << tar_prob(x, y) << "\n";
    //    if (tar_max < tar_prob(x, y))
    //      tar_max = tar_prob(x, y);
    //  }
    //}
    //vil_image_view<vxl_byte> change_vis;
    //vil_convert_stretch_range_limited(tar_prob, change_vis, 0.0f, tar_max);
    //vil_save(change_vis, (output_namebase  + ref_name + "_change.tif").c_str());
  } 
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // CHANGE DETECTION ON ALREADY CROPPED IMAGES
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
//std::string img_dir = "C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/Wide Area Crop/tar_20161016_083938_1_0c22/gradient inner product/";
//// declare all variables we will be using
//std::string img_name, img_file, ref_file, ref_name;
//vil_image_resource_sptr tarr, refr;
//vil_image_view<vxl_uint_16> tar, ref;
//vil_image_view<vxl_byte> score_in, img_in, overlay_img;
//// load target
//img_file = (img_dir + "tar_aligned.tif").c_str();
//tarr = vil_load_image_resource(img_file.c_str());
//tar = tarr->get_view();
//for (vul_file_iterator fn = (img_dir + "aligned images/*.tif").c_str(); fn; ++fn) {
//  ref_file = fn();
//  ref_name = vul_file::strip_directory(ref_file);
//  // Load reference
//  refr = vil_load_image_resource(ref_file.c_str());
//  ref = refr->get_view();
//
//  // Make valid image
//  vil_image_view<bool> ref_valid;
//  ref_valid.set_size(tar.ni(), tar.nj());
//  ref_valid.fill(true);
//  for (int y = 0; y < tar.nj(); y++) {
//    for (int x = 0; x < tar.ni(); x++) {
//      if (tar(x, y) == 0 || tar(x, y) == 0) ref_valid(x, y) = false;
//    }
//  }
//  // Detect changes
//  vil_image_view<float> tar_prob;
//  baml_change_detection_params params;
//  baml_change_detection cd(params);
//  cd.detect(tar, ref, ref_valid, tar_prob);
//
//  vil_image_view<vxl_byte> change_vis;
//  vil_convert_stretch_range_limited(tar_prob, change_vis, 0.0f, 1.0f);
//  vil_save(change_vis, ("C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/Wide Area Crop/tar_20161016_083938_1_0c22/census/" + ref_name).c_str());
//}



  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // RED STAINING
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*std::string root_dir = "C:/Users/sca0161/Documents/hamadan/updated images 2016.11.4/";
// declare all variables we will be using
std::string img_name, img_file, score_file, img_dir;
vil_image_resource_sptr imgr, scorer;
vil_image_view<vxl_uint_16> img, score;
vil_image_view<vxl_byte> score_in, img_in, overlay_img;
std::vector < std::string> wideArea_dir = { "Wide Area Crop Field/tar_20161014_092852_0c65/", "Wide Area Crop/tar_20161016_083938_1_0c22/" };
std::vector <std::string> method_dir = {"rowTiles_1_colTiles_1/ratio/", "rowTiles_5_colTiles_5/ratio/", "rowTiles_10_colTiles_10/ratio/",
"rowTiles_1_colTiles_1/difference/", "rowTiles_5_colTiles_5/difference/", "rowTiles_10_colTiles_10/difference/",
"rowTiles_1_colTiles_1/birchfield tomasi/", "rowTiles_5_colTiles_5/birchfield tomasi/", "rowTiles_10_colTiles_10/birchfield tomasi/",
"gradient inner product/", "gradient magnitude/", "census/"};
std::vector<int> upperBound = { 65535, 65535, 65535, 1000, 1000, 1000, 400, 400, 400, 8000, 500, 500 };
std::vector<int> lowerBound = { 10000, 10000, 10000, 50, 50, 50, 150, 150, 150, 75, 40, 40 };
std::vector<int> upperBound_min = { 20000, 20000, 20000, 300, 300, 300, 100, 100, 100, 200, 150, 75 };
std::vector<int> lowerBound_min = { 75, 75, 75, 15, 15, 15, 25, 25, 25, 50, 10, 0 };
for (int i = 0; i < wideArea_dir.size(); i++) {
  for (int j = 7; j < 8; j++) {// method_dir.size(); j++) {
      img_dir = (root_dir + wideArea_dir[i] + method_dir[j]).c_str(); 
      // loop over all reference images and overlay the corresponding scores in red
      for (vul_file_iterator fn = (img_dir + "scores/*.tif").c_str(); fn; ++fn) {
        img_name = fn();
        img_name = vul_file::strip_directory(img_name);
        img_file = (img_dir + "aligned images/" + img_name).c_str();
        score_file = (img_dir + "scores/" + img_name).c_str();

        // Load images
        imgr = vil_load_image_resource(img_file.c_str());
        scorer = vil_load_image_resource(score_file.c_str());
        img = imgr->get_view();
        score = scorer->get_view();

        // red stain the image
        vil_convert_stretch_range_limited(img, img_in, uint16_t(500), uint16_t(2500));
        vil_convert_stretch_range_limited(score, score_in, uint16_t(lowerBound[j]), uint16_t(upperBound[j]));

        baml_overlay_red(img_in, score_in, overlay_img, 0, 255);
        vil_save(overlay_img, (img_dir + "red overlay/" + img_name).c_str());
    }
      // overlay the minimized scores over the target image in red
      img_file = (img_dir + "tar_aligned.tif").c_str();
      score_file = (img_dir + "scores_min.tif").c_str();

      // Load images
      imgr = vil_load_image_resource(img_file.c_str());
      scorer = vil_load_image_resource(score_file.c_str());
      img = imgr->get_view();
      score = scorer->get_view();

      // red stain the image
      vil_convert_stretch_range_limited(img, img_in, uint16_t(500), uint16_t(2500));
      vil_convert_stretch_range_limited(score, score_in, uint16_t(lowerBound_min[j]), uint16_t(upperBound_min[j]));

      baml_overlay_red(img_in, score_in, overlay_img, 0, 255);
      vil_save(overlay_img, (img_dir + "red overlay.jpg").c_str());
  }
}*/
return 0;
};
