//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vnl_conjugate_gradient.h"
#endif
//
// Class: vnl_conjugate_gradient
// Author: Geoffrey Cross, Oxford RRG
// Created: 15 Feb 99
// Modifications:
//   990215 Geoff Initial version.
//
//-----------------------------------------------------------------------------
#include "vnl_conjugate_gradient.h"
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_svd.h>
#include <stdlib.h>
#include <assert.h>

// external netlib function
extern "C"
int cg_( double *x,                     // IO start guess
	 double *e,                     // O max-norm of gradient
         int    *it,                    // O number of iterations performed
         double *step,                  // I step=0 make guess at first direction
	                                // O step size along search direction for final iteration
	 double *t,                     // I tolerance (iterations stop when max-norm of gradient < t)
         int *limit,                    // I maximum number of iterations
	 int *n,                        // I number of unknowns
	 int *m,                        // I number of iterations before renormalizing (normally m=n)
	 double value( double *x),      // I value(x) is cost at x
         int grad( double *g,
		   double *x),          // I grad(g,x) puts gradient into g at x
	 int both( double *v,
		   double *g,
		   double *x),          // I both(v,g,x) puts value in v and gradient in g at x
	 int pre( double *y,
		  double *z),           // I preconditions (not necessarily needed) pre(y,z) 
	 double *h );                   // I space to work size h = 3*n

/////////////////////////////////////

class vnl_conjugate_gradient_Activate {
public:
  static vnl_conjugate_gradient* current;

  vnl_conjugate_gradient_Activate(vnl_conjugate_gradient* minimizer) {
    if (current) {
      cerr << "vnl_conjugate_gradient: ERROR: Nested minimizations not supported.\n";
      abort();
      // This is a copy of what goes on in LevenbergMarquardt, so if awf decides to
      // fix that one, then maybe he could do the same here...
    }
    current = minimizer;
  }
  ~vnl_conjugate_gradient_Activate() {
    current = 0;
  }
};

vnl_conjugate_gradient *vnl_conjugate_gradient_Activate::current= 0;


/////////////////////////////////////

vnl_conjugate_gradient::~vnl_conjugate_gradient()
{
}

void vnl_conjugate_gradient::init( vnl_least_squares_function &f)
{
  // not implemented if gradient known
  assert( !f.has_gradient());

  f_= &f;
  gradstep_= 1e-7;
  max_number_of_iterations= 10* f.get_number_of_unknowns();
  gradtolerance_= 1e-7;
  number_of_iterations= 0;
  startresidue= 0;
  endresidue= 0;
  number_of_evaluations= 0;
}

///////////////////////////////////////

double vnl_conjugate_gradient::valuecomputer_( double *x)
{
  vnl_conjugate_gradient* active = vnl_conjugate_gradient_Activate::current;
  vnl_least_squares_function* f = active->f_;
  vnl_vector<double> xv( x, f->get_number_of_unknowns());
  active->number_of_evaluations++;

  double rms_residuals = f->rms(xv);
  // capes - return value of cost function at x, i.e. sum of squared residuals
  return rms_residuals * rms_residuals * f->get_number_of_residuals();
}

int vnl_conjugate_gradient::gradientcomputer_( double *g, double *x)
{
  vnl_conjugate_gradient* active = vnl_conjugate_gradient_Activate::current;
  vnl_least_squares_function* f = active->f_;

  vnl_vector<double> xp1(x, f->get_number_of_unknowns());
  vnl_vector<double> xm1(x, f->get_number_of_unknowns());

  for( int i=0; i< f->get_number_of_unknowns(); i++)
    {
      xp1[i] += active->gradstep_;
      xm1[i] -= active->gradstep_;

      g[i] = (valuecomputer_(xp1.data_block())- valuecomputer_(xm1.data_block()))/(2*active->gradstep_);

      xp1[i] = x[i];
      xm1[i] = x[i];
    }
  return 0;
}

int vnl_conjugate_gradient::valueandgradientcomputer_( double *v, double *g, double *x)
{
  *v= valuecomputer_( x);
  gradientcomputer_( g, x);
  return 0;
}

int vnl_conjugate_gradient::preconditioner_( double *out, double *in)
{
  *out= *in;
  return 0;
}

///////////////////////////////////////

// avoid anachronism warning from fussy compilers
#ifdef VCL_SUNPRO_CC
extern "C" double vnl_conjugate_gradient__valuecomputer_( double *x)
{
  return vnl_conjugate_gradient::valuecomputer_(x);
}
extern "C" int vnl_conjugate_gradient__gradientcomputer_( double *g, double *x)
{
  return vnl_conjugate_gradient::gradientcomputer_(g,x);
}
extern "C" int vnl_conjugate_gradient__valueandgradientcomputer_( double *v, double *g, double *x)
{
  return vnl_conjugate_gradient::valueandgradientcomputer_(v,g,x);
}
extern "C" int vnl_conjugate_gradient__preconditioner_( double *out, double *in)
{
  return vnl_conjugate_gradient::preconditioner_(out,in);
}
#endif

bool vnl_conjugate_gradient::minimize( vnl_vector<double> &x)
{
  double *xp= x.data_block();
  vnl_vector<double>  maximum_gradient(f_->get_number_of_unknowns());
  double  step_size= 0;
  vnl_vector<double> tv( f_->get_number_of_unknowns(), gradtolerance_);
  double *gradient_tolerance= tv.data_block();
  vnl_vector<double>  workspace( f_->get_number_of_unknowns()*3 );
  int     number_of_unknowns= f_->get_number_of_unknowns();

  vnl_conjugate_gradient_Activate activator(this);

  startresidue= valuecomputer_( xp);
  number_of_evaluations= 0;

  cg_( xp, 
       maximum_gradient.data_block(), 
       &number_of_iterations,
       &step_size,
       gradient_tolerance,
       &max_number_of_iterations,
       &number_of_unknowns,
       &number_of_unknowns,
#ifdef VCL_SUNPRO_CC
       vnl_conjugate_gradient__valuecomputer_,
       vnl_conjugate_gradient__gradientcomputer_,
       vnl_conjugate_gradient__valueandgradientcomputer_,
       vnl_conjugate_gradient__preconditioner_,
#else
       valuecomputer_,
       gradientcomputer_,
       valueandgradientcomputer_,
       preconditioner_,
#endif
       workspace.data_block());

  endresidue= valuecomputer_(xp);

  return true;
}


void vnl_conjugate_gradient::diagnose_outcome(ostream& os) const
{
  os << "vnl_conjugate_gradient: " 
     << number_of_iterations 
     << " iterations, " 
     << number_of_evaluations 
     << " evaluations. RMS error start/end " 
     << sqrt(startresidue/f_->get_number_of_residuals())
     << "/" 
     << sqrt(endresidue/f_->get_number_of_residuals())
     << endl;
}
