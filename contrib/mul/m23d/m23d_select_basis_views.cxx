#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "m23d_select_basis_views.h"
//:
// \file
// \author Tim Cootes
// \brief Select a subset most suitable for use as a basis set

#include <mbl/mbl_random_n_from_m.h>
#include <vnl/algo/vnl_svd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Select a subset most suitable for use as a basis set
//  Data matrix is 2ns x np (ns= number of samples, np = number of points)
//  Each two rows gives the points in a single view.
//  This returns a list of point indices for views which have most
//  independent points, and are thus suitable for defining the basis.
std::vector<unsigned> m23d_select_basis_views(const vnl_matrix<double>& P2D,
                                        unsigned n_modes,
                                        unsigned n_tries)
{
  unsigned ns = P2D.rows()/2;

  // Initialise with first (n_modes+1) views
  std::vector<unsigned> s(n_modes+1),best_s(n_modes+1);
  for (unsigned i=0;i<=n_modes;++i) best_s[i]=i;
  double best_v = m23d_evaluate_basis(P2D,best_s);
  std::cout<<"Quality of first basis: "<<best_v<<std::endl;

  // Now generate random subsets and select the best
  mbl_random_n_from_m n_from_m;
  std::vector<unsigned> s1(n_modes);
  s[0]=0;  // Always include the first example
  for (unsigned i=0;i<n_tries;++i)
  {
    n_from_m.choose_n_from_m(s1,n_modes,ns-1);
    for (unsigned j=0;j<n_modes;++j) s[j+1]=s1[j]+1;

    double v = m23d_evaluate_basis(P2D,s);

    if (v>best_v)
    {
      best_v=v;
      best_s=s;
    }
  }

  std::sort(best_s.begin(),best_s.end());
  std::cout<<"Quality of selected basis: "<<best_v<<std::endl
          <<"Selected basis: [ ";
  for (unsigned int best_ : best_s)
    std::cout<<best_<<' ';
  std::cout<<']'<<std::endl;

  return best_s;
}

//: Evaluate quality of a basis set
//  Data matrix is 2ns x np (ns= number of samples, np = number of points)
//  Each two rows gives the points in a single view.
//  Form a basis from the pairs of rows defined by selected, and compute
//  a measure of how independent the rows are.
double m23d_evaluate_basis(const vnl_matrix<double>& P2D,
                           const std::vector<unsigned>& selected)
{
  unsigned np = P2D.columns();
  unsigned ns = P2D.rows()/2;
  unsigned n = selected.size();
  vnl_matrix<double> M(2*n,np);
  for (unsigned i=0;i<n;++i)
  {
    if (selected[i]>=ns)
    {
      std::cerr<<"m23d_evaluate_basis selected rows out of range."<<std::endl;
      std::abort();
    }

    // Copy selected pair of rows into M
    M.update(P2D.extract(2,np,2*selected[i],0),2*i,0);
  }

  vnl_svd<double> svd(M);
  return svd.W(2*n-1)/svd.W(0);  // Smallest singular value/largest SV
}
