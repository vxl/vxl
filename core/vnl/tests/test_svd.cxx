#include <vcl/vcl_cstdlib.h>
#include <vnl/vnl_test.h>
#include <vnl/vnl_complex.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matops.h>
#include <vnl/algo/vnl_svd.h>

vnl_matrix<double> solve_with_warning(const vnl_matrix<double>& M,
				      const vnl_matrix<double>& B)
// Solve LS problem M x = B, warning if M is nearly singular.
{
  // Take svd of vnl_matrix<double> M, trim the singular values at 1e-8,
  // and hold the result.
  vnl_svd<double> svd(M, 1e-8);
  // Check for rank-deficiency
  if (svd.singularities() > 1)
    cerr << "Warning: Singular matrix, condition = " << svd.well_condition() << endl;
  return svd.solve(B);
}

void test_hilbert()
{
  // Test inversion and recomposition of 5x5 hilbert matrix
  vnl_matrix<double> H(5,5);
  for(int i = 0; i < 5; ++i)
    for(int j = 0; j < 5; ++j)
      H(i,j) = 1.0 / (i+j+1); // sic, because i,j are zero based
  
  cout << "H = [ " << H << "]\n";

  vnl_svd<double> svd(H);
  
  cerr << "rcond(H) = " << svd.well_condition() << endl;
  
  vnl_matrix<double> Hinv = svd.inverse();

  vnl_matrix<double> X = Hinv * H;

  cout << "H*inv(H) = " << X << endl;

  vnl_matrix<double> I(5,5);
  I = 0.0;
  I.fill_diagonal(1.0);
  
  vnl_matrix<double> res = X - I;
  Assert("Hilbert recomposition residual", res.inf_norm() < 1.1e-10);
}

// Test recovery of parameters of least-squares parabola fit.
void test_ls()
{
  double a = 0.15;
  double b = 1.2;
  double c = 3.1;
  
  // Generate parabola design matrix
  vnl_matrix<double> D(100, 3);
  for(int n = 0; n < 100; ++n) {
    double x = n;
    D(n, 0) = x*x;
    D(n, 1) = x;
    D(n, 2) = 1.0;
  }

  // Generate Y vector
  vnl_vector<double> y(100);
  {for(int n = 0; n < 100; ++n) {
    double x = n;
    double fx = a * x * x + b * x + c;
    // Add sawtooth "noise"
    y(n) = fx + (n%4 - 2) / 10.0;
  }}
  cout << "y = [" << y << "]\n";

  // Extract vnl_svd<double>
  vnl_svd<double> svd(D);

  // Solve for parameteers
  vnl_double_3 A = svd.solve(y);
  cout << "A = " << A << "\n";

  vnl_double_3 T(a,b,c);
  cout << "residual = " << (A - T).squared_magnitude() << endl;
  Assert("Least squares residual", (A - T).squared_magnitude() < 0.005);
}  

// temporarily unused
double test_fmatrix() 
{
  double pdata[] = {
    2, 0, 0, 0,
    3, 10, 5, 5,
    5, 12, 6, 6,
  };
  vnl_matrix<double> P(pdata, 3,4);
  vnl_svd<double> svd(P);
  vnl_matrix<double> N = svd.nullspace();
  cout << "null(P) = " << N << endl;
  
  cout << "P * null(P) = " << P*N << endl;
  
  return sqrt(dot_product(P*N, P*N));
}

// Test nullspace extraction of rank=2 3x4 matrix.
void test_pmatrix() 
{
  double pdata[] = {
    2, 0, 0, 0,
    3, 10, 5, 5,
    5, 12, 6, 6,
  };
  vnl_matrix<double> P(pdata, 3,4);
  vnl_svd<double> svd(P, 1e-8);

  vnl_matrix<double> res = svd.recompose() - P;
  cout << "Recomposition residual = " << res.inf_norm() << endl;
  Assert("PMatrix recomposition residual", res.inf_norm() < 1e-12);
  cout << " Inv = " << svd.inverse() << endl;

  Assert("singularities = 2", svd.singularities() == 2);
  Assert("rank = 2", svd.rank() == 2);

  vnl_matrix<double> N = svd.nullspace();
  Assert("nullspace dimension", N.columns() == 2);
  cout << "null(P) = \n" << N << endl;
  
  vnl_matrix<double> PN = P*N;
  cout << "P * null(P) = \n" << PN << endl;
  cout << "nullspace residual = " << PN.inf_norm() << endl;
  Assert("P nullspace residual", PN.inf_norm() < 1e-12);
  
  vnl_vector<double> n = svd.nullvector();
  cout << "nullvector residual = " << (P*n).magnitude() << endl;
  Assert("P nullvector residual", (P*n).magnitude() < 1e-12);

  vnl_vector<double> l = svd.left_nullvector();
  cout << "left_nullvector(P) = " << l << endl;
  cout << "left_nullvector residual = " << (l*P).magnitude() << endl;
  Assert("P left nullvector residual", (l*P).magnitude() < 1e-12);
}

void test_I()
{
  double Idata[] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
  };
  vnl_matrix<double> P(3, 4, 12, Idata);
  vnl_svd<double> svd(P);
  cout << svd;
}

static double double_random() { return rand()/double(RAND_MAX); }
static void fill_random(float &x)  { x = double_random(); }
static void fill_random(double &x) { x = double_random(); }
static void fill_random(vnl_float_complex &x)  { x = vnl_float_complex (double_random(), double_random()); }
static void fill_random(vnl_double_complex &x) { x = vnl_double_complex(double_random(), double_random()); }

template <class T>
void test_svd_recomposition(char const *type, double maxres, T */*tag*/)
{
  // Test inversion of 5x5 matrix of T :
  cout << "----- testing vnl_svd<" << type << "> recomposition -----" << endl;

  vnl_matrix<T> A(5,5);
  for (unsigned i = 0; i < A.rows(); ++i)
    for (unsigned j = 0; j < A.columns(); ++j)
      fill_random(A(i, j));
  
  cout << "A = [ " << endl << A << "]" << endl;
  vnl_svd<T> svd(A);
  
  vnl_matrix<T> B=svd.recompose();
  cout << "B = [ " << endl << B << "]" << endl;
  
  double residual=(A - B).inf_norm();
  cout << "residual=" << residual << endl;
  Assert("vnl_svd<float> recomposition residual", residual < maxres);
}

template void test_svd_recomposition(char const *, double, float *);
template void test_svd_recomposition(char const *, double, double *);
template void test_svd_recomposition(char const *, double, vnl_float_complex *);
template void test_svd_recomposition(char const *, double, vnl_double_complex *);

// Driver
void test_svd()
{
  test_hilbert();
  test_ls();
  test_pmatrix();
  test_I();
  test_svd_recomposition("float",              1e-5 , (float*)0);
  test_svd_recomposition("double",             1e-10, (double*)0);
  test_svd_recomposition("vnl_float_complex",  1e-5 , (vnl_float_complex*)0);
  test_svd_recomposition("vnl_double_complex", 1e-10, (vnl_double_complex*)0);
}

TESTMAIN(test_svd);
