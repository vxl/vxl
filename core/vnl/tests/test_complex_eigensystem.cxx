//:
// \file
// \author F. Schaffalitzky, Oxford RRG
// \date    7 September 1999
#include <vcl_complex.h>
#include <vnl/vnl_test.h>
#include <vnl/algo/vnl_complex_eigensystem.h>

void test_complex_eigensystem1()
{
  const unsigned N=6;
  double a_real[N*N] = {
    0.5965,   -0.7781,   -1.6925,    9.8017,   -3.5993,   -1.2015,
    2.8105,    1.3566,   -3.9000,    5.7772,    9.2020,    8.6676,
   -5.8186,    5.8842,    7.4873,   -1.2268,    4.5326,    3.6666,
   -2.4036,   -8.8163,   -9.6998,   -0.0338,   -1.7609,   -5.7488,
    5.6666,    2.0574,    5.3590,   -5.7207,    4.8913,    6.7848,
    3.6169,   -8.9946,    9.4169,    2.8698,   -4.6411,    2.5757
  };
  vnl_matrix<double> A_real(a_real,N,N);

  double a_imag[N*N] = {
    6.9244,    3.6255,   -3.9077,   -6.9825,   -0.0690,   -3.1606,
    0.5030,   -2.4104,   -6.2069,    3.9580,    7.9954,   -4.2055,
   -5.9471,    6.6359,   -6.1314,   -2.4325,    6.4326,   -3.1761,
    3.4427,    0.0563,    3.6445,    7.2002,    2.8982,    0.6816,
    6.7624,    4.1894,   -3.9447,    7.0731,    6.3595,    4.5423,
   -9.6072,   -1.4222,    0.8335,    1.8713,    3.2046,   -3.8142
  };
  vnl_matrix<double> A_imag(a_imag,N,N);

  vnl_matrix<vcl_complex<double> > A(N,N);
  for (unsigned i=0;i<N;i++)
    for (unsigned j=0;j<N;j++)
      A(i,j) = vcl_complex<double>(A_real(i,j), A_imag(i,j));

  vnl_complex_eigensystem eig(A,     // compute both
                              true,  // left and right
                              true); // eigenvectors
#if 0
  vcl_cout << "A = " << A << vcl_endl
           << "eigenvalues = " << eig.W << vcl_endl
           << "L = " << eig.L << vcl_endl
           << "R = " << eig.R << vcl_endl;
#endif
  for (unsigned i=0;i<N;i++) {
    //vcl_cout << "i=" << i << vcl_endl;
    //
    vcl_complex<double> w = eig.W[i];
    vnl_vector<vcl_complex<double> > err;
    //vcl_cout << "  w = " << w << vcl_endl;
    //
    vnl_vector<vcl_complex<double> > l(eig.left_eigen_vector(i));
    err = (l*A - l*w);
    //vcl_cout << "  " << err << vcl_endl;
    vnl_test_assert("  Left  eigenvalue", err.magnitude() < 1e-10);
    //
    vnl_vector<vcl_complex<double> > r(eig.right_eigen_vector(i));
    err = (A*r - w*r);
    //vcl_cout << "  " << err << vcl_endl;
    vnl_test_assert("  Right eigenvalue", err.magnitude() < 1e-10);
  }
}

void test_complex_eigensystem2()
{
  // The standard version of ZLAHQR fails to converge on this 6x6 matrix
  // because the maximum number of iterations is reached. Removing the
  // upper limit makes it work, though.
  double Adata[6][6] = {
    { 6.81189847675500,  -0.75094724440200,   0.02962045905500,   0.08278481627400,  -0.00326537487000,   0.00012879986400},
    {-0.30264207899000,   7.24396703250300,  -0.23873370907200,  -1.59347941419300,   0.05767229376100,  -0.00207046888600},
    {-0.22478047851400,   1.66397856595400,   6.51603673051800,  -0.36414398064500,  -0.71120349595300,   0.05667215261300},
    { 0.00336147948700,  -0.16054853597700,   0.00528866726000,   7.66800229119600,  -0.25259347537300,   0.00832074135800},
    { 0.00499332392900,  -0.15593251059600,  -0.14083152011000,   3.50460364036400,   6.85617756909000,  -0.45550486394200},
    { 0.00185433854100,  -0.02724973652500,  -0.10751684805800,   0.40043828267200,   1.57997351477200,   6.23396017664100}
  };
  vnl_matrix<vcl_complex<double> > A(6, 6);
  for (int i=0; i<6; ++i)
    for (int j=0; j<6; ++j)
      A[i][j] = Adata[i][j]; //(0.77+i) + (0.1+j)*(0.33+j);
  vnl_complex_eigensystem eig(A);
  vnl_test_assert("  Funny eigensystem", true);
}

void test_complex_eigensystem()
{
  test_complex_eigensystem1();
  test_complex_eigensystem2();
}

TESTMAIN(test_complex_eigensystem);
