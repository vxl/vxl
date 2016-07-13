// This is brl/bseg/betr/tests/test_edgel_change_detection.cxx
#include <iostream>
#include <fstream>
#include <vil/vil_load.h>
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
#define chile 0
#define rajaei 1

void test_edgel_change_detection()
{
#if chile
  std::string img_dir = "D:/tests/chiletest/crop_dir/";

  std::string ref_image_name = "09DEC06145803-P1BS-052869858050_01_P002_bin_2";
  std::string ref_img_path = img_dir + ref_image_name + ".tif";
  vil_image_resource_sptr ref_imgr = vil_load_image_resource(ref_img_path.c_str());
  std::string ref_cam_path = img_dir + ref_image_name + ".rpb";
  vpgl_local_rational_camera<double>* ref_lcam = read_local_rational_camera<double>(ref_cam_path);
  vpgl_lvcs lvcs = ref_lcam->lvcs();
  vpgl_camera_double_sptr ref_camera = ref_lcam;

  std::string evt_image_name = "09NOV30151513-P1BS-052869835030_01_P001_bin_2";
  std::string evt_img_path = img_dir + evt_image_name + ".tif";
  vil_image_resource_sptr evt_imgr = vil_load_image_resource(evt_img_path.c_str());
  std::string evt_cam_path = img_dir + evt_image_name + ".rpb";
  vpgl_local_rational_camera<double>* evt_lcam = read_local_rational_camera<double>(evt_cam_path);
  vpgl_camera_double_sptr evt_camera = evt_lcam;

  //load vsol_spatial_object_3d from ply files
  std::string site_dir = "D:/tests/chiletest/site/chile-illum_objects/";
  std::string obj0_path = site_dir + "mesh_0.ply";
  std::string obj1_path = site_dir + "mesh_1.ply";
  std::string obj2_path = site_dir + "mesh_2.ply";
# if 0
  vsol_spatial_object_3d_sptr so_ply_ref, so_ply_evt, so_ply_3d;
  betr_io::read_so_ply(obj0_path, so_ply_ref);
  betr_io::read_so_ply(obj1_path, so_ply_evt);
  betr_io::read_so_ply(obj2_path, so_ply_3d);
  betr_geo_object_3d_sptr ply_ref_obj = new betr_geo_object_3d(so_ply_ref, lvcs);
  betr_geo_object_3d_sptr ply_evt_obj = new betr_geo_object_3d(so_ply_evt, lvcs);
  betr_geo_object_3d_sptr ply_3d_obj = new betr_geo_object_3d(so_ply_3d, lvcs);
#endif
  betr_event_trigger etr_ply("test_etr", lvcs);
  etr_ply.set_ref_camera(ref_camera);
  etr_ply.set_evt_camera(evt_camera);
  std::string  ply_ref_name = "ply_ref_region", ply_evt_name = "ply_evt_region";
  //  std::string name_3d ="mesh_volume";
  double lon, lat, elev;
  lvcs.get_origin(lat, lon, elev);
  etr_ply.add_geo_object(ply_ref_name, lon, lat, elev, obj0_path, true);
  etr_ply.add_geo_object(ply_evt_name, lon, lat, elev, obj1_path, false);
  //etr_ply.add_geo_object(name_3d, ply_3d_obj,false);
  // test project
  vsol_polygon_2d_sptr ply_ref_poly_2d, ply_evt_poly_2d, ply_mesh_poly_2d;
  bool good = etr_ply.project_object(ply_ref_name, ply_ref_poly_2d);
  good = good&& etr_ply.project_object(ply_evt_name, ply_evt_poly_2d);
  //good = good&& etr_ply.project_object(name_3d, ply_mesh_poly_2d);
  // end test project
  etr_ply.set_ref_image(ref_imgr);
  etr_ply.set_evt_image(evt_imgr);
  double pchange = 0.0;
  etr_ply.process("edgel_change_detection", pchange);
#elif rajaei
 std::string dir = "D:/tests/rajaei_test/trigger/";
 std::string ref_name = "20160601_124249_0c47";
 std::string evt_name = "20160609_094252_0c72";
 std::string ref_img_path = dir + ref_name +"_0.tiff";
 std::string evt_img_path = dir + evt_name +"_0.tiff";
 std::string ref_cam_path = dir + ref_name + "_RPC.TXT";
 std::string evt_cam_path = dir + evt_name + "_RPC.TXT";
 std::string ref_obj_path = dir + "rajaei_trigger_objects/mesh_1.ply";
 std::string evt_obj_path = dir + "rajaei_trigger_objects/mesh_2.ply";
 vil_image_resource_sptr ref_imgr = vil_load_image_resource(ref_img_path.c_str());
 vil_image_resource_sptr evt_imgr = vil_load_image_resource(evt_img_path.c_str());
 vpgl_local_rational_camera<double>* ref_lcam = read_local_rational_camera_from_txt<double>(ref_cam_path);
 vpgl_camera_double_sptr ref_camera = ref_lcam;
 vpgl_local_rational_camera<double>* evt_lcam = read_local_rational_camera_from_txt<double>(evt_cam_path);
 vpgl_camera_double_sptr evt_camera = evt_lcam;
 vpgl_lvcs lvcs = ref_lcam->lvcs();
 double lon, lat, elev;
 lvcs.get_origin(lat, lon, elev);
 betr_event_trigger etr("rajaei", lvcs);
 etr.set_ref_camera(ref_camera);
 etr.set_evt_camera(evt_camera);
 etr.add_geo_object("pier_ref", lon, lat, elev, ref_obj_path, true);
 etr.add_geo_object("pier_evt", lon, lat, elev, evt_obj_path, false);
 etr.set_ref_image(ref_imgr);
 etr.set_evt_image(evt_imgr);
 double pchange = 0.0;
 etr.process("edgel_change_detection", pchange);
#endif
  }
  TESTMAIN(test_edgel_change_detection);
