// This is mul/mbl/mbl_histogram.cxx
#include "mbl_histogram.h"
//:
// \file
// \brief Simple object to build histogram from supplied data.
// \author Tim Cootes

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vsl/vsl_vector_io.h>

mbl_histogram::mbl_histogram()
{
  clear();
}

// Construct with given number of bins over given range
mbl_histogram::mbl_histogram(double x_lo, double x_hi, int n_bins)
{
  set_bins(x_lo,x_hi,n_bins);
}

//: Define number and size of bins
void mbl_histogram::set_bins(double xlo, double xhi, int n_bins)
{
  assert(n_bins>0);
  assert(xhi>xlo);

  bins_.resize(n_bins+1);
  freq_.resize(n_bins);

  double dx = (xhi-xlo)/n_bins;
  for (int i=0;i<=n_bins;++i) bins_[i]=xlo+i*dx;
  clear();
}

void mbl_histogram::clear()
{
  n_obs_ = 0;
  n_below_ = 0;
  n_above_ = 0;
  for (unsigned int i=0;i<freq_.size();++i) freq_[i]=0;
}

void mbl_histogram::obs(double v)
{
  n_obs_++;
  if (v<bins_[0])
  {
    n_below_++;
    return;
  }

  int n = freq_.size();

  for (int i=1;i<=n;++i)
  {
    if (v<bins_[i])
    {
      freq_[i-1]++;
      return;
    }
  }

  // Not in any bin
  n_above_++;
}


const double MAX_ERROR = 1.0e-8;

//: Test for equality
bool mbl_histogram::operator==(const mbl_histogram& s) const
{
  if (s.n_bins()!=n_bins()) return false;
  if (s.n_obs_ != n_obs_) return false;
  if (s.n_below_!=n_below_) return false;
  if (s.n_above_!=n_above_) return false;

  int n = n_bins();
  for (int i=0;i<n;++i)
    if (s.freq_[i]!=freq_[i]) return false;
  for (int i=0;i<=n;++i)
    if (vcl_fabs(s.bins_[i]-bins_[i])>MAX_ERROR) return false;

  return true;
}

//: Version number for I/O
short mbl_histogram::version_no() const
{
  return 1;
}

void mbl_histogram::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,n_obs_);
  vsl_b_write(bfs,n_below_);
  vsl_b_write(bfs,n_above_);
  vsl_b_write(bfs,bins_);
  vsl_b_write(bfs,freq_);
}

void mbl_histogram::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short file_version_no;
  vsl_b_read(bfs,file_version_no);

  switch (file_version_no)
  {
   case 1:
    vsl_b_read(bfs,n_obs_);
    vsl_b_read(bfs,n_below_);
    vsl_b_read(bfs,n_above_);
    vsl_b_read(bfs,bins_);
    vsl_b_read(bfs,freq_);
    break;
   default:
    vcl_cerr << "I/O ERROR: mbl_histogram::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< file_version_no << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

void mbl_histogram::print_summary(vcl_ostream& os) const
{
  os << "mbl_histogram: ";
  if (n_bins()==0) { os<< "No bins defined."; return; }

  int n = n_bins();

  if (n_obs_==0)
    os << "No samples.";
  else
  {
    os << n_obs_ << " observations.\n"
       << "    < "<<bins_[0]<<"   "<<n_below_<<vcl_endl;
    for (int i=0;i<n;++i)
      os<<"  ["<<bins_[i]<<','<<bins_[i+1]<<")  "<<freq_[i]<<vcl_endl;
    os << "   >= "<<bins_[n]<<"   "<<n_above_<<vcl_endl;
  }
}

//: Write out histogram probabilities to a named file
//  Format: (bin-centre) prob     (one per line)
// \return true if successful
bool mbl_histogram::write_probabilities(const char* path)
{
  int n = n_bins();
  if (n==0) return false;

  vcl_ofstream ofs(path);
  if (!ofs) return false;
  for (int i=0;i<n_bins();++i)
  {
    ofs<<0.5*(bins_[i]+bins_[i+1])<<"  "<<double(freq_[i])/n_obs_<<vcl_endl;
  }
  ofs.close();
  return true;
}

vcl_ostream& operator<<(vcl_ostream& os, const mbl_histogram& histo)
{
  histo.print_summary(os);
  return os;
}

  //: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const mbl_histogram& histo)
{
  histo.print_summary(os);
}

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_histogram& h)
{
  h.b_write(bfs);
}

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_histogram& h)
{
  h.b_read(bfs);
}
