// This is core/vnl/vnl_fastops.cxx
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   08 Dec 96
//
//-----------------------------------------------------------------------------

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "vnl_fastops.h"

//: Compute $A^\top A$.
void
vnl_fastops::AtA(vnl_matrix<double> & out, const vnl_matrix<double> & A)
{
  out = (A.transpose() * A).eval();
}

//: Compute AxB.
void
vnl_fastops::AB(vnl_matrix<double> & out, const vnl_matrix<double> & A, const vnl_matrix<double> & B)
{
  out = (A*B).eval();
}

//: Compute $A^\top B$.
void
vnl_fastops::AtB(vnl_matrix<double> & out, const vnl_matrix<double> & A, const vnl_matrix<double> & B)
{
  out = (A.transpose()* B).eval();
}

//: Compute $A^\top b$ for vector b. out may not be b.
void
vnl_fastops::AtB(vnl_vector<double> & out, const vnl_matrix<double> & A, const vnl_vector<double> & b)
{
  out = (A.transpose() * b).eval();
}

//: Compute $A b$ for vector b. out may not be b.
void
vnl_fastops::Ab(vnl_vector<double> & out, const vnl_matrix<double> & A, const vnl_vector<double> & b)
{
  out = A * b;
}

//: Compute $A B^\top$.
void
vnl_fastops::ABt(vnl_matrix<double> & out, const vnl_matrix<double> & A, const vnl_matrix<double> & B)
{
  out = A * B.transpose();
}

//: Compute $A B A^\top$.
void
vnl_fastops::ABAt(vnl_matrix<double> & out, const vnl_matrix<double> & A, const vnl_matrix<double> & B)
{
  out = A* B * A.transpose();
}

//: Compute $b^\top A b$ for vector b and matrix A
double
vnl_fastops::btAb(const vnl_matrix<double> & A, const vnl_vector<double> & b)
{
  return b.transpose() * A * b;
}

//: Compute $ X += A^\top A$
void
vnl_fastops::inc_X_by_AtA(vnl_matrix<double> & X, const vnl_matrix<double> & A)
{
  X += A.transpose() * A;
}

//: Compute $X += A B$
void
vnl_fastops::inc_X_by_AB(vnl_matrix<double> & X, const vnl_matrix<double> & A, const vnl_matrix<double> & B)
{
  X += A * B;
}

//: Compute $X -= A B$
void
vnl_fastops::dec_X_by_AB(vnl_matrix<double> & X, const vnl_matrix<double> & A, const vnl_matrix<double> & B)
{
  X -= A * B;
}

//: Compute $X += A^\top B$
void
vnl_fastops::inc_X_by_AtB(vnl_matrix<double> & X, const vnl_matrix<double> & A, const vnl_matrix<double> & B)
{
  X += A.transpose() * B;
}

//: Compute $X -= A^\top B$
void
vnl_fastops::dec_X_by_AtB(vnl_matrix<double> & X, const vnl_matrix<double> & A, const vnl_matrix<double> & B)
{
  X -= A.transpose() * B;
}

//: Compute $X += A^\top b$
void
vnl_fastops::inc_X_by_AtB(vnl_vector<double> & X, const vnl_matrix<double> & A, const vnl_vector<double> & B)
{
  X+= A.transpose() * B;
}

//: Compute $X -= A^\top b$
void
vnl_fastops::dec_X_by_AtB(vnl_vector<double> & X, const vnl_matrix<double> & A, const vnl_vector<double> & B)
{
  X -= A.transpose() * B;
}

//: Compute $ X -= A^\top A$
void
vnl_fastops::dec_X_by_AtA(vnl_matrix<double> & X, const vnl_matrix<double> & A)
{
  X -= A.transpose() *A;
}

//: Compute dot product of a and b
double
vnl_fastops::dot(const double * a, const double * b, unsigned int n)
{
  return std::inner_product(a,a+n,b,double(0.0));
#if 0
#define METHOD 3 // Method 2 is fastest on the u170 -- weird.
  double accum = 0;
#if METHOD == 1
  const double * aend = a + n;
  while (a != aend)
    accum += *a++ * *b++;
#endif
#if METHOD == 2
  for (unsigned int k = 0; k < n; ++k)
    accum += a[k] * b[k];
#endif
#if METHOD == 3
  while (n--)
    accum += a[n] * b[n];
#endif
#if METHOD == 4
  unsigned int k = n;
  while (k > 0)
    --k, accum += a[k] * b[k];
#endif
  return accum;
#undef METHOD
#endif
}

//: Compute $X += A B^\top$
void
vnl_fastops::inc_X_by_ABt(vnl_matrix<double> & X, const vnl_matrix<double> & A, const vnl_matrix<double> & B)
{
  X += A * B.transpose();
}

//: Compute $X -= A B^\top$
void
vnl_fastops::dec_X_by_ABt(vnl_matrix<double> & X, const vnl_matrix<double> & A, const vnl_matrix<double> & B)
{
  X -= A * B.transpose();
}


//: Compute $X += A B A^\top$.
void
vnl_fastops::inc_X_by_ABAt(vnl_matrix<double> & X, const vnl_matrix<double> & A, const vnl_matrix<double> & B)
{
  X +=A * B * A.transpose();
}