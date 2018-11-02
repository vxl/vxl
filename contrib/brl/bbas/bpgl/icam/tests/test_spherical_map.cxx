#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vnl/vnl_math.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <icam/icam_spherical_map.h>
static void test_spherical_map()
{
   START("icam spherical map test");
   std::stringstream str0;
   str0 << 1593.82 << ' '<< 0 << ' ' << 752 << '\n';
   str0 << 0 << ' '<< 1593.82 << ' ' << 500 << '\n';
   str0 << 0 << ' '<< 0 << ' ' << 1 << '\n';
   str0 << -0.0525423 << ' ' << -0.998617 << ' ' << 0.00170576 << '\n';
   str0 << 0.000608705 << ' ' << -0.00174014 << ' ' << -0.999998 << '\n';
   str0 << 0.998618  << ' ' << -0.0525412  << ' ' << 0.000699295 << '\n';
   str0 << -5.71272  << ' ' << 2.69698  << ' ' << 18.3262  << '\n';
   vpgl_perspective_camera<double> C0;
   str0 >> C0;

   std::stringstream str1;
   str1 << 1477.45 << ' '<< 0 << ' ' << 752 << '\n';
   str1 << 0 << ' '<< 1477.45 << ' ' << 500 << '\n';
   str1 << 0 << ' '<< 0 << ' ' << 1 << '\n';
   str1 << -0.137869   << ' ' << 0.990053  << ' ' << 0.0280846 << '\n';
   str1 << -0.0171429  << ' ' << 0.0259658  << ' ' << -0.999517 << '\n';
   str1 << -0.990303   << ' ' << -0.138284  << ' ' << 0.0133925 << '\n';
   str1 << 1.49612  << ' ' << 2.60614  << ' ' << 9.84735 << '\n';
   vpgl_perspective_camera<double> C1;
   str1 >> C1;
   std::vector<vpgl_camera_double_sptr> cams;
   cams.push_back(new vpgl_perspective_camera<double>(C0));
   cams.push_back(new vpgl_perspective_camera<double>(C1));
   int ni = 1504, nj = 1000;
   vil_image_view<vxl_byte> img0(ni, nj, 3);
   vil_image_view<vxl_byte> img1(ni, nj, 3);
   img0.fill(0); img1.fill(0);
   int is0 = 621, js0 = 495;
   int ie0 = 690, je0 = 679;
   for(int j = js0; j<je0; ++j)
     for(int i = is0; i<ie0; ++i)
       img0(i,j, 1) = 255.0;
   int is1 = 326, js1 = 480;
   int ie1 = 428, je1 = 733;
   for(int j = js1; j<je1; ++j)
     for(int i = is1; i<ie1; ++i)
       img1(i,j, 0) = 255.0;
  std::vector<vil_image_view<vxl_byte> > images;
  images.push_back(img0); images.push_back(img1);

  double x = -0.881, y = -4.606, z = 1.5;
  double radius = 0.4, theta_cap = 0.314159;// 18 degrees
  vgl_point_3d<double> origin(x, y, z);
  unsigned n_theta = 200, n_phi = 500;
  icam_spherical_map sm(n_theta, n_phi, origin, radius, theta_cap);
  sm.set_data(images, cams);
  bool good = sm.map_sphere();
  if(good){
  vil_image_view<vxl_byte> sph = sm.sphere_map();
  int ig = 100, jg = 250;
  int ir = 100, jr = 70;
  int g = static_cast<int>(sph(ig, jg, 1));
  int r = static_cast<int>(sph(ir, jr, 0));
  good = good && g == 255 && r == 255;
  }
  TEST("Spherical Map", good, true);
}

TESTMAIN( test_spherical_map );
