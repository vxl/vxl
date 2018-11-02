#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_backproject_dem.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_camera.h>
//
//#define test_enabled
static void test_backproject_dem()
{
#ifdef test_enabled
  std::string datadir = "D:/tests/hamadan_test/";
  // the dem image geotiff file
  std::string dem_path = datadir + "Hamadan_all_sat.tif";
  std::string cam_path = datadir + "20160817_135113_0c68.tif_RPCG.txt";

  vil_image_resource_sptr dem_resc = vil_load_image_resource(dem_path.c_str());
  if(!dem_resc)
    return;
  //a rational camera covering at least part of the DEM region
  vpgl_rational_camera<double>* rpc_cam = read_rational_camera_from_txt<double>(cam_path);
  if(!rpc_cam)
    return;
  double zoff =  rpc_cam->offset(vpgl_rational_camera<double>::Z_INDX);
  double zscale =  rpc_cam->scale(vpgl_rational_camera<double>::Z_INDX);
  double zmax = zoff + zscale;
  double zmin = zoff - zscale;
  std::cout << "RPC camera z bounds " << zmin << " -> " << zmax << std::endl;
  vpgl_camera<double>* cam = dynamic_cast<vpgl_camera<double>*>(rpc_cam);
  if(!cam)
    return;
  vpgl_geo_camera* geo_cam;
  bool success = vpgl_geo_camera::init_geo_camera(dem_resc, geo_cam);
  std::cout << *geo_cam << std::endl;
  vpgl_backproject_dem bpdem(dem_resc);
  //vgl_point_2d<double> img_pt(4012.0, 2036.0);
  //vgl_point_2d<double> img_pt(4490.0, 406.0);
  vgl_point_2d<double> img_pt(4833.0, 2666.0);
  //  vgl_point_3d<double> initial_guess(48.65064, 35.21145,1825.0);
  double zmax_dem = bpdem.zmax(), zmin_dem = bpdem.zmin();
  std::cout << "DEM z bounds " << zmin_dem << " -> " << zmax_dem << std::endl;
  vgl_point_3d<double> initial_guess = bpdem.geo_center();
  vgl_point_3d<double> p3d;

  bpdem.bproj_dem(cam,img_pt,zmax_dem,zmin_dem,initial_guess,p3d, 1.0);
#endif
}

TESTMAIN(test_backproject_dem);
