#include "mfpf_sad_vec_cost.h"
// :
// \file
// \brief Computes weighted sum of absolute differences to a mean
// \author Tim Cootes

#include <vsl/vsl_binary_loader.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>

#include <vnl/io/vnl_io_vector.h>

// =======================================================================
// Dflt ctor
// =======================================================================

mfpf_sad_vec_cost::mfpf_sad_vec_cost()
{
}

// =======================================================================
// Destructor
// =======================================================================

mfpf_sad_vec_cost::~mfpf_sad_vec_cost()
{
}

// : Define mean and weights
void mfpf_sad_vec_cost::set(const vnl_vector<double>& mean,
                            const vnl_vector<double>& wts)
{
  assert(mean.size() == wts.size() );
  mean_ = mean;
  wts_ = wts;
  if( wts.mean() > 9999 )
    {
    vcl_cerr << "Mean weights excessive: " << wts.mean() << vcl_endl;
    vcl_abort();
    }
}

// : Evaluate weighted sum of absolute difference from mean
double mfpf_sad_vec_cost::evaluate(const vnl_vector<double>& v)
{
  double sum = 0;

  for( unsigned i = 0; i < v.size(); ++i )
    {
    sum += wts_[i] * vcl_fabs(v[i] - mean_[i]);
    }
  return sum;
}

// : Return the mean
void mfpf_sad_vec_cost::get_average(vnl_vector<double>& v) const
{
  v = mean_;
}

// =======================================================================
// Method: version_no
// =======================================================================

short mfpf_sad_vec_cost::version_no() const
{
  return 1;
}

// =======================================================================
// Method: is_a
// =======================================================================

vcl_string mfpf_sad_vec_cost::is_a() const
{
  return vcl_string("mfpf_sad_vec_cost");
}

// : Create a copy on the heap and return base class pointer
mfpf_vec_cost * mfpf_sad_vec_cost::clone() const
{
  return new mfpf_sad_vec_cost(*this);
}

// =======================================================================
// Method: print
// =======================================================================

void mfpf_sad_vec_cost::print_summary(vcl_ostream& os) const
{
  os << "Size: " << mean_.size()
     << " mean.sum():" << mean_.sum()
     << " wts.sum():" << wts_.sum();
}

// =======================================================================
// Method: save
// =======================================================================

void mfpf_sad_vec_cost::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no() );
  vsl_b_write(bfs, mean_);
  vsl_b_write(bfs, wts_);
}

// =======================================================================
// Method: load
// =======================================================================

void mfpf_sad_vec_cost::b_read(vsl_b_istream& bfs)
{
  if( !bfs ) {return; }
  short version;
  vsl_b_read(bfs, version);

  switch( version )
    {
    case (1):
      vsl_b_read(bfs, mean_);
      vsl_b_read(bfs, wts_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number " << version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
}
