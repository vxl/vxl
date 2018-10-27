// This is brl/bseg/betr/tests/test_geo_object.cxx
#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#include <betr/betr_geo_box_3d.h>
#include <betr/betr_geo_object_3d.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_box_3d.h>
#include <betr/vsol_mesh_3d.h>
#include <betr/vsol_mesh_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <vpgl/vpgl_lvcs.h>
#include <betr/betr_kml_utils.h>
void test_geo_object()
{
  double lat0 = -16.490904, lon0 = 179.996115, elv0 = 99.0;
  double  latg0 = 51.484443, long0 = -0.002913, elvg0 = 20.0;
  // polygon geo object
  vsol_point_3d_sptr p0 = new vsol_point_3d(0.0, 0.0, 0.0);
  vsol_point_3d_sptr p1 = new vsol_point_3d(5000.0, 0.0, 0.0);
  vsol_point_3d_sptr p2 = new vsol_point_3d(5000.0, -5000.0, 0.0);
  vsol_point_3d_sptr p3 = new vsol_point_3d(0.0, -5000.0, 0.0);
  std::vector<vsol_point_3d_sptr> verts;
  verts.push_back(p0);   verts.push_back(p1);   verts.push_back(p2);
  verts.push_back(p3);
  auto* poly = new vsol_polygon_3d(verts);
  vsol_spatial_object_3d_sptr so = dynamic_cast<vsol_spatial_object_3d*>(poly);
  vpgl_lvcs lvcs(lat0, lon0, elv0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG);
  betr_geo_object_3d gobj(so,  lvcs);
  betr_geo_box_3d gbox = gobj.bounding_box();
  std::string box_path = "d:/tests/test_poly_gbox.kml";
  std::ofstream ostr(box_path.c_str());
  betr_kml_utils::write_geo_box(ostr, gbox);
  ostr.close();
  // ---- 3d mesh geo object
  vsol_box_3d_sptr vbox = new vsol_box_3d();
  vbox->add_point(0.0, 0.0, 0.0);
  vbox->add_point(100.0, 100.0, 100.0);
  vsol_spatial_object_3d_sptr mesh = new vsol_mesh_3d(vbox);
  gobj.set_spatial_obj(mesh);
  gbox = gobj.bounding_box();
  std::string box_pathm = "d:/tests/test_mesh_gbox.kml";
  std::ofstream ostrm(box_pathm.c_str());
  betr_kml_utils::write_geo_box(ostrm, gbox);
  ostrm.close();

  vsol_point_3d_sptr mp0 = new vsol_point_3d(0.0, 0.0, 0.0);
  vsol_point_3d_sptr mp1 = new vsol_point_3d(20.0, 0.0, 0.0);
  vsol_point_3d_sptr mp2 = new vsol_point_3d(20.0, -20.0, 0.0);
  vsol_point_3d_sptr mp3 = new vsol_point_3d(10.0, -40.0, 0.0);
  vsol_point_3d_sptr mp4 = new vsol_point_3d(0.0, -20.0, 0.0);
  std::vector<vsol_point_3d_sptr> mverts;
  mverts.push_back(mp0);   mverts.push_back(mp1);   mverts.push_back(mp2);
  mverts.push_back(mp3); mverts.push_back(mp4);
  auto* mpoly = new vsol_polygon_3d(mverts);
  auto* pmesh = new vsol_mesh_3d();
  pmesh->set_mesh(mpoly, 200.0);
  vsol_spatial_object_3d_sptr extr_mesh = pmesh;
  gobj.set_spatial_obj(extr_mesh);
  std::string pathmex = "d:/tests/test_extr_mesh.kml";
  std::ofstream ostrmex(pathmex.c_str());
  betr_kml_utils::write_mesh(ostrmex, gobj);
  ostrmex.close();

}

TESTMAIN(test_geo_object);
