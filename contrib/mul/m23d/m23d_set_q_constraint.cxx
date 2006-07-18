//:
// \file
// \author Tim Cootes
// \brief Functions to construct linear constraints as rows in a matrix

#include <m23d/m23d_set_q_constraint.h>
#include <vcl_algorithm.h>

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
       else      A(c,k)=a[i]*b[j]+a[j]*b[i];
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


//: Generate matrix of all constraints on elements of Q
void m23d_set_q_constraints(const vnl_matrix<double> & M, unsigned k,
                            vnl_matrix<double>& A,
                            vnl_vector<double>& rhs)
{
  unsigned mc = M.cols();
  unsigned m = M.cols()/3 -1;
  unsigned ns = M.rows()/2;
  unsigned nq = ((M.cols()+1)*M.cols())/2;

  unsigned n_con = 4*(m*(ns-m-1)+(m+m*m)/2)+3*m+2*(ns-m)+1;

  // Set up constraints on elements of Q
  A.set_size(n_con,nq);
  rhs.set_size(n_con);

  unsigned c=0;

  // Impose the basis constraints
  for (unsigned j=0;j<ns;++j)
  {
    vnl_vector<double> rxj = M.get_row(2*j);
    vnl_vector<double> ryj = M.get_row(2*j+1);
    for (unsigned i=0;i<=vcl_min(j,m);++i)
    {
      vnl_vector<double> rxi = M.get_row(2*i);
      vnl_vector<double> ryi = M.get_row(2*i+1);

      if (i==j)
      {
        if  (i==k)
        {
          // MiQMi' = I
          m23d_set_q_constraint1(A,rhs,c,rxi,rxi,1.0); ++c;
          m23d_set_q_constraint1(A,rhs,c,ryi,ryi,1.0); ++c;
          m23d_set_q_constraint1(A,rhs,c,rxi,ryi,0.0); ++c;
        }
        else
        {
          // MiQMi' = 0
          m23d_set_q_constraint1(A,rhs,c,rxi,rxi,0); ++c;
          m23d_set_q_constraint1(A,rhs,c,ryi,ryi,0); ++c;
          m23d_set_q_constraint1(A,rhs,c,rxi,ryi,0); ++c;
        }
      }
      else   // i!=j
      if (!((i==k) && j>m))
      {
        // MiQMj' = 0
        m23d_set_q_constraint1(A,rhs,c,rxi,rxj,0); ++c;
        m23d_set_q_constraint1(A,rhs,c,ryi,ryj,0); ++c;
        m23d_set_q_constraint1(A,rhs,c,rxi,ryj,0); ++c;
        m23d_set_q_constraint1(A,rhs,c,ryi,rxj,0); ++c;
      }
    }
  }

  // These constraints aim to impose orthogonality on rows of projection
  // matrices.
  for (unsigned i=m+1;i<ns;++i)
  {
    vnl_vector<double> rxi = M.get_row(2*i);
    vnl_vector<double> ryi = M.get_row(2*i+1);
    m23d_set_q_constraint2(A,rhs,c,rxi,ryi); ++c;
    m23d_set_q_constraint1(A,rhs,c,rxi,ryi,0); ++c;
  }

  assert(c==n_con);
}



