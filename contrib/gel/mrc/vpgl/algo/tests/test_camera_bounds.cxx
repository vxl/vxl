#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
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
}

TESTMAIN(test_camera_bounds);
