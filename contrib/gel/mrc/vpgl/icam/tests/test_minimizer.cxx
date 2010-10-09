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
#include <vgl/vgl_point_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_bounds.h>
#include <icam/icam_minimizer.h>

static void test_minimizer()
{
  vcl_string root_dir = testlib_root_dir();
  vcl_string dest_file =   "C:/images/calibration/frame_142.png";
  vcl_string source_file = "C:/images/calibration/frame_145.png";
  vcl_string depth_file =  "C:/images/calibration/depth_142.tif";

  vil_image_view_base_sptr dest_img_base = vil_load(dest_file.c_str());
  if (!dest_img_base) {
    vcl_cerr << "error loading image." << vcl_endl;
    return;
  }
  vil_image_view_base_sptr source_img_base = vil_load(source_file.c_str());
  if (!source_img_base) {
    vcl_cerr << "error loading image." << vcl_endl;
    return;
  }

  vil_image_view_base_sptr depth_img_base = vil_load(depth_file.c_str());
  if (!depth_img_base) {
    vcl_cerr << "error loading image." << vcl_endl;
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
  // relative rotation for source camera
  double rv [] ={0.9949824417310001, 0.07167609924, -0.06980290590899998,
                 -0.073085399753, 0.997165853331, -0.017858933610000002,
                 0.06832371779200001, 0.02287012861500001, 0.997400346057};
  vnl_vector_fixed<double,3> zaxis(0.0, 0.0, 1.0),
    pa(0.06832371779200001, 0.02287012861500001, 0.997400346057);
  vgl_rotation_3d<double> Rpa(pa, zaxis);
  vcl_cout << "rotation for principal axis = " << Rpa.as_rodrigues() << '\n'
           << "Rotated principal ray = " << Rpa*pa << '\n';
  vnl_matrix_fixed<double,3, 3> Mr(rv), K(0.0);
  vgl_rotation_3d<double> Rr(Mr), Rid;
  Rid.set_identity();
  vcl_cout << "Transformed principal ray = " << Rr*pa << '\n';
  vnl_vector_fixed<double,3> rd = Rr.as_rodrigues();
  double ang = 180.0*rd.magnitude()/3.142;
  rd = rd.normalize();
  vgl_rotation_3d<double> Rpz = Rr*Rpa.transpose();
  vnl_vector_fixed<double, 3> Rpzr = Rpz.as_rodrigues();
  double angz = Rpzr[2];
  vnl_vector_fixed<double, 3> rot_around_pa(0.0, 0.0, angz);
  vgl_rotation_3d<double> Rapa(rot_around_pa);
  vgl_rotation_3d<double> Rpza =Rapa*Rpa;
  vcl_cout << "Rpz\n" << Rpz.as_matrix() << '\n'
           << "Rpza\n" << Rpza.as_matrix() << '\n'
           << "angle = " << ang << "  axis = " << rd << '\n';
  vgl_vector_3d<double> tr(0.3207432455793182, 0.04231364883145655, -0.019929923492081336);
  K[0][0]=1871.2;   K[1][1]=1871.2; K[0][2] = 640.0; K[1][2]=360.0; K[2][2]=1.0;
  bool adjust_to_fl = false;
  //  icam_depth_transform dt(K, depth_img_dbl, Rr, tr, adjust_to_fl);
  icam_depth_transform dt(K, depth_img_dbl, Rid, tr, adjust_to_fl);
  unsigned nparams = dt.n_params();
  vnl_vector<double> scales(nparams);
  // may not be needed but normalizes parameter space
  scales.fill(1.0);
#if 0
  scales[0]=10;   scales[1]=10;   scales[2]=10;   scales[3]=1;
  scales[4]=1;   scales[5]=1; scales[6]= 1/2000.0;
  dt.set_scale_factors(scales);
#endif
  icam_minimizer minimizer(source_img_flt, dest_img_flt, dt, 16);
  unsigned nl = minimizer.n_levels();
  unsigned lev = nl-1;

#if 0
  minimizer.minimize(Rr, tr, 1871.0);
  vcl_cout << "True Rotation\n" << Mr << '\n'
           << "True Translation\n" << tr << '\n'
           << "end error " << minimizer.end_error() << '\n'
           << "Minimized focal length "<< minimizer.to_fl() << '\n'
           << "Minimized Rotation\n" << minimizer.rotation().as_matrix() << '\n'
           << "Minimized Translation\n" << minimizer.translation() << vcl_endl;
#endif
#if 0
  unsigned L = 3, pindx = 0, ind = 0;
  double pmin = 0, pmax = 0.1, pinc = 0.01;
  vcl_vector<double> er = minimizer.error(Rr,tr, L, pindx, pmin, pmax, pinc);
  for (double p =pmin; p<=pmax; p+=pinc)
    vcl_cout << p << ' ' << er[ind++] << '\n';
#endif
#if 0
  vcl_vector<vil_image_view<float> > views =
    minimizer.views(Rr,tr, L, pindx, pmin, pmax, pinc);
  char* str = new char[30];
  for (double p =pmin; p<=pmax; p+=pinc)
  {
    vcl_sprintf(str, "pview_%.4g_.tif", p );
    vil_save(views[ind++], str);
  }
  delete [] str;
#endif
#if 0
  vnl_vector_fixed<double, 3> Rrar = Rr.as_rodrigues();
  vcl_cout << "True Rotation " << Rrar << '\n';
  // test scan over rotation angles
  icam_cost_func cost = minimizer.cost_fn(lev);
  double cst = cost.error(Rr.as_rodrigues(), tr);
  vcl_cout << "minimum cost " <<  cst
           << " for fraction = " << cost.frac_samples() << '\n';
  vpgl_perspective_camera<double> dcam = minimizer.dest_cam(lev);

  double pixel_cone_ang, pixel_solid_ang;
  vpgl_camera_bounds::pixel_solid_angle(dcam, pixel_cone_ang, pixel_solid_ang);
  vcl_cout << "pixel half_ang " << pixel_cone_ang << " pixel solid ang "
           << pixel_solid_ang << '\n';

  double image_cone_ang, image_solid_ang;
  vpgl_camera_bounds::image_solid_angle(dcam, image_cone_ang, image_solid_ang);
  vcl_cout << "image half_ang " << image_cone_ang << " image solid ang "
           << image_solid_ang << '\n';

  unsigned npts = static_cast<unsigned>(image_solid_ang/pixel_solid_ang);
  vcl_cout << "N pixel solid angs in image " << npts << '\n';
  double polar_inc = vpgl_camera_bounds::rotation_angle_interval(dcam);
  vcl_cout << "Polar rotation increment " << polar_inc << '\n';
  unsigned nangle_steps = static_cast<unsigned>(2.0*vnl_math::pi/polar_inc );
  vcl_cout << "N angle steps in 360 " << nangle_steps << '\n'<< vcl_flush;
#endif
#if 0
  principal_ray_scan prs(image_cone_ang, npts);
  vcl_cout << "Total scan steps "
           << static_cast<unsigned>(npts*nangle_steps) << '\n' << vcl_flush;
  double polar_range = vnl_math::pi;
  double min_cost = 1.0e10, min_frac;
  vnl_vector_fixed<double,3> min_rod;
  vul_timer tim;
  for (prs.reset(); prs.next(); ) {
    //vgl_rotation_3d<double> rot = prs.rot(-0.0725071);
    for (double ang = -polar_range; ang<=polar_range; ang+=polar_inc)
    {
      vgl_rotation_3d<double> rot = prs.rot(ang);
      vnl_vector_fixed<double, 3> rod = rot.as_rodrigues();
      double dist = (Rrar-rod).magnitude();
      double c = cost.error(rod, tr);
      if (c==vnl_numeric_traits<double>::maxval)
        continue;
#if 0
      if (c<20)
        vcl_cout << "cost( "<< dist << " )= " << c << '\n';
#endif
      if (c<min_cost) {
        min_cost = c;
        min_rod = rod;
        min_frac = cost.frac_samples();
      }
    }
  }
  vcl_cout << "scan took " << tim.real()/1000.0 << " seconds\n"
           << "min cost " << min_cost << "  min rod " << min_rod
           << " fraction " << min_frac << '\n';
#endif

  double min_alowed_overlap = 0.5;
  double min_error, min_overl;
  vgl_rotation_3d<double> min_rot, global_min_rot;
  vgl_vector_3d<double> min_trans;
  minimizer.exhaustive_rotation_search(tr, lev, min_alowed_overlap,
                                       min_rot, min_error,
                                       min_overl);
  vcl_cout << "correct translation is " << tr << '\n'
           << "granular rotation with correct translation is "
           << min_rot.as_rodrigues() << '\n'
           << "error for correct transformation is " << min_error
           << " with " << min_overl << " overlap\n";
#if 0
  icam_cost_func cost = minimizer.cost_fn(lev);
  vnl_vector<double> mapped_source, map_mask, dest;
  cost.samples(min_rot.as_rodrigues(), trom, mapped_source, map_mask, dest);
  for (unsigned i = 0; i<dest.size(); ++i)
    vcl_cout << mapped_source[i] << ' '
             << map_mask[i] << ' '
             << dest[i] << '\n';
#endif
#if 0
  double global_min = vnl_numeric_traits<double>::maxval;
  vgl_vector_3d<double> global_min_tr;
  for (double tro = -.5; tro<=.5; tro+=0.01) {
    vgl_vector_3d<double> trom(tr.x(), tr.y(), tr.z()+tro);
    vcl_cout << "searching at tro = " << tro << '\n'
             << " vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n";
    bool success = minimizer.exhaustive_rotation_search(trom, lev,min_alowed_overlap,
                                                        min_rot, min_error,
                                                        min_overl);
    if (!success) continue;
    vcl_cout << "min error " << min_error << " with " << min_overl
             << " overlap\n"
             << "rotation at minimum is " << min_rot.as_rodrigues() << '\n';
    double rot_dist = (min_rot.as_rodrigues()-Rr.as_rodrigues()).magnitude();
    vcl_cout << "distance from true rotation " << rot_dist << "\n\n";
    if (min_error<global_min) {
      global_min = min_error;
      global_min_rot = min_rot;
      global_min_tr = trom;
    }
  }
  vcl_cout << "global min error " << global_min << '\n'
           << "actual translation     " << tr << '\n'
           << "global min translation " << global_min_tr << '\n';
#endif
  vgl_box_3d<double> trans_box;
  trans_box.add(vgl_point_3d<double>(-.5, -.5, -.5));
  trans_box.add(vgl_point_3d<double>(.5, .5, .5));
  vgl_vector_3d<double> trans_steps(0.25, 0.25, .25);
  minimizer.exhaustive_camera_search(trans_box, trans_steps,
                                     lev,min_alowed_overlap,
                                     min_trans,
                                     global_min_rot,
                                     min_error,
                                     min_overl);

  vcl_cout << "global min error " << min_error << '\n'
           << "global min overlap " << min_overl << '\n'
           << "global min translation     " << min_trans << '\n'
           << "global min rotation " << global_min_rot.as_rodrigues() << '\n';

#if 0
  vil_image_view<float> dest_view = minimizer.dest(lev);
  vil_image_view<float> min_view = minimizer.view(global_min_rot,
                                                  global_min_tr, lev);
  vcl_string act_file = "c:/images/calibration/act_dest.tif";
  vcl_string min_file = "c:/images/calibration/min_dest.tif";
  vil_save(dest_view, act_file.c_str());
  vil_save(min_view, min_file.c_str());
#endif
}


TESTMAIN( test_minimizer );

