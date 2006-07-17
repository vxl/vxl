//:
// \file
// \author Tim Cootes
// \brief Implementation of the Tomasi & Kanade reconstruction algorithm

#include <m23d/m23d_ortho_rigid_builder.h>
#include <m23d/m23d_rotation_from_ortho_projection.h>
#include <m23d/m23d_scaled_ortho_projection.h>
#include <vnl/algo/vnl_svd.h>
#include <m23d/m23d_set_q_constraint.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vcl_iostream.h>

//: Reconstruct structure of 3D points given multiple 2D views
//  Data assumed to be scaled orthographic projections
//  The result is stored in the shape_3d() matrix.
//  The estimated projection matricies are stored in the projections() matrix
//  \param P2D 2ns x np matrix. Rows contain alternating x's and y's from 2D shapes
void m23d_ortho_rigid_builder::reconstruct(const vnl_matrix<double>& P2D)
{
  assert(P2D.rows()%2==0);
  unsigned ns = P2D.rows()/2;
  unsigned np = P2D.cols();

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

  // Use SVD to get first estimate of the projection/shape matrices
  // These are ambiguous up to a 3x3 affine transformation.
  vnl_svd<double> svd(P2Dc_);
  P_.set_size(2*ns,3);
  P3D_.set_size(3,np);
  for (unsigned i=0;i<3;++i)
  {
    P_.set_column(i,svd.W(i)*svd.U().get_column(i));
    P3D_.set_row(i,svd.V().get_column(i));
  }

  // Apply orthogonality constraints to estimate affine correction matrix G
  unsigned nq = 6;
  unsigned n_con = 2*ns+1;

  // Set up constraints on elements of Q
  // Q symmetric, encoded using elements i,j<=i in the vector q
  // q obtained by solving Aq=rhs
  vnl_matrix<double> A(n_con,nq);
  vnl_vector<double> rhs(n_con);

  unsigned c=0;
  // If P0 is projection matrix for first shape, arrange that P0.P0'=I
  vnl_vector<double> px0 = P_.get_row(0);
  vnl_vector<double> py0 = P_.get_row(1);
  m23d_set_q_constraint1(A,rhs,c,px0,px0,1); ++c;
  m23d_set_q_constraint1(A,rhs,c,py0,py0,1); ++c;
  m23d_set_q_constraint1(A,rhs,c,px0,py0,0); ++c;

  // These constraints aim to impose orthogonality on rows of projection
  // matrices.
  for (unsigned i=1;i<ns;++i)
  {
    vnl_vector<double> pxi = P_.get_row(2*i);
    vnl_vector<double> pyi = P_.get_row(2*i+1);
    m23d_set_q_constraint2(A,rhs,c,pxi,pyi); ++c;
    m23d_set_q_constraint1(A,rhs,c,pxi,pyi,0); ++c;
  }

  assert(c==n_con);

  vnl_svd<double> svd_A(A);
  vcl_cout<<"Singular values of constraints: "<<svd_A.W().diagonal()<<vcl_endl;

  vnl_vector<double> q = svd_A.solve(rhs);
  vcl_cout<<"RMS Error in q = "<<(A*q-rhs).rms()<<vcl_endl;

  vnl_matrix<double> Q(3,3);
  c=0;
  for (unsigned i=0;i<3;++i)
    for (unsigned j=0;j<=i;++j,++c)
      Q(i,j)=Q(j,i)=q[c];

  // If G is the 3 x 3 correction matrix, then G.G'=Q
  // Use cholesky decomposition to compute G
  vnl_symmetric_eigensystem<double> eig(Q);
  vnl_matrix<double> G(3,3);
  vcl_cout<<"Eigenvalues: "<<eig.D.diagonal()<<vcl_endl;
  for (unsigned i=0;i<3;++i)
  {
    G.set_column(i,vcl_sqrt(eig.get_eigenvalue(2-i))
                            *eig.get_eigenvector(2-i));
  }

  // Now need to apply an additional rotation so that the
  // first projection matrix is approximately the identity.
  vnl_matrix<double> P0=P_.extract(2,3)*G;

  // Compute a rotation matrix for this
  vnl_matrix<double> R=m23d_rotation_from_ortho_projection(P0);

vcl_cout<<"P0*Rt"<<vcl_endl<<P0*R.transpose()<<vcl_endl;
  // Apply inverse so that P.G gives unit projection
  G=G*R.transpose();

  // Apply the correction matrix
  P_=P_*G;
  vnl_svd<double> G_svd(G);
  P3D_=G_svd.inverse() * P3D_;

  // Disambiguate the ambiguity in the sign of the z ordinates
  // First non-zero element should be negative.
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

//: Modify projection matrices so they are scaled orthographic projections
//  P = s(I|0)*R
void m23d_ortho_rigid_builder::make_pure_projections()
{
  unsigned ns = P_.rows()/2;

  // Force first to be identity
  P_(0,0)=1; P_(0,1)=0; P_(0,2)=0;
  P_(1,0)=0; P_(1,1)=1; P_(1,2)=0;

  // Replace each subsequent 2x3 projection with the
  // closest pure scaled orthogonal projection.
  for (unsigned i=1;i<ns;++i)
  {
    vnl_matrix<double> newPi = m23d_scaled_ortho_projection(P_.extract(2,3,2*i,0));
    P_.update(newPi,2*i,0);
  }
}

//: Refine estimates of projection and structure
void m23d_ortho_rigid_builder::refine()
{
  make_pure_projections();
  // Re-estimate the 3D shape by solving the linear equation
  vnl_svd<double> svd(P_);
  P3D_ = svd.pinverse() * P2Dc_;
       // Slightly less stable than backsub, but what the heck.
}

