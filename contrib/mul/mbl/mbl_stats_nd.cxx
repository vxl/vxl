// This is mul/mbl/mbl_stats_nd.cxx
#include "mbl_stats_nd.h"
//:
// \file
// \brief Simple statistics (mean, variance) on vectors.
// \author Tim Cootes

#include <vcl_cmath.h>
#include <vnl/io/vnl_io_vector.h>

mbl_stats_nd::mbl_stats_nd()
{
  clear();
}

void mbl_stats_nd::clear()
{
  n_obs_ = 0;
  sum_.set_size(0);
  sum_sq_.set_size(0);
}

void mbl_stats_nd::obs(const vnl_vector<double>& v)
{
  unsigned n=v.size();
  if (n_obs_ == 0)
  {
    sum_ = v;
    sum_sq_.set_size(n);
    for (unsigned i=0;i<n;++i) sum_sq_[i]=v[i]*v[i];
    n_obs_++;
    return;
  }

  assert(v.size()==n);
  sum_ += v;
  for (unsigned i=0;i<n;++i) sum_sq_[i]+=v[i]*v[i];
  n_obs_++;
}

vnl_vector<double> mbl_stats_nd::mean() const
{
  if (n_obs_==0) return vnl_vector<double>();
  else return sum_/n_obs_;
}

vnl_vector<double> mbl_stats_nd::variance() const
{
  if (n_obs_==0) return vnl_vector<double>();

  unsigned n=sum_.size();
  vnl_vector<double> var(n);
  if (n_obs_==1) { var.fill(0.0); return var; }

  for (unsigned i=0;i<n;++i)
  {
    var[i] = (sum_sq_[i]-(sum_[i]*sum_[i]/n_obs_))/(n_obs_-1);
  }
  return var;
}


vnl_vector<double> mbl_stats_nd::sd() const
{
  if (n_obs_==0) return vnl_vector<double>();

  unsigned n=sum_.size();
  vnl_vector<double> sd(n);
  if (n_obs_==1) { sd.fill(0.0); return sd; }

  for (unsigned i=0;i<n;++i)
  {
    double var = (sum_sq_[i]-(sum_[i]*sum_[i]/n_obs_))/(n_obs_-1);
    if (var<=0) sd[i]=0.0;
    else        sd[i]=vcl_sqrt(var);
  }
  return sd;
}

vnl_vector<double> mbl_stats_nd::stdError() const
{
  if (n_obs_==0) return vnl_vector<double>();

  unsigned n=sum_.size();
  vnl_vector<double> se(n);
  if (n_obs_==1) { se.fill(0.0); return se; }

  for (unsigned i=0;i<n;++i)
  {
    double var = (sum_sq_[i]-(sum_[i]*sum_[i]/n_obs_))/(n_obs_-1);
    if (var<=0) se[i]=0.0;
    else        se[i]=vcl_sqrt(var/n_obs_);
  }
  return se;
}


mbl_stats_nd& mbl_stats_nd::operator+=(const mbl_stats_nd& s1)
{
  sum_ += s1.sum();
  sum_sq_ += s1.sumSq();
  n_obs_ += s1.n_obs();
  return *this ;
}

const double MAX_ERROR = 1.0e-8;

//: Test for equality
bool mbl_stats_nd::operator==(const mbl_stats_nd& s) const
{
  if (n_obs_==0 && s.n_obs()==0) return true;
  return n_obs_==s.n_obs() &&
         vnl_vector_ssd(sum_,s.sum())/n_obs_<MAX_ERROR &&
         vnl_vector_ssd(sum_sq_,s.sumSq())/n_obs_<MAX_ERROR;
}

//: Version number for I/O
short mbl_stats_nd::version_no() const
{
  return 1;
}

void mbl_stats_nd::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,n_obs_);
  if (n_obs_==0) return;
  vsl_b_write(bfs,sum_);
  vsl_b_write(bfs,sum_sq_);
}

void mbl_stats_nd::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short file_version_no;
  vsl_b_read(bfs,file_version_no);

  switch (file_version_no)
  {
  case 1:
    vsl_b_read(bfs,n_obs_);
    if (n_obs_<=0) clear();
    else
    {
      vsl_b_read(bfs,sum_);
      vsl_b_read(bfs,sum_sq_);
    }
    break;
  default :
    vcl_cerr << "I/O ERROR: mbl_stats_nd::b_read(vsl_b_istream&) \n";
    vcl_cerr << "           Unknown version number "<< file_version_no << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

void mbl_stats_nd::print_summary(vcl_ostream& os) const
{
  os << "mbl_stats_nd: ";
  if (n_obs_==0)
    os << "No samples.";
  else
  {
    os << "mean: "<< mean();
  }
}

vcl_ostream& operator<<(vcl_ostream& os, const mbl_stats_nd& stats)
{
  stats.print_summary(os);
  return os;
}

  //: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const mbl_stats_nd& stats)
{
  stats.print_summary(os);
}

mbl_stats_nd operator+(const mbl_stats_nd& s1, const mbl_stats_nd& s2)
{
  mbl_stats_nd r = s1;
  r+=s2;

  return r;
}

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_stats_nd& b)
{
  b.b_write(bfs);
}

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_stats_nd& b)
{
  b.b_read(bfs);
}
