// This is mul/mbl/mbl_thin_plate_spline_2d.cxx
#include "mbl_thin_plate_spline_2d.h"
//:
// \file
// \brief Construct thin plate spline to map 2D to 2D
// \author Tim Cootes

#include <vcl_cmath.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vgl/io/vgl_io_point_2d.h>
#include <mbl/mbl_matxvec.h>

//=======================================================================

//=======================================================================
// Dflt ctor
// Default constructor gives identity mapping
//=======================================================================

mbl_thin_plate_spline_2d::mbl_thin_plate_spline_2d()
  : Wx_(0),Wy_(0),
    Ax0_(0),AxX_(1),AxY_(0),Ay0_(0),AyX_(0),AyY_(1),
    energy_x_(0),energy_y_(0),return_pure_affine_(false)
{
}

//=======================================================================
// Destructor
//=======================================================================

mbl_thin_plate_spline_2d::~mbl_thin_plate_spline_2d()
{
}
// First some useful maths functions

#if 0 // unused
inline double r2lnr(const vgl_point_2d<double>&  pt)
{
  double r2 = pt.x() * pt.x() + pt.y() * pt.y();
  if (r2>1e-8)   return 0.5 * r2 * vcl_log(r2);
  else     return 0;
}
#endif

inline double r2lnr(const vgl_vector_2d<double>&  pt)
{
  double r2 = pt.x() * pt.x() + pt.y() * pt.y();
  if (r2>1e-8)   return 0.5 * r2 * vcl_log(r2);
  else     return 0;
}

inline double r2lnr(double x, double y)
{
  double r2 = x * x + y * y;
  if (r2>1e-8) return 0.5 * r2 * vcl_log(r2);
  else      return 0;
}


// Sets L to be a symmetric square matrix of size n + 3 (n = pts.nelems)
// with L(i,j) = Uij = r2lnr(pts(i)-pts(j)) for i,j <= n
static void build_K_part(vnl_matrix<double>& L,
                         const vcl_vector<vgl_point_2d<double> >& pts)
{
  unsigned int n = pts.size();
  if ( (L.rows()!=n+3) | (L.columns()!=n+3) ) L.set_size(n+3,n+3);

  const vgl_point_2d<double> * pts_data = &pts[0];
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

// L is a (n+3) x (n+3) matrix;
// L = ( K  Q )
//     ( Q' 0 )
// Where K is n x n, K(i,j) = Uij = r2lnr(pts(i)-pts(j)) for i,j <= n
// and Q is ( 1 x0 y0 )
//          ( 1 x1 y1 )
//             . .  .
static void build_L(vnl_matrix<double>& L, const vcl_vector<vgl_point_2d<double> >& pts)
{
  int i,j;

  build_K_part(L,pts);

  int n = pts.size();

  const vgl_point_2d<double> * pts_data = &pts[0];
  double** L_data = L.data_array();

  // Build Q part

  for (i=0;i<n;i++)
  {
    L_data[i][n] = 1;
    L_data[i][n+1] = pts_data[i].x();
    L_data[i][n+2] = pts_data[i].y();

    L_data[n][i] = 1;
    L_data[n+1][i] = pts_data[i].x();
    L_data[n+2][i] = pts_data[i].y();
  }

  // put 0's in bottom left corner.
  for (i=n;i<n+3;i++)
    for (j=n;j<n+3;j++)
      L_data[i][j] = 0;
}

//: Build from small number of points
void mbl_thin_plate_spline_2d::build_pure_affine(
        const vcl_vector<vgl_point_2d<double> >& source_pts,
        const vcl_vector<vgl_point_2d<double> >& dest_pts)
{
  int n=source_pts.size();
  L_inv_.set_size(0,0);
  if (n==0)
  {
    // Set identity transformation
    Ax0_ = 0;
    Ay0_ = 0;
    AxX_ = 1; AxY_ = 0;
    AyX_ = 0; AyY_ = 1;

    Wx_.set_size(0);
    Wy_.set_size(0);

    src_pts_.resize(0);

    return;
  }

  if (n==1)
  {
    // Just apply a translation :
    Ax0_ = dest_pts[0].x() - source_pts[0].x();
    Ay0_ = dest_pts[0].y() - source_pts[0].y();
    Wx_.set_size(0);
    Wy_.set_size(0);
    AxX_ = 1; AxY_ = 0;
    AyX_ = 0; AyY_ = 1;
    src_pts_.resize(0);

    return;
  }

  if (n==2)
  {
    // Calculate trans, scaling & rotation required to
    // do mapping.

    // If points x1 & x2 get mapped to points u1 and u2
    // then the equation mapping a general point x into
    // u is
    //  u = (u1 - Rx1) + Rx
    // where R is the rotation matrix ( a -b )
    //                                ( b  a )
    // with a & b derived from the relative vectors in u & v
    // (see below for details)

    vgl_point_2d<double>  x1,x2,u1,u2;
    double dx,dy,du,dv;    // Points (x2-x1) and (u2-u1)
    double a,b;

    x1 = source_pts[0];
    x2 = source_pts[1];
    u1 = dest_pts[0];
    u2 = dest_pts[1];

    dx = x2.x() - x1.x();
    dy = x2.y() - x1.y();
    du = u2.x() - u1.x();
    dv = u2.y() - u1.y();

    double L2=(dx * dx + dy * dy);
    a = (dx * du + dy * dv)/L2;
    b = (dv * dx - du * dy)/L2;

    AxX_ = a;
    AxY_ = -b;
    AyX_ = b;
    AyY_ = a;
    Ax0_ = u1.x() - (a * x1.x() - b * x1.y());
    Ay0_ = u1.y() - (b * x1.x() + a * x1.y());

    Wx_.set_size(0);
    Wy_.set_size(0);
    src_pts_.resize(0);

    return;
  }


  if (n==3)
  {
    // Calculate the 6 affine parameters do mapping.

    // If points x0,x1 & x2 get mapped to points u0,u1 and u2
    // then the equation mapping a general point x into
    // u is
    //  u = (u0 - A.x0) + A.x
    // where A is the affine matrix ( a b )
    //                              ( c  d )

    vgl_point_2d<double>  x0,x1,x2,u0,u1,u2;

    x0 = source_pts[0];
    x1 = source_pts[1];
    x2 = source_pts[2];
    u0 = dest_pts[0];
    u1 = dest_pts[1];
    u2 = dest_pts[2];

    double dx1 = x1.x() - x0.x();
    double dy1 = x1.y() - x0.y();
    double dx2 = x2.x() - x0.x();
    double dy2 = x2.y() - x0.y();

    double du1 = u1.x() - u0.x();
    double dv1 = u1.y() - u0.y();
    double du2 = u2.x() - u0.x();
    double dv2 = u2.y() - u0.y();

    double L2=(dx1 * dy2 - dy1 * dx2);
    double a = (du1*dy2 - du2*dy1)/L2;
    double b = (du2*dx1 - du1*dx2)/L2;
    double c = (dv1*dy2 - dv2*dy1)/L2;
    double d = (dv2*dx1 - dv1*dx2)/L2;

    AxX_ = a;
    AxY_ = b;
    AyX_ = c;
    AyY_ = d;
    Ax0_ = u0.x() - (a * x0.x() + b * x0.y());
    Ay0_ = u0.y() - (c * x0.x() + d * x0.y());

    Wx_.set_size(0);
    Wy_.set_size(0);
    src_pts_.resize(0);

    return;
  }
}

//: Set parameters from vectors
void mbl_thin_plate_spline_2d::set_params(const vnl_vector<double>& W1,
                                          const vnl_vector<double>& W2)
{
  int n = W1.size()-3;

  if (int(Wx_.size()) < n) Wx_.set_size(n);
  if (int(Wy_.size()) < n) Wy_.set_size(n);

  double *Wx_data=Wx_.data_block();
  double *Wy_data=Wy_.data_block();
  const double *W1_data=W1.data_block();
  const double *W2_data=W2.data_block();

  for (int i=0;i<n;i++)
  {
    Wx_data[i] = W1_data[i];
    Wy_data[i] = W2_data[i];
  }

  Ax0_ = W1_data[n];
  AxX_ = W1_data[n+1];
  AxY_ = W1_data[n+2];

  Ay0_ = W2_data[n];
  AyX_ = W2_data[n+1];
  AyY_ = W2_data[n+2];
}

void mbl_thin_plate_spline_2d::compute_energy(vnl_vector<double>& W1,
                                              vnl_vector<double>& W2,
                                              const vnl_matrix<double>& L)

{
  int n = W1.size()-3;
  double *W1_data=W1.data_block();
  double *W2_data=W2.data_block();

  // Compute bending energy = W_t.K.W/(8pi)
  // Set last elements to zero
  W1_data[n]=0.0;
  W1_data[n+1]=0.0;
  W1_data[n+2]=0.0;
  vnl_vector<double> LW;
  mbl_matxvec_prod_mv(L,W1,LW);
  energy_x_ = dot_product(W1,LW) / (8*vnl_math::pi);

  // Set last elements to zero
  W2_data[n]=0.0;
  W2_data[n+1]=0.0;
  W2_data[n+2]=0.0;
  mbl_matxvec_prod_mv(L,W2,LW);
  energy_y_ = dot_product(W2,LW) / (8*vnl_math::pi);
}

void mbl_thin_plate_spline_2d::set_up_rhs(vnl_vector<double>& Bx,
                                          vnl_vector<double>& By,
                                          const vcl_vector<vgl_point_2d<double> >& dest_pts)
{
  int n =dest_pts.size();

  Bx.set_size(n+3);
  By.set_size(n+3);
  double* Bx_data=Bx.data_block();
  double* By_data=By.data_block();
  const vgl_point_2d<double>  *d_pts_data=&dest_pts[0];

  for (int i=0;i<n;i++)
  {
    Bx_data[i] = d_pts_data[i].x();
    By_data[i] = d_pts_data[i].y();
  }
  for (int i=n;i<n+3;i++)
  {
    Bx_data[i] = 0;
    By_data[i] = 0;
  }
}

void mbl_thin_plate_spline_2d::build(const vcl_vector<vgl_point_2d<double> >& source_pts,
                   const vcl_vector<vgl_point_2d<double> >& dest_pts,
                   bool compute_the_energy)
{
  // See Booksteins paper in IPMI 1993 for details of calculation

  unsigned int n=source_pts.size();
  if (dest_pts.size() != n)
  {
    vcl_cerr<<"mbl_thin_plate_spline_2d::build - incompatible number of points.\n";
    vcl_abort();
  }

  L_inv_.set_size(0,0);

  if (n<=3)
  {
    build_pure_affine(source_pts,dest_pts);
    return;
  }

  src_pts_ = source_pts;

  vnl_matrix<double> L;
  vnl_vector<double> Bx(n+3), W1(n+3);  // Used to compute X parameters
  vnl_vector<double> By(n+3), W2(n+3);  // Used to compute Y parameters

  build_L(L,source_pts);

  set_up_rhs(Bx,By,dest_pts);

  // Solve LW = B for W1 and W2 :
  // Note that both Cholesky and QR decompositions fail, apparently because of the
  // zeroes on the diagonal.  Use SVD to be safe.
  {
    vnl_svd<double> svd(L);
    svd.solve(Bx.data_block(),W1.data_block());
    svd.solve(By.data_block(),W2.data_block());
  }

  set_params(W1,W2);
  if (compute_the_energy)
    compute_energy(W1,W2,L);
}

//: Define source point positions
//  Performs pre-computations so that build(dest_points) can be
//  called multiple times efficiently
void mbl_thin_plate_spline_2d::set_source_pts(const vcl_vector<vgl_point_2d<double> >& source_pts)
{
  unsigned int n=source_pts.size();
  src_pts_ = source_pts;

  if (n<=3)
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
void mbl_thin_plate_spline_2d::build(const vcl_vector<vgl_point_2d<double> >& dest_pts)
{
  unsigned int n=src_pts_.size();
  if (dest_pts.size() != n)
  {
    vcl_cerr<<"mbl_thin_plate_spline_2d::build - incompatible number of points.\n";
    vcl_abort();
  }

  if (n<=3)
  {
    build_pure_affine(src_pts_,dest_pts);
    return;
  }

  vnl_vector<double> Bx(n+3), W1(n+3);  // Used to compute X parameters
  vnl_vector<double> By(n+3), W2(n+3);  // Used to compute Y parameters

  set_up_rhs(Bx,By,dest_pts);

  mbl_matxvec_prod_mv(L_inv_,Bx,W1);
  mbl_matxvec_prod_mv(L_inv_,By,W2);

  set_params(W1,W2);
  energy_x_ = -1;
  energy_y_ = -1;
}


vgl_point_2d<double>  mbl_thin_plate_spline_2d::operator()(double x, double y) const
{
  unsigned int n = src_pts_.size();

  double x_sum = Ax0_ + AxX_ * x + AxY_ * y;
  double y_sum = Ay0_ + AyX_ * x + AyY_ * y;

  if (n<=3 || return_pure_affine_)  // Pure affine
    return vgl_point_2d<double>(x_sum,y_sum);

  const vgl_point_2d<double> * pts_data = &src_pts_[0];
  const double* Wx_data = Wx_.data_block();
  const double* Wy_data = Wy_.data_block();

  for (unsigned int i=0;i<n;i++)
  {
    double Ui = r2lnr(x - pts_data[i].x(), y - pts_data[i].y() );
    x_sum += (Ui * Wx_data[i]);
    y_sum += (Ui * Wy_data[i]);
  }

  return vgl_point_2d<double>(x_sum,y_sum);
}

//=======================================================================
// Method: version_no
//=======================================================================

short mbl_thin_plate_spline_2d::version_no() const
{
  return 1;
}


//=======================================================================
// Method: print
//=======================================================================

  // required if data is present in this class
void mbl_thin_plate_spline_2d::print_summary(vcl_ostream& os) const
{
  os<<"\nfx: "<<Ax0_<<" + "<<AxX_<<"*x + "<<AxY_<<"*y   Nonlinear terms:";
  for (unsigned int i=0;i<Wx_.size();++i)
    os<<" "<<Wx_[i];
  os<<"\nfy: "<<Ay0_<<" + "<<AyX_<<"*x + "<<AyY_<<"*y   Nonlinear terms:";
  for (unsigned int i=0;i<Wy_.size();++i)
    os<<" "<<Wy_[i];
  os<<'\n';
}

//=======================================================================
// Method: save
//=======================================================================

  // required if data is present in this class
void mbl_thin_plate_spline_2d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,Wx_); vsl_b_write(bfs,Wy_);
  vsl_b_write(bfs,Ax0_); vsl_b_write(bfs,AxX_); vsl_b_write(bfs,AxY_);
  vsl_b_write(bfs,Ay0_); vsl_b_write(bfs,AyX_); vsl_b_write(bfs,AyY_);
  vsl_b_write(bfs,energy_x_); vsl_b_write(bfs,energy_y_);
  vsl_b_write(bfs,src_pts_);
  vsl_b_write(bfs,L_inv_);
}

//=======================================================================
// Method: load
//=======================================================================

  // required if data is present in this class
void mbl_thin_plate_spline_2d::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,Wx_);
      vsl_b_read(bfs,Wy_);
      vsl_b_read(bfs,Ax0_); vsl_b_read(bfs,AxX_); vsl_b_read(bfs,AxY_);
      vsl_b_read(bfs,Ay0_); vsl_b_read(bfs,AyX_); vsl_b_read(bfs,AyY_);
      vsl_b_read(bfs,energy_x_); vsl_b_read(bfs,energy_y_);
      vsl_b_read(bfs,src_pts_);
      vsl_b_read(bfs,L_inv_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, mbl_thin_plate_spline_2d &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Comparison operator
bool mbl_thin_plate_spline_2d::operator==(const mbl_thin_plate_spline_2d& tps) const
{
  if (&tps==this) return true;
  if (vcl_fabs(Ax0_-tps.Ax0_)>1e-8) return false;
  if (vcl_fabs(AxX_-tps.AxX_)>1e-8) return false;
  if (vcl_fabs(AxY_-tps.AxY_)>1e-8) return false;
  if (vcl_fabs(Ay0_-tps.Ay0_)>1e-8) return false;
  if (vcl_fabs(AyX_-tps.AyX_)>1e-8) return false;
  if (vcl_fabs(AyY_-tps.AyY_)>1e-8) return false;
  if (vnl_vector_ssd(Wx_,tps.Wx_)>1e-6) return false;
  if (vnl_vector_ssd(Wy_,tps.Wy_)>1e-6) return false;
  return true;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mbl_thin_plate_spline_2d& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mbl_thin_plate_spline_2d& b)
{
    b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mbl_thin_plate_spline_2d& b)
{
  os << "mbl_thin_plate_spline_2d: ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}
