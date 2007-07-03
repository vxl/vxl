#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h> // for std::abs
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <mrc/vpgl/algo/vpgl_ortho_procrustes.h>
static void test_ortho_procrustes()
{
  //First have perfectly transformed points
  vnl_matrix<double> X(3, 5), Y(3, 5);
  Y[0][0] = 1; Y[1][0] = 0; Y[2][0] = 0;
  Y[0][1] = 0; Y[1][1] = 1; Y[2][1] = 0;
  Y[0][2] = 0; Y[1][2] = 0; Y[2][2] = 1;
  Y[0][3] = 1; Y[1][3] = 1; Y[2][3] = 0;
  Y[0][4] = 1; Y[1][4] = 1; Y[2][4] = 1;

  vnl_vector_fixed<double, 3> rv, trans;
  for(unsigned i = 0; i<3; ++i)
    rv[i]=0.9068996774314604; // axis along diagonal, rotation of 90 degrees
  vgl_rotation_3d<double> rr(rv);
  vnl_matrix_fixed<double, 3, 3> rrr = rr.as_matrix();
  trans[0]=10.0;   trans[1]=20.0; trans[2] = 30;

  vcl_cout << "The ideal rotation\n" << rr << '\n';
  vcl_cout << "The ideal translation\n" << trans << '\n';

  for(unsigned c = 0; c<5; ++c)
    {
      vnl_vector_fixed<double, 3> v;
      for(unsigned r = 0; r<3; ++r)
        v[r]=Y[r][c];
      
      vnl_vector_fixed<double, 3> trans_v = rrr*v + trans;
      vcl_cout << "|Y|(" << c << ")=" << v.magnitude() << "  |X|(" << c 
               << ")=" << trans_v.magnitude() << '\n';

      for(unsigned r = 0; r<3; ++r)
        X[r][c] = trans_v[r];
    }
   vpgl_ortho_procrustes op(X, Y);
  vgl_rotation_3d<double> R = op.R();
  double s = op.s();
  double error = op.residual_mean_sq_error();
  vcl_cout << "Procrustes rotation\n" << R << '\n';
  vcl_cout << "Procrustes translation\n" << op.t() << '\n';
  vcl_cout << "scale = " << s << '\n';
  vcl_cout << "error = " << error << '\n';
  //Now perturb the points 
  Y[0][0] = 1.1; Y[1][0] = -0.05; Y[2][0] = 0.01;
  Y[0][1] = 0.02; Y[1][1] = 0.995; Y[2][1] = -0.1;
  Y[0][2] = -0.01; Y[1][2] = 0.04; Y[2][2] = 1.04;
  Y[0][3] = 1.15; Y[1][3] = 0.97; Y[2][3] = -0.1;
  Y[0][4] = 1.01; Y[1][4] = 1.03; Y[2][4] = 0.96;
  vpgl_ortho_procrustes op1(X, Y);
  vgl_rotation_3d<double> R1 = op1.R();
  double s1 = op1.s();
  double error1 = op1.residual_mean_sq_error();
  vcl_cout << "Procrustes rotation\n" << R1 << '\n';
  vcl_cout << "Procrustes angle\n" << R1.angle() << '\n';
  vcl_cout << "Procrustes axis\n" << R1.axis() << '\n';
  vcl_cout << "Procrustes translation\n" << op1.t() << '\n';
  vcl_cout << "scale = " << s1 << '\n';
  vcl_cout << "error = " << error1 << '\n';
  // should be pi/2 but with errors is 1.65321
  TEST_NEAR("Procrustes Rotation (angle)", R1.angle(), 1.571, 0.1);
  TEST_NEAR("Procrustes Rotation (axis)" , (R1.axis())[0], 0.577, 0.1);
}

TESTMAIN(test_ortho_procrustes);
