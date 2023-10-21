#include "testlib/testlib_test.h"

#include <iostream>
#include <array>
#include <cmath>
#include <tuple>
#include <vector>
#include <vil/vil_load.h>
#include <vpgl/vpgl_RSM_camera.h>
#include <vpgl/file_formats/vpgl_nitf_RSM_camera.h>
#include <vpgl/file_formats/vpgl_replacement_sensor_model_tres.h>
#define test 0
static void test_RSM_camera()
{
#if test
  vpgl_replacement_sensor_model_tres::define();
  double x, y, u, v;
  vil_nitf2_tagged_record_sequence isxhd_tres;
  vil_nitf2_tagged_record_sequence::iterator tres_itr;
  std::string rsm_path = "D:/tests/ReplacementSensorModel/07APR2005_Hyperion_331406N0442000E_SWIR172_1p2A_L1R-BIB.ntf";
  {
    vil_image_resource_sptr resc = vil_load_image_resource(rsm_path.c_str());
    vil_nitf2_image* nitfr = reinterpret_cast<vil_nitf2_image*>(resc.as_pointer());
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
  bool test = rcam.test_rsm_params();
  if(test) std::cout << "RSM TRES PASSED" << std::endl;
  bool success = rcam.get_rsm_camera_params(powers, coeffs, scale_offsets);
  if (success) std::cout << "RSM CAMERA PARAMETERS PASSED" << std::endl;
#endif //test
}

TESTMAIN(test_RSM_camera);
