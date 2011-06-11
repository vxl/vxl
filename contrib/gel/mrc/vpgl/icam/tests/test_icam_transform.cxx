#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <icam/icam_depth_transform.h>
#include <icam/icam_sample.h>


static void test_icam_transform()
{
  START("icam transform test");
  // a constant depth at 100
  vil_image_view<double> depth(5,5);
  depth.fill(100.0);
  vnl_matrix_fixed<double, 3,3> K(0.0);
  K[0][0]=100;   K[1][1]=100; K[0][2] = 2.5; K[1][2]=2.5; K[2][2]=1.0;

  vnl_vector_fixed<double,3> x(1,0,0), v45(vnl_math::sqrt1_2, vnl_math::sqrt1_2, 0.0);
  vgl_rotation_3d<double> R(x, v45);

  vgl_vector_3d<double> t(1.5, 2.5, 4.0);
  icam_depth_transform dt(K, depth, R, t);
  double from_u = 3.0, from_v = 1.5, to_u, to_v;
  dt.transform(from_u, from_v, to_u, to_v);
  TEST_NEAR("icam_transform simple", to_u, 4.9621732532978, 1e-12);
  TEST_NEAR("icam_transform simple", to_v, 4.5638907328248, 1e-12);
  vgl_point_2d<double> from(3.0, 1.5), to;
  dt.transform(from, to);
  TEST_NEAR("icam_transform point", to.x(), 4.9621732532978, 1e-12);
  TEST_NEAR("icam_transform point", to.y(), 4.5638907328248, 1e-12);
  //--------------------  test with actual case ------------------

  vcl_string dest_file =   "C:/images/calibration/frame_142.png";
  vcl_string source_file = "C:/images/calibration/frame_145.png";
  vcl_string depth_file =  "C:/images/calibration/depth_142.tif";

  vil_image_view_base_sptr dest_img_base = vil_load(dest_file.c_str());
  if (!dest_img_base) {
    vcl_cerr << "error loading image.\n";
    return;
  }
  vil_image_view_base_sptr source_img_base = vil_load(source_file.c_str());
  if (!source_img_base) {
    vcl_cerr << "error loading image.\n";
    return;
  }

  vil_image_view_base_sptr depth_img_base = vil_load(depth_file.c_str());
  if (!depth_img_base) {
    vcl_cerr << "error loading image.\n";
    return;
  }
  vil_image_view<vxl_byte> *dest_img_byte   = dynamic_cast<vil_image_view<vxl_byte>*>(dest_img_base.ptr());
  vil_image_view<vxl_byte> *source_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(source_img_base.ptr());
  vil_image_view<float>    *depth_img_flt   = dynamic_cast<vil_image_view<float>*>(depth_img_base.ptr());
  unsigned ni = dest_img_byte->ni(), nj = dest_img_byte->nj();
  vil_image_view<float> dest_img_flt(ni,nj);
  vil_convert_cast(*dest_img_byte,dest_img_flt);
  vil_image_view<float> source_img_flt(ni,nj);
  vil_convert_cast(*source_img_byte,source_img_flt);
  vil_image_view<double> depth_img_dbl(ni, nj);
  vil_convert_cast(*depth_img_flt,depth_img_dbl);
  // relative rotation for source camera
  double rv [] ={0.9949824417310001, 0.07167609924, -0.06980290590899998,
                 -0.073085399753, 0.997165853331, -0.017858933610000002,
                 0.06832371779200001, 0.02287012861500001, 0.997400346057};
  vnl_matrix_fixed<double,3, 3> Mr(rv);
  vgl_rotation_3d<double> Rr(Mr);
  vgl_vector_3d<double> tr(0.3207432455793182, 0.04231364883145655, -0.019929923492081336);
  K[0][0]=1871.2;   K[1][1]=1871.2; K[0][2] = 640.0; K[1][2]=360.0; K[2][2]=1.0;
  icam_depth_transform dt2(K, depth_img_dbl, Rr, tr);
  vil_image_view<float> resmp, mask;
  unsigned n_samples;
  icam_sample::resample(ni, nj, source_img_flt, dt2, resmp, mask, n_samples);
  vcl_cout<< "found " << n_samples << " samples or " << (1.0*n_samples)/(ni*nj) << " fraction\n";
  vil_save(resmp, "C:/images/calibration/resmp_145_to_142.tif");
  vil_save(mask,  "C:/images/calibration/resmp_145_to_142_mask.tif");
}

TESTMAIN( test_icam_transform );
