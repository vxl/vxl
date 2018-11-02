// This is mul/mbl/mbl_clamped_plate_spline_2d.cxx
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "mbl_clamped_plate_spline_2d.h"
//:
// \file
// \brief Construct thin plate spline to map 2D to 2D
// \author Tim Cootes

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/algo/vnl_svd.h>
#include <mbl/mbl_matxvec.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vgl/io/vgl_io_point_2d.h>

//=======================================================================

//=======================================================================
// Dflt ctor
// Default constructor gives identity mapping
//=======================================================================

mbl_clamped_plate_spline_2d::mbl_clamped_plate_spline_2d()
  : Wx_(0),Wy_(0)
{
}

//=======================================================================
// Destructor
//=======================================================================

mbl_clamped_plate_spline_2d::~mbl_clamped_plate_spline_2d() = default;

//: Check that all points are inside unit circle
bool mbl_clamped_plate_spline_2d::all_in_unit_circle(const std::vector<vgl_point_2d<double> >& pts)
{
  int n = pts.size();
  const vgl_point_2d<double> *p = &pts[0];
  for (int i=0;i<n;++i)
  {
    if (p[i].x()*p[i].x()+p[i].y()*p[i].y()>1.0) return false;
  }

  return true;
}

//: Green's function for the clamped plate spline
//  0.5*|x-y|^2(A2-log(A2)-1)  where A2 = (|x|^2|y|^2-2x.y+1)/|x-y|^2
//  If two points are x and y
inline double cps_green(const vgl_point_2d<double>&  p1, const vgl_point_2d<double>& p2)
{
  double dx = p1.x()-p2.x();
  double dy = p1.y()-p2.y();
  double d2 = (dx*dx+dy*dy);
  if (d2<1e-8)   return 0.5;  // Avoid numeric instability near zero

  double L1 = p1.x() * p1.x() + p1.y() * p1.y();
  double L2 = p2.x() * p2.x() + p2.y() * p2.y();
  double d  = p1.x() * p2.x() + p1.y() * p2.y();  // Dot product
  double A2 = (L1*L2-2*d+1)/d2;

  return 0.5*d2*(A2-std::log(A2)-1);
}

//: Green's function for the clamped plate spline
//  0.5*|x-y|^2(A2-log(A2)-1)  where A2 = (|x|^2|y|^2-2x.y+1)/|x-y|^2
//  If two points are x and y
inline double cps_green(double x, double y, const vgl_point_2d<double>& p2)
{
  double dx = x-p2.x();
  double dy = y-p2.y();
  double d2 = (dx*dx+dy*dy);
  if (d2<1e-8)   return 0.5;  // Avoid numeric instability near zero

  double L1 = x * x + y * y;
  double L2 = p2.x() * p2.x() + p2.y() * p2.y();
  double d  = x * p2.x() + y * p2.y();  // Dot product
  double A2 = (L1*L2-2*d+1)/d2;

  return 0.5*d2*(A2-std::log(A2)-1);
}

// Sets L to be a symmetric square matrix of size n x n (n = pts.nelems)
// with L(i,j) = cps_green(pts[i],pts[j])
static void build_L(vnl_matrix<double>& L,
                    const std::vector<vgl_point_2d<double> >& pts)
{
  unsigned int n = pts.size();
  if ( (L.rows()!=n) || (L.columns()!=n) ) L.set_size(n,n);

  const vgl_point_2d<double> * pts_data = &pts[0];
  double** K_data = L.data_array();

    // Set the diagonal
  for (unsigned int i=0;i<n;i++)
    K_data[i][i] = 0.5;
    // Now fill upper & lower triangles
  for (unsigned int i=1;i<n;i++)
    for (unsigned int j=0;j<i;j++)
    {
      K_data[i][j] = K_data[j][i] = cps_green(pts_data[i],pts_data[j]);
    }
}

//: Set parameters from vectors
void mbl_clamped_plate_spline_2d::set_params(const vnl_vector<double>& Wx,
                                             const vnl_vector<double>& Wy)
{
  Wx_ = Wx;
  Wy_ = Wy;
}

void mbl_clamped_plate_spline_2d::set_up_rhs(vnl_vector<double>& Bx,
                                             vnl_vector<double>& By,
                                             const std::vector<vgl_point_2d<double> >& src_pts,
                                             const std::vector<vgl_point_2d<double> >& dest_pts)
{
  int n =dest_pts.size();

  Bx.set_size(n);
  By.set_size(n);
  double* Bx_data=Bx.data_block();
  double* By_data=By.data_block();
  const vgl_point_2d<double>  *d_pts_data=&dest_pts[0];
  const vgl_point_2d<double>  *s_pts_data=&src_pts[0];

  for (int i=0;i<n;i++)
  {
    Bx_data[i] = d_pts_data[i].x()-s_pts_data[i].x();
    By_data[i] = d_pts_data[i].y()-s_pts_data[i].y();
  }
}

void mbl_clamped_plate_spline_2d::build(const std::vector<vgl_point_2d<double> >& source_pts,
                                        const std::vector<vgl_point_2d<double> >& dest_pts)
{
  assert(all_in_unit_circle(source_pts));
  assert(all_in_unit_circle(dest_pts));

  unsigned int n=source_pts.size();
  if (dest_pts.size() != n)
  {
    std::cerr<<"mbl_clamped_plate_spline_2d::build - incompatible number of points.\n";
    std::abort();
  }

  L_inv_.set_size(0,0);

  src_pts_ = source_pts;

  vnl_matrix<double> L;
  vnl_vector<double> Bx(n);  // Used to compute X parameters
  vnl_vector<double> By(n);  // Used to compute Y parameters

  Wx_.set_size(n);
  Wy_.set_size(n);

  build_L(L,source_pts);

  set_up_rhs(Bx,By,source_pts,dest_pts);

  // Solve LW = B for W1 and W2 :
  // Note that both Cholesky and QR decompositions fail, apparently because of the
  // zeroes on the diagonal.  Use SVD to be safe.
  {
    vnl_svd<double> svd(L);
    svd.solve(Bx.data_block(),Wx_.data_block());
    svd.solve(By.data_block(),Wy_.data_block());
  }
}

//: Define source point positions
//  Performs pre-computations so that build(dest_points) can be
//  called multiple times efficiently
void mbl_clamped_plate_spline_2d::set_source_pts(const std::vector<vgl_point_2d<double> >& source_pts)
{
  assert(all_in_unit_circle(source_pts));

  src_pts_ = source_pts;

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
void mbl_clamped_plate_spline_2d::build(const std::vector<vgl_point_2d<double> >& dest_pts)
{
  assert(all_in_unit_circle(dest_pts));

  unsigned int n=src_pts_.size();
  if (dest_pts.size() != n)
  {
    std::cerr<<"mbl_clamped_plate_spline_2d::build - incompatible number of points.\n";
    std::abort();
  }

  vnl_vector<double> Bx(n);  // Used to compute X parameters
  vnl_vector<double> By(n);  // Used to compute Y parameters

  Wx_.set_size(n);
  Wy_.set_size(n);

  set_up_rhs(Bx,By,src_pts_,dest_pts);

  mbl_matxvec_prod_mv(L_inv_,Bx,Wx_);
  mbl_matxvec_prod_mv(L_inv_,By,Wy_);
}


vgl_point_2d<double> mbl_clamped_plate_spline_2d::operator()(double x, double y) const
{
  unsigned int n = src_pts_.size();

  double x_sum = x;
  double y_sum = y;

  const vgl_point_2d<double> * pts_data = &src_pts_[0];
  const double* Wx_data = Wx_.data_block();
  const double* Wy_data = Wy_.data_block();

  for (unsigned int i=0;i<n;i++)
  {
    double Ui = cps_green(x,y,pts_data[i]);
    x_sum += (Ui * Wx_data[i]);
    y_sum += (Ui * Wy_data[i]);
  }

  return {x_sum,y_sum};
}

//=======================================================================
// Method: version_no
//=======================================================================

short mbl_clamped_plate_spline_2d::version_no() const
{
  return 1;
}


//=======================================================================
// Method: print
//=======================================================================

// required if data is present in this class
void mbl_clamped_plate_spline_2d::print_summary(std::ostream& os) const
{
  os<<"\nfx:";
  for (double i : Wx_)
    os<<" "<<i;
  os<<"\nfy:";
  for (double i : Wy_)
    os<<" "<<i;
  os<<'\n';
}

//=======================================================================
// Method: save
//=======================================================================

  // required if data is present in this class
void mbl_clamped_plate_spline_2d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,Wx_); vsl_b_write(bfs,Wy_);
  vsl_b_write(bfs,src_pts_);
  vsl_b_write(bfs,L_inv_);
}

//=======================================================================
// Method: load
//=======================================================================

  // required if data is present in this class
void mbl_clamped_plate_spline_2d::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,Wx_);
      vsl_b_read(bfs,Wy_);
      vsl_b_read(bfs,src_pts_);
      vsl_b_read(bfs,L_inv_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, mbl_clamped_plate_spline_2d &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//: Comparison operator
bool mbl_clamped_plate_spline_2d::operator==(const mbl_clamped_plate_spline_2d& tps) const
{
  if (&tps==this) return true;
  if (vnl_vector_ssd(Wx_,tps.Wx_)>1e-6) return false;
  if (vnl_vector_ssd(Wy_,tps.Wy_)>1e-6) return false;
  return true;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mbl_clamped_plate_spline_2d& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mbl_clamped_plate_spline_2d& b)
{
    b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mbl_clamped_plate_spline_2d& b)
{
  os << "mbl_clamped_plate_spline_2d: ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}
