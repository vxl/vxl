#ifndef pdf1d_prob_ks_h
#define pdf1d_prob_ks_h
//:
// \file
// \author Tim Cootes
// \brief Probability used in Kolmogorov-Smirnov test

//: Probability used in Kolmogorov-Smirnov test
//  This is used to compute the prob. that the maximum difference between two cumulative
//  distributions is larger than x.
double pdf1d_prob_ks(double x);

#endif // pdf1d_prob_ks_h
