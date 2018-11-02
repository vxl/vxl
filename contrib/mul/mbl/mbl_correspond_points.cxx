// This is mul/mbl/mbl_correspond_points.cxx
#include <cmath>
#include <iostream>
#include <algorithm>
#include "mbl_correspond_points.h"
//:
// \file
// \brief Shapiro & Brady's point correspondence algorithm
// \author Tim Cootes

#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================
// Dflt ctor
//=======================================================================

mbl_correspond_points::mbl_correspond_points() = default;

//: Return index of row in H2 most similar to row i of H1
unsigned mbl_correspond_points::closest_row(const vnl_matrix<double>& H1,
                                            const vnl_matrix<double>& H2,
                                            unsigned i1)
{
  unsigned nc = std::min(H1.cols(),H2.cols());
  unsigned best_i = 0;
  double best_d2 = -1.0;
  const double* h1 = &H1(i1,0);
  for (unsigned i=0;i<H2.rows();++i)
  {
    const double* h2 = &H2(i,0);
    double sum=0.0;
    for (unsigned j=0;j<nc;++j)
    {
      double d = h1[j]-h2[j];
      sum += d*d;
    }
    if (best_d2 < 0 || sum<best_d2)
    {
      best_d2=sum;
      best_i = i;
    }
  }

  return best_i;
}

//: Ensure each column vector points in the same way
void mbl_correspond_points::fix_eigenvectors(vnl_matrix<double>& P)
{
  for (unsigned j=0;j<P.cols();++j)
  {
    vnl_vector<double> p = P.get_column(j);
    if (p.sum()<0) { p*=-1.0; P.set_column(j,p); }
  }
}

//: Find best correspondence between points1 and points2
//  On exit, matches[i] gives index of points2 which
//  corresponds to points1[i].
//  \param sigma Scaling factor defining kernel width
void mbl_correspond_points::correspond(const std::vector<vgl_point_2d<double> >& points1,
                                       const std::vector<vgl_point_2d<double> >& points2,
                                       std::vector<unsigned>& matches, double sigma)
{
  unsigned n1 = points1.size();
  unsigned n2 = points2.size();

  vnl_matrix<double> H1(n1,n1),H2(n2,n2);
  proximity_by_tanh(points1,H1,sigma);
  proximity_by_tanh(points2,H2,sigma);


  // Compute eigen structure of each proximity
  vnl_matrix<double> P1(n1,n1),P2(n2,n2);
  evals1_.set_size(n1);
  evals2_.set_size(n2);
  vnl_symmetric_eigensystem_compute(H1,P1,evals1_);
  vnl_symmetric_eigensystem_compute(H2,P2,evals2_);

  // Arrange that values/vectors ordered with largest first
  P1.fliplr(); evals1_.flip();
  P2.fliplr(); evals2_.flip();

  // Directions of eigenvectors are ambiguous.
  // Ensure they're all facing the same way.
  fix_eigenvectors(P1);
  fix_eigenvectors(P2);

  // Select best matches based on row correspondence
  // Note that there may be a many to one correspondence here.
  matches.resize(n1);
  for (unsigned i=0;i<n1;++i)
    matches[i] = closest_row(P1,P2,i);
}

//: Construct distance matrix using cosh kernel
//  On exit, D(i,j) = tanh(pi*d_ij/sigma) * 2/(pi*d_ij)
//  where d_ij is the distance between points i and j
void mbl_correspond_points::proximity_by_tanh(const std::vector<vgl_point_2d<double> >& points,
                                              vnl_matrix<double>& H, double sigma)
{
  const unsigned n = points.size();
  const vgl_point_2d<double> *p = &points[0];
  H.set_size(n,n);

  const double k1 = 2.0/vnl_math::pi;
  const double k2 = vnl_math::pi/sigma;

  for (unsigned i=0;i<n;++i)
  {
    H(i,i)=0.0;
    for (unsigned j=i+1;j<n;++j)
    {
      double d = vgl_distance(p[i],p[j]);
      H(i,j) = H(j,i) = k1*std::tanh(k2*d)/d;
    }
  }
}
