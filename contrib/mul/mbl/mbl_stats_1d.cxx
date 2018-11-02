// This is mul/mbl/mbl_stats_1d.cxx
#include <iostream>
#include <cmath>
#include "mbl_stats_1d.h"
//:
// \file
// \brief Simple statistics on a 1D variable.
// \author Tim Cootes

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

mbl_stats_1d::mbl_stats_1d()
{
  clear();
}

mbl_stats_1d::mbl_stats_1d(const std::vector<double>& observations)
{
  clear();
  std::vector<double>::const_iterator it;
  for (it=observations.begin(); it != observations.end(); ++it)
  {
    obs(*it);
  }
}

void mbl_stats_1d::clear()
{
  n_obs_ = 0;
  w_obs_ = 0;
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
    w_obs_=1.0;
    n_obs_=1;
    return;
  }

  if (v<min_v_) min_v_ = v;
  if (v>max_v_) max_v_ = v;
  sum_ += v;
  sum_sq_ += v * v;
  n_obs_++;
  w_obs_++;
}

void mbl_stats_1d::obs(double v, double weight)
{
  if (n_obs_ == 0)
  {
    min_v_ = v;
    max_v_ = v;
    sum_ = v * weight;
    sum_sq_ = v * v * weight;
    w_obs_=weight;
    n_obs_=1;
    return;
  }

  if (v<min_v_) min_v_ = v;
  if (v>max_v_) max_v_ = v;
  sum_ += v * weight;
  sum_sq_ += v * v * weight;
  w_obs_+=weight;
  n_obs_++;
}

double mbl_stats_1d::mean() const
{
  if (n_obs_==0) return 0;
  else return sum_/w_obs_;
}

double mbl_stats_1d::variance() const
{
  if (n_obs_==0) return 0;

  double mean_v = mean();
  return sum_sq_/w_obs_ - mean_v * mean_v;
}


double mbl_stats_1d::sd() const
{
  if (n_obs_==0) return 0;

  double var_v = variance();
  // Use of numerically dodgy Sum{x^2} - {Sum x}^2
  // can return negative numbers.
  if (var_v<0) var_v=0;
  return std::sqrt(var_v);
}

double mbl_stats_1d::stdError() const
{
  if (n_obs_==0) return 0;

  double var_v = variance();
  return std::sqrt(var_v/w_obs_);
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

double mbl_stats_1d::rms() const
{
  return n_obs_==0 ? -1.0 : std::sqrt(sum_sq_/w_obs_);
}


mbl_stats_1d& mbl_stats_1d::operator+=(const mbl_stats_1d& s1)
{
  sum_ += s1.sum_;
  sum_sq_ += s1.sum_sq_;
  n_obs_ += s1.n_obs_;
  w_obs_ += s1.w_obs_;
  if (s1.min()<min_v_) min_v_ = s1.min_v_;
  if (s1.max()>max_v_) max_v_ = s1.max_v_;
  return *this ;
}

const double MAX_ERROR = 1.0e-8;

//: Test for equality
bool mbl_stats_1d::operator==(const mbl_stats_1d& s) const
{
  return n_obs_==s.n_obs_ &&
         std::fabs(w_obs_-s.w_obs_)<MAX_ERROR &&
         std::fabs(sum_-s.sum_)<MAX_ERROR &&
         std::fabs(sum_sq_-s.sum_sq_)<MAX_ERROR &&
         std::fabs(min_v_-s.min_v_)<MAX_ERROR &&
         std::fabs(max_v_-s.max_v_)<MAX_ERROR;
}


void mbl_stats_1d::b_write(vsl_b_ostream& bfs) const
{
  constexpr short version = 2;
  vsl_b_write(bfs,version);
  vsl_b_write(bfs,n_obs_);
  if (n_obs_==0) return;
  vsl_b_write(bfs,min_v_); vsl_b_write(bfs,max_v_);
  vsl_b_write(bfs,sum_); vsl_b_write(bfs,sum_sq_);
  vsl_b_write(bfs,w_obs_);
}

void mbl_stats_1d::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short file_version_no;
  vsl_b_read(bfs,file_version_no);

  switch (file_version_no)
  {
   case 1:
    {
      int tmp;
      vsl_b_read(bfs, tmp);
      n_obs_ = static_cast<unsigned>(tmp);
    }
    if (n_obs_<=0) clear();
    else
    {
      vsl_b_read(bfs,min_v_);
      vsl_b_read(bfs,max_v_);
      vsl_b_read(bfs,sum_);
      vsl_b_read(bfs,sum_sq_);
    }
    w_obs_ = n_obs_;
    break;
   case 2:
    vsl_b_read(bfs, n_obs_);
    if (n_obs_<=0) clear();
    else
    {
      vsl_b_read(bfs,min_v_);
      vsl_b_read(bfs,max_v_);
      vsl_b_read(bfs,sum_);
      vsl_b_read(bfs,sum_sq_);
      vsl_b_read(bfs,w_obs_);
    }
    break;
   default:
    std::cerr << "I/O ERROR: mbl_stats_1d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< file_version_no << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

void mbl_stats_1d::print_summary(std::ostream& os) const
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

std::ostream& operator<<(std::ostream& os, const mbl_stats_1d& stats)
{
  stats.print_summary(os);
  return os;
}

  //: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const mbl_stats_1d& stats)
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
