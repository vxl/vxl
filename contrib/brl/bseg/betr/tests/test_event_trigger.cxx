// This is brl/bseg/betr/tests/test_event_trigger.cxx
#include <iostream>
#include <fstream>
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
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_camera.h>
void test_event_trigger()
{
  //create vsol_spatial_object_3d directly from polygon vertices
  vsol_point_3d_sptr p0 = new vsol_point_3d(235.8696284389948200, 406.1228461892747500, 9.6798152424242456);
  vsol_point_3d_sptr p1 = new vsol_point_3d(223.5715434156161300, 450.5470718703127800, 9.6798152424242456);
  vsol_point_3d_sptr p2 = new vsol_point_3d(191.7069643406025800, 444.0498655036974500, 9.6798152424242456);
  vsol_point_3d_sptr p3 = new vsol_point_3d(196.8278478137980300, 421.6872655445207600, 9.6798152424242456);
  vsol_point_3d_sptr p4 = new vsol_point_3d(202.9746837489035700, 396.9806647746743200, 9.6798152424242456);
  std::vector<vsol_point_3d_sptr> verts_ref;
  verts_ref.push_back(p0);   verts_ref.push_back(p1);   verts_ref.push_back(p2);
  verts_ref.push_back(p3);   verts_ref.push_back(p4);
  auto* poly_3d_ref = new vsol_polygon_3d(verts_ref);
  vsol_spatial_object_3d_sptr so_3d_ref = poly_3d_ref;
  vsol_point_3d_sptr pe0 = new vsol_point_3d(146.1171943827043800, 429.1333838455852300, 3.8836995690295124);
  vsol_point_3d_sptr pe1 = new vsol_point_3d(139.2825252680649700, 449.8591446870622100, 3.8836995690295124);
    vsol_point_3d_sptr pe2 = new vsol_point_3d(91.3167515951369780, 440.2827566788528800, 3.8836995690295115);
  vsol_point_3d_sptr pe3 = new vsol_point_3d(108.0650610929029900, 394.7929440913377400, 3.8836995690295120);
  vsol_point_3d_sptr pe4 = new vsol_point_3d(146.7831158857193000, 405.1760185076439000, 3.8836995690295115);

  std::vector<vsol_point_3d_sptr> verts_evt;
  verts_evt.push_back(pe0);   verts_evt.push_back(pe1);   verts_evt.push_back(pe2);
  verts_evt.push_back(pe3);   verts_evt.push_back(pe4);
  auto* poly_3d_evt = new vsol_polygon_3d(verts_evt);
  vsol_spatial_object_3d_sptr so_3d_evt = poly_3d_evt;
  std::string img_dir = "D:/tests/chiletest/crop_dir/";
  std::string image_name = "09DEC06145803-P1BS-052869858050_01_P002_bin_2";
  std::string cam_path = img_dir + image_name + ".rpb";
  vpgl_local_rational_camera<double>* lcam = read_local_rational_camera<double>(cam_path);
  vpgl_lvcs lvcs = lcam->lvcs();
  vpgl_camera_double_sptr camera = lcam;
  betr_event_trigger etr;
  etr.set_lvcs(lvcs);
  etr.set_ref_camera(camera);
  etr.set_evt_camera(camera);
  betr_geo_object_3d_sptr ref_obj = new betr_geo_object_3d(so_3d_ref, lvcs);
  betr_geo_object_3d_sptr evt_obj = new betr_geo_object_3d(so_3d_evt, lvcs);
  std::string ref_name = "ref_region", evt_name = "evt_region";
  etr.add_geo_object(ref_name, ref_obj, true);
  etr.add_geo_object(evt_name, evt_obj, false);
  vsol_polygon_2d_sptr ref_poly_2d, evt_poly_2d;
  bool good = etr.project_object(camera,ref_name, ref_poly_2d);
  good = good&& etr.project_object(camera,evt_name, evt_poly_2d);
  //load vsol_spatial_object_3d from ply files
  std::string site_dir = "D:/tests/chiletest/site/chile-illum_objects/";
  std::string obj0_path = site_dir + "mesh_0.ply";
  std::string obj1_path = site_dir + "mesh_1.ply";
  std::string obj2_path = site_dir + "mesh_2.ply";
  vsol_spatial_object_3d_sptr so_ply_ref, so_ply_evt, so_ply_3d;
  betr_io::read_so_ply(obj0_path, so_ply_ref);
  betr_io::read_so_ply(obj1_path, so_ply_evt);
  betr_io::read_so_ply(obj2_path, so_ply_3d);
  betr_geo_object_3d_sptr ply_ref_obj = new betr_geo_object_3d(so_ply_ref, lvcs);
  betr_geo_object_3d_sptr ply_evt_obj = new betr_geo_object_3d(so_ply_evt, lvcs);
  betr_geo_object_3d_sptr ply_3d_obj = new betr_geo_object_3d(so_ply_3d, lvcs);
  betr_event_trigger etr_ply;
  etr_ply.set_lvcs(lvcs);
  etr_ply.set_ref_camera(camera);
  etr_ply.set_evt_camera(camera);
  ref_name = "ply_ref_region"; evt_name = "ply_evt_region";
  std::string name_3d ="mesh_volume";
  etr_ply.add_geo_object(ref_name, ply_ref_obj, true);
  etr_ply.add_geo_object(evt_name, ply_evt_obj, false);
  etr_ply.add_geo_object(name_3d, ply_3d_obj, false);
  vsol_polygon_2d_sptr ply_ref_poly_2d, ply_evt_poly_2d, ply_mesh_poly_2d;
  good = etr_ply.project_object(camera,ref_name, ply_ref_poly_2d);
  good = good&& etr_ply.project_object(camera,evt_name, ply_evt_poly_2d);
  good = good&& etr_ply.project_object(camera,name_3d, ply_mesh_poly_2d);
}
TESTMAIN(test_event_trigger);
