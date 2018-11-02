#include "mfpf_log_lin_class_cost.h"
//:
// \file
// \brief Computes log prob based on output of a linear classifier
// \author Tim Cootes

#include <vsl/vsl_binary_loader.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/io/vnl_io_vector.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_log_lin_class_cost::mfpf_log_lin_class_cost() = default;

//=======================================================================
// Destructor
//=======================================================================

mfpf_log_lin_class_cost::~mfpf_log_lin_class_cost() = default;

//: Define weights, bias and minp
void mfpf_log_lin_class_cost::set(const vnl_vector<double>& wts,
                                  double bias, double min_p)
{
  wts_ =wts;
  bias_=bias;
  min_p_=min_p;
}

//: Returns -1*log(minp + (1-minp)/(1+exp(-(x.wts-bias)))
double mfpf_log_lin_class_cost::evaluate(const vnl_vector<double>& x)
{
  double z = bias_-dot_product(wts_,x);
  return -1*std::log(min_p_+(1-min_p_)/(1+std::exp(z)));
}

//: Return the weights
void mfpf_log_lin_class_cost::get_average(vnl_vector<double>& v) const
{
  v=wts_;
}


//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_log_lin_class_cost::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mfpf_log_lin_class_cost::is_a() const
{
  return std::string("mfpf_log_lin_class_cost");
}

//: Create a copy on the heap and return base class pointer
mfpf_vec_cost* mfpf_log_lin_class_cost::clone() const
{
  return new mfpf_log_lin_class_cost(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_log_lin_class_cost::print_summary(std::ostream& os) const
{
  os<<"Size: "<<wts_.size();
}

//=======================================================================
// Method: save
//=======================================================================

void mfpf_log_lin_class_cost::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,wts_);
  vsl_b_write(bfs,bias_);
  vsl_b_write(bfs,min_p_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_log_lin_class_cost::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,wts_);
      vsl_b_read(bfs,bias_);
      vsl_b_read(bfs,min_p_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
