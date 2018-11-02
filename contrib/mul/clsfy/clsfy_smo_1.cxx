// This is mul/clsfy/clsfy_smo_1.cxx
#include <iostream>
#include <cmath>
#include "clsfy_smo_1.h"
//:
// \file
// \author Ian Scott
// \date 14-Nov-2001
// \brief Sequential Minimum Optimisation algorithm
// This code is based on the C++ code of
// Xianping Ge, ( http://www.ics.uci.edu/~xge ) which he kindly
// put in the public domain.
// That code was in turn based on the algorithms of
// John Platt, ( http://research.microsoft.com/~jplatt ) described in
// Platt, J. C. (1998). Fast Training of Support Vector Machines Using Sequential
// Minimal Optimisation. In Advances in Kernel Methods - Support Vector Learning.
// B. Scholkopf, C. Burges and A. Smola, MIT Press: 185-208. and other papers.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <cassert>

// Linear SMO

// ----------------------------------------------------------------

double clsfy_smo_1_lin::kernel(int i1, int i2)
{
  if (i1==i2)
    return precomputed_self_dot_product_[i1];
  else
    return dot_product(data_point(i1),data_point(i2));
}

// ----------------------------------------------------------------

double clsfy_smo_1_rbf::kernel(int i1, int i2)
{
  if (i1==i2) return 1.0;
  double s = dot_product(data_point(i1),data_point(i2));
  s *= -2.0f;
  s += precomputed_self_dot_product_[i1] + precomputed_self_dot_product_[i2];
  return std::exp(gamma_ * s);
}

// ----------------------------------------------------------------

//: Takes a copy of the data wrapper, but not the data.
// Be careful not to destroy the underlying data while using this object.
void clsfy_smo_1_lin::set_data(const mbl_data_wrapper<vnl_vector<double> >& data, const std::vector<int>& targets)
{
  const unsigned N = data.size();
  data_ = data.clone();

  assert(targets.size() == N);
  target_ = targets;

  precomputed_self_dot_product_.resize(N);
  for (unsigned int i=0; i<N; i++)
      precomputed_self_dot_product_[i] = dot_product(data_point(i),data_point(i));
}

// ----------------------------------------------------------------

double clsfy_smo_1_lin::C() const
{
  return C_;
}

// ----------------------------------------------------------------

void clsfy_smo_1_lin::set_C(double C)
{
  if (C <= 0) C_ = vnl_huge_val(double());
  else C_ = C;
}

// ----------------------------------------------------------------

//: 0.5 sigma^-2, where sigma is the width of the Gaussian kernel
double clsfy_smo_1_rbf::gamma() const
{
  return -gamma_;
}

// ----------------------------------------------------------------

//: Control sigma, the width of the Gaussian kernel.
// gamma = 0.5 sigma^-2
void clsfy_smo_1_rbf::set_gamma(double gamma)
{
  gamma_ = -gamma;
}

// ----------------------------------------------------------------

clsfy_smo_1_rbf::clsfy_smo_1_rbf():
  gamma_((double)-0.5)
{
}

// ----------------------------------------------------------------

clsfy_smo_1_lin::clsfy_smo_1_lin():
  C_(vnl_huge_val(double()))
{
}

// ----------------------------------------------------------------

int clsfy_smo_1_lin::take_step(int i1, int i2, double E1)
{
  int  s;
  double a1, a2;       /// new values of alpha_1, alpha_2
  double E2, L, H, k11, k22, k12, eta, Lobj, Hobj;

  if (i1 == i2) return 0;


  const double alph1 = alph_[i1]; // old_values of alpha_1
  const int y1 = target_[i1];

  const double alph2 = alph_[i2]; // old_values of alpha_2
  const int y2 = target_[i2];
  if (alph2 > 0 && alph2 < C_)
    E2 = error_cache_[i2];
  else
    E2 = learned_func(i2) - y2;

  s = y1 * y2;

  if (y1 == y2) {
    const double g = alph1 + alph2;
    if (g > C_) {
        L = g-C_;
        H = C_;
    }
    else {
        L = 0;
        H = g;
    }
  }
  else
  {
    const double g = alph1 - alph2;
    if (g > 0) {
        L = 0;
        H = C_ - g;
    }
    else {
        L = -g;
        H = C_;
    }
  }

  if (L == H)
    return 0;

  k11 = kernel(i1, i1);
  k12 = kernel(i1, i2);
  k22 = kernel(i2, i2);
  eta = 2.0 * k12 - k11 - k22;


  if (eta < 0) {
    a2 = alph2 + y2 * (E2 - E1) / eta;
    if (a2 < L)
      a2 = L;
    else if (a2 > H)
      a2 = H;
  }
  else {
    {
      double c1 = eta/2;
      double c2 = y2 * (E1-E2)- eta * alph2;
      Lobj = c1 * L * L + c2 * L;
      Hobj = c1 * H * H + c2 * H;
    }

    if (Lobj > Hobj+eps_)
      a2 = L;
    else if (Lobj < Hobj-eps_)
      a2 = H;
    else
      a2 = alph2;
  }

  if (vnl_math::abs(a2-alph2) < eps_*(a2+alph2+eps_) )
    return 0;

  a1 = alph1 - s * (a2 - alph2);
  if (a1 < 0.0) {
    a2 += s * a1;
    a1 = 0;
  }
  else if (a1 > C_) {
    double t = a1-C_;
    a2 += s * t;
    a1 = C_;
  }


  double delta_b;
  {
    double b1, b2, bnew;

    const double eps_2 = eps_*eps_;

    if (a1 > eps_2 && a1 < (C_*(1-eps_2)))
      bnew = b_ + E1 + y1 * (a1 - alph1) * k11 + y2 * (a2 - alph2) * k12;
    else {
      if (a2 > eps_2 && a2 < (C_*(1-eps_2)))
        bnew = b_ + E2 + y1 * (a1 - alph1) * k12 + y2 * (a2 - alph2) * k22;
      else {
        b1 = b_ + E1 + y1 * (a1 - alph1) * k11 + y2 * (a2 - alph2) * k12;
        b2 = b_ + E2 + y1 * (a1 - alph1) * k12 + y2 * (a2 - alph2) * k22;
        bnew = (b1 + b2) / 2;
      }
    }
    delta_b = bnew - b_;
    b_ = bnew;
  }

  {
    const double t1 = y1 * (a1-alph1);
    const double t2 = y2 * (a2-alph2);

    for (unsigned int i=0; i<data_->size(); i++)
      if (0 < alph_[i] && alph_[i] < C_)
        error_cache_[i] +=  t1 * kernel(i1,i) + t2 * kernel(i2,i) - delta_b;
    error_cache_[i1] = 0.0;
    error_cache_[i2] = 0.0;
  }

  alph_[i1] = a1;  // Store a1 in the alpha array.
  alph_[i2] = a2;  // Store a2 in the alpha array.

  return 1;
}

// ----------------------------------------------------------------

int clsfy_smo_1_lin::examine_example(int i1)
{
  double  E1, r1;
  const unsigned long N = data_->size();

  const double y1 = target_[i1];
  const double alph1 = alph_(i1);

  if (alph1 > 0 && alph1 < C_)
    E1 = error_cache_[i1];
  else
    E1 = learned_func(i1) - y1;

  r1 = y1 * E1;
  if ((r1 < -tolerance_ && alph1 < C_) ||
      (r1 > tolerance_ && alph1 > 0)) // is the KKT condition for alph1 broken?
  {
    // Try i2 by three ways; if successful, then immediately return 1;
    {
      unsigned int k;
      int i2;
      double tmax;

      // Second choice heuristic A - Find the example i2 which maximises
      // |E1 - E2| where E1

      for (i2 = (-1), tmax = 0, k = 0; k < N; ++k)
        if (alph_(k) > 0 && alph_(k) < C_)
        {
          double E2, temp;

          E2 = error_cache_[k];
          temp = vnl_math::abs(E1 - E2);
          if (temp > tmax)
          {
            tmax = temp;
            i2 = k;
          }
        }

      if (i2 >= 0) {
        if (take_step (i1, i2, E1))
          return 1;
      }
    }

    // second choice Heuristic B - Find any unbound example that give positive progress.
    // start from random location
    for (unsigned long k0 = rng_.lrand32(N-1), k = k0; k < N + k0; ++k)
    {
      unsigned long i2 = k % N;
      if (alph_(i2) > 0 && alph_(i2) < C_)
      {
        if (take_step(i1, i2, E1))
          return 1;
      }
    }

    // second choice Heuristic C - Find any example that give positive progress.
    // start from random location
    for (unsigned long k0 = rng_.lrand32(N-1), k = k0; k < N + k0; ++k)
    {
      unsigned long i2 = k % N;
      if (alph_(i2) == 0 || alph_(i2) == C_)
      {
        if (take_step(i1, i2, E1))
          return 1;
      }
    }
  }
  return 0;
}

// ----------------------------------------------------------------

int clsfy_smo_1_rbf::calc()
{
  assert(gamma_!=0.0);  //IS gamma set?
  return clsfy_smo_1_lin::calc();
}

// ----------------------------------------------------------------

int clsfy_smo_1_lin::calc()
{
  //Check a bunch of things

  assert (data_ != nullptr); // Check that the data has been set.

  const unsigned long N = data_->size();
  assert(N != 0);     // Check that there is some data.

  if (alph_.empty()) // only initialise alph if it hasn't been externally set.
  {
    alph_.set_size(N);
    alph_.fill(0.0);
  }

  // E_i = u_i - y_i = 0 - y_i = -y_i
  error_cache_.resize(N);

  unsigned long numChanged = 0;
  bool examineAll = true;
  while (numChanged > 0 || examineAll)
  {
    numChanged = 0;
    if (examineAll)
      for (unsigned int k = 0; k < N; k++)
        numChanged += examine_example (k);
    else
      for (unsigned int k = 0; k < N; k++)
        if (alph_[k] != 0 && alph_[k] != C_)
          numChanged += examine_example (k);
    if (examineAll)
      examineAll = false;
    else if (numChanged == 0)
      examineAll = true;

#if !defined NDEBUG &&  CLSFY_SMO_BASE_PRINT_PROGRESS >1
    {
      double s = 0.;
      for (int i=0; i<N; i++)
        s += alph_[i];
      double t = 0.;
      for (int i=0; i<N; i++)
      {
        if (alph_(i) != 0.0)
        {
          for (int j=0; j<N; j++)
            if (alph_[j] != 0.0)
              t += alph_[i]*alph_[j]*target_[i]*target_[j]*kernel(i,j);
        }
      }
      std::cerr << "Objective function=" << (s - t/2.) << '\t';
      for (int i=0; i<N; i++)
        if (alph_[i] < 0)
          std::cerr << "alph_[" << i << "]=" << alph_[i] << " < 0\n";
      s = 0.;
      for (int i=0; i<N; i++)
        s += alph_[i] * target_[i];
      std::cerr << "s=" << s << "\terror_rate=" << error_rate() << '\t';
    }
#endif

#if !defined NDEBUG && CLSFY_SMO_BASE_PRINT_PROGRESS
    {
      int non_bound_support =0;
      int bound_support =0;
      for (int i=0; i<N; i++)
        if (alph_[i] > 0)
        {
          if (alph_[i] < C_)
            non_bound_support++;
          else
            bound_support++;
        }
      std::cerr << "non_bound=" << non_bound_support << '\t'
               << "bound_support=" << bound_support << '\n';
    }
#endif
  }

  error_ = error_rate();

#if !defined NDEBUG && CLSFY_SMO_BASE_PRINT_PROGRESS
  std::cerr << "Threshold=" << b_ << '\n';
  std::cout << "Error rate=" << error_ << std::endl;
#endif

  return 0;
}
