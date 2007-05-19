#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_rank.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_rational_camera.h>

static void composite_projection(vpgl_proj_camera<double> const& camera,
                                 const vpgl_scale_offset<double>& sox,
                                 const vpgl_scale_offset<double>& soy,
                                 const vpgl_scale_offset<double>& soz,
                                 const double x, const double y, 
                                 const double z,
                                 double& u, double& v
                                 )
{
  double xn = sox.normalize(x);
  double yn = soy.normalize(y);
  double zn = soz.normalize(z);
  camera.project(xn, yn, zn, u, v);
}
static void test_camera_compute()
{
  // PART 1: Test the affine camera computation

  vnl_vector_fixed<double,4> r1( 1, 2, 3, 4 );
  vnl_vector_fixed<double,4> r2( -1, 4, -2, 0 );
  vpgl_affine_camera<double> C1( r1, r2 );
  vcl_vector< vgl_point_3d<double> > world_pts;
  world_pts.push_back( vgl_point_3d<double>( 1, 0, -1 ) );
  world_pts.push_back( vgl_point_3d<double>( 6, 1, 2 ) );
  world_pts.push_back( vgl_point_3d<double>( -1, -3, -2 ) );
  world_pts.push_back( vgl_point_3d<double>( 0, 0, 2 ) );
  world_pts.push_back( vgl_point_3d<double>( 2, -1, -5 ) );
  world_pts.push_back( vgl_point_3d<double>( 8, 1, -2 ) );
  world_pts.push_back( vgl_point_3d<double>( -4, -4, 5 ) );
  world_pts.push_back( vgl_point_3d<double>( -1, 3, 4 ) );
  world_pts.push_back( vgl_point_3d<double>( 1, 2, -7 ) );
  vcl_vector< vgl_point_2d<double> > image_pts;
  for ( unsigned int i = 0; i < world_pts.size(); ++i )
    image_pts.push_back( C1.project( vgl_homg_point_3d<double>(world_pts[i]) ) );

  vpgl_affine_camera_compute acc;
  vpgl_affine_camera<double> C1e;
  acc.compute( image_pts, world_pts, C1e );

  vcl_cerr << "\nTrue camera matrix:\n" << C1.get_matrix() << '\n'
           << "\nEstimated camera matrix:\n" << C1e.get_matrix() << '\n';
  TEST_NEAR( "vpgl_affine_camera_compute:",
             ( C1.get_matrix()-C1e.get_matrix() ).frobenius_norm(), 0, 1 );

  //============== Test projective approximation to rational camera =======
  // the coefficients
  double n_u[20] =  { 38790, 2099.25, 2900.55, 2.64833e+006, -218.811, 3245.65,
                      5.79441e+006 , -1662.86, -1.94933e+006, 8.62956e+008,
                      -11.1462, -1136.15, -28872.1, 691.187, 1366.12,
                      -4.85465e+006, -93.2575, -40860.3, 2.36095e+007,
                      -1.22247e+008};

  double d_u[20] = { -1.36512, 3.803, -101.798, 44842.5, 0.709332, -8.52022,
                     3336.12 , 0.254181, -5913.26, 3.06869e+006, 0.198631,
                     -4.1618, 1450.89 , -5.10025 , -10430.8 , 6.96131e+006, 
                     3.2923, 3058.19, -4.37677e+006, 1e+009};

  double n_v[20]={ -572.892, -6095.51, -1297.11, -89466.6, -401.141, 931.828,
                   -386354, -267.976, 62537.5, -1.45554e+006, -170.631,
                   1578.18, -809130, -808.989, 637494, -1.09453e+008, 79.3162,
                   -58735.7, 7.95173e+006, 1e+009};

  double d_v[20]={ -0.459374, 1.27974, -34.2558, 15089.9, 0.238696, -2.86712,
                   1122.63, 0.085534,  -1989.86, 1.03264e+006, 0.0668408,
                   -1.40048, 488.235, -1.71628, -3510.04, 2.34254e+006,
                   1.10788, 1029.11, -1.47282e+006 , 3.36508e+008};

  vnl_matrix_fixed<double, 4, 20> cmatrix;
  for(unsigned i = 0; i<20; ++i)
    {
      cmatrix[0][i]=n_u[i];  cmatrix[1][i]=d_u[i];
      cmatrix[2][i]=n_v[i];  cmatrix[3][i]=d_v[i];
    }
  //The scales and offsets
  vpgl_scale_offset<double> sox(0.0667, -115.725);
  vpgl_scale_offset<double> soy(0.0183, 36.4349);
  vpgl_scale_offset<double> soz(1257, 2019);
  vpgl_scale_offset<double> sou(6936, 6936);
  vpgl_scale_offset<double> sov(5866, 5866);
  vcl_vector<vpgl_scale_offset<double> > scale_offsets;
  scale_offsets.push_back(sox);   scale_offsets.push_back(soy);
  scale_offsets.push_back(soz);   scale_offsets.push_back(sou);
  scale_offsets.push_back(sov); 
  //Construct the rational camera
  vpgl_rational_camera<double> rat_cam(cmatrix, scale_offsets);

  //Approximate the camera at the center of the volume of view
  vpgl_proj_camera<double> camera;
  vgl_point_3d<double> center(-115.725, 36.4349, 2019);

  vpgl_proj_camera_compute pcc(rat_cam);
  pcc.compute(center, camera);

  //the normalizing scale transformation
  vgl_h_matrix_3d<double> T = pcc.norm_trans();
  vcl_cout << "T \n" << T << '\n';
  //Points to project for testing the approximation
  vgl_homg_point_3d<double> p0(-115.725, 36.4349, 2019), hp0n;
  vgl_homg_point_3d<double> p1(-115.73, 36.44, 2019), hp1n;
  //Note that p1 is about 1/2 mile from p0.
  //Non-homogenous forms
  vgl_point_3d<double> wp0(p0), wp1(p1);
  //Normalize the world coordinates
  hp0n = T(p0); 
  hp1n = T(p1); 
  //3-d points
  vgl_point_3d<double> p0n(hp0n), p1n(hp1n);
  vcl_cout << "Normalized p0 " << p0n << '\n';
  vcl_cout << "Normalized p1 " << p1n << '\n';
  
  //Project points
  vgl_point_2d<double> hpp0, hpp1;
  vgl_point_2d<double> rpp0,  rpp1;
  
  vcl_cout << "Project p0 \n";
  hpp0 = camera(hp0n);
  vgl_point_2d<double> pp0(hpp0);
  vcl_cout << "Projective camera " << pp0 << '\n';

  rpp0 = rat_cam.project(wp0);
  vcl_cout << "Rational camera " << rpp0 << '\n';
  double p0_error = 
    vcl_fabs(pp0.x()-rpp0.x())+vcl_fabs(pp0.y()-rpp0.y());
  TEST_NEAR("Project Origin", p0_error, 0.0, 0.1);

  vcl_cout << "Project p1 \n";
  hpp1 = camera(hp1n);
  vgl_point_2d<double> pp1(hpp1);
  vcl_cout << "Projective camera " << pp1 << '\n';

  rpp1 = rat_cam.project(wp1);
  vcl_cout << "Rational camera " << rpp1 << '\n';

  double p1_error = 
    vcl_fabs(pp1.x()-rpp1.x())+vcl_fabs(pp1.y()-rpp1.y());
  TEST_NEAR("Project P1", p1_error, 0.0, 2.0);

  //Re-approximate the camera at the shifted position
  vpgl_proj_camera<double> s_camera;
  vgl_point_3d<double> s_center(-115.73, 36.44, 2019);
  pcc.compute(s_center, s_camera);

  vcl_cout << "Project shifted p1 \n";
  hpp1 = s_camera(hp1n);
  vgl_point_2d<double> s_pp1(hpp1);
  vcl_cout << "Shifted Projective camera " << s_pp1 << '\n';

  rpp1 = rat_cam.project(wp1);
  vcl_cout << "Rational camera " << rpp1 << '\n';

  double s_p1_error = 
    vcl_fabs(s_pp1.x()-rpp1.x())+vcl_fabs(s_pp1.y()-rpp1.y());
  TEST_NEAR("Project Shifted P1", s_p1_error, 0.0, 2.0);
  
}

TESTMAIN(test_camera_compute);
