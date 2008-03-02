#include "m23d_ortho_flexible_builder.h"
//:
// \file
// \author Tim Cootes
// \brief Implementation of the Tomasi & Kanade reconstruction algorithm

#include <m23d/m23d_rotation_from_ortho_projection.h>
#include <m23d/m23d_scaled_ortho_projection.h>
#include <m23d/m23d_set_q_constraint.h>
#include <m23d/m23d_correction_matrix_error.h>
#include <m23d/m23d_select_basis_views.h>
#include <m23d/m23d_pure_ortho_projection.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h>  // abort()
#include <vcl_cassert.h>


//: Reconstruct structure of 3D points given multiple 2D views
//  Data assumed to be scaled orthographic projections
//  The result is stored in the shape_3d() matrix.
//  The estimated projection matrices are stored in the projections() matrix
//  \param P2D 2ns x np matrix. Rows contain alternating x's and y's from 2D shapes
void m23d_ortho_flexible_builder::reconstruct_with_first_as_basis(const vnl_matrix<double>& P2D,
                                                                  unsigned n_modes)
{
  partial_reconstruct(P2D,n_modes);
  refine();
}

//: Swap the n rows beginning at i with those at j
inline void m23d_swap_rows(vnl_matrix<double>& M,
                           unsigned i, unsigned j, unsigned n)
{
  vnl_matrix<double> Mi = M.extract(n,M.cols(),i,0);
  M.update(M.extract(n,M.cols(),j,0),i,0);  // Copy rows at j to i
  M.update(Mi,j,0);                         // Copy rows from i to j
}


//: Reconstruct structure from set of 2d pts
// Formulates measurement matrix P2D then calls reconstruct() function
void m23d_ortho_flexible_builder::reconstruct(
   const vcl_vector< vcl_vector< vgl_point_2d<double> > >& pt_vec_list,
                            const unsigned& n_modes )
{
  // convert pts into a matrix
  int nf= pt_vec_list.size();
  int n0= pt_vec_list[0].size();
  vnl_matrix<double> D(2*nf,n0);
  for (int i=0; i<nf; ++i)
  {
    if ( (unsigned)n0!= pt_vec_list[i].size() )
    {
      vcl_cerr<< "ERROR m23d_ortho_rigid_builder::reconstruct()\n"
              << "problem with different numbers of pts\n"
              << "pt_vec_list[0].size()= " << pt_vec_list[0].size() << '\n'
              << "pt_vec_list[" << i << "].size()= " << pt_vec_list[i].size()
              << vcl_endl;
      vcl_abort();
    }

    for (int p=0; p<n0; ++p)
    {
      D(2*i,p)= pt_vec_list[i][p].x();
      D(2*i+1,p)= pt_vec_list[i][p].y();
    }
  }

  reconstruct(D, n_modes);
}


//: Reconstruct structure of 3D points given multiple 2D views
//  Data assumed to be scaled orthographic projections
//  The result is stored in the shape_3d() matrix.
//  The estimated projection matrices are stored in the projections() matrix
//  Automatically select views which form a good basis.
//  \param P2D 2ns x np matrix. Rows contain alternating x's and y's from 2D shapes
void m23d_ortho_flexible_builder::reconstruct(const vnl_matrix<double>& P2D, unsigned n_modes)
{
  assert(P2D.rows()%2==0);

  set_view_data(P2D);

  vcl_vector<unsigned> basis = m23d_select_basis_views(P2Dc_,n_modes,1000);

  // Swap in basis here
  for (unsigned i=0;i<=n_modes;++i)
    m23d_swap_rows(P2Dc_,2*i,2*basis[i],2);

  initial_decomposition(n_modes);

  vnl_matrix<double> G;
  compute_correction(P_,G);

  // Apply the correction matrix
  P_=P_*G;
  vnl_svd<double> G_svd(G);
  P3D_=G_svd.inverse() * P3D_;

  disambiguate_z();
  correct_coord_frame(P_,P3D_);
  refine();

  // Swap out basis here, and re-order derived matrices
  for (int i=n_modes;i>=0;--i)
  {
    m23d_swap_rows(P_,2*i,2*basis[i],2);
    m23d_swap_rows(P2Dc_,2*i,2*basis[i],2);
    m23d_swap_rows(pure_P_,2*i,2*basis[i],2);
    m23d_swap_rows(coeffs_,i,basis[i],1);
  }
}

//: Take copy of 2D points and remove CoG from each
void m23d_ortho_flexible_builder::set_view_data(const vnl_matrix<double>& P2D)
{
  assert(P2D.rows()%2==0);
  unsigned ns = P2D.rows()/2;

  // Take copy of 2D points and remove CoG from each
  P2Dc_=P2D;
  cog_.resize(ns);
  for (unsigned i=0;i<ns;++i)
  {
    vnl_vector<double> row_x=P2D.get_row(2*i);
    vnl_vector<double> row_y=P2D.get_row(2*i+1);
    double cog_x = row_x.mean();
    double cog_y = row_y.mean();
    row_x-=cog_x;
    row_y-=cog_y;
    P2Dc_.set_row(2*i,row_x);
    P2Dc_.set_row(2*i+1,row_y);
    cog_[i]=vgl_point_2d<double>(cog_x,cog_y);
  }
}

//: Decompose centred view data to get initial estimate of shape/projection
//  Uncertain up to an affine transformation
void m23d_ortho_flexible_builder::initial_decomposition(unsigned n_modes)
{
  unsigned ns = P2Dc_.rows()/2;
  unsigned np = P2Dc_.cols();

  // Use SVD to get first estimate of the projection/shape matrices
  // These are ambiguous up to a txt affine transformation.
  vnl_svd<double> svd(P2Dc_);
  unsigned t=3*(1+n_modes);
  P_.set_size(2*ns,t);
  P3D_.set_size(t,np);
  for (unsigned i=0;i<t;++i)
  {
    P_.set_column(i,svd.W(i)*svd.U().get_column(i));
    P3D_.set_row(i,svd.V().get_column(i));
  }

  vcl_cout<<"Initial reconstruction error: "<<(P_*P3D_-P2Dc_).rms()<<vcl_endl;
}

//: Disambiguate the ambiguity in the sign of the z ordinates
// First non-zero element should be negative.
void m23d_ortho_flexible_builder::disambiguate_z()
{
  unsigned ns = P2Dc_.rows()/2;
  unsigned np = P2Dc_.cols();

  for (unsigned i=0;i<np;++i)
  {
    if (P3D_(2,i)<0) break;
    if (P3D_(2,i)>0)
    {
      // Flip sign of z elements
      for (unsigned j=0;j<np;++j) P3D_(2,j)*=-1;
      for (unsigned j=0;j<2*ns;++j) P_(j,2)*=-1;
      break;
    }
  }
}


//: Reconstruct structure of 3D points given multiple 2D views
//  Data assumed to be scaled orthographic projections
//  The result is stored in the shape_3d() matrix.
//  The estimated projection matrices are stored in the projections() matrix
//  \param P2D 2ns x np matrix. Rows contain alternating x's and y's from 2D shapes
void m23d_ortho_flexible_builder::partial_reconstruct(const vnl_matrix<double>& P2D,
                                                      unsigned n_modes)
{
  assert(P2D.rows()%2==0);

  set_view_data(P2D);
  initial_decomposition(n_modes);

  vnl_matrix<double> G;
  compute_correction(P_,G);

  // Apply the correction matrix
  P_=P_*G;
  vnl_svd<double> G_svd(G);
  P3D_=G_svd.inverse() * P3D_;

  disambiguate_z();
  correct_coord_frame(P_,P3D_);
}

//: Fill a symmetric matrix with elements from v
static vnl_matrix<double> sym_matrix_from_vec(const vnl_vector<double>& v, unsigned n)
{
  vnl_matrix<double> S(n,n);
  unsigned k=0;
   for (unsigned i=0;i<n;++i)
     for (unsigned j=0;j<=i;++j,++k)
       S(i,j)=S(j,i)=v[k];
  return S;
}

//: Solve for correction matrix for zero mode case
static vnl_matrix<double> am_solve_for_G0(const vnl_matrix<double>& A,
                                          const vnl_vector<double>& rhs)
{
  unsigned n=3;
  vnl_svd<double> svd(A);
  vnl_vector<double> q0 = svd.solve(rhs);
  vnl_matrix<double> Q0=sym_matrix_from_vec(q0,n);

  // If Gk is the t x 3 matrix, the k-th triplet of columns of G,
  // then Gk.Gk'=Q0
  // Use eigen decomposition to compute Gk
  vnl_symmetric_eigensystem<double> eig(Q0);
  vnl_matrix<double> Gk(n,3);
  for (unsigned i=0;i<3;++i)
  {
    //Gk.set_column(i,vcl_sqrt(eig.get_eigenvalue(n-1-i))
    //                        *eig.get_eigenvector(n-1-i));


    // nb critical bit making sure Gk is pos def
    double s= vcl_sqrt(  vcl_fabs(eig.get_eigenvalue(n-1-i)) );
    Gk.set_column(i, s*eig.get_eigenvector(n-1-i) );
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

  if (!LM.minimize_using_gradient(g))
  {
    vcl_cout<<"LM failed!!" << vcl_endl;
    vcl_abort();
  }

  vcl_cout<<"am_solve_for_Gk (k="<<k<<") RMS="<<err_fn.rms(g)<<vcl_endl;

  // Reshape g into 3(m+1) x 3 matrix
  return vnl_matrix<double>(g.data_block(),3*(m+1),3);
}

#if 0 // refinement of am_solve_for_Gk()

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

static vnl_matrix<double> am_solve_for_Gk(const vnl_matrix<double>& A,
                                          const vnl_vector<double>& rhs,
                                          unsigned m, unsigned k)
{
  // Now refine the solution   *** Not sure that this helps ***
  vnl_diag_matrix<double> W(nq);
  double w0=0.01*svd.W(0);
  for (unsigned i=0;i<nq;++i)
  {
    if (svd.W(i)>w0) W[i]=0.0;
    else             W[i]=1.0; //w0*w0/(w0*w0+svd.W(i)*svd.W(i));
  }
  vnl_matrix<double> K=svd.V()*W*svd.V().transpose();
  for (unsigned k=0;k<3;++k)
  {
    vnl_matrix<double> Q1=Gk*Gk.transpose();
    vnl_vector<double> q1=vec_from_sym_matrix(Q1);
    vcl_cout<<"Error for q1 = "<<(A*q1-rhs).rms()<<vcl_endl;
    vnl_vector<double> q2=q0+K*(q1-q0);

    Q0=sym_matrix_from_vec(q2,n);
    vnl_symmetric_eigensystem<double> eig2(Q0);
    vcl_cout<<"Eigenvalues: "<<eig2.D.diagonal()<<vcl_endl;
    for (unsigned i=0;i<3;++i)
    {
      Gk.set_column(i,vcl_sqrt(eig2.get_eigenvalue(n-1-i))
                              *eig2.get_eigenvector(n-1-i));
    }
  }
  vcl_cout<<"Gk*Gk'="<<Gk*Gk.transpose()<<vcl_endl;

  return Gk;
}
#endif // 0

static void compute_Gk(const vnl_matrix<double> & M, unsigned k,
                       vnl_matrix<double>& Gk)
{
  assert(M.cols()>=3);
  unsigned m = M.cols()/3 -1;

  vnl_matrix<double> A;
  vnl_vector<double> rhs;
  m23d_set_q_constraints(M,k,A,rhs);

  if (m==0)
    Gk=am_solve_for_G0(A,rhs);
  else
  {
    Gk=am_solve_for_Gk(A,rhs,m,k);
  }

  if (k==0)
  {
    // Compute extra rotation so that the projection matrix for the
    // first shape is a scaled identity projection.
    vnl_matrix<double> M0=M.extract(2,3*(1+m))*Gk;

    // Compute a rotation matrix for this
    vnl_matrix<double> R=m23d_rotation_from_ortho_projection(M0);

    // Apply inverse so that P.G gives unit projection
    Gk=Gk*R.transpose();
  }
}

// Compute correction matrix G (t x t) s.t.  M=M1.G and B=(inv(G)).B1
void m23d_ortho_flexible_builder::compute_correction(
                           const vnl_matrix<double>& M1,
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

#if 0 // unused static function
static double min_row_scale(const vnl_matrix<double>& M)
{
  double r1=0,r2=0;
  for (unsigned j=0;j<3;++j)
  {
    r1+=M(0,j)*M(0,j);
    r2+=M(1,j)*M(1,j);
  }
  return vcl_sqrt(vcl_min(r1,r2));
}
#endif // 0

// Apply rotation matrices to each 3 columns of M (and inverse to rows of B)
// Matrix selected so that projection matrices in each 3 cols have same
// effective rotation.
void m23d_ortho_flexible_builder::correct_coord_frame(vnl_matrix<double>& M,
                                                      vnl_matrix<double>& B)
{
  unsigned m = M.cols()/3-1;
  unsigned ns = M.rows()/2;
  unsigned np = B.cols();
  // Compute rotation matrix required to arrange that first 2x3 is unit projection
  vnl_matrix<double> U0 = m23d_rotation_from_ortho_projection(M.extract(2,3));
  M.update(M.extract(2*ns,3) * U0.transpose());
  B.update(U0*B.extract(3,np));

  for (unsigned j=1;j<=m;++j)
  {
    // Due to the ambiguity in the 3rd row of the rotation
    // matrix returned by m23d_rotation_from_ortho_projection,
    // and the inherent sign ambiguity of the whole matrix,
    // there are four possible rotations for each row.
    // Compute all rotations, then select the consistent ones.

    vcl_vector<vnl_matrix<double> > U;
    for (unsigned i=0;i<ns;++i)
    {
      vnl_matrix<double> Mi0=M.extract(2,3,2*i,0);
      vnl_matrix<double> Mij=M.extract(2,3,2*i,3*j);
#if 0  // **** TEST
      if (Mij(0,0)/Mi0(0,0)<0) Mij*=-1;
#endif
      vnl_matrix<double> Ui0 = m23d_rotation_from_ortho_projection(Mi0);
      vnl_matrix<double> Uij = m23d_rotation_from_ortho_projection(Mij);
#if 0 // DEBUG
      vcl_cout<<Mij*Uij.transpose()<<vcl_endl;
#endif
      U.push_back(Uij.transpose()*Ui0);
#if 0 // DEBUG
      vcl_cout<<"j="<<j<<" i="<<i<<") C1:"<<vcl_endl
              <<Uij.transpose()*Ui0<<vcl_endl;
#endif
      // Swap sign of 3rd row of Uij
      for (unsigned k=0;k<3;++k) Uij(2,k)*=-1;
      U.push_back(Uij.transpose()*Ui0);
#if 0 // DEBUG
      vcl_cout<<"j="<<j<<" i="<<i<<") C2:"<<vcl_endl
              <<Uij.transpose()*Ui0<<vcl_endl
              <<"Rel wts: "<<Mij(0,0)/Mi0(0,0)<<vcl_endl;
#endif
    }

    // Now find the mean and variance for a variety of possible
    // rotation options.
    // For each one, select the closest of the four possible matrices
    // for each shape
    vnl_matrix<double> best_meanU;
    double best_rms = 1e99;

    for (unsigned i=0;i<vcl_min(2*ns,40u);++i)
    {
      vnl_matrix<double> baseR = U[i];
      vnl_matrix<double> sumU(3,3,0.0), bestU;
      double sum_rms=0.0;

      for (unsigned k=0;k<ns;++k)
      {
        double d_min=(U[2*k]-baseR).rms();
        bestU = U[2*k];
        double d1=(U[2*k]+baseR).rms();
        if (d1<d_min) { d_min=d1; bestU=-U[2*k]; }
        double d2=(U[2*k+1]-baseR).rms();
        if (d2<d_min) { d_min=d2; bestU=U[2*k+1]; }
        double d3=(U[2*k+1]+baseR).rms();
        if (d3<d_min) { d_min=d3; bestU=-U[2*k+1]; }

        sumU+=bestU;
        sum_rms+=d_min;
      }
      sumU/=ns;
      sum_rms/=ns;
      if (i==0 || sum_rms<best_rms)
      {
        best_meanU=sumU;
        best_rms=sum_rms;
      }
    }
    // Remove scaling effects introduced by averaging
    vnl_svd<double> Usvd(best_meanU);
    vnl_matrix<double> Uj = Usvd.U()*Usvd.V().transpose();
    M.update(M.extract(2*ns,3,0,3*j) * Uj,0,3*j);
    B.update(Uj.transpose()*B.extract(3,np,3*j,0),3*j,0);
  }
}

//: Modify projection matrices so they are scaled orthographic projections
//  P = s(I|0)*R
void m23d_ortho_flexible_builder::make_pure_projections()
{
  unsigned ns = P_.rows()/2;
  unsigned m = P_.cols()/3-1;

  pure_P_.set_size(2*ns,3);
  coeffs_.set_size(ns,m+1);

  // Generate identity projection
  vnl_matrix<double> PI(2,3),Pzero(2,3,0.0);
  PI(0,0)=1; PI(0,1)=0; PI(0,2)=0;
  PI(1,0)=0; PI(1,1)=1; PI(1,2)=0;

  // Force first row to be the identity
  P_.update(PI,0,0);
  pure_P_.update(PI,0,0);
  coeffs_(0,0)=1.0;
  for (unsigned i=1;i<=m;++i)
  {
    P_.update(Pzero,0,3*i);
    coeffs_(0,i)=0.0;
  }

  // Force next m rows to only contain one non-zero pure projection
  for (unsigned i=1;i<=m;++i)
    for (unsigned j=0;j<=m;++j)
    {
      if (i==j)
      {
        vnl_matrix<double> P0=m23d_pure_ortho_projection(P_.extract(2,3,2*i,3*j));
        P_.update(P0,2*i,3*j);
        pure_P_.update(P0,2*i,0);
        coeffs_(i,j)=1.0;
      }
      else
      {
        P_.update(Pzero,2*i,3*j);
        coeffs_(i,j)=0.0;
      }
    }

  // Replace each subsequent 2x3 projection with the
  // closest pure scaled orthogonal projection.
  for (unsigned i=m+1;i<ns;++i)
  {
    // Generate a (m+1) x 6 matrix, each row containing one projection matrix
    vnl_matrix<double> Mi(m+1,6);
    for (unsigned j=0;j<=m;++j)
    {
      Mi(j,0)=P_(2*i,3*j);   Mi(j,1)=P_(2*i,3*j+1);   Mi(j,2)=P_(2*i,3*j+2);
      Mi(j,3)=P_(2*i+1,3*j); Mi(j,4)=P_(2*i+1,3*j+1); Mi(j,5)=P_(2*i+1,3*j+2);
    }
    // Mi should be a'p where a are coeffs and p is mean projection
    // |p| should be sqrt(2) for a pure projection (no scaling)
    vnl_svd<double> Mi_svd(Mi);
    vnl_vector<double> p=Mi_svd.V().get_column(0)*vcl_sqrt(2.0);

    vnl_matrix<double> Pi0 = vnl_matrix<double>(p.data_block(),2,3);
    vnl_matrix<double> Pi = m23d_pure_ortho_projection(Pi0);
    vnl_vector<double> pi(Pi.data_block(),6);

    // Estimate weight for each basis vector
    // Each row of Mi should be a[j]*pi'
    // Since pi.pi = 2  (each 3 elements is a unit vector)
    // a = 0.5*Mi*pi
    vnl_vector<double> a = Mi*pi*0.5;

    for (unsigned j=0;j<=m;++j)
      P_.update(Pi*a[j],2*i,3*j);

    pure_P_.update(Pi,2*i,0);
    coeffs_.set_row(i,a);
  }
}

//: Refine estimates of projection and structure
void m23d_ortho_flexible_builder::refine()
{
  vcl_cout<<"RMS Reconstruction error before refine = "
          <<(P2Dc_-P_*P3D_).rms()<<vcl_endl;

  make_pure_projections();
  // Re-estimate the 3D shape by solving the linear equation
  vnl_svd<double> svd(P_);
  P3D_ = svd.pinverse() * P2Dc_;
       // Slightly less stable than backsub, but what the heck.

  vcl_cout<<"RMS Reconstruction error after refine = "
          <<(P2Dc_-P_*P3D_).rms()<<vcl_endl;

  compute_mean(mean_shape_,mean_coeffs_);
}

//: Compute the mean 3D shape using the current co-effs
void m23d_ortho_flexible_builder::compute_mean(vnl_matrix<double>& mean_shape,
                                               vnl_vector<double>& mean_coeffs)
{
  // Compute average for each coefficient
  unsigned ns=coeffs_.rows();
  unsigned nm=coeffs_.cols();
  unsigned np=P3D_.cols();
  mean_coeffs.set_size(nm);
  mean_coeffs.fill(0.0);
  for (unsigned i=0;i<ns;++i) mean_coeffs+=coeffs_.get_row(i);
  mean_coeffs/=ns;
  mean_shape.set_size(3,np);
  mean_shape.fill(0.0);
  for (unsigned j=0;j<nm;++j)
    mean_shape += P3D_.extract(3,np,3*j,0) * mean_coeffs[j];
}

//: Return 3D shape i as a 3 x np matrix
vnl_matrix<double> m23d_ortho_flexible_builder::shape(unsigned i) const
{
  unsigned np=P3D_.cols();
  vnl_matrix<double> S(3,np,0.0);
  for (unsigned j=0;j<coeffs_.cols();++j)
    S += P3D_.extract(3,np,3*j,0) * coeffs_(i,j);
  return S;
}


//: Get back 3d pts rotated and shifted for each frame
void m23d_ortho_flexible_builder::get_shape_3d_pts( vcl_vector< vgl_point_3d<double> >& pts ) const
{
#if 0
  if (P_.rows() < 2 || P_.cols() != 3 )
  {
    vcl_cerr<<"ERROR m23d_ortho_flexible_builder::get_shape_3d_pts()\n"
            <<"problem with size of P_\n"
            <<"P_.rows()= "<<P_.rows()<<'\n'
            <<"P_.cols()= "<<P_.cols()<<vcl_endl;
    vcl_abort();
  }


  //vnl_matrix<double> newPi = m23d_scaled_ortho_projection(P_.extract(2,3,2*i,0));
  //P_.update(newPi,2*i,0);
  vnl_matrix<double> P0= P_.extract(2,3);
#endif // 0

  // get projection matrix for first shape
  vnl_matrix<double> P0=shape(0);

  // Compute a rotation matrix for this projection
  // then update shape and
  vnl_matrix<double> R0=m23d_rotation_from_ortho_projection(P0);

  // apply rotation to base shape
  vnl_matrix<double> rot_pts_mat=R0* P3D_;
  mat_to_3d_pts( pts, rot_pts_mat );


  int np= pts.size();
  vgl_vector_3d<double> tran_vec( cog_[0].x(), cog_[0].y(), 0 );
  for (int p=0; p<np; ++p)
  {
    pts[p]= pts[p] + tran_vec;
  }
}


//: Return 3d rigid pts, i.e., aligned with first frame
void m23d_ortho_flexible_builder::mat_to_3d_pts(vcl_vector< vgl_point_3d<double> >& pt_vec,
                                                const vnl_matrix<double>& M) const
{
#if 0
  // get pts out of P3D_ matrix
  if ( M.rows() != 3 )
  {
    vcl_cerr<<"ERROR m23d_ortho_flexible_builder::mat_to_3d_pts()\n"
            <<"problem with size of matrix\n"
            <<"M.rows()= "<<M.rows()<<'\n'
            <<"M.cols()= "<<M.cols()<<vcl_endl;
    vcl_abort();
  }
#endif // 0

  int np= M.cols();
  pt_vec.resize(np);
  for (int i=0; i<np; ++i)
  {
    pt_vec[i].set( M(0,i), M(1,i), M(2,i) );
  }
}

