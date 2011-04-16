#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/algo/vpgl_ray.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vul/vul_timer.h>
static void test_camera_compute_setup()
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

  vpgl_affine_camera<double> C1e;
  vpgl_affine_camera_compute::compute( image_pts, world_pts, C1e );

  vcl_cerr << "\nTrue camera matrix:\n" << C1.get_matrix() << '\n'
           << "\nEstimated camera matrix:\n" << C1e.get_matrix() << '\n';
  TEST_NEAR( "vpgl_affine_camera_compute:",
             ( C1.get_matrix()-C1e.get_matrix() ).frobenius_norm(), 0, 1 );
}

//a rational camera from a commercial satellite image
vpgl_rational_camera<double> construct_rational_camera()
{
  double n_u[20] =  { 3.89528e-006, -7.41303e-008, 1.25847e-005,
                      0.00205724, 5.38537e-008, -6.45546e-007,
                      -9.52482e-005, 1.50544e-007, 0.000665835, 1.00827,
                      -1.58873e-006, -3.85506e-007, 9.73561e-006,
                      -6.13083e-007, -0.000361219, 0.0238256, 9.65955e-008,
                      -1.94861e-005, 0.0155788, -0.0040918};

  double d_u[20] = { -1.68064e-006, -9.85769e-007, 3.49708e-005,
                     4.28561e-006, 2.25889e-007, -1.49309e-007,
                     -3.56398e-007, 5.07889e-007, -4.95792e-006, 0.00204326,
                     3.18231e-008,  -8.82645e-008,  -1.68424e-007,
                     2.79561e-007, -1.87894e-007, 0.000199085,  -1.71801e-007,
                     -4.7099e-007,  -0.000610182,  1};

  double n_v[20] = { -1.21292e-006, 7.83222e-006, -6.63094e-005, -0.000224627,
                     -4.85207e-006, -2.07948e-006,  -1.55515e-005,
                     -3.03478e-007, -6.18288e-005, 0.0318755, 4.11169e-005,
                     -4.28023e-007,  0.000196132,  1.06738e-005, 0.000307117,
                     -0.960259,  -9.56042e-007,  2.42754e-005,  -0.0712854,
                     -0.000395718};

  double d_v[20] = {1.44065e-005, -2.47213e-006, -8.52173e-006, -1.28085e-005,
                    4.81707e-006,  1.50614e-006,  -5.23036e-006,  1.4626e-006,
                    -3.12355e-007, -1.20281e-006, -3.51175e-005, -6.15114e-006,
                    2.89264e-005, 1.55209e-006, 1.07945e-005, -0.000160384,
                    -1.15965e-007,  -7.89301e-006,  0.000275139, 1};

  vnl_matrix_fixed<double, 4, 20> cmatrix;
  for (unsigned i = 0; i<20; ++i)
  {
    cmatrix[0][i]=n_u[i];  cmatrix[1][i]=d_u[i];
    cmatrix[2][i]=n_v[i];  cmatrix[3][i]=d_v[i];
  }
  //The scales and offsets
  vpgl_scale_offset<double> sox(0.0347, -71.4049);
  vpgl_scale_offset<double> soy(0.0219, 41.8216);
  vpgl_scale_offset<double> soz(501, -30);
  vpgl_scale_offset<double> sou(4764, 4693);
  vpgl_scale_offset<double> sov(4221, 3921);
  vcl_vector<vpgl_scale_offset<double> > scale_offsets;
  scale_offsets.push_back(sox);   scale_offsets.push_back(soy);
  scale_offsets.push_back(soz);   scale_offsets.push_back(sou);
  scale_offsets.push_back(sov);
  //Construct the rational camera
  vpgl_rational_camera<double> rat_cam(cmatrix, scale_offsets);
  return rat_cam;
}
//local rational camera for testing the generic camera conversion
vpgl_local_rational_camera<double> construct_local_rational_camera()
{
  double n_u[20] = {-7.81964e-006,6.39341e-005,4.67275e-006,0.00423585,
                    -9.69009e-005,-5.10745e-006,-0.00114887,-1.64125e-005,
                    0.000489928,0.993274,-0.000131889,-4.58868e-006,
                    0.000902171,2.37398e-007,-0.000430274,-0.00825765,
                    2.42847e-007, -1.07917e-005,-0.0143585,-0.00327465};
  double d_u[20] = {7.0187e-008,-5.40373e-007,0,-2.08288e-005,-1.0929e-006,
                    -8.71068e-008,-4.65334e-005,8.89371e-008,-3.75089e-006,
                    -0.000956589,5.16632e-007,-1.30439e-007,7.87054e-005,
                    -4.07253e-008, 2.88935e-006, 0.00111511,2.12671e-008,
                    -1.69864e-005,-0.000537438,1};
  double n_v[20] = {9.93585e-007,4.86672e-006,2.26215e-007,-0.000188252,
                    5.27564e-005,-1.5669e-007,0.00153582,6.71638e-007,
                    2.8673e-005,-0.0486115,3.90973e-005,5.30928e-006,
                    -0.00419213,1.40611e-005,0.000269255,-1.03953,
                    1.22934e-007,3.19812e-006,-0.00884028,0.00575278};
  double d_v[20] = {8.51615e-007,6.74855e-007,1.26254e-007,-1.59186e-005,
                    1.97482e-005,-4.07044e-007,3.29326e-005,-1.30921e-008,
                    -6.1929e-007,0.0018664,0.000136792,-2.30798e-006,
                    4.13577e-005,-1.38488e-007,2.19449e-006,0.00166379,
                    8.0006e-008,-1.38346e-005,0.000332436,1};  
  vnl_matrix_fixed<double, 4, 20> cmatrix;
  for (unsigned i = 0; i<20; ++i)
  {
    cmatrix[0][i]=n_u[i];  cmatrix[1][i]=d_u[i];
    cmatrix[2][i]=n_v[i];  cmatrix[3][i]=d_v[i];
  }

  //The scales and offsets
  vpgl_scale_offset<double> sox(0.0986, 44.4692);
  vpgl_scale_offset<double> soy(0.1058, 33.252);
  vpgl_scale_offset<double> soz(500,34);
  vpgl_scale_offset<double> sou(13982, 12782.0673959);
  vpgl_scale_offset<double> sov(17213, 13798.1995849);
  vcl_vector<vpgl_scale_offset<double> > scale_offsets;
  scale_offsets.push_back(sox);   scale_offsets.push_back(soy);
  scale_offsets.push_back(soz);   scale_offsets.push_back(sou);
  scale_offsets.push_back(sov);
  bgeo_lvcs lvcs(33.331465, 44.37697, 0.0, bgeo_lvcs::wgs84,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  //Construct the rational camera
  vpgl_rational_camera<double> rat_cam(cmatrix, scale_offsets);
  //Construct the local rational camera
  vpgl_local_rational_camera<double> loc_rat_cam(lvcs, rat_cam);
  return loc_rat_cam;
}
#if 0
vpgl_rational_camera<double> read_rational_camera()
{
  vcl_string path = "C:/images/calibration/rational_camera/06MAR27155829-S2AS-005580823020_01_P001.RPB";
  vpgl_rational_camera<double> rat_cam(path);
  return rat_cam;
}
#endif

void test_perspective_compute()
{
  vcl_cout << "Test Perspective Compute\n";
  vnl_vector_fixed<double, 3> rv, trans;
  for (unsigned i = 0; i<3; ++i)
    rv[i]=0.9068996774314604; // axis along diagonal, rotation of 90 degrees
  vgl_rotation_3d<double> rr(rv);

  trans[0]=10.0;   trans[1]=20.0; trans[2] = 30;

  vnl_matrix<double> Y(3, 5);
  Y[0][0] = 1.1; Y[1][0] = -0.05; Y[2][0] = 0.01;
  Y[0][1] = 0.02; Y[1][1] = 0.995; Y[2][1] = -0.1;
  Y[0][2] = -0.01; Y[1][2] = 0.04; Y[2][2] = 1.04;
  Y[0][3] = 1.15; Y[1][3] = 0.97; Y[2][3] = -0.1;
  Y[0][4] = 1.01; Y[1][4] = 1.03; Y[2][4] = 0.96;

  vnl_matrix<double> J(4,6);
  for (unsigned c = 0; c<5; ++c)
  {
    for (unsigned r = 0; r<3; ++r)
      J[r][c]=Y[r][c];
    J[3][c] = 1.0;
  }
  J[0][5] = 0.5;   J[1][5] = 1.0;  J[2][5] = -0.5;  J[3][5] = 1.0;

  vnl_matrix_fixed<double, 3, 3> pr = rr.as_matrix();
  vnl_matrix_fixed<double, 3, 4> P;
  for (unsigned r = 0; r<3; ++r)
  {
    for (unsigned c = 0; c<3; ++c)
      P[r][c] = pr[r][c];
    P[r][3] = trans[r];
  }
  // Project the 3-d points
  vnl_matrix<double> Z(2, 6);
  for (unsigned c = 0; c<6; ++c)
  {
    vnl_vector_fixed<double, 4> vpr;
    for (unsigned r = 0; r<4; ++r)
       vpr[r]=J[r][c];
    vnl_vector_fixed<double, 3> pvpr = P*vpr;
    for (unsigned r = 0; r<2; ++r)
      Z[r][c] = pvpr[r]/pvpr[2];
  }
  vcl_cout << "Projected points\n " << Z << '\n';
  vcl_vector<vgl_point_2d<double> > image_pts;
  vcl_vector<vgl_point_3d<double> > world_pts;
  for (unsigned i = 0; i<6; ++i)
  {
    vgl_point_2d<double> ip(Z[0][i], Z[1][i]);
    vgl_point_3d<double> wp(J[0][i], J[1][i], J[2][i]);
    image_pts.push_back(ip);
    world_pts.push_back(wp);
  }
  vpgl_calibration_matrix<double> K;
  vpgl_perspective_camera<double> pc;

  vpgl_perspective_camera_compute::compute(image_pts, world_pts, K, pc);
  vcl_cout << pc << '\n';
  vgl_point_3d<double> cc = pc.get_camera_center();
  TEST_NEAR("perspective camera from 6 points exact", cc.z(), -14.2265, 0.001);
}

vcl_vector<vgl_point_3d<double> > world_points()
{
  vgl_point_3d<double> p0(-71.4146, 41.8286, 0);
  vgl_point_3d<double> p1(-71.41, 41.8267, 0);
  vgl_point_3d<double> p2(-71.4109, 41.8273, 30);
  vgl_point_3d<double> p3(-71.4125, 41.8255, 0);
  vgl_point_3d<double> p4(-71.4148, 41.8309, 20);
  vgl_point_3d<double> p5(-71.4105, 41.8292, 0);
  vgl_point_3d<double> p6(-71.4103, 41.8251, 100);
  vgl_point_3d<double> p7(-71.4229,41.8242, 10);
  vgl_point_3d<double> p8(-71.425, 41.8394, 0);
  vcl_vector<vgl_point_3d<double> > wp;
  wp.push_back(p0);   wp.push_back(p1);   wp.push_back(p2);
  wp.push_back(p3);   wp.push_back(p4);   wp.push_back(p5);
  wp.push_back(p6);   wp.push_back(p7);
  return wp;
}

void test_rational_camera_approx(vcl_string dir_base)
{
  vpgl_rational_camera<double> rat_cam = construct_rational_camera();
  vcl_cout << rat_cam;
  vpgl_scale_offset<double> sox =
    rat_cam.scl_off(vpgl_rational_camera<double>::X_INDX);
  vpgl_scale_offset<double> soy =
    rat_cam.scl_off(vpgl_rational_camera<double>::Y_INDX);
  vpgl_scale_offset<double> soz =
    rat_cam.scl_off(vpgl_rational_camera<double>::Z_INDX);
  vpgl_scale_offset<double> sou =
    rat_cam.scl_off(vpgl_rational_camera<double>::U_INDX);
  vpgl_scale_offset<double> sov =
    rat_cam.scl_off(vpgl_rational_camera<double>::V_INDX);
  vgl_point_3d<double> pmin(sox.offset()-sox.scale(),
                            soy.offset()-soy.scale(), 0);
  vgl_point_3d<double> pmax(sox.offset()+sox.scale(), soy.offset()+soy.scale(),
                            soz.scale());
  vgl_box_3d<double> approx_vol(pmin, pmax);

  vpgl_perspective_camera<double> pc;
  //vpgl_perspective_camera_compute pcc;

  vgl_h_matrix_3d<double> norm_trans;
#if 0
  vpgl_perspective_camera_compute::compute(rat_cam, approx_vol, pc, norm_trans);
  vcl_cout << "Test Result\n" << pc << '\n';
#endif

  // read a camera from file
  vcl_string file =  dir_base + "/07JAN27.RPB";

  vpgl_rational_camera<double> *rat_cam2 = read_rational_camera<double>(file);
  vcl_cout << *rat_cam2;
   vpgl_scale_offset<double> sox2 = rat_cam2->scl_off(vpgl_rational_camera<double>::X_INDX);
   vpgl_scale_offset<double> soy2 = rat_cam2->scl_off(vpgl_rational_camera<double>::Y_INDX);
   vpgl_scale_offset<double> soz2 = rat_cam2->scl_off(vpgl_rational_camera<double>::Z_INDX);

  vgl_point_3d<double> pmin2(sox2.offset()-sox2.scale(),
                             soy2.offset()-soy2.scale(), 0);
  vgl_point_3d<double> pmax2(sox2.offset()+sox2.scale(),
                             soy2.offset()+soy2.scale(),
                             soz2.scale()/2);
  vgl_box_3d<double> approx_vol2(pmin2, pmax2);

  vpgl_perspective_camera<double> pc2;
  vgl_h_matrix_3d<double> norm_trans2;
  vpgl_perspective_camera_compute::compute(*rat_cam2, approx_vol2, pc2, norm_trans2);
  vcl_cout << "Test Result\n" << pc2 << '\n';
}
void test_generic_camera_compute(){
  vpgl_local_rational_camera<double> lcam = construct_local_rational_camera();
  unsigned ni = 833, nj = 877;
  vul_timer t;
  vpgl_generic_camera<double> gcam;
  bool success = vpgl_generic_camera_compute::compute(lcam, ni, nj, gcam);
  vcl_cout << " computed generic cam in " << t.real()/1000.0 << " secs.\n";
  TEST("compute generic cam", success, true);
  if(success){
    double tu = 433, tv = 325;
	double x = 457.0765, y = 526.2103, z = 34.68;
	double u0=0, v0=0;
	lcam.project(x, y, z, u0, v0);
    vgl_ray_3d<double> lray, gray;
	success = vpgl_ray::ray(lcam, tu, tv, lray);
	gray = gcam.ray(tu, tv);
	vgl_point_3d<double> lorg = lray.origin(), gorg = gray.origin();
	vgl_vector_3d<double> ldir = lray.direction(), gdir = gray.direction();
	double dorg = (lorg-gorg).length();
	double dang = angle(ldir, gdir);
	double u, v;
    gcam.project(x, y, z, u, v);
	double du = vcl_fabs(u-u0);
	double dv = vcl_fabs(v-v0);
	double del = du + dv;
	TEST_NEAR("test ray projection at center", del, 0.0, 0.5);
  // four corners
  int ua[4] = { 0, 832,   0, 832};
  int va[4] = { 0,   0, 876, 876};
  dorg = 0.0; dang = 0.0;
  for(unsigned k = 0; k<4; ++k){
    gray = gcam.ray(ua[k], va[k]);
    success = success && vpgl_ray::ray(lcam, ua[k], va[k], lray);
    lorg = lray.origin(), gorg  = gray.origin();
    ldir = lray.direction(), gdir  = gray.direction();
    dorg += (lorg-gorg).length();
    dang += vcl_fabs(angle(ldir, gdir));
  }
  TEST("corner rays", success && dorg<0.1 && dang < 0.0001, true);
  }
}
static void test_camera_compute(int argc, char* argv[])
{
  vcl_string dir_base;

  if ( argc >= 2 ) {
    dir_base = argv[1];
#ifdef VCL_WIN32
    dir_base += "\\";
#else
    dir_base += "/";
#endif
  }
  test_camera_compute_setup();
  test_perspective_compute();
  test_generic_camera_compute();
#if 0 // commented out till the new code is created for that test
  test_rational_camera_approx(dir_base);
#endif

}

TESTMAIN_ARGS(test_camera_compute)
