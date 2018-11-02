#ifndef mbl_rbf_network_h_
#define mbl_rbf_network_h_
//:
// \file
// \brief A class to perform some of the functions of a Radial Basis Function Network
// \author tfc
//         wondrous VXL conversion started by gvw, errors corrected by ...

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <iosfwd>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A class to perform some of the functions of a Radial Basis Function Network.
//  This is a special case of a mixture model pdf, where the same
//  (radially symmetric) pdf kernel is used at each node.
//  The nodes are supplied by build().
//  calcWts(w,x) calculates the probabilities that x belongs to each
//  node.
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
class mbl_rbf_network
{
  std::vector<vnl_vector<double> > x_;
  vnl_matrix<double> W_;
  double s2_;

  bool sum_to_one_;

  //: workspace
  vnl_vector<double> v_;

  double distSqr(const vnl_vector<double>& x, const vnl_vector<double>& y) const;
  double rbf(double r2) const
    { return r2<=0.0 ? 1.0 : std::exp(-r2); }

  double rbf(const vnl_vector<double>& x, const vnl_vector<double>& y)
    { return rbf(distSqr(x,y)/s2_); }

 public:

  //: Dflt ctor
  mbl_rbf_network();

  //: Build weights given examples x.
  //  s gives the scaling to use in r2 * std::log(r2) r2 = distSqr/(s*s)
  //  If s<=0 then a suitable s is estimated from the data
  void build(const std::vector<vnl_vector<double> >& x, double s = -1);

  //: Build weights given n examples x[0] to x[n-1].
  //  s gives the scaling to use in r2 * std::log(r2) r2 = distSqr/(s*s)
  //  If s<=0 then a suitable s is estimated from the data
  void build(const vnl_vector<double>* x, int n, double s = -1);

  //: If true, then the returned weights sum to 1.0
  bool sumToOne() const { return sum_to_one_; }

  //: Set flag.  If false, calcWts returns raw weights
  void setSumToOne(bool flag);

  //: Array of training vectors x, supplied in last build()
  const std::vector<vnl_vector<double> >& x() const { return x_;}

  //: Compute weights for given new_x.
  //  If new_x = x()(i) then w(i+1)==1, w(j!=i+1)==0
  //  Otherwise w varies smoothly depending on distance
  //  of new_x from x()'s
  //  If sumToOne() then elements of w will sum to 1.0
  //  otherwise they will sum to <=1.0, decreasing as new_x
  //  moves away from the training examples x().
  void calcWts(vnl_vector<double>& w, const vnl_vector<double>& new_x);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const;

  //: True if this is (or is derived from) class named s
  bool is_class(std::string const& s) const;

  //: Print class to os
  void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_rbf_network& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_rbf_network& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mbl_rbf_network& b);

#endif //mbl_rbf_network_h_
