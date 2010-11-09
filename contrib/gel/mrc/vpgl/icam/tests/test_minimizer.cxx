#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_ios.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_point_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_numeric_traits.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_bounds.h>
#include <icam/icam_minimizer.h>

static void test_minimizer()
{
  vcl_string root_dir = testlib_root_dir();
  vcl_string dest_file = "C:/images/calibration/frame_142.png";
  vcl_string source_file = "C:/images/calibration/frame_145.png";
  //  vcl_string source_file = "C:/images/calibration/frame_138.png";
  //  vcl_string source_file = "C:/images/calibration/frame_146.png";
  vcl_string depth_file = "C:/images/calibration/depth_142.tif";

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
  vil_image_view<vxl_byte> *dest_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(dest_img_base.ptr());
  vil_image_view<vxl_byte> *source_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(source_img_base.ptr());
  vil_image_view<float> *depth_img_flt = dynamic_cast<vil_image_view<float>*>(depth_img_base.ptr());
  unsigned ni = dest_img_byte->ni(), nj = dest_img_byte->nj();
  vil_image_view<float> dest_img_flt(ni,nj);
  vil_convert_cast(*dest_img_byte,dest_img_flt);
  vil_image_view<float> source_img_flt(ni,nj);
  vil_convert_cast(*source_img_byte,source_img_flt);
  vil_image_view<double> depth_img_dbl(ni, nj);
  vil_convert_cast(*depth_img_flt,depth_img_dbl);
  // ========================Camera 145 =====================
  // relative rotation for source camera
  double rv [] ={0.9949824417310001, 0.07167609924, -0.06980290590899998,
                 -0.073085399753, 0.997165853331, -0.017858933610000002,
                 0.06832371779200001, 0.02287012861500001, 0.997400346057};
  vnl_double_3x3 Mr(rv);
  vgl_rotation_3d<double> Rr(Mr);
  vnl_double_3 rrd = Rr.as_rodrigues();
  vcl_cout << "Cam rotation " << rrd << '\n';
  vnl_double_3 zaxis(0.0, 0.0, 1.0);
  vcl_cout << "Principal ray dir " << Rr*zaxis << '\n';
  vnl_double_3 rtop(0.0198205, -0.109489, -0.0896672);
  vgl_rotation_3d<double> Rtop(rtop);
  vcl_cout << "top cam axis(0.5,0,0) " << Rtop*zaxis << '\n'
           << " axis angle diff (0.5, 0, 0) " << vpgl_camera_bounds::angle_between_rays(Rr, Rtop) << '\n'
           << " diff in rotation about axis " << vpgl_camera_bounds::rot_about_ray(Rr, Rtop) << '\n';
  vnl_double_3 rfinal(0.00942883, -0.0635162, -0.0676306);
  vgl_rotation_3d<double> Rfinal(rfinal);
  vcl_cout << "final(0.00942883,-0.0635162,-0.0676306) " << Rfinal*zaxis << '\n'
           << " axis angle diff level3 " << vpgl_camera_bounds::angle_between_rays(Rr, Rfinal) << '\n'
           << " diff in rotation about axis " << vpgl_camera_bounds::rot_about_ray(Rr, Rfinal) << '\n';
  // ========================Camera 138 =====================
  double rv138 [] = {0.996034852179, 0.043480236729, 0.07760390651200001,
                     0.042636569473, 0.999012669051, 0.01251074244199999,
                     -0.07807192351699999, -0.009152332035, 0.996905943118};
  vnl_double_3x3 Mr138(rv138);
  vgl_rotation_3d<double> Rr138(Mr138);
  vnl_double_3 rr138 = Rr138.as_rodrigues();
  vnl_double_3 t138(-0.41081, -0.08859, 0.061291);
   //=========================Camera 146 ================================
  double rv146 [] = {0.992577, 0.0862514, -0.0857363,
                     -0.0888177, 0.995693, -0.0265857,
                     0.0830739, 0.0340026, 0.995963};
  vnl_double_3x3 Mr146(rv146);
  vgl_rotation_3d<double> Rr146(Mr146);
  vnl_double_3 rr146 = Rr146.as_rodrigues();
  vnl_double_3 t146(0.429651, 0.051759, -0.02293);
  //=============================================================
  bool adjust_to_fl = false;
  double rv640 [] = {1871.2, 0.0,    640.0,
                     0.0,    1871.2, 360.0,
                     0.0,    0.0,    1.0  };
  vnl_double_3x3 K(rv640);
  vgl_vector_3d<double> tr(0.3207432455793182, 0.04231364883145655, -0.019929923492081336);
  icam_depth_transform dt(K, depth_img_dbl, Rr, tr, adjust_to_fl);
#if 0 // may not be needed but normalizes parameter space
  vnl_vector<double> scales(dt.n_params()); scales.fill(1.0);
  scales[0]=10;   scales[1]=10;   scales[2]=10;   scales[3]=1;
  scales[4]=1;   scales[5]=1; scales[6]= 1/2000.0;
  dt.set_scale_factors(scales);
#endif
  // Typical parameters
  unsigned min_pyramid_image_size = 16;
  unsigned box_reduction_k = 2;
  double axis_search_cone_multiplier = 10.0;
  double polar_range_multiplier = 2.0;
  double local_min_thresh = 0.005;
  //vcl_string base_path = "C:/images/calibration";
  vcl_string base_path = "";
  bool verbose = true;
  icam_minimizer minimizer(source_img_flt, dest_img_flt, dt,
                           min_pyramid_image_size, box_reduction_k,
                           axis_search_cone_multiplier, polar_range_multiplier,
                           local_min_thresh, base_path, verbose);
  if (verbose) {
    minimizer.set_actual_translation(tr);
    minimizer.set_actual_rotation(Rr);
  }
  unsigned nl = minimizer.n_levels();
  unsigned lev = nl-1;

  vgl_box_3d<double> trans_box;
  trans_box.add(vgl_point_3d<double>(-.5, -.5, -.5));
  trans_box.add(vgl_point_3d<double>(.5, .5, .5));
  vgl_vector_3d<double> trans_steps(0.5, 0.5, 0.5);

  vgl_vector_3d<double> full_min_trans;
  vgl_rotation_3d<double> full_min_rot;
  double min_allowed_overlap = 0.5, full_min_cost,full_min_overlap;
  bool full = minimizer.camera_search(trans_box,
                                      trans_steps,
                                      lev-2,
                                      min_allowed_overlap,
                                      false,//no refine step
                                      full_min_trans,
                                      full_min_rot,
                                      full_min_cost,
                                      full_min_overlap);
  if (full) {
    vcl_cout << "full min error " << full_min_cost << '\n'
             << "full min overlap " << full_min_overlap << '\n'
             << "full min translation     "
             << full_min_trans << '\n'
             << "full min rotation "
             << full_min_rot.as_rodrigues() << '\n';
  }
}


TESTMAIN( test_minimizer );

