// This is mul/m23d/tests/test_correction_matrix_error.cxx
#include <iostream>
#include <cmath>
#include <testlib/testlib_test.h>
#include <m23d/m23d_correction_matrix_error.h>
#include <m23d/m23d_make_ortho_projection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>
#include <m23d/m23d_rotation_matrix.h>
#include <m23d/m23d_set_q_constraint.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <m23d/m23d_rotation_from_ortho_projection.h>

bool test_projection_matrix(const vnl_matrix<double>& P)
{
  unsigned ns=P.rows()/2;
  unsigned m=P.cols()/3-1;

  bool test_ok=true;
  std::cout<<"Testing projection matrix."<<std::endl;
  for (unsigned k=0;k<=m;++k)
  {
    // Test orthogonality of rows of each projection matrix
    for (unsigned i=0;i<ns;++i)
    {
      vnl_matrix<double> Pik = P.extract(2,3,2*i,3*k);
      vnl_matrix<double> PPt=Pik*Pik.transpose();
      if (std::fabs(PPt(0,1))>1e-5)
      {
        std::cout<<"View "<<i<<" mode "<<k<<" ) rows not orthogonal. "<<PPt(0,1)<<std::endl;
        test_ok=false;
      }
      if (std::fabs(PPt(0,0)-PPt(1,1))>1e-5)
      {
        std::cout<<"View "<<i<<" mode "<<k<<" ) rows don't scale equally. "<<PPt(0,0)-PPt(1,1)<<std::endl;
        test_ok=false;
      }
    }
  }
  if (test_ok)
    std::cout<<"Projection matrix passed the tests."<<std::endl;

  return test_ok;
}

//  Fill a symmetric matrix with elements from v
static vnl_matrix<double> sym_matrix_from_vec(const vnl_vector<double>& v, unsigned n)
{
  vnl_matrix<double> S(n,n);
  unsigned k=0;
   for (unsigned i=0;i<n;++i)
     for (unsigned j=0;j<=i;++j,++k)
       S(i,j)=S(j,i)=v[k];
  return S;
}

//  Solve for correction matrix for zero mode case
static vnl_matrix<double> am_solve_for_G0(const vnl_matrix<double>& A,
                                   const vnl_vector<double>& rhs)
{
  unsigned n=3;
  vnl_svd<double> svd(A);
  std::cout<<"Singular Values of A: "<<svd.W().diagonal()<<std::endl;
  vnl_vector<double> q0 = svd.solve(rhs);
  vnl_matrix<double> Q0=sym_matrix_from_vec(q0,n);
//   unsigned nq = q0.size();
  std::cout<<"Error for q0 = "<<(A*q0-rhs).rms()<<std::endl;

  // If Gk is the t x 3 matrix, the k-th triplet of columns of G,
  // then Gk.Gk'=Q0
  // Use eigen decomposition to compute Gk
  vnl_symmetric_eigensystem<double> eig(Q0);
  vnl_matrix<double> Gk(n,3);
  std::cout<<"Eigenvalues: "<<eig.D.diagonal()<<std::endl;
  for (unsigned i=0;i<3;++i)
  {
    Gk.set_column(i,std::sqrt(eig.get_eigenvalue(n-1-i))
                            *eig.get_eigenvector(n-1-i));
  }

  return Gk;
}


static vnl_matrix<double> am_solve_for_Gk(const vnl_matrix<double>& A,
                                   const vnl_vector<double>& rhs,
                                   unsigned m, unsigned k)
{
  m23d_correction_matrix_error err_fn(A,rhs,m,k);

  vnl_levenberg_marquardt LM(err_fn);

  // Generate g for identity for k-th column.
  vnl_vector<double> g(9*(m+1),0.0);
  for (unsigned i=0;i<3;++i) g[9*k+4*i]=1.0;

//  LM.minimize_using_gradient(g);  *** Seem to get a different result with gradient! ***
  if (!LM.minimize_using_gradient(g))
    std::cout<<"LM failed!!"<<std::endl;

  std::cout<<"am_solve_for_Gk (k="<<k<<") RMS="<<err_fn.rms(g)<<std::endl;

  // Reshape g into 3(m+1) x 3 matrix
  return vnl_matrix<double>(g.data_block(),3*(m+1),3);
}


void compute_Gk(const vnl_matrix<double> & M, unsigned k,
                vnl_matrix<double>& Gk)
{
  unsigned m = M.cols()/3 -1;

  vnl_matrix<double> A;
  vnl_vector<double> rhs;
  m23d_set_q_constraints(M,k,A,rhs);

  if (m==0)
    Gk=am_solve_for_G0(A,rhs);
  else
  {
    std::cout<<"Solving for Gk:"<<std::endl;
    Gk=am_solve_for_Gk(A,rhs,m,k);
  }

  if (k==0)
  {
    // Compute extra rotation so that the projection matrix for the
    // first shape is a scaled identity projection.
    vnl_matrix<double> M0=M.extract(2,3*(1+m))*Gk;

    // Compute a rotation matrix for this
    vnl_matrix<double> R=m23d_rotation_from_ortho_projection(M0);

    std::cout<<"M0*Rt (k="<<k<<')'<<std::endl<<M0*R.transpose()<<std::endl;

    // Apply inverse so that P.G gives unit projection
    Gk=Gk*R.transpose();
  }
}

// Compute correction matrix G (t x t) s.t.  M=M1.G and B=(inv(G)).B1
void m23d_compute_correction(const vnl_matrix<double>& M1,
                             vnl_matrix<double>& G)
{
  unsigned t = M1.cols();
  unsigned n_modes=t/3-1;
  G.set_size(t,t);
  for (unsigned k=0;k<=n_modes;++k)
  {
    vnl_matrix<double> Gk;
    compute_Gk(M1,k,Gk);
    G.update(Gk,0,k*3);
  }
}

static vnl_vector<double> vec_from_sym_matrix(const vnl_matrix<double>& S)
{
  unsigned n = S.rows();
  vnl_vector<double> v((n*(n+1))/2);
  unsigned k=0;
   for (unsigned i=0;i<n;++i)
     for (unsigned j=0;j<=i;++j,++k)
       v[k]=S(i,j);
  return v;
}


void test_correction_matrix_error()
{
  std::cout<<"==== test m23d_correction_matrix_error ====="<<std::endl;

  vnl_random r(35813);
  unsigned ns = 20;
  unsigned n_modes=1;
  vnl_matrix<double> P=m23d_make_ortho_projection(r,ns,n_modes,true,true);
  bool b1=test_projection_matrix(P);
  TEST("Pure projection matrix OK",b1,true);

  std::cout<<"Apply a rotation to columns of P"<<std::endl;
  vnl_matrix<double> P2(P),G;
  vnl_matrix<double> R2=m23d_rotation_matrix(0.1,0.2,0.3);
  std::cout<<"Rotation applied: "<<std::endl<<R2<<std::endl;
  for (unsigned i=0;i<=n_modes;++i)
    P2.update(P.extract(2*ns,3,0,3*i)*R2,0,3*i);
  m23d_compute_correction(P2,G);
  std::cout<<"G for rotated P"<<std::endl<<G<<std::endl;
  b1=test_projection_matrix(P2*G);
  TEST("Corrected projection matrix OK",b1,true);

  std::cout<<"Apply a rotation and scaling to columns of P"<<std::endl;
  for (unsigned i=0;i<=n_modes;++i)
    P2.update(P.extract(2*ns,3,0,3*i)*R2*(3+i),0,3*i);
  m23d_compute_correction(P2,G);
  std::cout<<"G for scaled and rotated P"<<std::endl<<G<<std::endl;

  b1=test_projection_matrix(P2*G);
  TEST("Corrected projection matrix OK",b1,true);

  std::cout<<"=== Apply a random affine projection ==="<<std::endl;
  vnl_matrix<double> G0(G);
  for (unsigned i=0;i<G0.rows();++i)
    for (unsigned j=0;j<G0.cols();++j)  G0(i,j)=r.drand64(-1,1);
  vnl_matrix<double> P3=P*G0;
  m23d_compute_correction(P3,G);
  b1=test_projection_matrix(P3*G);
  TEST("Corrected projection matrix OK",b1,true);
  std::cout<<"G0*G="<<std::endl<<G0*G<<std::endl;

  std::cout<<"Test that the function is zero at a true minima"<<std::endl;
  vnl_matrix<double> A;
  vnl_vector<double> rhs;
  m23d_set_q_constraints(P3,0,A,rhs);
  m23d_correction_matrix_error cme(A,rhs,1,0);
  vnl_svd<double> G_svd(G0);
  vnl_matrix<double> G0_inv=G_svd.inverse();
  test_projection_matrix(P3*G0_inv);
  vnl_matrix<double> G0_inv0=G0_inv.extract(6,3,0,0);
  vnl_vector<double> gk0(G0_inv0.data_block(),18);
  vnl_vector<double> fx;
  cme.f(gk0,fx);
  std::cout<<"Function: "<<cme.rms(gk0)<<std::endl;

  vnl_matrix<double> Q=G0_inv0*G0_inv0.transpose();
  vnl_vector<double> fx1 = A*vec_from_sym_matrix(Q)-rhs;

  TEST_NEAR("f(x) correct given A",(fx-fx1).rms(),0,1e-6);
  TEST_NEAR("f(x) correct",fx.rms(),0,1e-6);

  std::cout<<"Check computation of Jacobian"<<std::endl;
  vnl_vector<double> g0(gk0.size(),0.123),g1;
  vnl_vector<double> f0,f1;
  vnl_matrix<double> J;
  cme.f(g0,f0);
  cme.gradf(g0,J);
  double delta=0.00001;
  for (unsigned i=0;i<g0.size();++i)
  {
    g1=g0;
    g1[i]+=delta;
    cme.f(g1,f1);
    f1-=f0;
    f1/=delta;
    std::cout<<f1(0)<<','<<J(0,i)/f1(0)<<std::endl;
    TEST_NEAR("Jacobian column",(f1-J.get_column(i)).rms(),0.0,1e-5);
  }
}

TESTMAIN(test_correction_matrix_error);
