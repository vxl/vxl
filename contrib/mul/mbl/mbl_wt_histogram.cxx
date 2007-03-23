// This is mul/mbl/mbl_wt_histogram.cxx
#include "mbl_wt_histogram.h"
//:
// \file
// \brief Simple object to build histogram from supplied data, with weights
// \author Tim Cootes

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vsl/vsl_vector_io.h>

mbl_wt_histogram::mbl_wt_histogram()
{
  clear();
}

// Construct with given number of bins over given range
mbl_wt_histogram::mbl_wt_histogram(double x_lo, double x_hi, int n_bins)
{
  set_bins(x_lo,x_hi,n_bins);
}

//: Define number and size of bins
void mbl_wt_histogram::set_bins(double xlo, double xhi, int n_bins)
{
  assert(n_bins>0);
  assert(xhi>xlo);

  wt_sum_.resize(n_bins);

  dx_ = (xhi-xlo)/n_bins;
  xlo_ = xlo;
  clear();
}

void mbl_wt_histogram::clear()
{
  n_obs_ = 0;
  total_wt_=0.0;
  wt_below_ = 0;
  wt_above_ = 0;
  for (unsigned int i=0;i<wt_sum_.size();++i) wt_sum_[i]=0;
}

void mbl_wt_histogram::obs(double v, double wt)
{
  n_obs_++;
  total_wt_ += wt;
  if (v<xlo_)
  {
    wt_below_+=wt;
    return;
  }

  // v-xlo_ >= 0
  unsigned int i = (unsigned int)((v-xlo_)/dx_);

  if (i<wt_sum_.size()) wt_sum_[i]+=wt;
  else                  wt_above_+=wt;
}


const double MAX_ERROR = 1.0e-8;

//: Test for equality
bool mbl_wt_histogram::operator==(const mbl_wt_histogram& s) const
{
  if (s.n_bins()!=n_bins()) return false;
  if (s.n_obs_ != n_obs_) return false;
  if (vcl_fabs(s.wt_below_-wt_below_)>MAX_ERROR) return false;
  if (vcl_fabs(s.wt_above_-wt_above_)>MAX_ERROR) return false;
  if (vcl_fabs(s.xlo_-xlo_)>MAX_ERROR) return false;
  if (vcl_fabs(s.dx_-dx_)>MAX_ERROR) return false;

  int n = n_bins();
  for (int i=0;i<n;++i)
    if (vcl_fabs(s.wt_sum_[i]-wt_sum_[i])>MAX_ERROR) return false;

  return true;
}

//: Version number for I/O
short mbl_wt_histogram::version_no() const
{
  return 1;
}

void mbl_wt_histogram::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,n_obs_);
  vsl_b_write(bfs,total_wt_);
  vsl_b_write(bfs,wt_below_);
  vsl_b_write(bfs,wt_above_);
  vsl_b_write(bfs,xlo_);
  vsl_b_write(bfs,dx_);
  vsl_b_write(bfs,wt_sum_);
}

void mbl_wt_histogram::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short file_version_no;
  vsl_b_read(bfs,file_version_no);

  switch (file_version_no)
  {
   case 1:
    vsl_b_read(bfs,n_obs_);
    vsl_b_read(bfs,total_wt_);
    vsl_b_read(bfs,wt_below_);
    vsl_b_read(bfs,wt_above_);
    vsl_b_read(bfs,xlo_);
    vsl_b_read(bfs,dx_);
    vsl_b_read(bfs,wt_sum_);
    break;
   default:
    vcl_cerr << "I/O ERROR: mbl_wt_histogram::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< file_version_no << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

void mbl_wt_histogram::print_summary(vcl_ostream& os) const
{
  os << "mbl_wt_histogram: ";
  if (n_bins()==0) { os<< "No bins defined."; return; }

  int n = n_bins();

  if (n_obs_==0)
    os << "No samples.";
  else
  {
    os << n_obs_ << " observations.\n"
       << "    < "<<xlo_<<"   "<<wt_below_<<vcl_endl;
    for (int i=0;i<n;++i)
      os<<"  ["<<xlo_+i*dx_<<','<<xlo_+(i+1)*dx_<<")  "<<wt_sum_[i]<<vcl_endl;
    os << "   >= "<<xlo_+n*dx_<<"   "<<wt_above_<<vcl_endl;
  }
}

//: Write out histogram probabilities to a named file
//  Format: (bin-centre) prob     (one per line)
// \return true if successful
bool mbl_wt_histogram::write_probabilities(const char* path)
{
  int n = n_bins();
  if (n==0) return false;

  vcl_ofstream ofs(path);
  if (!ofs) return false;
  for (int i=0;i<n;++i)
  {
    ofs<<xlo_+(i+0.5)*dx_<<"  "<<double(wt_sum_[i])/total_wt_<<vcl_endl;
  }
  ofs.close();
  return true;
}

vcl_ostream& operator<<(vcl_ostream& os, const mbl_wt_histogram& histo)
{
  histo.print_summary(os);
  return os;
}

  //: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const mbl_wt_histogram& histo)
{
  histo.print_summary(os);
}

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_wt_histogram& h)
{
  h.b_write(bfs);
}

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_wt_histogram& h)
{
  h.b_read(bfs);
}
