//:
// \file
// \brief Simplest possible 1D classifier: A single thresholding function
// \author Tim Cootes

//=======================================================================

#include "clsfy_mean_square_1d.h"
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>
#include <vnl/vnl_double_2.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>

//=======================================================================

//: Find the posterior probability of the input being in the positive class.
// The result is is outputs(0)
void clsfy_mean_square_1d::class_probabilities(vcl_vector<double> &outputs,
                                               double input) const
{
  outputs.resize(1);
  outputs[0] = 1.0 / (1.0 + vcl_exp(-log_l(input)));
}


//=======================================================================

//: Log likelihood of being in the positive class.
// Class probability = 1 / (1+exp(-log_l))
double clsfy_mean_square_1d::log_l(double input) const
{
  double k= mean_-input;
  return k*k; // - threshold_ ;  //ie distance above threshold!
                                 // this isn't a loglikelihood, as lower value
                                 // indicates more likely to be pos example
}

//: Return parameters defining classifier in a vector (format depends on classifier)
vnl_vector<double> clsfy_mean_square_1d::params() const
{
  return vnl_double_2(mean_,threshold_).as_vector();
}

//: Set parameters defining classifier with a vector (format depends on classifier)
void clsfy_mean_square_1d::set_params(const vnl_vector<double>& p)
{
  assert(p.size()==2);
  mean_=p[0];
  threshold_=p[1];
}

//=======================================================================

//: Equality operator for 1d classifiers
bool clsfy_mean_square_1d::operator==(const clsfy_classifier_1d& x) const
{
  assert( x.is_class("clsfy_mean_square_1d"));
  clsfy_mean_square_1d& x2= (clsfy_mean_square_1d&) x;
  return vcl_fabs(x2.mean_ - mean_) < 0.001 &&
         vcl_fabs(x2.threshold_ - threshold_) < 0.001;
}


vcl_string clsfy_mean_square_1d::is_a() const
{
  return vcl_string("clsfy_mean_square_1d");
}

bool clsfy_mean_square_1d::is_class(vcl_string const& s) const
{
  return s == clsfy_mean_square_1d::is_a() || clsfy_classifier_1d::is_class(s);
}

//=======================================================================

// required if data is present in this class
void clsfy_mean_square_1d::print_summary(vcl_ostream& os) const
{
  os << "mean: " << mean_ << "   threshold: "<<threshold_<<vcl_endl;
}

//=======================================================================

short clsfy_mean_square_1d::version_no() const
{
  return 1;
}

//=======================================================================

void clsfy_mean_square_1d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,mean_);
  vsl_b_write(bfs,threshold_);
}

//=======================================================================

void clsfy_mean_square_1d::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,mean_);
      vsl_b_read(bfs,threshold_);
      break;
    default:
      vcl_cerr << "I/O ERROR: clsfy_mean_square_1d::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const clsfy_mean_square_1d& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, clsfy_mean_square_1d& b)
{
  b.b_read(bfs);
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const clsfy_mean_square_1d& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const clsfy_mean_square_1d& b)
{
  vsl_print_summary(os,b);
  return os;
}
