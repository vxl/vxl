// This is mul/mbl/mbl_rbf_network.cxx
#include <iostream>
#include <cstdlib>
#include "mbl_rbf_network.h"
//:
// \file
// \brief A class to perform some of the functions of a Radial Basis Function Network.
// \author Tim Cootes
//
//  Given a set of n training vectors, x_i (i=0..n-1), a set of internal weights are computed.
//  Given a new vector, x, a vector of weights, w, are computed such that
//  if x = x_i then w(i+1) = 1, w(j !=i+1) = 0  The sum of the weights
//  should always be unity.
//  If x is not equal to any training vector, the vector of weights varies
//  smoothly.  This is useful for interpolation purposes.
//  It can also be used to define non-linear transformations between
//  vector spaces.  If Y is a matrix of n columns, each corresponding to
//  a vector in a new space which corresponds to one of the original
//  training vectors x_i, then a vector x can be mapped to Yw in the
//  new space.  (Note: y-space does not have to have the same dimension
//  as x space). This class is equivalent to
//  the basis of thin-plate spline warping.
//
//  I'm not sure if this is exactly an RBF network in the original
//  definition. I'll check one day.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_indent.h>
#include <mbl/mbl_stats_1d.h>
#include <vnl/algo/vnl_svd.h>
#include <mbl/mbl_matxvec.h>
#include <vnl/io/vnl_io_vector.h>
#include <vsl/vsl_vector_io.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mbl_rbf_network::mbl_rbf_network()
{
  sum_to_one_ = true;
}

//: Build weights given examples x.
//  s gives the scaling to use in r2 * std::log(r2) r2 = distSqr/(s*s)
//  If s<=0 then a suitable s is estimated from the data
void mbl_rbf_network::build(const std::vector<vnl_vector<double> >& x, double s)
{
  int n = x.size();
  build(&(x.front()),n,s);
}

//: Build weights given n examples x[0] to x[n-1].
//  s gives the scaling to use in r2 * std::log(r2) r2 = distSqr/(s*s)
//  If s<=0 then a suitable s is estimated from the data
void mbl_rbf_network::build(const vnl_vector<double>* x, int n, double s)
{
  assert (n>0);
  // Copy training examples
  if (x_.size()!=(unsigned)n) x_.resize(n);
  for (int i=0;i<n;++i)
    x_[i] = x[i];

  // Compute distances
  vnl_matrix<double> D(n,n);
  double **D_data = D.data_array();

  mbl_stats_1d d2_stats;

  for (int i=0;i<n;++i)
    D(i,i)=0.0;

  for (int i=0;i<n-1;++i)
    for (int j=i+1;j<n;++j)
    {
      double d2 = distSqr(x_[i],x_[j]);
      D_data[i][j] = d2;
      D_data[j][i] = d2;
      d2_stats.obs(d2);
    }

  if (s<=0)
  {
    s2_ = d2_stats.min();
  }
  else
    s2_ = s*s;

  // Apply rbf() to elements of D
  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j)
      D_data[i][j] = rbf(D_data[i][j]/s2_);

  // W_ is the inverse of D
  vnl_svd<double> svd(D);
  W_ = svd.inverse();
}

double mbl_rbf_network::distSqr(const vnl_vector<double>& x, const vnl_vector<double>& y) const
{
  unsigned int n = x.size();
  if (y.size()!=n)
  {
    std::cerr<<"mbl_rbf_network::distSqr() x and y different sizes.\n";
    std::abort();
  }

  const double *x_data = x.begin();
  const double *y_data = y.begin();
  double sum = 0.0;
  for (unsigned int i=0;i<n;++i)
  {
    double dx = x_data[i]-y_data[i];
    sum += dx*dx;
  }

  return sum;
}

//: Set flag.  If false, calcWts returns raw weights
void mbl_rbf_network::setSumToOne(bool flag)
{
  sum_to_one_ = flag;
}


//: Compute weights for given new_x.
//  If new_x = x()(i) then w(i+1)==1, w(j!=i+1)==0
//  Otherwise w varies smoothly depending on distance
//  of new_x from x()'s
//  If sumToOne() then elements of w will sum to 1.0
//  otherwise they will sum to <=1.0, decreasing as new_x
//  moves away from the training examples x().
void mbl_rbf_network::calcWts(vnl_vector<double>& w, const vnl_vector<double>& new_x)
{
  unsigned int n = x_.size();
  if (w.size()!=n) w.set_size(n);
  if (v_.size()!=n) v_.set_size(n);

  double* v_data = &v_[0];
  const vnl_vector<double>* x_data = &x_[0];

  if (n==1)
  {
    w(0)=1.0;
    return;
  }

  if (n==2)
  {
    // Use linear interpolation based on distance.
    double d0 = std::sqrt(distSqr(new_x,x_data[0]));
    double d1 = std::sqrt(distSqr(new_x,x_data[1]));
    w(0) = d1/(d0+d1);
    w(1) = 1.0 - w(0);
    return;
  }

  for (unsigned int i=0;i<n;++i)
  {
    v_data[i] = rbf(new_x,x_data[i]);
  }

  mbl_matxvec_prod_mv(W_,v_,w);

  if (sum_to_one_)
  {
    double sum = w.sum();
    if (sum!=0) w/=sum;
  }
}

//=======================================================================
// Method: version_no
//=======================================================================

short mbl_rbf_network::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mbl_rbf_network::is_a() const
{
  return std::string("mbl_rbf_network");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool mbl_rbf_network::is_class(std::string const& s) const
{
  return s==is_a();
}

//=======================================================================
// Method: print
//=======================================================================

// required if data is present in this class
void mbl_rbf_network::print_summary(std::ostream& os) const
{
  os << "Built with "<<x_.size()<<" examples.";
  //  os << x_ << '\n' << W_ << '\n' << s2_<< '\n';
}

//=======================================================================
// Method: save
//=======================================================================

// required if data is present in this class
void mbl_rbf_network::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,x_);
  vsl_b_write(bfs,W_);
  vsl_b_write(bfs,s2_);

  if (sum_to_one_)
    vsl_b_write(bfs,short(1));
  else
    vsl_b_write(bfs,short(0));
}

//=======================================================================
// Method: load
//=======================================================================

// required if data is present in this class
void mbl_rbf_network::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  short flag;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs,x_);
    vsl_b_read(bfs,W_);
    vsl_b_read(bfs,s2_);
    vsl_b_read(bfs,flag);  sum_to_one_ = (flag!=0);
    break;
  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, mbl_rbf_network &)\n"
             << "           Unknown version number "<< version << std::endl;
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mbl_rbf_network& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mbl_rbf_network& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mbl_rbf_network& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}
