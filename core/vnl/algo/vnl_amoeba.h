#ifndef vnl_amoeba_h_
#define vnl_amoeba_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
//  \file
//  \brief Nelder-Meade downhill simplex.
//  \author Andrew W. Fitzgibbon, Oxford RRG
//  \date   23 Oct 97
//
//  \verbatim
//  Modifications
//  971023 AWF Initial version
//  dac (Manchester) 26/03/2001: tidied up documentation
//  \endverbatim

//-----------------------------------------------------------------------------

#include <vnl/vnl_matops.h>

class vnl_cost_function;
class vnl_least_squares_function;

//: Nelder-Meade downhill simplex.
//  vnl_amoeba is an implementation of the Nelder-Meade downhill simplex
//  algorithm.  For most problems, it's a few times slower than
//  vnl_levenberg_marquardt, but it can perform much better on noisy error
//  functions.

class vnl_amoeba {
public:
  int verbose;
  int maxiter;
  double X_tolerance;
  double F_tolerance;

	  //: Scaling used to select starting verticies relative to intial x0
		//  ie the i'th vertex has x[i] = x0[i]*(1+relative_diameter)
  double relative_diameter;

	  //: Construct and supply function to be minimized
  vnl_amoeba(vnl_cost_function& f);

  void set_delta(vnl_vector<double> const& delta_x);

	  //: Modify x to minimise function supplied in constructor
  void minimize(vnl_vector<double>& x);

	  //: Number of evaluations used in last call to minimize
  int get_num_evaluations() const { return num_evaluations_; }

public:
    //: Modify x so as to minimise f(x)
  static void minimize(vnl_cost_function& f, vnl_vector<double>& x);

    //: Modify x so as to minimise f(x)
		//  delta defines relative size of initial simplex
		//  ie the i'th vertex has xi[i] = x[i]*(1+delta)
  static void minimize(vnl_cost_function& f, vnl_vector<double>& x,
                       double delta);

    //: Modify x so as to minimise f(x)
  static void minimize(vnl_least_squares_function& f, vnl_vector<double>& x);

  static bool default_verbose;

protected:
  vnl_cost_function* fptr;
  int num_evaluations_;
};

//: Private class needs to be declared here
//  in order to instantiate STL container of it.
struct vnl_amoeba_SimplexCorner {
  vnl_vector<double> v;
  double fv;

  vnl_amoeba_SimplexCorner(int = 0);
  vnl_amoeba_SimplexCorner& operator= (const vnl_amoeba_SimplexCorner& that);
  static int compare(vnl_amoeba_SimplexCorner const& s1,
                     vnl_amoeba_SimplexCorner const& s2);
};

#endif // vnl_amoeba_h_
