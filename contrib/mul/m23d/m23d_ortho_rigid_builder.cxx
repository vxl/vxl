#include "m23d_ortho_rigid_builder.h"
//:
// \file
// \author Tim Cootes
// \brief Implementation of the Tomasi & Kanade reconstruction algorithm

#include <m23d/m23d_rotation_from_ortho_projection.h>
#include <m23d/m23d_scaled_ortho_projection.h>
#include <m23d/m23d_set_q_constraint.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>

//: Reconstruct structure from set of 2d pts
// Formulates measurement matrix P2D then calls reconstruct funtion above
void m23d_ortho_rigid_builder::reconstruct(const vcl_vector< vcl_vector< vgl_point_2d<double> > >& pt_vec_list )
{
  // convert pts into a matrix
  int nf= pt_vec_list.size();
  int n0= pt_vec_list[0].size();
  vnl_matrix<double> D(2*nf,n0);
  for (int i=0; i<nf; ++i)
  {
    if ( (unsigned)n0!= pt_vec_list[i].size() )
    {
      vcl_cerr<<"ERROR m23d_ortho_rigid_builder::reconstruct()"<<vcl_endl
              <<"problem with different numbers of pts"<<vcl_endl
              <<"pt_vec_list[0].size()= "<<pt_vec_list[0].size()<<vcl_endl
              <<"pt_vec_list["<<i<<"].size()= "<<pt_vec_list[i].size()<<vcl_endl;
      vcl_abort();
    }

    for (int p=0; p<n0; ++p)
    {
       D(2*i,p)= pt_vec_list[i][p].x();
       D(2*i+1,p)= pt_vec_list[i][p].y();
    }
  }

  reconstruct(D);
}


//: Reconstruct structure of 3D points given multiple 2D views
//  Data assumed to be scaled orthographic projections.
//  The result is stored in the shape_3d() matrix.
//  The estimated projection matrices are stored in the projections() matrix.
//  \param P2D 2ns x np matrix. Rows contain alternating x's and y's from 2D shapes
void m23d_ortho_rigid_builder::reconstruct(const vnl_matrix<double>& P2D)
{
  assert(P2D.rows()%2==0);
  unsigned nf = P2D.rows()/2;
  unsigned np = P2D.cols();

  // Take copy of 2D points and remove CoG from each
  P2Dc_=P2D;
  cog_.resize(nf);
  for (unsigned i=0;i<nf;++i)
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
  P_.set_size(2*nf,3);
  P3D_.set_size(3,np);
  vnl_matrix<double> W(3,3);
  W.fill(0);

  for (unsigned i=0;i<3;++i)
  {
    P_.set_column(i, /* svd.W(i)* */ svd.U().get_column(i) );
    P3D_.set_row(i, svd.V().get_column(i) );
    W(i,i)= vcl_sqrt( svd.W(i) );
  }

#if 0
  vcl_cout<<"W= "<<W<<vcl_endl
          <<"P_.rows()= "<<P_.rows()<<vcl_endl
          <<"P_.cols()= "<<P_.cols()<<vcl_endl
          <<"P3D_.rows()= "<<P3D_.rows()<<vcl_endl
          <<"P3D_.cols()= "<<P3D_.cols()<<vcl_endl;
#endif
  P_= P_*W;
  P3D_= W*P3D_;
#if 0
  vcl_cout<<"P_.extract(2,3)= "<<P_.extract(2,3)<<vcl_endl
          <<"P3D_.extract(3,5)= "<<P3D_.extract(3,5)<<vcl_endl;
#endif


  // start of new method

  //using notation from orig paper
  //ie
  //i QQt i=1
  //j QQt j=1
  //i QQt j=0
  // find this matrix Q
  vnl_matrix<double> Q;
  find_correction_matrix( Q, P_);

  // apply correction
  P_= P_*Q;
  //P3D_= Q.transpose() * P3D_;
  vnl_svd<double> Q_svd(Q);
  P3D_=Q_svd.inverse() * P3D_;

  // align model frame with first frame
  // Now need to apply an additional rotation so that the
  // first projection matrix is approximately the identity.
  vnl_matrix<double> P0=P_.extract(2,3);

  vcl_cout<<"P0= "<<P0<<vcl_endl;

  // Compute a rotation matrix for this projection
  vnl_matrix<double> R0=m23d_rotation_from_ortho_projection(P0);

  // apply rotation to mean shape
  P_= P_*R0.transpose();

  // apply reverse rotation to projections
  P3D_=R0* P3D_;


#if 0 // commented out

  // older method (still works fine!)
  // uses fewer constraints
  vnl_matrix<double> Q;
  find_correction_matrix_alt( Q, P_);

  //apply correction + rotation at same time

  // Now need to apply an additional rotation so that the
  // first projection matrix is approximately the identity.

  vnl_matrix<double> P0=P_.extract(2,3)*Q;

#if 0
  vcl_cout<<"P_.extract(2,3)= "<<P_.extract(2,3)<<vcl_endl
          <<"Q= "<<Q<<vcl_endl
          <<"P0= "<<P0<<vcl_endl;
#endif // 0

  // Compute a rotation matrix for this
  vnl_matrix<double> R=m23d_rotation_from_ortho_projection(P0);

  vcl_cout<<"P0*Rt\n"<<P0*R.transpose()<<vcl_endl;

  // Apply inverse so that P.Q gives unit projection
  // ie apply rotation to the correction matrix
  Q=Q*R.transpose();

  // Apply the correction matrix
  P_=P_*Q;
  vnl_svd<double> Q_svd(Q);
  P3D_=Q_svd.inverse() * P3D_;
#endif // 0


  // Disambiguate the ambiguity in the sign of the z ordinates
  // First non-zero element should be negative.
  // nb mainly for benefit of test program!
  for (unsigned i=0;i<np;++i)
  {
    if (P3D_(2,i)<0) break;
    if (P3D_(2,i)>0)
    {
      // Flip sign of z elements
      vcl_cout<<"flipping z coords!"<<vcl_endl;
      for (unsigned j=0;j<np;++j) P3D_(2,j)*=-1;
      for (unsigned j=0;j<2*nf;++j) P_(j,2)*=-1;
      break;
    }
  }
}

//: find matrix Q using constraints on matrix P which must contain orthonormal projects in each (2*3) submatrix for each frame old method
void m23d_ortho_rigid_builder::find_correction_matrix_alt( vnl_matrix<double>& Q,
                                                           const vnl_matrix<double>& P)
{
  // Apply orthogonality constraints to estimate affine correction matrix G
  unsigned nq = 6;
  int nf= P.rows()/2;
  unsigned n_con = 2*nf+1;

  // Set up constraints on elements of L=QQt
  // L symmetric, encoded using elements i,j<=i in the vector q
  // q obtained by solving Aq=rhs
  vnl_matrix<double> A(n_con,nq);
  vnl_vector<double> rhs(n_con);

  unsigned c=0;
  // If P0 is projection matrix for first shape, arrange that P0.P0'=I
  vnl_vector<double> px0 = P.get_row(0);
  vnl_vector<double> py0 = P.get_row(1);
  m23d_set_q_constraint1(A,rhs,c,px0,px0,1); ++c;
  m23d_set_q_constraint1(A,rhs,c,py0,py0,1); ++c;
  m23d_set_q_constraint1(A,rhs,c,px0,py0,0); ++c;

  // These constraints aim to impose orthogonality on rows of projection
  // matrices.
  for (unsigned i=1;i<(unsigned)nf;++i)
  {
    vnl_vector<double> pxi = P.get_row(2*i);
    vnl_vector<double> pyi = P.get_row(2*i+1);
    m23d_set_q_constraint2(A,rhs,c,pxi,pyi); ++c;
    m23d_set_q_constraint1(A,rhs,c,pxi,pyi,0); ++c;
  }

  assert(c==n_con);

  vnl_svd<double> svd_A(A);
  vcl_cout<<"Singular values of constraints: "<<svd_A.W().diagonal()<<vcl_endl;

  vnl_vector<double> q = svd_A.solve(rhs);
  vcl_cout<<"RMS Error in q = "<<(A*q-rhs).rms()<<vcl_endl;

  vnl_matrix<double> L(3,3);
  c=0;
  for (unsigned i=0;i<3;++i)
    for (unsigned j=0;j<=i;++j,++c)
      L(i,j)=L(j,i)=q[c];
#if 0
  vcl_cout<<"L= "<<L<<vcl_endl;
#endif
  // If G is the 3 x 3 correction matrix, then G.G'=Q
  // Use cholesky decomposition to compute G
  vnl_symmetric_eigensystem<double> eig(L);
#if 0
  vnl_matrix<double> Q(3,3);
#endif
  Q.set_size(3,3);
  vcl_cout<<"Eigenvalues: "<<eig.D.diagonal()<<vcl_endl;
  for (unsigned i=0;i<3;++i)
  {
    // Deal with case where Q is not pos-definite (ie has -ive eigenvalues).
    //double s = 0.00001;
    //if (eig.get_eigenvalue(2-i)>0.0)
    //  s = vcl_sqrt(eig.get_eigenvalue(2-i));

    // nb critical bit making sure Q is pos def
    double s= vcl_sqrt(  vcl_fabs(eig.get_eigenvalue(2-i)) );
    Q.set_column(i,s*eig.get_eigenvector(2-i));
  }

#if 0
  vcl_cout<<"Q= "<<Q<<vcl_endl;
#endif
}


//: find matrix Q using constraints on matrix P which must contain orthonormal projects in each (2*3) submatrix for each frame
void m23d_ortho_rigid_builder::find_correction_matrix( vnl_matrix<double>& Q,
                                                        const vnl_matrix<double>& P)
{
  int nf= P.rows()/2;

  // build ortho constraint matrix G
  // where GI=c
  // G= 3nf * 6
  // I= 6 * 1
  // c= 3nf * 1

  // then
  //L = [I(1) I(2) I(3);
  //     I(2) I(4) I(5);
  //      I(3) I(5) I(6)];
  // and L=QQt

  // based on
  //i QQt i=1
  //j QQt j=1
  //i QQt j=0
  vnl_matrix<double> G(nf*3,6);
  vnl_vector<double> con_vec(nf*3);
  for (int f=0; f<nf; ++f)
  {
    // 3 constraints per frame
    vnl_vector<double> i_vec= P.get_row(f*2);
    vnl_vector<double> j_vec= P.get_row(f*2+1);
#if 0
    vcl_cout<<"f="<<f<<vcl_endl
            <<"i_vec= "<<i_vec<<vcl_endl
            <<"j_vec= "<<j_vec<<vcl_endl;
#endif
    //i QQt i=1
    int r0= f*3;
    vnl_vector<double> c0_vec;
    compute_one_row_of_constraints( c0_vec, i_vec, i_vec );
    G.set_row(r0,c0_vec);
    con_vec(r0)=1;

    //j QQt j=1
    int r1= f*3+1;
    vnl_vector<double> c1_vec;
    compute_one_row_of_constraints( c1_vec, j_vec, j_vec );
    G.set_row(r1,c1_vec);
    con_vec(r1)=1;

    //i QQt j=0
    int r2= f*3+2;
    vnl_vector<double> c2_vec;
    compute_one_row_of_constraints( c2_vec, i_vec, j_vec );
    G.set_row(r2,c2_vec);
    con_vec(r2)=0;
  }

  // solve for I
  //GI=con
  vnl_svd<double> svd_G(G);
  vnl_vector<double> I= svd_G.pinverse()*con_vec;

  //3x3 matrix L
  vnl_matrix<double> L(3,3);
  L(0,0)= I(0);
  L(0,1)= I(1);
  L(0,2)= I(2);
  L(1,0)= I(1);
  L(1,1)= I(3);
  L(1,2)= I(4);
  L(2,0)= I(2);
  L(2,1)= I(4);
  L(2,2)= I(5);

  // solve QQt= L
  // use symmetric eigen decomposition L= V*D*Vt
#if 0
  vcl_cout<<"L= "<<L<<vcl_endl;
#endif

  vnl_symmetric_eigensystem<double> eig(L);
#if 0
  vcl_cout<<"eig.V= "<<eig.V<<vcl_endl
          <<"eig.D= "<<eig.D<<vcl_endl;
  vnl_matrix<double> Q(3,3);
#endif
  Q.set_size(3,3);
  vcl_cout<<"Eigenvalues: "<<eig.D.diagonal()<<vcl_endl;
  for (unsigned i=0;i<3;++i)
  {
    // Deal with case where Q is not pos-definite (ie has -ive eigenvalues).
#if 0
    double s=0.00001; // = vcl_sqrt(0.00001);
    if (eig.get_eigenvalue(2-i)>0.0)
    {
      s = vcl_sqrt(eig.get_eigenvalue(2-i));
    }
#endif

    // nb critical bit making sure Q is pos def
    double s= vcl_sqrt(  vcl_fabs(eig.get_eigenvalue(2-i)) );
    Q.set_column(i,s*eig.get_eigenvector(2-i) );
  }
#if 0
  vcl_cout<<"Q= "<<Q<<vcl_endl;
#endif
}

//: find matrix Q using constraints on matrix P which must contain from two rows of a projection matrix (a+b) find six constraints used to compute (QQt) symmetric matrix
void m23d_ortho_rigid_builder::compute_one_row_of_constraints( vnl_vector<double>& c,
                                                               const vnl_vector<double>& a,
                                                               const vnl_vector<double>& b)
{
  c.set_size(6);
  c(0)=a(0)*b(0);
  c(1)=a(1)*b(0)+a(0)*b(1);
  c(2)=a(2)*b(0)+a(0)*b(2);
  c(3)=a(1)*b(1);
  c(4)=a(2)*b(1)+a(1)*b(2);
  c(5)=a(2)*b(2);
}


//: Modify projection matrices so they are scaled orthographic projections
//  $ P = s(I|0)*R $
void m23d_ortho_rigid_builder::make_pure_projections()
{
  unsigned nf = P_.rows()/2;

  // Force first to be identity
  P_(0,0)=1; P_(0,1)=0; P_(0,2)=0;
  P_(1,0)=0; P_(1,1)=1; P_(1,2)=0;

  // Replace each subsequent 2x3 projection with the
  // closest pure scaled orthogonal projection.
  for (unsigned i=1;i<nf;++i)
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
  // Slightly less stable than backsub, but what the heck
  //(you live on the edge Tim!-dac)
}


//: Return 3d rigid pts
// I.e., aligned with first frame
void m23d_ortho_rigid_builder::mat_to_3d_pts(vcl_vector< vgl_point_3d<double> >& pt_vec,
                                                  const vnl_matrix<double>& M) const
{
  // get pts out of P3D_ matrix
  if ( M.rows() != 3 )
  {
    vcl_cerr<<"ERROR m23d_ortho_rigid_builder::mat_to_3d_pts()"<<vcl_endl
            <<"problem with size of matrix"<<vcl_endl
            <<"M.rows()= "<<M.rows()<<vcl_endl
            <<"M.cols()= "<<M.cols()<<vcl_endl;
    vcl_abort();
  }

  int np= M.cols();
  pt_vec.resize(np);
  for (int i=0; i<np; ++i)
  {
    pt_vec[i].set( M(0,i), M(1,i), M(2,i) );
  }
}

//: Get back 3d pts rotated and shifted for each frame
void m23d_ortho_rigid_builder::recon_shapes(vcl_vector< vcl_vector< vgl_point_3d<double> > >& pt_vec_list ) const
{
  if (P_.rows() < 2 || P_.cols() != 3 )
  {
    vcl_cerr<<"ERROR m23d_ortho_rigid_builder::recon_shapes()"<<vcl_endl
            <<"problem with size of P_"<<vcl_endl
            <<"P_.rows()= "<<P_.rows()<<vcl_endl
            <<"P_.cols()= "<<P_.cols()<<vcl_endl;
    vcl_abort();
  }

  unsigned nf= P_.rows()/2;
  pt_vec_list.resize(nf);
  for (unsigned i=0;i<nf;++i)
  {
    //vnl_matrix<double> newPi = m23d_scaled_ortho_projection(P_.extract(2,3,2*i,0));
    //P_.update(newPi,2*i,0);
    vnl_matrix<double> P0= P_.extract(2,3,2*i,0);

    // Compute a rotation matrix for this projection
    // then update shape and
    vnl_matrix<double> R0=m23d_rotation_from_ortho_projection(P0);

    // apply rotation to base shape
    vnl_matrix<double> rot_pts_mat=R0* P3D_;
    mat_to_3d_pts( pt_vec_list[i], rot_pts_mat );

    // apply cog_ translation to each pt vector
    int np= pt_vec_list[i].size();
    vgl_vector_3d<double> tran_vec( cog_[i].x(), cog_[i].y(), 0 );
    for (int p=0; p<np; ++p)
    {
      pt_vec_list[i][p]= pt_vec_list[i][p] + tran_vec;
    }
  }
}


//: Get back 3d pts rotated and shifted for each frame
void m23d_ortho_rigid_builder::get_shape_3d_pts( vcl_vector< vgl_point_3d<double> >& pts ) const
{
  if (P_.rows() < 2 || P_.cols() != 3 )
  {
    vcl_cerr<<"ERROR m23d_ortho_rigid_builder::get_shape_3d_pts()"<<vcl_endl
            <<"problem with size of P_"<<vcl_endl
            <<"P_.rows()= "<<P_.rows()<<vcl_endl
            <<"P_.cols()= "<<P_.cols()<<vcl_endl;
    vcl_abort();
  }


  //vnl_matrix<double> newPi = m23d_scaled_ortho_projection(P_.extract(2,3,2*i,0));
  //P_.update(newPi,2*i,0);
  vnl_matrix<double> P0= P_.extract(2,3);

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

