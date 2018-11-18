//:
// \file
// \author Tim Cootes
// \brief No limits applied to parameters

#include "msm_no_limiter.h"
#include <vnl/io/vnl_io_vector.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================
msm_no_limiter::msm_no_limiter() = default;

//: Define number of SDs to limit at
void msm_no_limiter::set_n_sds(double)
{
}

//: Define variance on each parameter
void msm_no_limiter::set_param_var(const vnl_vector<double>&)
{
}

//: Set the limits so that a given proportion pass
//  Does nothing.
void msm_no_limiter::set_acceptance(double, unsigned)
{
}

//: Apply limit to parameter vector b (does nothing)
void msm_no_limiter::apply_limit(vnl_vector<double>&) const
{
}

//=======================================================================
//: Print class to os
void msm_no_limiter::print_summary(std::ostream& os) const
{
  os << "{ }";
}

constexpr static short version_no = 1;

//: Save class to binary file stream
void msm_no_limiter::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no);
}


//: Load class from binary file stream
void msm_no_limiter::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      break;
    default:
      std::cerr << "msm_no_limiter::b_read() :\n"
               << "Unexpected version number " << version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

std::string msm_no_limiter::is_a() const
{
  return std::string("msm_no_limiter");
}

//: Create a copy on the heap and return base class pointer
msm_param_limiter* msm_no_limiter::clone() const
{
  return new msm_no_limiter(*this);
}
