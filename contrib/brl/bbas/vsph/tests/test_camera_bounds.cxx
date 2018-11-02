#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#ifdef DEBUG
#include <testlib/testlib_root_dir.h>
#endif
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <bpgl/algo/bpgl_project.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vsph/vsph_camera_bounds.h>

#ifdef DEBUG
static void
write_points_vrml(std::ofstream& str,
                  std::vector<vgl_point_3d<double> > const& pts3d)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n"
      << "PointLight {\n"
      << "  on FALSE\n"
      << "  intensity 1\n"
      << "ambientIntensity 0\n"
      << "color 1 1 1\n"
      << "location 0 0 0\n"
      << "attenuation 1 0 0\n"
      << "radius 100\n"
      << "}\n"

      << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry PointSet {\n"
      << "      color NULL\n"
      << "      coord Coordinate{\n"
      << "       point[\n";
  int n = pts3d.size();
  for (int i =0; i<n; i++)
    str << -pts3d[i].x() << ' ' << pts3d[i].y() << ' ' << pts3d[i].z() << '\n';
  str << "   ]\n  }\n }\n}\n"

      << "Transform {\n"
      << "translation " << 0 << ' ' << 0 << ' '
      << ' ' << 0 << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << 0 << ' ' << 1 << ' ' << 0 << '\n'
      << "      transparency " << 0 << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Sphere\n"
      <<   "{\n"
      << "  radius " << 0.95 << '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
}
#endif

static void test_camera_bounds()
{
  const double rad_to_deg = vnl_math::deg_per_rad;
  vnl_matrix_fixed<double, 3, 3> Km(0.0);
  Km[0][0] = 1871.2; Km[1][1] = 1871.2;
  Km[0][2] = 640.0;   Km[1][2] = 360.0;
  Km[2][2] = 1.0;
  vpgl_calibration_matrix<double> K(Km);
  //camera rotation
  double rv [] ={0.067851, 0.994060, 0.085090,
                 0.669585, 0.017856, -0.742521,
                 -0.739630, 0.107355, -0.664396 };
  vnl_matrix_fixed<double,3, 3> Mr(rv);
  vgl_rotation_3d<double> Rr(Mr);
  //camera translation
  vgl_vector_3d<double> t(-0.013036, 0.387514, 4.134744);
  vpgl_perspective_camera<double> C(K, Rr, t);
  vgl_ray_3d<double> cone_axis;
  double half_ang, sang;
  vsph_camera_bounds::pixel_solid_angle(C, 640, 360,
                                        cone_axis,half_ang, sang);
  std::cout << " pixel "<< cone_axis << '\n'
           << " half angle (deg) " << half_ang*rad_to_deg
           << " solid_ang (ster) = " << sang << '\n';
  TEST_NEAR("pixel bounds", sang, 4.48621e-007, 1e-8);
  vsph_camera_bounds::image_solid_angle(C,cone_axis,half_ang, sang);
  std::cout << " image "<< cone_axis << '\n'
           << " half angle (deg)" << half_ang*rad_to_deg
           << " solid_ang = " << sang << '\n';
  TEST_NEAR("image bounds", sang, vnl_math::log10e, 1e-4);
  vgl_point_3d<double> minp(-0.3, -0.3, 0), maxp(-0.9, 0.3, 0.3);
  vgl_box_3d<double> box;
  box.add(minp); box.add(maxp);
  vgl_box_2d<double> box_prj = bpgl_project::project_bounding_box(C, box);
  std::cout << "Project bounding box: " << box_prj << '\n';
  bool good = vsph_camera_bounds::box_solid_angle(C, box, cone_axis,half_ang, sang);
  double mask = good ? 1.0 : 0.0;
  std::cout << " box "<< cone_axis << '\n'
           << " half angle (deg) " << half_ang*rad_to_deg
           << " solid_ang = " << sang << '\n';
  TEST_NEAR("box bounds", sang, mask*0.0294621, 1e-6);

  double pixel_interval = vsph_camera_bounds::rotation_angle_interval(C);
  std::cout << " pixel angle interval (deg)  "<< pixel_interval*rad_to_deg << '\n';
  TEST_NEAR("pixel interval (deg)", pixel_interval*rad_to_deg, 0.159155, 1e-5);
  double scl = 0.0625;
  Km[0][0] = scl*1871.2; Km[1][1] = scl*1871.2;
  Km[0][2] = scl*640.0;   Km[1][2] = scl*360.0;
  Km[2][2] = 1.0;
  vpgl_calibration_matrix<double> Ks(Km);
  vpgl_perspective_camera<double> Cs(Ks, Rr, t);
  pixel_interval = vsph_camera_bounds::rotation_angle_interval(Cs);
  std::cout << " pixel angle interval (deg) - scaled K "<< pixel_interval*rad_to_deg << '\n';
  TEST_NEAR("pixel interval (deg) -Ks ", pixel_interval*rad_to_deg, 2.54606, 1e-5);
  vsph_camera_bounds::pixel_solid_angle(Cs, static_cast<unsigned>(scl*640),
                                        static_cast<unsigned>(scl*360),
                                        cone_axis,half_ang, sang);
  std::cout << " pixel  - Ks "<< cone_axis << '\n'
           << " half angle (deg)" << half_ang*rad_to_deg
           << " solid_ang (ster) = " << sang << '\n';
  TEST_NEAR("pixel bounds - Ks", sang, 0.000114845, 1e-8);

  vsph_camera_bounds::image_solid_angle(Cs,cone_axis,half_ang, sang);
  std::cout << " image -Ks "<< cone_axis << '\n'
           << " half angle " << half_ang*rad_to_deg
           << " solid_ang = " << sang << '\n';
  TEST_NEAR("image bounds -Ks", sang, vnl_math::log10e, 1e-4);
  // test sphere samples
  unsigned npts = 400;
  principal_ray_scan prs(0.785, npts);
#ifdef DEBUG
  std::vector<vgl_point_3d<double> > pts;
  for (prs.reset(); prs.next();)
  {
    std::cout << "theta = " << prs.theta()*rad_to_deg
             << " phi = " << prs.phi()*rad_to_deg << '\n';
    pts.push_back(prs.pt_on_unit_sphere());
  }
  std::ofstream os(testlib_root_dir() + "/contrib/gel/mrc/vpgl/tests/images/calibration/sphere.wrl");
  write_points_vrml(os, pts);
  os.close();
#endif
  unsigned indx = 200;
  double ang_pr = 1.0/std::sqrt(2.0);
  vgl_point_3d<double> pt =  prs.pt_on_unit_sphere(indx), zaxis(0.0, 0.0, 1.0);
  vgl_rotation_3d<double> rot = prs.rot(indx, ang_pr);
  vgl_point_3d<double> rot_z = (rot.transpose())*zaxis;
  std::cout << "theta = " << prs.theta(indx)*rad_to_deg
           << " phi = " << prs.phi(indx)*rad_to_deg << '\n'
           << " pt on unit sphere " << pt << '\n'
           << " rotated zaxis " << rot_z << '\n';
  TEST_NEAR("rotation scan", rot_z.x(), pt.x(), 1e-4);

  vnl_matrix_fixed<double, 3, 3> k;
  k.set_identity();
  vpgl_calibration_matrix<double> K01(k);
  // test relative camera transform
  // c142
  double r0 [] ={   0.067851,  0.994060,  0.085090,
                    0.669585,  0.017856, -0.742521,
                   -0.739630,  0.107355, -0.664396 };
  vnl_matrix_fixed<double, 3, 3> M0(r0);
  vgl_rotation_3d<double> R0(M0);
  vgl_vector_3d<double> t0(-0.013036, 0.387514, 4.134744);

  //c 145
  double r1 [] ={ 0.167131,  0.982859,  0.077819,
                  0.675937, -0.056764, -0.734770,
                 -0.717758,  0.175403, -0.673837 };

  vnl_matrix_fixed<double, 3, 3> M1(r1);
  vgl_rotation_3d<double> R1(M1);
  vgl_vector_3d<double> t1(0.046931, 0.355840, 4.112037);

  vpgl_perspective_camera<double> c0(K01, R0, t0);
  vpgl_perspective_camera<double> c1(K01, R1, t1);

  vgl_rotation_3d<double> Rrel;
  vgl_vector_3d<double> trel;
  vsph_camera_bounds::relative_transf(c0, c1, Rrel, trel);
  vnl_vector_fixed<double,3> Rrod = Rrel.as_rodrigues();
  vnl_vector_fixed<double, 3> rod_act(0.020400079326396786,
                                      -0.069183868389146475,
                                      -0.072507096936119245);
  vgl_vector_3d<double> trel_act(0.320743, 0.04231, -0.0199299);
  double rod_er  = (Rrod-rod_act).magnitude();
  double t_er  = (trel_act-trel).length();
  TEST_NEAR("relative camera transform", rod_er + t_er, 0.0, 0.00001);

  // test cylinder
  vbl_array_2d<vgl_ray_3d<double> > rays(2,2);
  vgl_vector_3d<double> dir(vnl_math::sqrt1_2, 0.0, -vnl_math::sqrt1_2);
  rays[0][0]= vgl_ray_3d<double>(vgl_point_3d<double>(0.0,0.0,1.0), dir);
  rays[0][1]= vgl_ray_3d<double>(vgl_point_3d<double>(1.0,0.0,1.0), dir);
  rays[1][0]= vgl_ray_3d<double>(vgl_point_3d<double>(0.0,1.0,1.0), dir);
  rays[1][1]= vgl_ray_3d<double>(vgl_point_3d<double>(1.0,1.0,1.0), dir);
  vpgl_generic_camera<double> gcam(rays);
  double r00, r01, r10, r11;
  vgl_ray_3d<double> axis;
  vsph_camera_bounds::pixel_cylinder(gcam, 0, 0, axis, r00);
  vsph_camera_bounds::pixel_cylinder(gcam, 0, 1, axis, r01);
  vsph_camera_bounds::pixel_cylinder(gcam, 1, 0, axis, r10);
  vsph_camera_bounds::pixel_cylinder(gcam, 1, 1, axis, r11);
  bool cy1 = axis.origin() == vgl_point_3d<double>(1.0,1.0,1.0);
  double er = (axis.direction()-dir).length();
  bool cy2 = er<0.00001;
  bool cy3 = (r00 == r01)&&(r00 == r10)&&(r00 == r11);
  TEST("pixel cylinder",cy1&&cy2&&cy3, true);
}

TESTMAIN(test_camera_bounds);
