// This is mul/mbl/mbl_mv_stats.cxx
#include <iostream>
#include <cmath>
#include "mbl_mv_stats.h"
//:
// \file
// \brief Compute multivariate stat.s (mean, covar)
// \author Tim Cootes


#include <vcl_compiler.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

mbl_mv_stats::mbl_mv_stats()
{
  clear();
}

void mbl_mv_stats::clear()
{
  n_obs_ = 0;
  sum_.set_size(0);
  sum2_.set_size(0,0);
}

void mbl_mv_stats::add_sample(const vnl_vector<double>& v)
{
  unsigned n=v.size();
  if (n_obs_ == 0)
  {
    sum_ = v;
    sum2_.set_size(n,n);
    for (unsigned i=0;i<n;++i)
    {
      double* row=sum2_[i];
      for (unsigned j=0;j<n;++j) row[j]=v[i]*v[j];
    }
    n_obs_++;
    return;
  }

  assert(v.size()==n);
  sum_ += v;
  for (unsigned i=0;i<n;++i)
  {
    double* row=sum2_[i];
    double vi=v[i];
    for (unsigned j=0;j<n;++j) row[j]+=vi*v[j];
  }
  n_obs_++;
}

vnl_vector<double> mbl_mv_stats::mean() const
{
  if (n_obs_==0) return vnl_vector<double>();
  else return sum_/n_obs_;
}

vnl_vector<double> mbl_mv_stats::variance() const
{
  if (n_obs_==0) return vnl_vector<double>();

  unsigned n=sum_.size();
  vnl_vector<double> var(n);
  if (n_obs_==1) { var.fill(0.0); return var; }

  for (unsigned i=0;i<n;++i)
  {
    var[i] = (sum2_(i,i)-(sum_[i]*sum_[i]/n_obs_))/(n_obs_-1);
  }
  return var;
}


vnl_vector<double> mbl_mv_stats::sd() const
{
  if (n_obs_==0) return vnl_vector<double>();

  unsigned n=sum_.size();
  vnl_vector<double> sd(n);
  if (n_obs_==1) { sd.fill(0.0); return sd; }

  for (unsigned i=0;i<n;++i)
  {
    double var = (sum2_(i,i)-(sum_[i]*sum_[i]/n_obs_))/(n_obs_-1);
    if (var<=0) sd[i]=0.0;
    else        sd[i]=std::sqrt(var);
  }
  return sd;
}

vnl_vector<double> mbl_mv_stats::stdError() const
{
  if (n_obs_==0) return vnl_vector<double>();

  unsigned n=sum_.size();
  vnl_vector<double> se(n);
  if (n_obs_==1) { se.fill(0.0); return se; }

  for (unsigned i=0;i<n;++i)
  {
    double var = (sum2_(i,i)-(sum_[i]*sum_[i]/n_obs_))/(n_obs_-1);
    if (var<=0) se[i]=0.0;
    else        se[i]=std::sqrt(var/n_obs_);
  }
  return se;
}

//: Covariance matrix of samples added so far
vnl_matrix<double> mbl_mv_stats::covar() const
{
  if (n_obs_==0) return vnl_matrix<double>();

  unsigned n=sum_.size();
  vnl_matrix<double> covar(n,n);
  covar.fill(0);
  if (n_obs_==1) { return covar; }

  for (unsigned i=0;i<n;++i)
    for (unsigned j=i;j<n;++j)
    {
      covar(i,j) = (sum2_(i,j)-(sum_[i]*sum_[j]/n_obs_))/(n_obs_-1);
      covar(j,i) = covar(i,j);
    }

  return covar;
}

//: Return the n_modes eigenvectors associated with the largest eigenvalues
void mbl_mv_stats::get_eigen(vnl_matrix<double>& EVecs, vnl_vector<double>& evals, int n_modes) const
{
  if (n_obs_<=1)
  {
    EVecs.set_size(0,0);
    evals.set_size(0);
    return;
  }

  unsigned n=sum_.size();

  // Compute eigenvectors and values of the covariance matrix
  vnl_matrix<double> P(n,n);
  vnl_vector<double> e(n);
  vnl_symmetric_eigensystem_compute(covar(),P,e);

  // Note: EVecs and EVals are ordered with the smallest first
  e.flip();

  if (n_modes<0) n_modes=n;  // Retain all

  // Extract the relevant n_modes from EVecs and EVals
  EVecs.set_size(n,n_modes);
  evals.set_size(n_modes);
  double **P_data = P.data_array();
  double **EV_data = EVecs.data_array();

  for (unsigned i=0;i<n;++i)
  {
    // Flip each row
    double *PV = P_data[i]-1;
    double *EV = EV_data[i];
    for (unsigned j=0;j<n_modes;++j) EV[j]=PV[n-j];
  }
  for (unsigned j=0;j<n_modes;++j)
    evals[j]=e[j];

}

mbl_mv_stats& mbl_mv_stats::operator+=(const mbl_mv_stats& s1)
{
  sum_ += s1.sum();
  sum2_ += s1.sum2();
  n_obs_ += s1.n_obs();
  return *this ;
}

const double MAX_ERROR = 1.0e-8;

//: Test for equality
bool mbl_mv_stats::operator==(const mbl_mv_stats& s) const
{
  if (n_obs_==0 && s.n_obs()==0) return true;
  return n_obs_==s.n_obs() &&
         vnl_vector_ssd(sum_,s.sum())/n_obs_<MAX_ERROR &&
         sum2_.is_equal(s.sum2(),MAX_ERROR*n_obs_);
}

void mbl_mv_stats::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,short(1));  // Version number
  vsl_b_write(bfs,n_obs_);
  if (n_obs_==0) return;
  vsl_b_write(bfs,sum_);
  vsl_b_write(bfs,sum2_);
}

void mbl_mv_stats::b_read(vsl_b_istream& bfs)
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
      vsl_b_read(bfs,sum2_);
    }
    break;
   default:
    std::cerr << "I/O ERROR: mbl_mv_stats::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< file_version_no << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

void mbl_mv_stats::print_summary(std::ostream& os) const
{
  os << "mbl_mv_stats: ";
  if (n_obs_==0)
    os << "No samples.";
  else
  {
    os << "mean: "<< mean();
  }
}

std::ostream& operator<<(std::ostream& os, const mbl_mv_stats& stats)
{
  stats.print_summary(os);
  return os;
}

  //: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const mbl_mv_stats& stats)
{
  stats.print_summary(os);
}

mbl_mv_stats operator+(const mbl_mv_stats& s1, const mbl_mv_stats& s2)
{
  mbl_mv_stats r = s1;
  r+=s2;

  return r;
}

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_mv_stats& b)
{
  b.b_write(bfs);
}

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_mv_stats& b)
{
  b.b_read(bfs);
}
