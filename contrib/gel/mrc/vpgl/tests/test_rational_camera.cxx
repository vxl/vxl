#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vnl/vnl_matrix_fixed.h>

static void test_rational_camera()
{
  //Test indentity camera
  vpgl_rational_camera<double> icam;//default constructor
  double x = 1.0, y = 2.0, z = 10.0;
  double u = 0, v = 0;
  icam.project(x, y, z, u, v);
  TEST( "test identity camera", u==1&&v==2, true);
  //Test a populated rational camera
  //Actual world values
  double act_x[8]={150.0, 150.0, 150.0, 150.0, 200.0, 200.0, 200.0, 200.0};
  double act_y[8]={100.0, 100.0, 225.0, 225.0, 100.0, 100.0, 225.0, 225.0};
  double act_z[8]={10.0, 15.0, 10.0, 15.0, 10.0, 15.0, 10.0, 15.0};
  // Actual projection values
  double act_u[8]={1250., 1370.65, 1388.29, 1421.9, 1047.62, 1194.53, 1205.08, 1276.68};
  double act_v[8]={365., 327.82, 405.854, 379.412, 378.572, 376.955, 400.635, 397.414};

  //Rational polynomial coefficients
  vcl_vector<double> neu_u(20,0.0), den_u(20,0.0), neu_v(20,0.0), den_v(20,0.0);
  neu_u[0]=0.1; neu_u[10]=0.071; neu_u[7]=0.01;  neu_u[9]=0.3;
  neu_u[15]=1.0; neu_u[18]=1.0, neu_u[19]=0.75;

  den_u[0]=0.1; den_u[10]=0.05; den_u[17]=0.01; den_u[9]=1.0;
  den_u[15]=1.0; den_u[18]=1.0; den_u[19]=1.0;

  neu_v[0]=0.02; neu_v[10]=0.014; neu_v[7]=0.1; neu_v[9]=0.4;
  neu_v[15]=0.5; neu_v[18]=0.01; neu_v[19]=0.33;

  den_v[0]=0.1; den_v[10]=0.05; den_v[17]=0.03; den_v[9]=1.0;
  den_v[15]=1.0; den_v[18]=0.3; den_v[19]=1.0;
  //Scale and offsets
  double sx = 50.0, ox = 150.0;
  double sy = 125.0, oy = 100.0;
  double sz = 5.0, oz = 10.0;
  double su = 1000.0, ou = 500;
  double sv = 500.0, ov = 200;
  vpgl_rational_camera<double> rcam(neu_u, den_u, neu_v, den_v,
                                    sx, ox,
                                    sy, oy,
                                    sz, oz,
                                    su, ou,
                                    sv, ov);
  bool good = true;
  for (unsigned i = 0; i<8; ++i)
  {
    rcam.project(act_x[i], act_y[i], act_z[i], u, v);
    vcl_cout << '(' << act_x[i]<< ' '<< act_y[i]<< ' '<<act_z[i]
             << ")-> (" << u << ' ' << v << ")\n";
    double eu = vcl_fabs(u-act_u[i]), ev = vcl_fabs(v-act_v[i]);
    vcl_cout << "error = (" << eu << ' ' << ev << ")\n";
    good = good && eu<0.01 && ev < 0.01;
  }
  TEST("test rational camera projection", good, true);
  //Test various constructors
  // Set values on default constructor
  vcl_vector<vcl_vector<double> > coeff_array;
  coeff_array.push_back(neu_u);
  coeff_array.push_back(den_u);
  coeff_array.push_back(neu_v);
  coeff_array.push_back(den_v);
  icam.set_coefficients(coeff_array);
  icam.set_scale(vpgl_rational_camera<double>::X_INDX, sx);
  icam.set_offset(vpgl_rational_camera<double>::X_INDX, ox);
  icam.set_scale(vpgl_rational_camera<double>::Y_INDX, sy);
  icam.set_offset(vpgl_rational_camera<double>::Y_INDX, oy);
  icam.set_scale(vpgl_rational_camera<double>::Z_INDX, sz);
  icam.set_offset(vpgl_rational_camera<double>::Z_INDX, oz);
  icam.set_scale(vpgl_rational_camera<double>::U_INDX, su);
  icam.set_offset(vpgl_rational_camera<double>::U_INDX, ou);
  icam.set_scale(vpgl_rational_camera<double>::V_INDX, sv);
  icam.set_offset(vpgl_rational_camera<double>::V_INDX, ov);
  //
  good = true;
  for (unsigned i = 0; i<8; ++i)
  {
    icam.project(act_x[i], act_y[i], act_z[i], u, v);
    double eu = vcl_fabs(u-act_u[i]), ev = vcl_fabs(v-act_v[i]);
    vcl_cout << "error = (" << eu << ' ' << ev << ")\n";
    good = good && eu<0.01 && ev < 0.01;
  }
  TEST("test default constructor with member setting", good, true);
  vpgl_scale_offset<double> sox(sx, ox);
  vpgl_scale_offset<double> soy(sy, oy);
  vpgl_scale_offset<double> soz(sz, oz);
  vpgl_scale_offset<double> sou(su, ou);
  vpgl_scale_offset<double> sov(sv, ov);
  vcl_vector<vpgl_scale_offset<double> > soffs;
  soffs.push_back(sox);   soffs.push_back(soy);   soffs.push_back(soz);
  soffs.push_back(sou);   soffs.push_back(sov);
  vpgl_rational_camera<double> rcam1(coeff_array, soffs);
  good = true;
  for (unsigned i = 0; i<8; ++i)
  {
    rcam1.project(act_x[i], act_y[i], act_z[i], u, v);
    double eu = vcl_fabs(u-act_u[i]), ev = vcl_fabs(v-act_v[i]);
    vcl_cout << "error = (" << eu << ' ' << ev << ")\n";
    good = good && eu<0.01 && ev < 0.01;
  }
  TEST("test constructor with coeff array and vector of vpgl_scale_offset instances", good, true);

  vnl_matrix_fixed<double, 4, 20> coeff_matrix;
  for (unsigned i = 0; i<20; ++i)
  {
    coeff_matrix[0][i]=neu_u[i];
    coeff_matrix[1][i]=den_u[i];
    coeff_matrix[2][i]=neu_v[i];
    coeff_matrix[3][i]=den_v[i];
  }
  vpgl_rational_camera<double> rcam2(coeff_matrix, soffs);
  good = true;
  for (unsigned i = 0; i<8; ++i)
  {
    rcam2.project(act_x[i], act_y[i], act_z[i], u, v);
    double eu = vcl_fabs(u-act_u[i]), ev = vcl_fabs(v-act_v[i]);
    vcl_cout << "error = (" << eu << ' ' << ev << ")\n";
    good = good && eu<0.01 && ev < 0.01;
  }
  TEST("test constructor with coeff matrix and vector of vpgl_scale_offset instances", good, true);

  //test getting the coefficient matrix
  good = true;
  vnl_matrix_fixed<double, 4, 20> cm = rcam1.coefficient_matrix();
  for (unsigned j=0; j<4; ++j)
    for (unsigned i=0; i<20; ++i)
      good = good && cm[j][i]==coeff_matrix[j][i];
  TEST("test getting coefficient matrix", good, true);

  //test getting the coefficient array
  good = true;
  vcl_vector<vcl_vector<double> > coefficients = rcam1.coefficients();
  for (unsigned j=0; j<4; ++j)
    for (unsigned i=0; i<20; ++i)
      good = good && coefficients[j][i]==coeff_matrix[j][i];
  TEST("test getting coefficient array", good, true);

  //test getting the scale and offsets
  good = true;
  good = good && sx == rcam.scale(vpgl_rational_camera<double>::X_INDX);
  good = good && ox == rcam.offset(vpgl_rational_camera<double>::X_INDX);
  good = good && sy == rcam.scale(vpgl_rational_camera<double>::Y_INDX);
  good = good && oy == rcam.offset(vpgl_rational_camera<double>::Y_INDX);
  good = good && sz == rcam.scale(vpgl_rational_camera<double>::Z_INDX);
  good = good && oz == rcam.offset(vpgl_rational_camera<double>::Z_INDX);
  good = good && su == rcam.scale(vpgl_rational_camera<double>::U_INDX);
  good = good && ou == rcam.offset(vpgl_rational_camera<double>::U_INDX);
  good = good && sv == rcam.scale(vpgl_rational_camera<double>::V_INDX);
  good = good && ov == rcam.offset(vpgl_rational_camera<double>::V_INDX);
  TEST("test getting scale and offset values", good, true);
}

TESTMAIN(test_rational_camera);
