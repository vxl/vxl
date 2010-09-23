#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/algo/vpgl_project.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vpgl/algo/vpgl_camera_bounds.h>
#if 0 //debug
static void 
write_points_vrml(vcl_ofstream& str, 
                  vcl_vector<vgl_point_3d<double> > const& pts3d)
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
      << "}\n";

  str << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry PointSet {\n"
      << "      color NULL\n"
      << "      coord Coordinate{\n"
      << "       point[\n";
  int n = pts3d.size();
  for (int i =0; i<n; i++)
    str << -pts3d[i].x() << ' ' << pts3d[i].y() << ' ' << pts3d[i].z() << '\n';
  str << "   ]\n  }\n }\n}\n";

  str << "Transform {\n"
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
  double rad_to_deg = 180.0/vnl_math::pi;
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
  vpgl_camera_bounds::pixel_solid_angle(C, 640, 360,
                                        cone_axis,half_ang, sang);
  vcl_cout << " pixel "<< cone_axis << '\n' 
           << " half angle (deg) " << half_ang*rad_to_deg
           << " solid_ang (ster) = " << sang << '\n';
  TEST_NEAR("pixel bounds", sang, 4.48621e-007, 1.0e-008);
  vpgl_camera_bounds::image_solid_angle(C,cone_axis,half_ang, sang);
  vcl_cout << " image "<< cone_axis << '\n' 
           << " half angle (deg)" << half_ang*rad_to_deg
           << " solid_ang = " << sang << '\n';
  TEST_NEAR("image bounds", sang, 0.434238, 1.0e-006);
  vgl_point_3d<double> minp(-0.3, -0.3, 0), maxp(-0.9, 0.3, 0.3);
  vgl_box_3d<double> box;
  box.add(minp); box.add(maxp);
  vgl_box_2d<double> box_prj = vpgl_project::project_bounding_box(C, box);
  bool good = vpgl_camera_bounds::box_solid_angle(C, box, cone_axis,half_ang, sang);
  double mask = good ? 1.0 : 0.0;
  vcl_cout << " box "<< cone_axis << '\n' 
           << " half angle (deg) " << half_ang*rad_to_deg
           << " solid_ang = " << sang << '\n';
  TEST_NEAR("box bounds", sang, mask*0.0294621, 1.0e-006);

  double pixel_interval = vpgl_camera_bounds::rotation_angle_interval(C);
  vcl_cout << " pixel angle interval (deg)  "<< pixel_interval*rad_to_deg << '\n'; 
  TEST_NEAR("pixel interval (deg)", pixel_interval*rad_to_deg, 0.159155,
            1.0e-005);
  double scl = 0.0625;
  Km[0][0] = scl*1871.2; Km[1][1] = scl*1871.2; 
  Km[0][2] = scl*640.0;   Km[1][2] = scl*360.0;
  Km[2][2] = 1.0;
  vpgl_calibration_matrix<double> Ks(Km);
  vpgl_perspective_camera<double> Cs(Ks, Rr, t);
  pixel_interval = vpgl_camera_bounds::rotation_angle_interval(Cs);
  vcl_cout << " pixel angle interval (deg) - scaled K "<< pixel_interval*rad_to_deg << '\n'; 
  TEST_NEAR("pixel interval (deg) -Ks ", pixel_interval*rad_to_deg, 2.54606,
            1.0e-005);
  vpgl_camera_bounds::pixel_solid_angle(Cs, static_cast<unsigned>(scl*640), 
                                        static_cast<unsigned>(scl*360),
                                        cone_axis,half_ang, sang);
  vcl_cout << " pixel  - Ks "<< cone_axis << '\n' 
           << " half angle (deg)" << half_ang*rad_to_deg
           << " solid_ang (ster) = " << sang << '\n';
  TEST_NEAR("pixel bounds - Ks", sang, 0.000114845, 1.0e-008);

  vpgl_camera_bounds::image_solid_angle(Cs,cone_axis,half_ang, sang);
  vcl_cout << " image -Ks "<< cone_axis << '\n' 
           << " half angle " << half_ang*rad_to_deg
           << " solid_ang = " << sang << '\n';
  TEST_NEAR("image bounds -Ks", sang, 0.434238, 1.0e-006);
  // test sphere samples
  unsigned npts = 400;
  principal_ray_scan prs(0.785, npts);
#if 0 //debug
  vcl_vector<vgl_point_3d<double> > pts;
  for(prs.reset(); prs.next();)
    {
      vcl_cout << "theta = " << prs.theta()*180.0/vnl_math::pi 
               << " phi = " << prs.phi()*180.0/vnl_math::pi << '\n';
      pts.push_back(prs.pt_on_unit_sphere());
    }
  vcl_ofstream os("c:/images/Calibration/sphere.wrl");
  write_points_vrml(os, pts);
  os.close();
#endif
  //0.366254 -0.0426265 0.60335
  unsigned indx = 200;
  double ang_pr = 1.0/vcl_sqrt(2.0);
  vgl_point_3d<double> pt =  prs.pt_on_unit_sphere(indx), zaxis(0.0, 0.0, 1.0);
  vgl_rotation_3d<double> rot = prs.rot(indx, ang_pr);
  vgl_point_3d<double> rot_z = rot*zaxis;
  vcl_cout << "theta = " << prs.theta(indx)*180.0/vnl_math::pi  
           << " phi = " << prs.phi(indx)*180.0/vnl_math::pi << '\n';
  vcl_cout << " pt on unit sphere " << pt << '\n';
  vcl_cout << " rotated zaxis " << rot_z << '\n';
  TEST_NEAR("rotation scan", rot_z.x(), pt.x(), 1.0e-4);
}

TESTMAIN(test_camera_bounds);
