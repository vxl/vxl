//-*- c++ -*-------------------------------------------------------------------
// Module: Minimization of Rosenbrock banana function
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 31 Aug 96
// Converted to vxl by Peter Vanroose, February 2000
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_least_squares_cost_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vnl/algo/vnl_conjugate_gradient.h>
#include <vnl/algo/vnl_lbfgs.h>

// Make a compute object for the "banana" function
//			      2      
//	    f(x, y) = {10*(y-x ), 1-x}
//
// It is called the banana function because of the way the
// curvature bends around the origin. It is notorious in
// optimization examples because of the slow convergence
// of most methods.
//

class vnl_rosenbrock : public vnl_least_squares_function {
public:
  vnl_rosenbrock(): vnl_least_squares_function(2, 2, no_gradient) {}

  void f(const vnl_vector<double>& x, vnl_vector<double>& fx)
  {
    fx[0] = 10*(x[1] - x[0]*x[0]);
    fx[1] = 1 - x[0];
  }
};

class vnl_rosenbrock_grad_cost_fun : public vnl_cost_function {
public:
  vnl_rosenbrock_grad_cost_fun(): vnl_cost_function(2) {}
  
  double f(const vnl_vector<double>& x) {
    double a = 10*(x[1] - x[0]*x[0]);
    double b = 1 - x[0];
    return a*a + b*b;
  }

  void gradf(const vnl_vector<double>& x, vnl_vector<double>& g) {
    double a = 10*(x[1] - x[0]*x[0]);
    double b = 1 - x[0];
    g[0] = 2 * a * (-20*x[0]) - 2 * b;
    g[1] = 20 * a;
  }
  
};

int main()
{
  // Set up a Rosenbrock compute object
  vnl_rosenbrock f;

  // Set up the initial guess
  vnl_double_2 x0(-1.9, 2);
  
  // Temp variable.
  vnl_double_2 x;

  // Make a Levenberg Marquardt minimizer, attach f to it, and
  // run the minimization
  x = x0;
  vnl_levenberg_marquardt levmarq(f, x);

  // Summarize the results
  cout << "** LevenbergMarquardt default **" << endl;
  cout << "Rosenbrock min of " << levmarq.get_end_error() << " at " << x << endl;
  cout << "Iterations: " << levmarq.get_num_iterations() << "    ";
  cout << "Evaluations: " << levmarq.get_num_evaluations() << endl;
  levmarq.diagnose_outcome();

  // Now rerun the optimizer with a new, looser, X tolerance.
  //
  levmarq.set_x_tolerance(0.1);
  x = x0;
  levmarq.minimize(x);

  // Summarize the results. It has taken fewer iterations to reach the same
  // answer.
  cout << "** LevenbergMarquardt xtol=0.1 **" << endl;
  cout << "Rosenbrock min of " << levmarq.get_end_error() << " at " << x << endl;
  cout << "Iterations: " << levmarq.get_num_iterations() << "    ";
  cout << "Evaluations: " << levmarq.get_num_evaluations() << endl;
  levmarq.diagnose_outcome();

  // Make a vnl_cost_function, and use vnl_amoeba
  cout << "** Amoeba (Nelder Meade downhill simplex)  ** \n";
  vnl_least_squares_cost_function cf(&f);
  vnl_amoeba amoeba(cf);
  x = x0;
  amoeba.minimize(x);
  cout << "Rosenbrock min of " << cf.f(x) << " at " << x << endl;
  cout << "Evaluations: " << amoeba.get_num_evaluations() << endl;

  cout << "** Conjugate Gradient ** \n";
  vnl_conjugate_gradient cg(f);
  x = x0;
  cg.minimize(x);
  cout << "CG min of " << cf.f(x) << " at " << x << endl;
  cg.diagnose_outcome();

  cout << "** LBFGS (Limited memory Broyden Fletcher Goldfarb Shanno) ** \n";
  vnl_rosenbrock_grad_cost_fun rcf;
  vnl_lbfgs lbfgs(rcf);
  x = x0;
  lbfgs.minimize(x);
  assert(lbfgs.get_end_error() == rcf.f(x));
  cout << "L-BFGS min of " << lbfgs.get_end_error() << " at " << x << endl;
  cout << "Evaluations: " << lbfgs.get_num_evaluations() << endl;
  
  return 0;
}
