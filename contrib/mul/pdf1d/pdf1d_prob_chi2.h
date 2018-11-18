#ifndef pdf1d_prob_chi2_h
#define pdf1d_prob_chi2_h

//:
// \file
// \author Tim Cootes
// \date 12-Apr-2001
// \brief Functions for Chi distribution

//: P(chi<chi2)
// Calculates the probability that a value generated
// at random from a chi-square distribution with given
// degrees of freedom is less than the value chi2
// \param n_dof  Number of degrees of freedom
// \param chi2  Value of chi-squared
double pdf1d_cum_prob_chi2(int n_dof, double chi2);



//: Returns X such that P(chi<X | dof==n)==p
// The value of Chi-Squared such that the probability
// that a random variable drawn from a chi-2 distribution
// is less than Chi_Squared is p.
// \param p  Target probability
// \param n  Number of dimensions
// \param tol  Tolerance of result (default = 0.001)
double pdf1d_chi2_for_cum_prob(double p, int n, double tol=0.001);

#endif
