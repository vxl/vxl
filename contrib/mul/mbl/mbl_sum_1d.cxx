// This is mul/mbl/mbl_sum_1d.cxx
#include "mbl_sum_1d.h"
//:
// \file
// \brief Computes sum and mean of 1D variable.
// \author Tim Cootes

#include <vcl_cmath.h>
#include <vcl_iostream.h>

mbl_sum_1d::mbl_sum_1d()
{
  clear();
}

void mbl_sum_1d::clear()
{
  n_obs_ = 0;
  sum_ = 0;
}

mbl_sum_1d& mbl_sum_1d::operator+=(const mbl_sum_1d& s1)
{
  sum_ += s1.sum();
  n_obs_ += s1.nObs();
  return *this ;
}

const double MAX_ERROR = 1.0e-8;

//: Test for equality
bool mbl_sum_1d::operator==(const mbl_sum_1d& s) const
{
  return n_obs_==s.nObs() &&
         vcl_fabs(sum_-s.sum())<MAX_ERROR;
}

//: Version number for I/O
short mbl_sum_1d::version_no() const
{
  return 1;
}

void mbl_sum_1d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,n_obs_);
  vsl_b_write(bfs,sum_);
}

void mbl_sum_1d::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short file_version_no;
  vsl_b_read(bfs,file_version_no);

  switch (file_version_no)
  {
   case 1:
    vsl_b_read(bfs,n_obs_);
    vsl_b_read(bfs,sum_);
    break;
   default:
    vcl_cerr << "I/O ERROR: mbl_sum_1d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< file_version_no << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

void mbl_sum_1d::print_summary(vcl_ostream& os) const
{
  os << "mbl_sum_1d: ";
  if (n_obs_==0)
    os << "No samples.";
  else
  {
    os << "mean: "<< mean() <<" N:"<<n_obs_;
  }
}

vcl_ostream& operator<<(vcl_ostream& os, const mbl_sum_1d& stats)
{
  stats.print_summary(os);
  return os;
}

  //: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const mbl_sum_1d& stats)
{
  stats.print_summary(os);
}

mbl_sum_1d operator+(const mbl_sum_1d& s1, const mbl_sum_1d& s2)
{
  mbl_sum_1d r = s1;
  r+=s2;

  return r;
}

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_sum_1d& b)
{
  b.b_write(bfs);
}

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_sum_1d& b)
{
  b.b_read(bfs);
}
