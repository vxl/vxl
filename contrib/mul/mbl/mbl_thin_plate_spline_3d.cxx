// This is mul/mbl/mbl_thin_plate_spline_3d.cxx
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "mbl_thin_plate_spline_3d.h"
//:
// \file
// \brief Construct thin plate spline to map 3D to 3D
// \author Tim Cootes

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/io/vgl_io_point_3d.h>
#include <mbl/mbl_matxvec.h>

//=======================================================================

//=======================================================================
// Dflt ctor
// Default constructor gives identity mapping
//=======================================================================

mbl_thin_plate_spline_3d::mbl_thin_plate_spline_3d()
  : Wx_(0),Wy_(0),Wz_(0),
    Ax0_(0),AxX_(1),AxY_(0),AxZ_(0),
    Ay0_(0),AyX_(0),AyY_(1),AyZ_(0),
    Az0_(0),AzX_(0),AzY_(0),AzZ_(1),
    energy_x_(0),energy_y_(0),energy_z_(0)
{
}

//=======================================================================
// Destructor
//=======================================================================

mbl_thin_plate_spline_3d::~mbl_thin_plate_spline_3d() = default;

// First some useful maths functions

inline double r2lnr(const vgl_vector_3d<double>&  pt)
{
  double r2 = pt.x() * pt.x() + pt.y() * pt.y() + pt.z() * pt.z();
  if (r2>1e-8)   return 0.5 * r2 * std::log(r2);
  else     return 0;
}

inline double r2lnr(double x, double y, double z)
{
  double r2 = x * x + y * y + z*z;
  if (r2>1e-8) return 0.5 * r2 * std::log(r2);
  else      return 0;
}


// Sets L to be a symmetric square matrix of size n + 4 (n = pts.nelems)
// with L(i,j) = Uij = r2lnr(pts(i)-pts(j)) for i,j <= n
static void build_K_part(vnl_matrix<double>& L,
                         const std::vector<vgl_point_3d<double> >& pts)
{
  unsigned int n = pts.size();
  if ( (L.rows()!=n+4) | (L.columns()!=n+4) ) L.set_size(n+4,n+4);

  const vgl_point_3d<double> * pts_data = &pts[0];
  double** K_data = L.data_array();

    // Zero the diagonal
  for (unsigned int i=0;i<n;i++)
    K_data[i][i] = 0;
    // Now fill upper & lower triangles
  for (unsigned int i=1;i<n;i++)
    for (unsigned int j=0;j<i;j++)
    {
      K_data[i][j] = K_data[j][i] = r2lnr(pts_data[i]-pts_data[j]);
    }
}

// L is a (n+4) x (n+43) matrix;
// L = ( K  Q )
//     ( Q' 0 )
// Where K is n x n, K(i,j) = Uij = r2lnr(pts(i)-pts(j)) for i,j <= n
// and Q is ( 1 x0 y0 z0)
//          ( 1 x1 y1 z1)
//             . .  .
static void build_L(vnl_matrix<double>& L, const std::vector<vgl_point_3d<double> >& pts)
{
  int i,j;

  build_K_part(L,pts);

  int n = pts.size();

  const vgl_point_3d<double> * pts_data = &pts[0];
  double** L_data = L.data_array();

  // Build Q part

  for (i=0;i<n;i++)
  {
    L_data[i][n] = 1;
    L_data[i][n+1] = pts_data[i].x();
    L_data[i][n+2] = pts_data[i].y();
    L_data[i][n+3] = pts_data[i].z();

    L_data[n][i] = 1;
    L_data[n+1][i] = pts_data[i].x();
    L_data[n+2][i] = pts_data[i].y();
    L_data[n+3][i] = pts_data[i].z();
  }

  // put 0's in bottom right corner.
  for (i=n;i<n+4;i++)
    for (j=n;j<n+4;j++)
      L_data[i][j] = 0;
}

//: Build from small number of points
void mbl_thin_plate_spline_3d::build_pure_affine(
        const std::vector<vgl_point_3d<double> >& source_pts,
        const std::vector<vgl_point_3d<double> >& dest_pts)
{
  int n=source_pts.size();
  L_inv_.set_size(0,0);
  if (n==0)
  {
    // Set identity transformation
    Ax0_ = 0;
    Ay0_ = 0;
    Az0_ = 0;
    AxX_ = 1; AxY_ = 0; AxZ_ = 0;
    AyX_ = 0; AyY_ = 1; AyZ_ = 0;
    AzX_ = 0; AzY_ = 0; AzZ_ = 1;

    Wx_.set_size(0);
    Wy_.set_size(0);
    Wz_.set_size(0);

    src_pts_.resize(0);

    return;
  }

  if (n==1)
  {
    // Just apply a translation :
    Ax0_ = dest_pts[0].x() - source_pts[0].x();
    Ay0_ = dest_pts[0].y() - source_pts[0].y();
    Az0_ = dest_pts[0].z() - source_pts[0].z();
    Wx_.set_size(0);
    Wy_.set_size(0);
    Wz_.set_size(0);
    AxX_ = 1; AxY_ = 0; AxZ_ = 0;
    AyX_ = 0; AyY_ = 1; AyZ_ = 0;
    AzX_ = 0; AzY_ = 0; AzZ_ = 1;
    src_pts_.resize(0);

    return;
  }
  if (n>=2)
  {
    std::cerr<<"mbl_thin_plate_spline_3d::build_pure_affine() Incomplete. sorry.\n";
    std::abort();
  }
}

//: Set parameters from vectors
void mbl_thin_plate_spline_3d::set_params(const vnl_vector<double>& W1,
                                          const vnl_vector<double>& W2,
                                          const vnl_vector<double>& W3)
{
  int n = W1.size()-4;

  if (int(Wx_.size()) < n) Wx_.set_size(n);
  if (int(Wy_.size()) < n) Wy_.set_size(n);
  if (int(Wz_.size()) < n) Wz_.set_size(n);

  double *Wx_data=Wx_.data_block();
  double *Wy_data=Wy_.data_block();
  double *Wz_data=Wz_.data_block();
  const double *W1_data=W1.data_block();
  const double *W2_data=W2.data_block();
  const double *W3_data=W3.data_block();

  for (int i=0;i<n;i++)
  {
    Wx_data[i] = W1_data[i];
    Wy_data[i] = W2_data[i];
    Wz_data[i] = W3_data[i];
  }

  Ax0_ = W1_data[n];
  AxX_ = W1_data[n+1];
  AxY_ = W1_data[n+2];
  AxZ_ = W1_data[n+3];

  Ay0_ = W2_data[n];
  AyX_ = W2_data[n+1];
  AyY_ = W2_data[n+2];
  AyZ_ = W2_data[n+3];

  Az0_ = W3_data[n];
  AzX_ = W3_data[n+1];
  AzY_ = W3_data[n+2];
  AzZ_ = W3_data[n+3];
}

void mbl_thin_plate_spline_3d::compute_energy(vnl_vector<double>& W1,
                                              vnl_vector<double>& W2,
                                              vnl_vector<double>& W3,
                                              const vnl_matrix<double>& L)
{
  int n = W1.size()-3;
  double *W1_data=W1.data_block();
  double *W2_data=W2.data_block();
  double *W3_data=W3.data_block();

  // Set last elements to zero
  for (int i = n;i<=n+3;++i)
  {
    W1_data[i]=0.0;
    W2_data[i]=0.0;
    W3_data[i]=0.0;
  }

  // Compute bending energy = W_t.K.W/(8pi)
  vnl_vector<double> LW;
  mbl_matxvec_prod_mv(L,W1,LW);
  energy_x_ = dot_product(W1,LW) / (8*vnl_math::pi);

  mbl_matxvec_prod_mv(L,W2,LW);
  energy_y_ = dot_product(W2,LW) / (8*vnl_math::pi);

  mbl_matxvec_prod_mv(L,W3,LW);
  energy_z_ = dot_product(W3,LW) / (8*vnl_math::pi);
}

void mbl_thin_plate_spline_3d::set_up_rhs(vnl_vector<double>& Bx,
                                          vnl_vector<double>& By,
                                          vnl_vector<double>& Bz,
                                          const std::vector<vgl_point_3d<double> >& dest_pts)
{
  int n =dest_pts.size();

  Bx.set_size(n+4);
  By.set_size(n+4);
  Bz.set_size(n+4);
  double* Bx_data=Bx.data_block();
  double* By_data=By.data_block();
  double* Bz_data=Bz.data_block();
  const vgl_point_3d<double>  *d_pts_data=&dest_pts[0];

  for (int i=0;i<n;i++)
  {
    Bx_data[i] = d_pts_data[i].x();
    By_data[i] = d_pts_data[i].y();
    Bz_data[i] = d_pts_data[i].z();
  }
  for (int i=n;i<n+4;i++)
  {
    Bx_data[i] = 0;
    By_data[i] = 0;
    Bz_data[i] = 0;
  }
}

void mbl_thin_plate_spline_3d::build(const std::vector<vgl_point_3d<double> >& source_pts,
                                     const std::vector<vgl_point_3d<double> >& dest_pts,
                                     bool compute_the_energy)
{
  // See Booksteins paper in IPMI 1993 for details of calculation

  unsigned int n=source_pts.size();
  if (dest_pts.size() != n)
  {
    std::cerr<<"mbl_thin_plate_spline_3d::build - incompatible number of points.\n";
    std::abort();
  }

  L_inv_.set_size(0,0);

  if (n<2)  // build_pure_affine only copes with trivial cases at the moment
  {
    build_pure_affine(source_pts,dest_pts);
    return;
  }

  src_pts_ = source_pts;

  vnl_matrix<double> L;
  vnl_vector<double> Bx(n+4), W1(n+4);  // Used to compute X parameters
  vnl_vector<double> By(n+4), W2(n+4);  // Used to compute Y parameters
  vnl_vector<double> Bz(n+4), W3(n+4);  // Used to compute YZ parameters

  build_L(L,source_pts);

  set_up_rhs(Bx,By,Bz,dest_pts);

  // Solve LW = B for W1,W2,W3 :
  // Note that both Cholesky and QR decompositions fail, apparently because of the
  // zeroes on the diagonal.  Use SVD to be safe.
  {
    vnl_svd<double> svd(L);
    svd.solve(Bx.data_block(),W1.data_block());
    svd.solve(By.data_block(),W2.data_block());
    svd.solve(Bz.data_block(),W3.data_block());
  }

  set_params(W1,W2,W3);
  if (compute_the_energy)
    compute_energy(W1,W2,W3,L);
}

//: Define source point positions
//  Performs pre-computations so that build(dest_points) can be
//  called multiple times efficiently
void mbl_thin_plate_spline_3d::set_source_pts(const std::vector<vgl_point_3d<double> >& source_pts)
{
  unsigned int n=source_pts.size();
  src_pts_ = source_pts;

  if (n<2) // build_pure_affine only copes with trivial cases at the moment
  {
    L_inv_.set_size(0,0);
    return;
  }

  vnl_matrix<double> L;
  build_L(L,source_pts);

  // Compute inverse of L
  // Note that both Cholesky and QR decompositions fail, apparently because of the
  // zeroes on the diagonal.  Use SVD to be safe.
  {
    vnl_svd<double> svd(L);
    L_inv_ = svd.inverse();
  }
}

//: Sets up internal transformation to map source_pts onto dest_pts
void mbl_thin_plate_spline_3d::build(const std::vector<vgl_point_3d<double> >& dest_pts)
{
  unsigned int n=src_pts_.size();
  if (dest_pts.size() != n)
  {
    std::cerr<<"mbl_thin_plate_spline_3d::build - incompatible number of points.\n";
    std::abort();
  }

  if (n<2)  // Only copes with trivial cases at the moment
  {
    build_pure_affine(src_pts_,dest_pts);
    return;
  }

  vnl_vector<double> Bx(n+4), W1(n+4);  // Used to compute X parameters
  vnl_vector<double> By(n+4), W2(n+4);  // Used to compute Y parameters
  vnl_vector<double> Bz(n+4), W3(n+4);  // Used to compute Z parameters

  set_up_rhs(Bx,By,Bz,dest_pts);

  mbl_matxvec_prod_mv(L_inv_,Bx,W1);
  mbl_matxvec_prod_mv(L_inv_,By,W2);
  mbl_matxvec_prod_mv(L_inv_,Bz,W3);

  set_params(W1,W2,W3);
  energy_x_ = -1;
  energy_y_ = -1;
  energy_z_ = -1;
}


vgl_point_3d<double>  mbl_thin_plate_spline_3d::operator()(double x, double y, double z) const
{
  unsigned int n = src_pts_.size();

  double x_sum = Ax0_ + AxX_ * x + AxY_ * y + AxZ_ * z;
  double y_sum = Ay0_ + AyX_ * x + AyY_ * y + AyZ_ * z;
  double z_sum = Az0_ + AzX_ * x + AzY_ * y + AzZ_ * z;

  if (n<=4)  // Pure affine
    return {x_sum,y_sum,z_sum};

  const vgl_point_3d<double> * pts_data = &src_pts_[0];
  const double* Wx_data = Wx_.data_block();
  const double* Wy_data = Wy_.data_block();
  const double* Wz_data = Wz_.data_block();

  for (unsigned int i=0;i<n;i++)
  {
    double Ui = r2lnr(x - pts_data[i].x(), y - pts_data[i].y(), z - pts_data[i].z() );
    x_sum += (Ui * Wx_data[i]);
    y_sum += (Ui * Wy_data[i]);
    z_sum += (Ui * Wz_data[i]);
  }

  return {x_sum,y_sum,z_sum};
}

//=======================================================================
// Method: version_no
//=======================================================================

short mbl_thin_plate_spline_3d::version_no() const
{
  return 1;
}


//=======================================================================
// Method: print
//=======================================================================

  // required if data is present in this class
void mbl_thin_plate_spline_3d::print_summary(std::ostream& os) const
{
  os<<"\nfx: "<<Ax0_<<" + "<<AxX_<<"*x + "<<AxY_
    <<"*y + "<<AxZ_<<"*z   Nonlinear terms:";
  for (double i : Wx_)
    os<<' '<<i;
  os<<"\nfy: "<<Ay0_<<" + "<<AyX_<<"*x + "<<AyY_
    <<"*y + "<<AyZ_<<"*z   Nonlinear terms:";
  for (double i : Wy_)
    os<<' '<<i;
  os<<"\nfy: "<<Az0_<<" + "<<AzX_<<"*x + "<<AzY_
    <<"*y + "<<AzZ_<<"*z   Nonlinear terms:";
  for (double i : Wz_)
    os<<' '<<i;
  os<<'\n';
}

//=======================================================================
// Method: save
//=======================================================================

  // required if data is present in this class
void mbl_thin_plate_spline_3d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,Wx_); vsl_b_write(bfs,Wy_);vsl_b_write(bfs,Wz_);
  vsl_b_write(bfs,Ax0_); vsl_b_write(bfs,AxX_);
  vsl_b_write(bfs,AxY_); vsl_b_write(bfs,AxZ_);
  vsl_b_write(bfs,Ay0_); vsl_b_write(bfs,AyX_);
  vsl_b_write(bfs,AyY_); vsl_b_write(bfs,AyZ_);
  vsl_b_write(bfs,Az0_); vsl_b_write(bfs,AzX_);
  vsl_b_write(bfs,AzY_); vsl_b_write(bfs,AzZ_);
  vsl_b_write(bfs,energy_x_);
  vsl_b_write(bfs,energy_y_);
  vsl_b_write(bfs,energy_z_);
  vsl_b_write(bfs,src_pts_);
  vsl_b_write(bfs,L_inv_);
}

//=======================================================================
// Method: load
//=======================================================================

  // required if data is present in this class
void mbl_thin_plate_spline_3d::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,Wx_); vsl_b_read(bfs,Wy_);vsl_b_read(bfs,Wz_);
      vsl_b_read(bfs,Ax0_); vsl_b_read(bfs,AxX_);
      vsl_b_read(bfs,AxY_); vsl_b_read(bfs,AxZ_);
      vsl_b_read(bfs,Ay0_); vsl_b_read(bfs,AyX_);
      vsl_b_read(bfs,AyY_); vsl_b_read(bfs,AyZ_);
      vsl_b_read(bfs,Az0_); vsl_b_read(bfs,AzX_);
      vsl_b_read(bfs,AzY_); vsl_b_read(bfs,AzZ_);
      vsl_b_read(bfs,energy_x_);
      vsl_b_read(bfs,energy_y_);
      vsl_b_read(bfs,energy_z_);
      vsl_b_read(bfs,src_pts_);
      vsl_b_read(bfs,L_inv_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, mbl_thin_plate_spline_3d &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Comparison operator
bool mbl_thin_plate_spline_3d::operator==(const mbl_thin_plate_spline_3d& tps) const
{
  if (&tps==this) return true;
  if (std::fabs(Ax0_-tps.Ax0_)>1e-8) return false;
  if (std::fabs(AxX_-tps.AxX_)>1e-8) return false;
  if (std::fabs(AxY_-tps.AxY_)>1e-8) return false;
  if (std::fabs(AxZ_-tps.AxZ_)>1e-8) return false;
  if (std::fabs(Ay0_-tps.Ay0_)>1e-8) return false;
  if (std::fabs(AyX_-tps.AyX_)>1e-8) return false;
  if (std::fabs(AyY_-tps.AyY_)>1e-8) return false;
  if (std::fabs(AyZ_-tps.AyZ_)>1e-8) return false;
  if (std::fabs(Az0_-tps.Az0_)>1e-8) return false;
  if (std::fabs(AzX_-tps.AzX_)>1e-8) return false;
  if (std::fabs(AzY_-tps.AzY_)>1e-8) return false;
  if (std::fabs(AzZ_-tps.AzZ_)>1e-8) return false;
  if (vnl_vector_ssd(Wx_,tps.Wx_)>1e-6) return false;
  if (vnl_vector_ssd(Wy_,tps.Wy_)>1e-6) return false;
  if (vnl_vector_ssd(Wz_,tps.Wz_)>1e-6) return false;
  return true;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mbl_thin_plate_spline_3d& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mbl_thin_plate_spline_3d& b)
{
    b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mbl_thin_plate_spline_3d& b)
{
  os << "mbl_thin_plate_spline_3d: ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}
