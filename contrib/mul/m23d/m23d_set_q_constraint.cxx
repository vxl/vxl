//:
// \file
// \author Tim Cootes
// \brief Functions to construct linear constraints as rows in a matrix

#include <m23d/m23d_set_q_constraint.h>

//: Defines constraint on elements of Q of form a'Qb=r
//  Q is a symmetric matrix, with n(n+1)/2 independent elements
//  The constraints on these elements are placed in row c of A
void m23d_set_q_constraint1(vnl_matrix<double> & A,
                      vnl_vector<double> & rhs,
                      unsigned c,
                      const vnl_vector<double>& a,
                      const vnl_vector<double>& b,
                      double r)
{
   rhs[c]=r;
   unsigned n = a.size();
   unsigned k=0;
   for (unsigned i=0;i<n;++i)
     for (unsigned j=0;j<=i;++j,++k)
     {
       if (i==j) A(c,k)=a[i]*b[j];
       else      A(c,k)=2*a[i]*b[j];
     }
}

//: Defines constraint on elements of Q of form aQa'-bQb'=0
//  Q is a symmetric matrix, with n(n+1)/2 independent elements
//  The constraints on these elements are placed in row c of A
void m23d_set_q_constraint2(vnl_matrix<double> & A,
                      vnl_vector<double> & rhs,
                      unsigned c,
                      const vnl_vector<double>& a,
                      const vnl_vector<double>& b)
{
   rhs[c]=0;
   unsigned n = a.size();
   unsigned k=0;
   for (unsigned i=0;i<n;++i)
     for (unsigned j=0;j<=i;++j,++k)
     {
       if (i==j) A(c,k)=a[i]*a[j]-b[i]*b[j];
       else      A(c,k)=2*(a[i]*a[j]-b[i]*b[j]);
     }
}




