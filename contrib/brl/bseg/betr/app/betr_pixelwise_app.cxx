#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <bil/bil_convert_to_grey.h>
#include <testlib/testlib_test.h>
#include <betr/betr_geo_box_3d.h>
#include <betr/betr_geo_object_3d.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_box_3d.h>
#include <betr/betr_io.h>
#include <betr/vsol_mesh_3d.h>
#include <betr/vsol_mesh_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <vpgl/vpgl_lvcs.h>
#include <betr/betr_kml_utils.h>
#include <betr/betr_event_trigger.h>
#include <betr/betr_edgel_change_detection.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_camera.h>
#include <vul/vul_file_iterator.h>


int main(int  /*argc*/, char *  /*argv*/[])
{

  std::vector<std::string> img_names;
  std::string dir_name("C:/Users/sca0161/Documents/hamadan_test/images/*.tif");
  //std::string dir_name("C:/Users/sca0161/Documents/data/hamadan/*.tif");
  for (vul_file_iterator fi(dir_name); fi; ++fi) {
    std::string name = fi.filename();
    name = name.substr(0, name.length() - 4);
    img_names.push_back(name);
  }

  /*img_names.clear();
  img_names.resize(2);
  img_names[0] = "20160902_094643_0c19";
  img_names[1] = "20160717_043904_0c19";*/
  // set directory where cameras and images are held and declare the names of the target and reference(s)
  std::string dir = "C:/Users/sca0161/Documents/hamadan_test/images/";
  //std::string dir("C:/Users/sca0161/Documents/data/hamadan/");

  // set up coordinate system
  double lon = 48.6546831212;
  double lat = 35.1964842393;
  double elev = 1678.81629561;
  /*double lon = 48.6546831212; // from drew's polygon
  double lat = 35.1964842393;
  double elev = 1678.81629561;*/
 /* double lon = 48.69445444578883; // from joe's code
  double lat = 35.19416264891438;
  double elev = 1685.814575195312;*/
  vpgl_lvcs lvcs = vpgl_lvcs(lat, lon, elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  ////convert global coordinates of interest to lvcs coordinates
  //double local_x, local_y, local_z;
  //std::cout.precision(16);
  //lvcs.global_to_local(48.69445444578883, 35.19416264891438, 1685.814575195312, vpgl_lvcs::wgs84, local_x, local_y, local_z);
  //std::cout << "x = " << local_x << ", y = " << local_y << ", z = " << local_z << "\n";
  //lvcs.global_to_local(48.69318580030755, 35.19295451952000, 1685.816406250000, vpgl_lvcs::wgs84, local_x, local_y, local_z);
  //std::cout << "x = " << local_x << ", y = " << local_y << ", z = " << local_z << "\n";
  //lvcs.global_to_local(48.69490535998052, 35.19184479699520, 1685.816406250000, vpgl_lvcs::wgs84, local_x, local_y, local_z);
  //std::cout << "x = " << local_x << ", y = " << local_y << ", z = " << local_z << "\n";
  //lvcs.global_to_local(48.69609058294397, 35.19307847601662, 1684.811157226562, vpgl_lvcs::wgs84, local_x, local_y, local_z);
  //std::cout << "x = " << local_x << ", y = " << local_y << ", z = " << local_z << "\n";
  //return 0;

  // set up ROI
  //std::string ref_img_path = dir + ref_name + ".tif";
  //std::string ref_cam_path = dir + ref_name + ".tif_RPC.txt";
  //std::string evt_obj_path = "C:/Users/sca0161/Documents/data/hamadan/hamadan_objects/event_big.ply";
  //std::string ref_obj_path = "C:/Users/sca0161/Documents/data/hamadan/hamadan_objects/ref_full.ply";
  std::string evt_obj_path = "C:/Users/sca0161/Documents/hamadan_test/event_new.ply";
  std::string ref_obj_path = "C:/Users/sca0161/Documents/hamadan_test/event_new.ply";
  betr_event_trigger etr("hamadan", lvcs);
  etr.set_verbose(true);
  etr.add_geo_object("tarmac_ref", lon, lat, elev, ref_obj_path, true);
  etr.add_geo_object("tarmac_plane_evt", lon, lat, elev, evt_obj_path, false);

  //----------------------------------------------------------------------
  // experiment 1
  //----------------------------------------------------------------------
  //// use first image as reference
  ////std::string ref_name = img_names[0]; std::string corruption_type = "";// first image in the directory will be our reference; compare all other img_names
  ////std::string ref_name = "20161008_114613_0d05"; std::string corruption_type = "blurry_";
  ////std::string ref_name = "20161028_035612_0c65"; std::string corruption_type = "cloudy_";
  //std::string ref_name = "20170218_064426_0e2f"; std::string corruption_type = "weird_";
  //// set up reference
  //std::string ref_img_path = dir + "bw/" + ref_name + ".tif";
  //std::string ref_cam_path = dir + ref_name + "_RPC.txt";
  //vil_image_resource_sptr imgr = vil_load_image_resource(ref_img_path.c_str());
  //vpgl_rational_camera<double>* rpccam = read_rational_camera_from_txt<double>(ref_cam_path);
  //vpgl_local_rational_camera<double>* lcam = new vpgl_local_rational_camera<double>(lvcs, *rpccam);
  //vpgl_camera_double_sptr camera = dynamic_cast<vpgl_camera<double>*>(lcam);
  //etr.set_ref_camera(camera);
  //etr.set_ref_image(imgr);

  //// open file that show whether or not we have a valid file
  //std::ifstream is("C:/Users/sca0161/Documents/hamadan_test/valid_files.txt");
  ////std::ifstream is("C:/Users/sca0161/Documents/hamadan_test/valid_files_with_corruption.txt");


  //std::ofstream cd_score_bt;
  //cd_score_bt.precision(16);
  //cd_score_bt.open(("C:/Users/sca0161/Documents/hamadan_test/cd_scores_" + corruption_type + "bt.txt").c_str());
  //std::ofstream cd_score_census;
  //cd_score_census.precision(16);
  //cd_score_census.open(("C:/Users/sca0161/Documents/hamadan_test/cd_scores_" + corruption_type + "census.txt").c_str());
  //std::ofstream cd_score_diff;
  //cd_score_diff.precision(16);
  //cd_score_diff.open(("C:/Users/sca0161/Documents/hamadan_test/cd_scores_" + corruption_type + "diff.txt").c_str());
  //std::ofstream cd_score_grad;
  //cd_score_grad.precision(16);
  //cd_score_grad.open(("C:/Users/sca0161/Documents/hamadan_test/cd_scores_" + corruption_type + "grad.txt").c_str());
  //std::ofstream cd_score_np;
  //cd_score_np.precision(16);
  //cd_score_np.open(("C:/Users/sca0161/Documents/hamadan_test/cd_scores_" + corruption_type + "np.txt").c_str());

  //// loop over img_names using each as the evt name
  //vil_image_resource_sptr evt_imgr;
  //vpgl_camera_double_sptr evt_camera;
  //std::string valid_str;
  //for (int i = 0; i < img_names.size(); i++) {
  //  std::string evt_name = img_names[i];
  //  std::cerr << "Processing event: " << evt_name << " with reference: " << ref_name << "\n";
  //  getline(is, valid_str);
  //  if (strcmp(ref_name.c_str(), evt_name.c_str()) == 0) {
  //    cd_score_bt << "-1\n";
  //    cd_score_census << "-1\n";
  //    cd_score_diff << "-1\n";
  //    cd_score_grad << "-1\n";
  //    cd_score_np << "-1\n";
  //    std::cerr << "\tevent image is currently being used as reference image! continuing...\n";
  //    continue;
  //  }
  //  if (strcmp(valid_str.c_str(), "0") == 0) {
  //    cd_score_bt << "-1\n";
  //    cd_score_census << "-1\n";
  //    cd_score_diff << "-1\n";
  //    cd_score_grad << "-1\n";
  //    cd_score_np << "-1\n";
  //    std::cerr << "\tevent image not valid! continuing...\n";
  //    continue;
  //  }
  //  std::string evt_img_path = dir + "bw/" + evt_name + ".tif";
  //  std::string evt_cam_path = dir + evt_name + "_RPC.txt";
  //  //std::string evt_img_path = dir + evt_name + ".tif";
  //  //std::string evt_cam_path = dir + evt_name + ".tif_RPC.txt";
  //  evt_imgr = vil_load_image_resource(evt_img_path.c_str());
  //  vpgl_rational_camera<double>* rpccam = read_rational_camera_from_txt<double>(evt_cam_path);
  //  vpgl_local_rational_camera<double>* lcam = new vpgl_local_rational_camera<double>(lvcs, *rpccam);
  //  evt_camera = dynamic_cast<vpgl_camera<double>*>(lcam);
  //  evt_imgr = vil_load_image_resource(evt_img_path.c_str());
  //  etr.set_evt_camera(evt_camera);
  //  etr.set_evt_image(evt_imgr);

  //  std::vector<double> pchange;
  //  std::vector<vgl_point_2d<unsigned> > offsets;
  //  std::vector<vil_image_resource_sptr> rescs;
  //  std::string cd_json = "{\"method\" : 0}";
  //  etr.process("pixelwise_change_detection", pchange, rescs, offsets, cd_json);
  //  cd_score_bt << pchange[0] << "\n";
  //  cd_json = "{\"method\" : 1}";
  //  etr.process("pixelwise_change_detection", pchange, rescs, offsets, cd_json);
  //  cd_score_census << pchange[0] << "\n";
  //  cd_json = "{\"method\" : 2}";
  //  etr.process("pixelwise_change_detection", pchange, rescs, offsets, cd_json);
  //  cd_score_diff << pchange[0] << "\n";
  //  cd_json = "{\"method\" : 3}";
  //  etr.process("pixelwise_change_detection", pchange, rescs, offsets, cd_json);
  //  cd_score_grad << pchange[0] << "\n";
  //  cd_json = "{\"method\" : 4}";
  //  etr.process("pixelwise_change_detection", pchange, rescs, offsets, cd_json);
  //  cd_score_np << pchange[0] << "\n";
  //}
  //cd_score_bt.close();
  //cd_score_census.close();
  //cd_score_diff.close();
  //cd_score_grad.close();
  //cd_score_np.close();
  //----------------------------------------------------------------------
  // END experiment 1
  //----------------------------------------------------------------------


  //----------------------------------------------------------------------
  // experiment 2
  //----------------------------------------------------------------------
  //// use first image as reference
  //std::string ref_name = img_names[0]; // first image in the directory will be our reference; compare all other img_names
  //// set up reference (pretend it is our first event image because that makes looping easier)
  //std::vector<vpgl_camera_double_sptr> ref_cams;
  //std::vector<vil_image_resource_sptr> ref_imgrs;
  //std::string ref_img_path = dir + "bw/" + ref_name + ".tif";
  //std::string ref_cam_path = dir + ref_name + "_RPC.txt";
  //vil_image_resource_sptr evt_imgr = vil_load_image_resource(ref_img_path.c_str());
  //vpgl_rational_camera<double>* rpccam = read_rational_camera_from_txt<double>(ref_cam_path);
  //vpgl_local_rational_camera<double>* lcam = new vpgl_local_rational_camera<double>(lvcs, *rpccam);
  //vpgl_camera_double_sptr evt_camera = dynamic_cast<vpgl_camera<double>*>(lcam);

  //// open file that show whether or not we have a valid file
  ////std::ifstream is("C:/Users/sca0161/Documents/hamadan_test/valid_files.txt"); std::string corrupt = "";
  //std::ifstream is("C:/Users/sca0161/Documents/hamadan_test/valid_files_with_corruption.txt"); std::string corrupt = "corrupt_";
  //std::string valid_str;
  //getline(is, valid_str); // first file is known valid and is our reference

  //std::string multi_method = "minimum";
  //std::ofstream cd_score_bt;
  //cd_score_bt.precision(16);
  //cd_score_bt.open(("C:/Users/sca0161/Documents/hamadan_test/cd_scores_multi_" + corrupt + multi_method + "_bt.txt").c_str());
  //cd_score_bt << "-1\n"; // no score for the first file; it is our reference
  //std::ofstream cd_score_census;
  //cd_score_census.precision(16);
  //cd_score_census.open(("C:/Users/sca0161/Documents/hamadan_test/cd_scores_multi_" + corrupt + multi_method + "_census.txt").c_str());
  //cd_score_census << "-1\n";
  //std::ofstream cd_score_diff;
  //cd_score_diff.precision(16);
  //cd_score_diff.open(("C:/Users/sca0161/Documents/hamadan_test/cd_scores_multi_" + corrupt + multi_method + "_diff.txt").c_str());
  //cd_score_diff << "-1\n";
  //std::ofstream cd_score_grad;
  //cd_score_grad.precision(16);
  //cd_score_grad.open(("C:/Users/sca0161/Documents/hamadan_test/cd_scores_multi_" + corrupt + multi_method + "_grad.txt").c_str());
  //cd_score_grad << "-1\n";
  //std::ofstream cd_score_np;
  //cd_score_np.precision(16);
  //cd_score_np.open(("C:/Users/sca0161/Documents/hamadan_test/cd_scores_multi_" + corrupt + multi_method + "_np.txt").c_str());
  //cd_score_np << "-1\n";

  //// loop over img_names using each as the evt name
  //for (int i = 1; i < img_names.size(); i++) {
  //  std::string evt_name = img_names[i];
  //  std::cerr << "Processing event: " << evt_name << "\n";
  //  getline(is, valid_str);
  //  if (strcmp(valid_str.c_str(), "0") == 0) {
  //    cd_score_bt << "-1\n";
  //    cd_score_census << "-1\n";
  //    cd_score_diff << "-1\n";
  //    cd_score_grad << "-1\n";
  //    cd_score_np << "-1\n";
  //    std::cerr << "\tevent image not valid! continuing...\n";
  //    continue;
  //  }
  //  ref_cams.push_back(evt_camera);
  //  ref_imgrs.push_back(evt_imgr);
  //  etr.set_ref_cameras(ref_cams);
  //  etr.set_ref_images(ref_imgrs);
  //  std::string evt_img_path = dir + "bw/" + evt_name + ".tif";
  //  std::string evt_cam_path = dir + evt_name + "_RPC.txt";
  //  //std::string evt_img_path = dir + evt_name + ".tif";
  //  //std::string evt_cam_path = dir + evt_name + ".tif_RPC.txt";
  //  evt_imgr = vil_load_image_resource(evt_img_path.c_str());
  //  vpgl_rational_camera<double>* rpccam = read_rational_camera_from_txt<double>(evt_cam_path);
  //  vpgl_local_rational_camera<double>* lcam = new vpgl_local_rational_camera<double>(lvcs, *rpccam);
  //  evt_camera = dynamic_cast<vpgl_camera<double>*>(lcam);
  //  evt_imgr = vil_load_image_resource(evt_img_path.c_str());
  //  etr.set_evt_camera(evt_camera);
  //  etr.set_evt_image(evt_imgr);

  //  std::vector<double> pchange;
  //  std::vector<vgl_point_2d<unsigned> > offsets;
  //  std::vector<vil_image_resource_sptr> rescs;
  //  /*std::ifstream ifs("C:/Users/sca0161/Documents/change detection json/texturelessChange.json");
  //  std::string cd_json((std::istreambuf_iterator<char>(ifs)),
  //  (std::istreambuf_iterator<char>()));*/
  //  std::string cd_json = ("{\"method\" : 0, \"multi_method\" : \"" + multi_method + "\"}").c_str();
  //  etr.process("pixelwise_change_detection", pchange, rescs, offsets, cd_json);
  //  cd_score_bt << pchange[0] << "\n";
  //  cd_json = "{\"method\" : 1}";
  //  etr.process("pixelwise_change_detection", pchange, rescs, offsets, cd_json);
  //  cd_score_census << pchange[0] << "\n";
  //  cd_json = "{\"method\" : 2}";
  //  etr.process("pixelwise_change_detection", pchange, rescs, offsets, cd_json);
  //  cd_score_diff << pchange[0] << "\n";
  //  cd_json = "{\"method\" : 3}";
  //  etr.process("pixelwise_change_detection", pchange, rescs, offsets, cd_json);
  //  cd_score_grad << pchange[0] << "\n";
  //  cd_json = "{\"method\" : 4}";
  //  etr.process("pixelwise_change_detection", pchange, rescs, offsets, cd_json);
  //  cd_score_np << pchange[0] << "\n";
  //}
  //cd_score_bt.close();
  //cd_score_census.close();
  //cd_score_diff.close();
  //cd_score_grad.close();
  //cd_score_np.close();
  //----------------------------------------------------------------------
  // END experiment 2
  //----------------------------------------------------------------------

  //----------------------------------------------------------------------
  // experiment 1: Edgel
  //----------------------------------------------------------------------
  // use first image as reference
  std::string ref_name = img_names[0]; std::string corruption_type = "";// first image in the directory will be our reference; compare all other img_names
  //std::string ref_name = "20161008_114613_0d05"; std::string corruption_type = "blurry_";
  //std::string ref_name = "20161028_035612_0c65"; std::string corruption_type = "cloudy_";
  //std::string ref_name = "20170218_064426_0e2f"; std::string corruption_type = "weird_";
  // set up reference
  std::string ref_img_path = dir + "bw/" + ref_name + ".tif";
  std::string ref_cam_path = dir + ref_name + "_RPC.txt";
  vil_image_resource_sptr imgr = vil_load_image_resource(ref_img_path.c_str());
  vpgl_rational_camera<double>* rpccam = read_rational_camera_from_txt<double>(ref_cam_path);
  auto* lcam = new vpgl_local_rational_camera<double>(lvcs, *rpccam);
  vpgl_camera_double_sptr camera = dynamic_cast<vpgl_camera<double>*>(lcam);
  etr.set_ref_camera(camera);
  etr.set_ref_image(imgr);

  // open file that show whether or not we have a valid file
  std::ifstream is("C:/Users/sca0161/Documents/hamadan_test/valid_files.txt");
  //std::ifstream is("C:/Users/sca0161/Documents/hamadan_test/valid_files_with_corruption.txt");


  std::ofstream cd_score_edgel;
  cd_score_edgel.precision(16);
  cd_score_edgel.open(("C:/Users/sca0161/Documents/hamadan_test/cd_scores_" + corruption_type + "edgel.txt").c_str());

  // loop over img_names using each as the evt name
  vil_image_resource_sptr evt_imgr;
  vpgl_camera_double_sptr evt_camera;
  std::string valid_str;
  for (const auto& evt_name : img_names) {
    std::cerr << "Processing event: " << evt_name << " with reference: " << ref_name << "\n";
    getline(is, valid_str);
    if (strcmp(ref_name.c_str(), evt_name.c_str()) == 0) {
      cd_score_edgel << "-1\n";
      std::cerr << "\tevent image is currently being used as reference image! continuing...\n";
      continue;
    }
    if (strcmp(valid_str.c_str(), "0") == 0) {
      cd_score_edgel << "-1\n";
      std::cerr << "\tevent image not valid! continuing...\n";
      continue;
    }
    std::string evt_img_path = dir + "bw/" + evt_name + ".tif";
    std::string evt_cam_path = dir + evt_name + "_RPC.txt";
    //std::string evt_img_path = dir + evt_name + ".tif";
    //std::string evt_cam_path = dir + evt_name + ".tif_RPC.txt";
    evt_imgr = vil_load_image_resource(evt_img_path.c_str());
    vpgl_rational_camera<double>* rpccam = read_rational_camera_from_txt<double>(evt_cam_path);
    auto* lcam = new vpgl_local_rational_camera<double>(lvcs, *rpccam);
    evt_camera = dynamic_cast<vpgl_camera<double>*>(lcam);
    evt_imgr = vil_load_image_resource(evt_img_path.c_str());
    etr.set_evt_camera(evt_camera);
    etr.set_evt_image(evt_imgr);

    std::vector<double> pchange;
    std::vector<vgl_point_2d<unsigned> > offsets;
    std::vector<vil_image_resource_sptr> rescs;
    std::string cd_json = "{\"edgel_factory_params\" : {\"gradient_range\" : 60.0,\"min_region_edge_length\" : 10.0,\"nbins\" : 20,\"upsample_factor\" : 2.0   },\"noise_mul\" : 0.75,\"sigma\" : 1.0}";
    etr.process("edgel_change_detection", pchange, rescs, offsets, cd_json);
    cd_score_edgel << pchange[0] << "\n";

  }
  cd_score_edgel.close();
  //----------------------------------------------------------------------
  // END experiment 1: Edgel
  //----------------------------------------------------------------------
  return 0;
};
