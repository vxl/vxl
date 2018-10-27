// This is brl/bseg/betr/tests/test_edgel_change_detection.cxx
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
#define chile 0
#define rajaei 0
#define kandahar 0
#define hamadan 0
#define hamadan_pixelwise 1
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
  //std::string ref_name = "20160601_124249_0c47";
 // std::string evt_name = "20160609_094252_0c72";
  std::string evt_name = "20160601_124249_0c47";
  std::string ref_name = "20160609_094252_0c72";
  std::string ref_img_path = dir + ref_name + "_0.tiff";
  std::string evt_img_path = dir + evt_name + "_0.tiff";
  std::string ref_cam_path = dir + ref_name + "_RPC.TXT";
  std::string evt_cam_path = dir + evt_name + "_RPC.TXT";
  // std::string ref_obj_path = dir + "rajaei_trigger_objects/mesh_1.ply";
  // std::string evt_obj_path = dir + "rajaei_trigger_objects/mesh_2.ply";

// std::string evt_obj0_path = dir + "rajaei_ref_channel_large_objects/mesh_1.ply";
// std::string evt_obj1_path = dir + "rajaei_ref_channel_large_objects/mesh_1.ply";
// std::string ref_obj_path = dir + "rajaei_ref_channel_large_objects/mesh_0.ply";
  std::string ref_obj_path = dir + "rajaei_grid_event_objects/mesh_0.ply";
  std::string evt_grid_obj_path = dir + "rajaei_grid_event_objects/mesh_1.ply";
  vil_image_resource_sptr ref_imgr = vil_load_image_resource(ref_img_path.c_str());
  vil_image_resource_sptr evt_imgr = vil_load_image_resource(evt_img_path.c_str());
  vpgl_local_rational_camera<double>* ref_lcam = read_local_rational_camera_from_txt<double>(ref_cam_path);
  vpgl_camera_double_sptr ref_camera = ref_lcam;
  vpgl_local_rational_camera<double>* evt_lcam = read_local_rational_camera_from_txt<double>(evt_cam_path);
  vpgl_camera_double_sptr evt_camera = evt_lcam;
  vpgl_lvcs lvcs = ref_lcam->lvcs();
  double lon, lat, elev;
  lvcs.get_origin(lat, lon, elev);
  std::cout << std::setprecision(10) << "lat = " << lat << " lon = " << lon << " elev = " << elev << std::endl;
  betr_event_trigger etr("rajaei", lvcs);
  etr.set_verbose(true);
  etr.set_ref_camera(ref_camera);
  etr.set_evt_camera(evt_camera);
  etr.add_geo_object("grid_ref", lon, lat, elev, ref_obj_path, true);
  etr.add_gridded_event_poly("grid_evt", lon, lat, elev, evt_grid_obj_path, 100.0);
  etr.set_ref_image(ref_imgr);
  etr.set_evt_image(evt_imgr);
#if 1
  std::cout << "====PROCESSING WITH CHANGE =====\n";
  std::vector<double> pchange;
  std::vector<vil_image_resource_sptr> change_images;
  std::vector<vgl_point_2d<unsigned> > offsets;
  std::vector<std::string> evt_region_names;
  etr.process("edgel_change_detection", pchange, evt_region_names, change_images, offsets);
  const std::map<std::string, betr_geo_object_3d_sptr>& evt_objs = etr.evt_objects();
  unsigned i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++) {
    std::string ern = evt_region_names[i];
    std::map<std::string, betr_geo_object_3d_sptr>::const_iterator oit = evt_objs.find(ern);
    if (oit == evt_objs.end()) {
      std::cout << "In betr_execute_multi_chimg_evt_poly_process - event object " << ern << " not found" << std::endl;
      return;
    }
    vsol_polygon_3d_sptr bpoly = (oit->second)->base_polygon();
    unsigned nv = bpoly->size();
    std::cout << "for region " << ern << " pchange = " << *pit << std::endl;
    std::cout << "poly[";
    for (unsigned k = 0; k < nv; ++k) {
      vsol_point_3d_sptr p = bpoly->vertex(k);
      std::cout << '(' << p->x() << ' ' << p->y() << ' ' << p->z() << ") ";
    }
    std::cout << std::endl;
  }
  double pr = 0.0;
  //etr.process("edgel_change_detection", pr);
  //std::cout << "pr = " << pr << '\n';
  return;
#endif
  std::string evt_name_2 = "20160625_031318_0c68";
  std::string evt_img_path_2 = dir + evt_name_2 + "_0.tiff";
  vil_image_resource_sptr evt_imgr_2 = vil_load_image_resource(evt_img_path_2.c_str());
  std::string evt_cam_path_2 = dir + evt_name_2 + "_RPC.TXT";
  vpgl_local_rational_camera<double>* evt_lcam_2 = read_local_rational_camera_from_txt<double>(evt_cam_path_2);
  vpgl_camera_double_sptr evt_camera_2 = evt_lcam_2;
  etr.set_evt_camera(evt_camera_2);
  etr.set_evt_image(evt_imgr_2);
  //Kill remote and middle channel test
  std::cout << "====PROCESSING WITH NO CHANGE =====\n";
  pchange.clear();
  etr.process("edgel_change_detection", pchange);
  i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++)
    std::cout << "pchange[" << i << "] = " << *pit << '\n';
#if 0
  pr = 0.0;
  std::cout << "====PROCESSING WITH REFERENCE ONLY =====\n";
  etr.process("edgel_reference_cd", pchange);
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++)
    std::cout << "reference_pchange[" << i << "] = " << *pit << '\n';

  // reference region in middle of channel
  std::string evt0_self_path = dir + "rajaei_trigger_self_ref_objects/mesh_1.ply";
  std::string evt1_self_path = dir + "rajaei_trigger_self_ref_objects/mesh_2.ply";
  std::string ref_self_path = dir + "rajaei_trigger_self_ref_objects/mesh_0.ply";
  // std::string ref_self_path = dir + "rajaei_trigger_self_ref_objects/mesh_5.ply";
  betr_event_trigger etr_self("rajaei_self", lvcs);
  etr_self.set_verbose(true);
  etr_self.set_ref_camera(ref_camera);
  etr_self.set_evt_camera(evt_camera);
  etr_self.add_geo_object("pier_self_ref", lon, lat, elev, ref_self_path, true);
  etr_self.add_geo_object("pier_evt0_self", lon, lat, elev, evt0_self_path, false);
  etr_self.add_geo_object("pier_evt1_self", lon, lat, elev, evt1_self_path, false);
  etr_self.set_ref_image(ref_imgr);
  // etr_self.set_evt_image(evt_imgr);
  etr_self.set_evt_camera(evt_camera_2);
  etr_self.set_evt_image(evt_imgr_2);
  std::vector<double> pchange;
  etr_self.process("edgel_reference_cd", pchange);
  unsigned i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++)
    std::cout << "pchange[" << i << "] = " << *pit << '\n';
  double pr = 0.0;

#endif // kill middle channel test

  // four small refernce regions
  dir = "D:/tests/rajaei_test/object-test/";
  std::string evt0_small_path = dir + "object_detection_objects/evt_small_0.ply";
  std::string evt1_small_path = dir + "object_detection_objects/evt_small_1.ply";
  std::string evt2_small_path = dir + "object_detection_objects/evt_small_2.ply";
  std::string evt3_small_path = dir + "object_detection_objects/evt_small_3.ply";
  std::string ref_small_path = dir + "object_detection_objects/self_ref_small.ply";

  lon = 56.0671097675;
  lat = 27.109287683;
  elev = 0.0;
  lvcs = vpgl_lvcs(27.109287683, 56.0671097675, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  betr_event_trigger etr_self_small("rajaei_self", lvcs);
  etr_self_small.add_geo_object("pier_self_ref", lon, lat, elev, ref_small_path, true);
  etr_self_small.add_geo_object("pier_evt0_self", lon, lat, elev, evt0_small_path, false);
  etr_self_small.add_geo_object("pier_evt1_self", lon, lat, elev, evt1_small_path, false);
  etr_self_small.add_geo_object("pier_evt2_self", lon, lat, elev, evt2_small_path, false);
  etr_self_small.add_geo_object("pier_evt3_self", lon, lat, elev, evt3_small_path, false);
  etr_self_small.set_verbose(true);
  std::string ref_iname = "20160601_124249_0c47";
  std::string iname = "20160710_073428_0c1b";

  std::string ref_img_path_s = dir + ref_iname + ".tif";
  std::string ref_cam_path_s = dir + ref_iname + "_RPC.TXT";
  vil_image_resource_sptr ref_imgr_s = vil_load_image_resource(ref_img_path_s.c_str());
  vpgl_local_rational_camera<double>* ref_lcam_s = read_local_rational_camera_from_txt<double>(ref_cam_path_s);
  vpgl_camera_double_sptr ref_camera_s = ref_lcam_s;

  std::string img_path = dir + iname + ".tif";
  std::string cam_path = dir + iname + "_RPC.TXT";
  vil_image_resource_sptr imgr = vil_load_image_resource(img_path.c_str());
  vpgl_local_rational_camera<double>* lcam = read_local_rational_camera_from_txt<double>(cam_path);
  vpgl_camera_double_sptr camera = lcam;

  etr_self_small.set_ref_camera(ref_camera_s);
  etr_self_small.set_evt_camera(camera);
  etr_self_small.set_ref_image(ref_imgr_s);
  etr_self_small.set_evt_image(imgr);
  etr_self_small.set_ref_path(ref_img_path_s);
  etr_self_small.set_evt_path(img_path);
  // std::vector<double> pchange;
  etr_self_small.process("edgel_change_detection", pchange);
  // unsigned i =0;
  i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++)
    std::cout << "pchange[" << i << "] = " << *pit << '\n';

  iname = "20160609_094253_0c72";
  img_path = dir + iname + ".tif";
  cam_path = dir + iname + "_RPC.TXT";
  imgr = vil_load_image_resource(img_path.c_str());
  lcam = read_local_rational_camera_from_txt<double>(cam_path);
  camera = lcam;
  etr_self_small.set_ref_path(ref_img_path_s);
  etr_self_small.set_evt_path(img_path);
  etr_self_small.set_ref_camera(ref_camera_s);
  etr_self_small.set_evt_camera(camera);
  etr_self_small.set_ref_image(ref_imgr_s);
  etr_self_small.set_evt_image(imgr);
  etr_self_small.process("edgel_change_detection", pchange);
  i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++)
    std::cout << "pchange[" << i << "] = " << *pit << '\n';
#elif kandahar
  std::string dir = "D:/tests/kandahar_test/";
  std::string ref_name = "20160603_105531_1_0b0e.tif";
  std::string evt_name = "20160706_052626_0b09.tif";
  std::string ref_img_path = dir + ref_name;
  std::string evt_img_path = dir + evt_name;
  std::string ref_cam_path = dir + ref_name + "_RPC.TXT";
  std::string evt_cam_path = dir + evt_name + "_RPC.TXT";
  std::string evt_obj_path = dir + "kandahar_objects/event.ply";
  std::string evt2_obj_path = dir + "kandahar_objects/event2.ply";
  std::string ref_obj_path = dir + "kandahar_objects/ref.ply";
  vil_image_resource_sptr ref_imgr = vil_load_image_resource(ref_img_path.c_str());
  vpgl_local_rational_camera<double>* ref_lcam = read_local_rational_camera_from_txt<double>(ref_cam_path);
  vpgl_camera_double_sptr ref_camera = ref_lcam;

  vil_image_resource_sptr imgr = vil_load_image_resource(evt_img_path.c_str());
  vpgl_local_rational_camera<double>* lcam = read_local_rational_camera_from_txt<double>(evt_cam_path);
  vpgl_camera_double_sptr camera = lcam;
  double lon = 65.7720234438;
  double lat = 31.6265753757;
  double elev = 989.798815176;
  vpgl_lvcs lvcs = vpgl_lvcs(lat, lon, elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  betr_event_trigger etr("kandahar", lvcs);
  etr.set_verbose(true);
  etr.add_geo_object("empty_lot_ref", lon, lat, elev, ref_obj_path, true);
  etr.add_geo_object("empty_lot_evt", lon, lat, elev, evt_obj_path, false);
  etr.add_geo_object("occ_lot_evt", lon, lat, elev, evt2_obj_path, false);

  etr.set_ref_camera(ref_camera);
  etr.set_evt_camera(camera);
  etr.set_ref_image(ref_imgr);
  etr.set_evt_image(imgr);
  std::vector<double> pchange;
  std::cout << "processing " << evt_name << '\n';
  etr.process("edgel_change_detection", pchange);
  int i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++)
    std::cout << "pchange[" << i << "] = " << *pit << '\n';

  evt_name = "20160702_080452_0c64.tif";
  evt_img_path = dir + evt_name;
  evt_cam_path = dir + evt_name + "_RPC.TXT";
  imgr = vil_load_image_resource(evt_img_path.c_str());
  lcam = read_local_rational_camera_from_txt<double>(evt_cam_path);
  camera = lcam;
  etr.set_evt_camera(camera);
  etr.set_evt_image(imgr);
  std::cout << "processing " << evt_name << '\n';
  etr.process("edgel_change_detection", pchange);
  i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++)
    std::cout << "pchange[" << i << "] = " << *pit << '\n';
#elif hamadan
  std::string dir = "D:/tests/hamadan_test/";
  //std::string dir = "D:/data/sattel/hamadan/";
  // std::string ref_name = "20160821_063826_0e20.tif";
  // std::string ref_name = "20160623_050936_0c64.tif";
  std::string ref_name = "20160902_094643_0c19.tif";
  // std::string evt_name = "20160822_064308_0c1b.tif";
  std::string evt_name = "20160717_043904_0c19.tif";
  std::cout << "Reference" << ref_name << '\n';
  std::string ref_img_path = dir + ref_name;
  std::string evt_img_path = dir + evt_name;
  std::string ref_cam_path = dir + ref_name + "_RPC.txt";
  std::string evt_cam_path = dir + evt_name + "_RPC.txt";
  std::string evt_obj_path = dir + "hamadan_objects/event_big.ply";
  // std::string evt2_obj_path = dir + "hamadan_objects/event.ply";
  std::string ref_obj_path = dir + "hamadan_objects/ref_full.ply";

  double lon = 48.6546831212;
  double lat = 35.1964842393;
  double elev = 1678.81629561;
  vpgl_lvcs lvcs = vpgl_lvcs(lat, lon, elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vil_image_resource_sptr ref_imgr = vil_load_image_resource(ref_img_path.c_str());
  //vpgl_local_rational_camera<double>* ref_lcam = read_local_rational_camera_from_txt<double>(ref_cam_path);
  vpgl_rational_camera<double>* ref_rpccam = read_rational_camera_from_txt<double>(ref_cam_path);
  if (!ref_rpccam)
  return;

  vpgl_local_rational_camera<double>* ref_lcam = new vpgl_local_rational_camera<double>(lvcs, *ref_rpccam);
  vpgl_camera_double_sptr ref_camera = ref_lcam;
  if (!ref_camera)
  return;
  vil_image_resource_sptr imgr = vil_load_image_resource(evt_img_path.c_str());
  //vpgl_local_rational_camera<double>* lcam = read_local_rational_camera_from_txt<double>(evt_cam_path);
  vpgl_rational_camera<double>* rpccam = read_rational_camera_from_txt<double>(evt_cam_path);

  vpgl_local_rational_camera<double>* lcam = new vpgl_local_rational_camera<double>(lvcs, *rpccam);
  vpgl_camera_double_sptr camera = dynamic_cast<vpgl_camera<double>* >(lcam);

  betr_event_trigger etr("hamadan", lvcs);
  etr.set_verbose(true);
  etr.add_geo_object("tarmac_ref", lon, lat, elev, ref_obj_path, true);
  // etr.add_geo_object("tarmac_plane_evt", lon+0.001, lat+0.002, elev+10.0, evt_obj_path, false);
  etr.add_geo_object("tarmac_plane_evt", lon, lat, elev, evt_obj_path, false);

  etr.set_ref_camera(ref_camera);
  etr.set_evt_camera(camera);
  etr.set_ref_image(ref_imgr);
  etr.set_evt_image(imgr);
  std::vector<double> pchange;
  std::vector<vgl_point_2d<unsigned> > offsets;
  std::vector<vil_image_resource_sptr> rescs;

  // Read json
  std::string cd_json = "{\"edgel_factory_params\" : {\"gradient_range\" : 60.0,\"min_region_edge_length\" : 10.0,\"nbins\" : 20,\"upsample_factor\" : 2.0   },\"noise_mul\" : 0.75,\"sigma\" : 1.0}";
  //std::string cd_json ="{\"method\" : 2, \"registration_rad\" : 2, \"change_prior\" : 0.01 }";

  std::cout << "===>processing " << evt_name << '\n';
  etr.process("edgel_change_detection", pchange, rescs, offsets, cd_json);
  //etr.process("pixelwise_change_detection", pchange, rescs, offsets,cd_json);
  int i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++) {
    std::cout << "pchange[" << i << "] = " << *pit << '\n';
    std::cout << "Offset " << offsets[i] << " (" << rescs[i]->ni() << ' ' << rescs[i]->nj() << ")\n";
    std::stringstream ss;
    ss << i;
    std::string change_path = dir + evt_name + "change_image_" + ss.str() + ".tif";
    vil_save_image_resource(rescs[i], change_path.c_str());
  }
  return;
  //============== end of processing


  evt_name = "20160705_092219_0c81.tif";
  evt_img_path = dir + evt_name;
  evt_cam_path = dir + evt_name + "_RPC.TXT";
  imgr = vil_load_image_resource(evt_img_path.c_str());
  lcam = read_local_rational_camera_from_txt<double>(evt_cam_path);
  camera = lcam;
  etr.set_evt_camera(camera);
  etr.set_evt_image(imgr);
  std::cout << "===>processing " << evt_name << '\n';
  etr.process("edgel_change_detection", pchange);
  i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++)
    std::cout << "pchange[" << i << "] = " << *pit << '\n';

  evt_name = "20160817_135113_0c68.tif";
  evt_img_path = dir + evt_name;
  evt_cam_path = dir + evt_name + "_RPC.TXT";
  imgr = vil_load_image_resource(evt_img_path.c_str());
  lcam = read_local_rational_camera_from_txt<double>(evt_cam_path);
  camera = lcam;
  etr.set_evt_camera(camera);
  etr.set_evt_image(imgr);
  std::cout << "===>processing " << evt_name << '\n';
  etr.process("edgel_change_detection", pchange);
  i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++)
    std::cout << "pchange[" << i << "] = " << *pit << '\n';

  evt_name = "20160902_094643_0c19.tif";
  evt_img_path = dir + evt_name;
  evt_cam_path = dir + evt_name + "_RPC.TXT";
  imgr = vil_load_image_resource(evt_img_path.c_str());
  lcam = read_local_rational_camera_from_txt<double>(evt_cam_path);
  camera = lcam;
  etr.set_evt_camera(camera);
  etr.set_evt_image(imgr);
  std::cout << "===>processing " << evt_name << '\n';
  etr.process("edgel_change_detection", pchange);
  i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++)
    std::cout << "pchange[" << i << "] = " << *pit << '\n';

  evt_name = "20160831_063745_0e0d.tif";
  evt_img_path = dir + evt_name;
  // evt_cam_path = dir + evt_name + "_RPC.TXT";
  evt_cam_path = dir + evt_name + "_RPCG.TXT";
  imgr = vil_load_image_resource(evt_img_path.c_str());
  // lcam = read_local_rational_camera_from_txt<double>(evt_cam_path);
   // camera = lcam;
  vpgl_rational_camera<double>* cam = read_rational_camera_from_txt<double>(evt_cam_path);
  camera = cam;
  etr.set_evt_camera(camera);
  etr.set_evt_image(imgr);
  std::cout << "===>processing " << evt_name << '\n';
  etr.process("edgel_change_detection", pchange);
  i = 0;
  for (std::vector<double>::iterator pit = pchange.begin();
    pit != pchange.end(); ++pit, i++)
    std::cout << "pchange[" << i << "] = " << *pit << '\n';
  TEST_NEAR("745_hamadan_test", pchange[0], 0.006, 0.0001);

#elif hamadan_pixelwise
  //// set directory where cameras and images are held and declare the names of the target and reference(s)
  std::string dir = "C:/Users/sca0161/Documents/data/hamadan/";
  //// std::string ref_name = "20160821_063826_0e20.tif";
  //// std::string ref_name = "20160623_050936_0c64.tif";
  std::vector<std::string> ref_name;
  ref_name.emplace_back("20160902_094643_0c19.tif");
  //ref_name.push_back("20160619_064846_0c72.tif");
  //ref_name.push_back("20160623_050936_0c64.tif");
  //ref_name.push_back("20160701_091757_0c64.tif");
  //ref_name.push_back("20160701_091758_0c64.tif");
  //ref_name.push_back("20160702_103254_0c59.tif");

//std::string dir = "C:/Users/sca0161/Downloads/test_script/test_script/";
//std::vector<std::string> ref_name;
//ref_name.push_back("ref_image.tif");
//std::string evt_name = "evt_image.tif";


  // std::string evt_name = "20160822_064308_0c1b.tif";
  std::string evt_name = "20160717_043904_0c19.tif"; //---------------------------------------------------------

  // set up coordinate system
  double lon = 48.6546831212; //--------------------------------------------------------
  double lat = 35.1964842393; //--------------------------------------------------------
  double elev = 1678.81629561; //--------------------------------------------------------
//double lon = 65.6942830165;
//double lat = 31.6134971167;
//double elev = 0.0;
  vpgl_lvcs lvcs = vpgl_lvcs(lat, lon, elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  // set up the event image and all the things that aren't related to the reference image(s)
  std::string evt_img_path = dir + evt_name;
  std::string evt_cam_path = dir + evt_name + "_RPC.txt"; //--------------------------------------------------------
  //std::string evt_cam_path = dir + "evt_rpc.txt";
   std::string evt_obj_path = dir + "hamadan_objects/event_big.ply"; //-------------------------------------------------------- -
  //std::string evt_obj_path = dir + "evt_poly.ply";
  // std::string evt2_obj_path = dir + "hamadan_objects/event.ply";
   std::string ref_obj_path = dir + "hamadan_objects/ref_full.ply"; //-------------------------------------------------------- -
 // std::string ref_obj_path = dir + "ref_poly.ply";
  vil_image_resource_sptr imgr = vil_load_image_resource(evt_img_path.c_str());
  //vpgl_local_rational_camera<double>* lcam = read_local_rational_camera_from_txt<double>(evt_cam_path);
  vpgl_rational_camera<double>* rpccam = read_rational_camera_from_txt<double>(evt_cam_path);
  betr_event_trigger etr("hamadan", lvcs);
  etr.set_verbose(true);
  etr.add_geo_object("tarmac_ref", 65.6900995012, 31.6148879491, elev, ref_obj_path, true);
  // etr.add_geo_object("tarmac_plane_evt", lon+0.001, lat+0.002, elev+10.0, evt_obj_path, false);
  etr.add_geo_object("tarmac_plane_evt", lon, lat, elev, evt_obj_path, false);
  auto* lcam = new vpgl_local_rational_camera<double>(lvcs, *rpccam);
  vpgl_camera_double_sptr camera = dynamic_cast<vpgl_camera<double>*>(lcam);
  etr.set_evt_camera(camera);
  etr.set_evt_image(imgr);

  // create a vector of reference images and reference cameras
  std::vector<vil_image_resource_sptr> ref_imgr(ref_name.size());
  std::vector<vpgl_camera_double_sptr> ref_camera(ref_name.size());
  // fill vectors with images and cameras ---------------------------------------------------------
  for (int i = 0; i < ref_name.size(); i++) {
    std::cout << "Reference" << ref_name[i] << '\n';
    std::string ref_img_path = dir + ref_name[i];
    std::string ref_cam_path = dir + ref_name[i] + "_RPC.txt"; //---------------------------------------------------------
    //std::string ref_cam_path = dir + "ref_rpc.txt";
    ref_imgr[i] = vil_load_image_resource(ref_img_path.c_str());
    //vpgl_local_rational_camera<double>* ref_lcam = read_local_rational_camera_from_txt<double>(ref_cam_path);
    vpgl_rational_camera<double>* ref_rpccam = read_rational_camera_from_txt<double>(ref_cam_path);
    if (!ref_rpccam)
      return;
    auto* ref_lcam = new vpgl_local_rational_camera<double>(lvcs, *ref_rpccam);
    ref_camera[i] = ref_lcam;
    if (!ref_camera[i])
      return;
  }
  // set the reference images and cameras in the betr_event_trigger object
  etr.set_ref_cameras(ref_camera);
  etr.set_ref_images(ref_imgr);

  std::vector<double> pchange;
  std::vector<vgl_point_2d<unsigned> > offsets;
  std::vector<vil_image_resource_sptr> rescs;

  // Read json
  //std::string cd_json ="{\"edgel_factory_params\" : {\"gradient_range\" : 60.0,\"min_region_edge_length\" : 10.0,\"nbins\" : 20,\"upsample_factor\" : 2.0   },\"noise_mul\" : 0.75,\"sigma\" : 1.0}";
  // read in json file with all your necessary parameters
  std::ifstream ifs("C:/Users/sca0161/Documents/change detection json/test.json");
  std::string cd_json((std::istreambuf_iterator<char>(ifs)),
    (std::istreambuf_iterator<char>()));
  std::cout << cd_json << "\n";
  std::cout << "===>processing " << evt_name << '\n';
  //etr.process("edgel_change_detection", pchange, rescs, offsets,cd_json);
  etr.process("pixelwise_change_detection", pchange, rescs, offsets, cd_json);
  int i = 0;
  for (auto pit = pchange.begin();
    pit != pchange.end(); ++pit, i++) {
    std::cout << "pchange[" << i << "] = " << *pit << '\n';
    std::cout << "Offset " << offsets[i] << " (" << rescs[i]->ni() << ' ' << rescs[i]->nj() << ")\n";
    std::stringstream ss;
    ss << i;
    std::string change_path = dir + "debug 20170202/change_imageS_" + ss.str() + ".tif";
    vil_save_image_resource(rescs[i], change_path.c_str());
  }

  return;
#endif
}
TESTMAIN(test_edgel_change_detection);
