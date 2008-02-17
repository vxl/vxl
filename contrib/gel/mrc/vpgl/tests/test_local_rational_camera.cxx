#include <testlib/testlib_test.h>
#include <vpl/vpl.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_double_3.h>

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

static void test_local_rational_camera()
{
  vpgl_rational_camera<double>& rcam = construct_rational_camera();
  double xoff = rcam.offset(vpgl_rational_camera<double>::X_INDX);
  double yoff = rcam.offset(vpgl_rational_camera<double>::Y_INDX);
  double zoff = rcam.offset(vpgl_rational_camera<double>::Z_INDX);
  
  bgeo_lvcs lvcs(yoff, xoff, zoff);
  vpgl_local_rational_camera<double> lrcam(lvcs, rcam);
  double ug, vg, ul, vl;
  rcam.project(xoff, yoff, zoff, ug, vg);
  lrcam.project(0.0, 0.0, 0.0, ul, vl);
  vcl_cout << "Global (u v) (" << ug << ' ' << vg << ")\n";
  vcl_cout << "Local (u v) (" << ul << ' ' << vl << ")\n";
  TEST_NEAR("local projection", vcl_fabs(ug-ul)+vcl_fabs(vg-vl), 0.0, 1e-3);
  //---- test file I/O
  vcl_string path = "./test.lrcam";
  bool good = lrcam.save(path);
  TEST("save to file", good, true);
  vpgl_local_rational_camera<double>* lrc_r = read_local_rational_camera<double>(path);
  double ulr, vlr;
  lrc_r->project(0.0, 0.0, 0.0, ulr, vlr);
  TEST_NEAR("read from file", vcl_fabs(ug-ulr)+vcl_fabs(vg-vlr), 0.0, 1e-3);
  vpl_unlink(path.c_str());
  // test binary I/O
  vcl_string b_path = "./test_binary.vsl";
  vsl_b_ofstream os(b_path);
  lrcam.b_write(os);
  os.close();
  vsl_b_ifstream is(b_path);
  vpgl_local_rational_camera<double> lrcam_r;
  lrcam_r.b_read(is);
  double ulb, vlb;
  lrc_r->project(0.0, 0.0, 0.0, ulb, vlb);
  TEST_NEAR("read from binary file", vcl_fabs(ug-ulb)+vcl_fabs(vg-vlb),
            0.0, 1e-3);
  //-- test other geographic locations
  double x0 = -71.402457, y0 = 41.821589, z0 = 20;
  double ug0, vg0, ul0, vl0;
  rcam.project(x0, y0, z0, ug0, vg0);
  lrcam.project(202.47, 0, 50, ul0, vl0);
  TEST_NEAR("test displacement East", vcl_fabs(ug0-ul0)+vcl_fabs(vg0-vl0),
            0.0, 3);
  double x1 = -71.404887, y1 = 41.823402, z1 = 16;
  double ug1, vg1, ul1, vl1;
  rcam.project(x1, y1, z1, ug1, vg1);
  lrcam.project(0, 200, 46, ul1, vl1);
  TEST_NEAR("test displacement North", vcl_fabs(ug1-ul1)+vcl_fabs(vg1-vl1),
            0.0, 3);
}

TESTMAIN(test_local_rational_camera);

