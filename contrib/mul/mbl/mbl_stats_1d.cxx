// This is mul/mbl/mbl_stats_1d.cxx
#include "mbl_stats_1d.h"
//:
// \file
// \brief Simple statistics on a 1D variable.
// \author Tim Cootes

#include <vcl_cmath.h>
#include <vcl_iostream.h>

mbl_stats_1d::mbl_stats_1d()
{
  clear();
}

void mbl_stats_1d::clear()
{
  n_obs_ = 0;
  sum_ = 0;
  sum_sq_ = 0;
}

void mbl_stats_1d::obs(double v)
{
  if (n_obs_ == 0)
  {
    min_v_ = v;
    max_v_ = v;
    sum_ = v;
    sum_sq_ = v * v;
    n_obs_++;
    return;
  }

  if (v<min_v_) min_v_ = v;
  if (v>max_v_) max_v_ = v;
  sum_ += v;
  sum_sq_ += v * v;
  n_obs_++;
}

int mbl_stats_1d::nObs() const
{
  return n_obs_;
}

double mbl_stats_1d::mean() const
{
  if (n_obs_==0) return 0;
  else return sum_/n_obs_;
}

double mbl_stats_1d::variance() const
{
  if (n_obs_==0) return 0;

  double mean_v = mean();
  return sum_sq_/n_obs_ - mean_v * mean_v;
}


double mbl_stats_1d::sd() const
{
  if (n_obs_==0) return 0;

  double var_v = variance();
  return vcl_sqrt(var_v);
}

double mbl_stats_1d::stdError() const
{
  if (n_obs_==0) return 0;

  double var_v = variance();
  return vcl_sqrt(var_v/n_obs_);
}

double mbl_stats_1d::min() const
{
  if (n_obs_==0) return 0;
  else return min_v_;
}

double mbl_stats_1d::max() const
{
  if (n_obs_==0) return 0;
  else return max_v_;
}

double mbl_stats_1d::sum() const
{
  return sum_;
}

double mbl_stats_1d::sumSq() const
{
  return sum_sq_;
}

mbl_stats_1d& mbl_stats_1d::operator+=(const mbl_stats_1d& s1)
{
  sum_ += s1.sum();
  sum_sq_ += s1.sumSq();
  n_obs_ += s1.nObs();
  if (s1.min()<min_v_) min_v_ = s1.min();
  if (s1.max()>max_v_) max_v_ = s1.max();
  return *this ;
}

const double MAX_ERROR = 1.0e-8;

//: Test for equality
bool mbl_stats_1d::operator==(const mbl_stats_1d& s) const
{
  return n_obs_==s.nObs() &&
         vcl_fabs(sum_-s.sum())<MAX_ERROR &&
         vcl_fabs(sum_sq_-s.sumSq())<MAX_ERROR &&
         vcl_fabs(min_v_-s.min())<MAX_ERROR &&
         vcl_fabs(max_v_-s.max())<MAX_ERROR;
}

//: Version number for I/O
short mbl_stats_1d::version_no() const
{
  return 1;
}

void mbl_stats_1d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,n_obs_);
  if (n_obs_==0) return;
  vsl_b_write(bfs,min_v_); vsl_b_write(bfs,max_v_);
  vsl_b_write(bfs,sum_); vsl_b_write(bfs,sum_sq_);
}

void mbl_stats_1d::b_read(vsl_b_istream& bfs)
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
      vsl_b_read(bfs,min_v_);
      vsl_b_read(bfs,max_v_);
      vsl_b_read(bfs,sum_);
      vsl_b_read(bfs,sum_sq_);
    }
    break;
   default:
    vcl_cerr << "I/O ERROR: mbl_stats_1d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< file_version_no << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

void mbl_stats_1d::print_summary(vcl_ostream& os) const
{
  os << "mbl_stats_1d: ";
  if (n_obs_==0)
    os << "No samples.";
  else
  {
    os << "mean: "<< mean()
       << " sd: "<< sd()
       << " ["<<min_v_<<','<<max_v_<<"] N:"<<n_obs_;
  }
}

vcl_ostream& operator<<(vcl_ostream& os, const mbl_stats_1d& stats)
{
  stats.print_summary(os);
  return os;
}

  //: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const mbl_stats_1d& stats)
{
  stats.print_summary(os);
}

mbl_stats_1d operator+(const mbl_stats_1d& s1, const mbl_stats_1d& s2)
{
  mbl_stats_1d r = s1;
  r+=s2;

  return r;
}

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_stats_1d& b)
{
  b.b_write(bfs);
}

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_stats_1d& b)
{
  b.b_read(bfs);
}
