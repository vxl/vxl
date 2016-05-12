//:
// \file
// \author Tim Cootes
// \brief Functions to compare sets of samples.

#include "pdf1d_compare_samples.h"

//: Return the average number of elements of x2 greater than each x1[i]
double pdf1d_prop_larger(const double* x1, int n1, const double* x2, int n2)
{
  int n_larger = 0;
  for (int i=0;i<n1;++i)
    for (int j=0;j<n2;++j)
      if (x1[i]>x2[j]) n_larger++;

  return double(n_larger)/(n1*n2);
}

//: Return true if x1[i] significantly larger than x2[j]
//  Essentially tests if on average a sample from x1 is larger than
//  one from x2
bool pdf1d_larger_samples(const double* x1, int n1, const double* x2, int n2, double p)
{
  double prop_larger = pdf1d_prop_larger(x1,n1,x2,n2);
  return prop_larger>p;
}

//: Return true if x1[i] significantly smaller than x2[j]
//  Essentially tests if on average a sample from x1 is smaller than
//  one from x2
bool pdf1d_smaller_samples(const double* x1, int n1, const double* x2, int n2, double p)
{
  double prop_larger = pdf1d_prop_larger(x1,n1,x2,n2);
  return prop_larger<(1.0-p);
}

//: Select the samples which are on average largest
//  Where there is ambiguity return smallest index.
int pdf1d_select_largest_samples(std::vector<const double*>& x,
                                 std::vector<int>& n, double p)
{
  int t = n.size();

  int best_i = 0;
  int best_n_larger = 0;

  // Compare every distribution with every other distribution
  // Slightly inefficient implementation - effectively does
  // the comparison twice.  Fix this one day.
  for (int i=0;i<t;++i)
  {
    int n_larger=0;
    for (int j=0;j<t;++j)
    {
      if (i==j) continue;
      if (pdf1d_larger_samples(x[i],n[i],x[j],n[j],p))
        n_larger++;
    }

    // If this is better than all the others, then this is the one we want
    if (n_larger==(t-1)) return i;

    if (i==0 || n_larger>best_n_larger)
    {
      best_i = i;
      best_n_larger = n_larger;
    }
  }

  return best_i;
}
