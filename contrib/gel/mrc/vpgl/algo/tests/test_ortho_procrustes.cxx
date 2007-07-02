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

  trans[0]=10.0;   trans[1]=20.0; trans[2] = 30;

  vcl_cout << "The ideal rotation\n" << rr << '\n';
  vcl_cout << "The ideal translation\n" << trans << '\n';

  for(unsigned c = 0; c<5; ++c)
    {
      vnl_vector_fixed<double, 3> v;
      for(unsigned r = 0; r<3; ++r)
        v[r]=Y[r][c];
      
      vnl_vector_fixed<double, 3> trans_v = rr*v + trans;
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
  vnl_matrix<double> J(4,6);
  for(unsigned c = 0; c<5; ++c)
    {
      for(unsigned r = 0; r<3; ++r)
        J[r][c]=Y[r][c];
      J[3][c] = 1.0;
    }
  J[0][5] = 0.5;   J[1][5] = 1.0;  J[2][5] = -0.5;  J[3][5] = 1.0; 
  vcl_cout << "J matrix \n" << J << '\n';
  vnl_svd<double> SVD(J);
  vnl_matrix<double> V = SVD.V(); 
  vcl_cout << "U \n" << SVD.U() << '\n';
  vcl_cout << "V \n" << V << '\n';
  vcl_cout << "W \n" << SVD.W() << '\n';
  vnl_matrix_fixed<double, 6, 2> nulv;
  nulv[0][0]= V[0][4]; nulv[1][0]= V[1][4]; nulv[2][0]= V[2][4];
  nulv[3][0]= V[3][4]; nulv[4][0]= V[4][4]; nulv[5][0]= V[5][4];

  nulv[0][1]= V[0][5]; nulv[1][1]= V[1][5]; nulv[2][1]= V[2][5];
  nulv[3][1]= V[3][5]; nulv[4][1]= V[4][5]; nulv[5][1]= V[5][5];
vcl_cout << "Null Space \n" << nulv << '\n';
 vcl_cout << "Null Space J \n" << J*nulv << '\n';

 vnl_matrix_fixed<double, 3, 3> pr = rr.as_matrix();
 vnl_matrix_fixed<double, 3, 4> P;
 for(unsigned r = 0; r<3; ++r)
   {
     for(unsigned c = 0; c<3; ++c)
       P[r][c] = pr[r][c];
     P[r][3] = 0;
   }
 // Project the 3-d points
 vnl_matrix<double> Z(3, 6);
 for(unsigned c = 0; c<6; ++c)
   {
     vnl_vector_fixed<double, 4> vpr;
     for(unsigned r = 0; r<4; ++r)
        vpr[r]=J[r][c];
     vnl_vector_fixed<double, 3> pvpr = P*vpr;
     for(unsigned r = 0; r<2; ++r)
       Z[r][c] = pvpr[r]/pvpr[2];
     Z[2][c]=1.0;
   }
 vcl_cout << "Projected points \n " << Z << '\n';
 //form Kronecker produck of the Nullv with K^-1 (identity in this case)
 vnl_matrix<double> v2k(6, 18);
 v2k.fill(0);
 vnl_matrix_fixed<double, 3, 3> id;
 id.fill(0);
 id[0][0]=1.0;  id[1][1]=1.0;  id[2][2]=1.0;
 for(unsigned r = 0; r<2; ++r)
   for(unsigned c = 0; c<6; ++c)
     for(unsigned ri = 0; ri<3; ++ri)
       for(unsigned ci = 0; ci<3; ++ci)
         v2k[ri+3*r][ci+3*c] = id[ri][ci]*nulv[c][r];

 //Test with the stacked image points
 vnl_matrix<double> vec(18,1);
 for(unsigned r = 0; r<3; ++r)
   for(unsigned c = 0; c<6; ++c)
     vec[r+3*c][0] = Z[r][c];
 vcl_cout << "v2k \n" << v2k << '\n';
 vcl_cout << "vec \n" << vec << '\n';
 //The matrix of projected points
 vnl_matrix<double> D(18,6);
 D.fill(0);
 for(unsigned c = 0; c<6; ++c)
   for(unsigned rw = 0; rw<3; ++rw)
     D[3*c+rw][c] = Z[rw][c];
 vcl_cout << "point matrix D\n" << D << '\n';
 //the matrix whose nullvector is the solution
 vnl_matrix<double> H = v2k*D;
 vnl_svd<double> sh(H);
 vnl_matrix<double> Vsh = sh.V(); 
  vcl_cout << "Ush \n" << sh.U() << '\n';
  vcl_cout << "V \n" << Vsh << '\n';
  vcl_cout << "W \n" << sh.W() << '\n';
  vcl_cout << "depths \n" << sh.nullvector() << '\n';
  vnl_vector_fixed<double, 6> shnv = sh.nullvector();
  vnl_matrix<double> Zy(3, 6);
  for(unsigned c = 0; c<6; ++c)
    for(unsigned r = 0; r<3; ++r)
      {
        Z[r][c] *= shnv[c];
        Zy[r][c] = J[r][c];
      }
  vpgl_ortho_procrustes op2(Z, Zy);
  vgl_rotation_3d<double> R2 = op2.R();
  double s2 = op2.s();
  double error2 = op2.residual_mean_sq_error();
  vcl_cout << "Procrustes rotation\n" << R2 << '\n';
  vcl_cout << "Procrustes angle\n" << R2.angle() << '\n';
  vcl_cout << "Procrustes axis\n" << R2.axis() << '\n';
  vcl_cout << "Procrustes translation\n" << op2.t() << '\n';
  vcl_cout << "scale = " << s2 << '\n';
  vcl_cout << "error = " << error2 << '\n';

}

TESTMAIN(test_ortho_procrustes);
