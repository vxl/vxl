/*
  fsm@robots.ox.ac.uk
*/
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

struct vnl_rosenbrock_nograd : vnl_least_squares_function
{
  vnl_rosenbrock_nograd(): vnl_least_squares_function(2, 2, no_gradient) {}
  
  void f(vnl_vector<double> const &x, vnl_vector<double> &y) {
    y[0] = 10*(x[1] - x[0]*x[0]);
    y[1] = 1 - x[0];
  }
};

struct vnl_rosenbrock_grad : vnl_least_squares_function
{
  vnl_rosenbrock_grad(): vnl_least_squares_function(2, 2, use_gradient) {}
  
  void f(vnl_vector<double> const &x, vnl_vector<double> &y) {
    y[0] = 10*(x[1] - x[0]*x[0]);
    y[1] = 1 - x[0];
  }
  
  void gradf(vnl_vector<double> const &x, vnl_matrix<double> &J) {
    J[0][0] = -20 * x[0]; J[0][1] = 10;
    J[1][0] = -1;         J[1][1] = 0;
  }
};

//----------------------------------------------------------------------

#include <vnl/vnl_test.h>

static
void do_test(vnl_least_squares_function &f)
{
  vnl_vector<double> x0(2);
  x0[0] =  2.7;
  x0[1] = -1.3;
  vcl_cout << "x0 = " << x0 << vcl_endl;
  
  vnl_levenberg_marquardt lm(f);
  
  vnl_vector<double> x1 = x0;
  if (f.has_gradient())
    lm.minimize_using_gradient(x1);
  else
    lm.minimize_without_gradient(x1);
  vcl_cout << "x1 = " << x1 << vcl_endl;
  
  double err = vcl_abs(x1[0] - 1) + vcl_abs(x1[1] - 1);
  vnl_test_assert("converged to (1, 1)", err <= 1e-10);
}

static
void do_test()
{
  {
    vnl_rosenbrock_nograd f;
    do_test(f);
  }
  {
    vnl_rosenbrock_grad f;
    do_test(f);
  }
}

TESTMAIN(do_test);
