//:
// \file
// \brief Simplest possible 1D classifier: A single thresholding function
// \author Tim Cootes

//=======================================================================

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/io/vnl_io_vector.h>
#include <mbl/mbl_matxvec.h>
#include <mbl/mbl_matrix_products.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <clsfy/clsfy_binary_threshold_1d.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vsl/vsl_binary_io.h>

//=======================================================================

//: Find the posterior probability of the input being in the positive class.
// The result is is outputs(0)
void clsfy_binary_threshold_1d::class_probabilities(vcl_vector<double> &outputs,
  double input) const
{
  outputs.resize(1);
  outputs[0] = 1.0 / (1.0 + vcl_exp(-log_l(input)));
}


//=======================================================================

//: Log likelyhood of being in the positive class.
// Class probability = 1 / (1+exp(-log_l))
double clsfy_binary_threshold_1d::log_l(double input) const
{
  return s_*input - threshold_;
}

//: Return parameters defining classifier in a vector (format depends on classifier)
vnl_vector<double> clsfy_binary_threshold_1d::params() const
{
  vnl_vector<double> p(2);
  p[0] = s_;
  p[1] = threshold_;
  return p;
}

//: Set parameters defining classifier with a vector (format depends on classifier)
void clsfy_binary_threshold_1d::set_params(const vnl_vector<double>& p)
{
  assert(p.size()==2);
  s_=p[0];
  threshold_=p[1];
}

//=======================================================================

//: Equality operator for 1d classifiers
bool clsfy_binary_threshold_1d::operator==(const clsfy_classifier_1d& x) const
{
  assert( x.is_a()== "clsfy_binary_threshold_1d");
  clsfy_binary_threshold_1d& x2= (clsfy_binary_threshold_1d&) x;
  return (x2.s_ == s_ &&
          x2.threshold_ == threshold_      
    ); 
}


vcl_string clsfy_binary_threshold_1d::is_a() const
{
  return vcl_string("clsfy_binary_threshold_1d");
}

//=======================================================================

// required if data is present in this class
void clsfy_binary_threshold_1d::print_summary(vcl_ostream& os) const
{
  os << "s: " << s_ << "   threshold: "<<threshold_<<vcl_endl;
}

//=======================================================================

short clsfy_binary_threshold_1d::version_no() const
{
  return 1;
}

//=======================================================================

void clsfy_binary_threshold_1d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,s_);
  vsl_b_write(bfs,threshold_);
}

//=======================================================================

void clsfy_binary_threshold_1d::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,s_);
      vsl_b_read(bfs,threshold_);
      break;
    default:
      vcl_cerr << "I/O ERROR: clsfy_binary_threshold_1d::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << "\n";
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}

