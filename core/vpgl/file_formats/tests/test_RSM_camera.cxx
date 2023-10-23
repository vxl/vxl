#include "testlib/testlib_test.h"

#include <iostream>
#include <array>
#include <cmath>
#include <tuple>
#include <vector>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vpgl/vpgl_RSM_camera.h>
#include <vpgl/file_formats/vpgl_nitf_RSM_camera.h>
#include <vpgl/file_formats/vpgl_replacement_sensor_model_tres.h>
#include <vil/vil_image_view.h>
#define test 0
static void test_RSM_camera()
{
#if test
  vpgl_replacement_sensor_model_tres::define();
  vil_nitf2_tagged_record_sequence isxhd_tres;
  vil_nitf2_tagged_record_sequence::iterator tres_itr;
  std::string rsm_dir = "D:/tests/ReplacementSensorModel/";
  std::string rsm_path = rsm_dir +  "07APR2005_Hyperion_331406N0442000E_SWIR172_1p2A_L1R-BIB.ntf";
  std::string rsm_rgb_path = rsm_dir + "rsm_image_rgb.tif";
  {
    vil_image_resource_sptr resc = vil_load_image_resource(rsm_path.c_str());
    vil_nitf2_image* nitfr = reinterpret_cast<vil_nitf2_image*>(resc.as_pointer());
    nitfr->set_current_image(1);
    vil_image_view<short> view = nitfr->get_view();
    vil_image_view<vxl_byte> usview(view.ni(), view.nj(), 3);
    int gg;
    for (size_t j = 0; j < view.nj(); ++j)
        for (size_t i = 0; i < view.ni(); ++i)
            for (size_t p = 0; p < 3; ++p) {
                short v = view(i, j, p + 6);
                if (v < 0) v = 0;
                v /= 40;
             usview(i, j, p) = vxl_byte(v);   
            }
                
    vil_save(usview, rsm_rgb_path.c_str());
    std::vector< vil_nitf2_image_subheader* > img_headers = nitfr->get_image_headers();
    for (auto h : img_headers) {
      if (h->get_property("IXSHD", isxhd_tres)) {
        if (isxhd_tres.size() > 0) {
          for (tres_itr = isxhd_tres.begin(); tres_itr != isxhd_tres.end(); ++tres_itr) {
            std::string type = (*tres_itr)->name();
            if (type == "RSMIDA")
              std::cout << "RSM TRES PRESENT" << std::endl;
          }
        }
      }
    }
  }
  vpgl_nitf_RSM_camera rcam(rsm_path.c_str());
  std::vector<std::vector<int> > powers;
  std::vector<std::vector<double> > coeffs;
  std::vector<vpgl_scale_offset<double> > scale_offsets;
  bool testp = rcam.test_rsm_params();
  if(testp) std::cout << "RSM TRES PASSED" << std::endl;
  vnl_double_2 ul = rcam.upper_left();
  vnl_double_2 lr = rcam.lower_right();
  vnl_double_2 ur = rcam.upper_right();
  vnl_double_2 ll = rcam.lower_left();
  vnl_double_2 mid = 0.5 * (ul + lr);
  std::cout << "MIDDLE " << mid << std::endl;
  bool success = rcam.get_rsm_camera_params(powers, coeffs, scale_offsets);
  if (success) std::cout << "RSM CAMERA PARAMETERS PASSED" << std::endl;
  // test constructor
  vpgl_RSM_camera<double> rsm_cam(powers, coeffs, scale_offsets);
  double d2r = 3.14159 / 180.0;
  //double X = 0.77377, Y = 0.58, Z = 20.7799, u, v;
  double u, v;
  //double X = 44.377807*d2r, Y = 33.293708*d2r, Z = 34.0;
  //double X = 44.333154 * d2r, Y = 33.261389 * d2r, Z = 34.0;
  double X = 44.404525 * d2r, Y = 33.616347 * d2r, Z = 34.0;
  rsm_cam.project(X, Y, Z, u, v);
  int minv = 0, maxv = 3351, minu = 0, maxu = 255;
  bool proj = u>=minu && u<=maxu && v>=minv && v<=maxv;
  if (proj) std::cout << "RSM CAMERA PROJECTION PASSED" << std::endl;
#endif //test
}

TESTMAIN(test_RSM_camera);
