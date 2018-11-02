#ifndef pdf1d_compare_samples_h
#define pdf1d_compare_samples_h
//:
// \file
// \author Tim Cootes
// \brief Functions to compare sets of samples.

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Return true if x1[i] significantly larger than x2[j]
//  Essentially tests if on average a sample from x1 is larger than
//  a proportion p of those from x2
bool pdf1d_larger_samples(const double* x1, int n1, const double* x2, int n2, double p=0.6);

//: Return true if x1[i] significantly smaller than x2[j]
//  Essentially tests if on average a sample from x1 is smaller than
//  a proportion p of those from x2
bool pdf1d_smaller_samples(const double* x1, int n1, const double* x2, int n2, double p=0.6);

//: Return the average number of elements of x2 greater than each x1[i]
double pdf1d_prop_larger(const double* x1, int n1, const double* x2, int n2);

//: Select the samples which are on average largest.
//  x[i] is considered larger than x[j] if on average each element of x[i] is
//  larger than a proportion p of those in x[j].
//  Where there is ambiguity return smallest index.
// \param x[i] pointer to data with n[i] elements
int pdf1d_select_largest_samples(std::vector<const double*>& x, std::vector<int>& n, double p=0.6);
#endif // pdf1d_compare_samples_h
