#include <mbl/mbl_sample_stats_1d.h>
//:
// \file
#include <vsl/vsl_vector_io.h>
#include <vcl_cmath.h>
#include <vcl_limits.h>
#include <vcl_algorithm.h>


// ============================================================
mbl_sample_stats_1d::mbl_sample_stats_1d(const vcl_vector<double> &samples)
{
  clear();

  for (unsigned i=0;i<samples.size();++i)
  {
    add_sample(samples[i]);
  }
}


// ============================================================
mbl_sample_stats_1d::mbl_sample_stats_1d(const vnl_vector<double> &samples)
{
  clear();

  for (unsigned i=0;i<samples.size();++i)
  {
    add_sample(samples[i]);
  }
}

// ============================================================
mbl_sample_stats_1d::mbl_sample_stats_1d()
{
  clear();
}

// ============================================================
mbl_sample_stats_1d::~mbl_sample_stats_1d()
{
}

// ============================================================
void mbl_sample_stats_1d::clear()
{
  samples_.resize(0);
  stats_1d_.clear();
  use_mvue_=true;
}


// ============================================================
void mbl_sample_stats_1d::add_sample(double v)
{
  stats_1d_.obs(v);
  samples_.push_back(v);
  return;
}

// ============================================================
int mbl_sample_stats_1d::n_samples() const
{
  return samples_.size();
}

// ============================================================
double mbl_sample_stats_1d::mean() const
{
  return stats_1d_.mean();
}

// ============================================================
double mbl_sample_stats_1d::mean_of_absolutes() const
{
  double abs_sum = 0;
  for (int i=0;i<n_samples();++i) abs_sum+=vcl_fabs(samples_[i]);
  return abs_sum/n_samples();
}

// ============================================================
double mbl_sample_stats_1d::median() const
{
  //     return nth_percentile(50);
  double ret;

  if (n_samples()>0)
  {
    if ( samples_.size() % 2 == 0 )
    {
      unsigned index = samples_.size() / 2 - 1;

      vcl_vector<double> tmp=samples_;
      vcl_vector<double>::iterator index_it0 = tmp.begin() + index;
      vcl_nth_element(tmp.begin(),index_it0,tmp.end(),vcl_less<double>());
      vcl_vector<double>::iterator index_it1 = tmp.begin() + index + 1;
      vcl_nth_element(tmp.begin(),index_it1,tmp.end(),vcl_less<double>());

      ret = (*index_it0 + *index_it1);
      ret /= 2.0;
    }
    else
    {
      unsigned index = (samples_.size() - 1) / 2;

      vcl_vector<double> tmp=samples_;
      vcl_vector<double>::iterator index_it = tmp.begin() + index;
      vcl_nth_element(tmp.begin(),index_it,tmp.end(),vcl_less<double>());

      ret = *index_it;
    }
  }
  else // crazy value if  no samples
  {
    ret = vcl_numeric_limits<double>::max();
  }
  return ret;
}


// ============================================================
double mbl_sample_stats_1d::nth_percentile(int n) const
{
  double ret;

  if (n_samples()>0)
  {
    double fact = double(n)/100.0;
    int index=int(fact*(n_samples()-1));

    vcl_vector<double> tmp=samples_;
    vcl_vector<double>::iterator index_it = tmp.begin() + index;
    vcl_nth_element(tmp.begin(),index_it,tmp.end(),vcl_less<double>());

    ret = *index_it;
  }
  else // crazy value if no samples
  {
    ret = vcl_numeric_limits<double>::max();
  }

  return ret;
}

// ============================================================
double mbl_sample_stats_1d::variance() const
{
  double v=0;

  if (n_samples()>1)
  {
    double mean_v = mean();
    double sum_sq = sum_squares();
    v = sum_sq - n_samples()*(mean_v * mean_v);

    if (use_mvue_)
    {
      v /= (n_samples()-1);
    }
    else
    {
      v /= n_samples();
    }
  }

  return v;
}


// ============================================================
double mbl_sample_stats_1d::sd() const
{
  return vcl_sqrt(variance());
}


// ============================================================
double mbl_sample_stats_1d::stdError() const
{
  double se = variance();
  if (use_mvue_)
  {
    se /= n_samples()-1;
  }
  else
  {
    se /= n_samples();
  }

  return vcl_sqrt(se);
}


// ============================================================
double mbl_sample_stats_1d::skewness() const
{
  double skew = 0;

  // skew
  // calculated as
  // ( Sum_i (Y_i-MEAN)^3 ) / ((N-1)*sigma^3)
  // where N is the number of samples
  // sigma is the standard deviation

  if (n_samples()>1)
  {
    double s=sd();
    double m=mean();

    for (int i=0;i<n_samples();++i)
    {
      double tmp=samples_[i]-m;
      skew += (tmp*tmp*tmp) ;
    }

    skew /= ( (n_samples()-1) * s * s * s );
  }

  return skew;
}

// ============================================================
double mbl_sample_stats_1d::kurtosis() const
{
  double kurt = 0;

  // kurtosis
  // calculated as
  // -3 + ( Sum_i (Y_i-MEAN)^4 ) / ((N-1)*sigma^4)
  // where N is the number of samples
  // sigma is the standard deviation

  if (n_samples()>1)
  {
    double s=sd();
    double m=mean();

    for (int i=0;i<n_samples();++i)
    {
      double tmp=samples_[i]-m;
      kurt += (tmp*tmp*tmp*tmp) ;
    }

    kurt /= ( (n_samples()-1) * s * s * s *s);
    kurt -= 3;
  }
  return kurt;
}

// ============================================================
double mbl_sample_stats_1d::min() const
{
  if (n_samples()==0) return vcl_numeric_limits<double>::max();
  else return stats_1d_.min();
}

// ============================================================
double mbl_sample_stats_1d::max() const
{
  if (n_samples()==0) return vcl_numeric_limits<double>::min();
  else return stats_1d_.max();
}

// ============================================================
double mbl_sample_stats_1d::sum() const
{
  return stats_1d_.sum();
}

// ============================================================
double mbl_sample_stats_1d::sum_squares() const
{
  return stats_1d_.sumSq();
}

// ============================================================
double mbl_sample_stats_1d::rms() const
{
  double ms=sum_squares()/stats_1d_.nObs();
  return vcl_sqrt( ms );
}

// ============================================================
mbl_sample_stats_1d& mbl_sample_stats_1d::operator+=(const mbl_sample_stats_1d& s1)
{
  // add new samples
  for (unsigned i=0;i<s1.samples().size();++i)
  {
    add_sample(s1.samples()[i]);
  }

  return *this ;
}

//: Test for equality
bool mbl_sample_stats_1d::operator==(const mbl_sample_stats_1d& s) const
{
  return samples_==s.samples_ && use_mvue_==s.use_mvue_;
}

// =============================================
short mbl_sample_stats_1d::version_no() const
{
  return 1;
}

// =============================================
void mbl_sample_stats_1d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,samples_);
  vsl_b_write(bfs,stats_1d_);
  vsl_b_write(bfs,use_mvue_);
}

// =============================================
void mbl_sample_stats_1d::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short file_version_no;
  vsl_b_read(bfs,file_version_no);

  switch (file_version_no)
  {
   case 1:
    vsl_b_read(bfs,samples_);
    vsl_b_read(bfs,stats_1d_);
    vsl_b_read(bfs,use_mvue_);
    break;
   default :
    vcl_cerr << "I/O ERROR: mbl_sample_stats_1d::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< file_version_no << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

void mbl_sample_stats_1d::print_summary(vcl_ostream& os) const
{
  os << "mbl_sample_stats_1d: ";
  if (n_samples()==0)
  {
    os << "No samples.";
  }
  else
  {
    os << "mean: "<< mean()
       << " use MVUE: "<< use_mvue_
       << " sd: "<< sd()
       << " ["<<stats_1d_.min()<<','<<stats_1d_.max()<<"] N:"<<n_samples();
  }
}


//: Print all data
void mbl_sample_stats_1d::print_all(vcl_ostream& os,
                                    const vcl_string& delim/*="\n"*/) const
{
  unsigned nSamples = samples_.size();
  for (unsigned i=0; i<nSamples; ++i)
  {
    os << samples_[i] << delim;
  }
}


vcl_ostream& operator<<(vcl_ostream& os, const mbl_sample_stats_1d& stats)
{
  stats.print_summary(os);
  return os;
}

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const mbl_sample_stats_1d& stats)
{
  stats.print_summary(os);
}


//: Print all data
void vsl_print_all(vcl_ostream& os, const mbl_sample_stats_1d& stats)
{
  stats.print_all(os);
}


mbl_sample_stats_1d operator+(const mbl_sample_stats_1d& s1, const mbl_sample_stats_1d& s2)
{
  mbl_sample_stats_1d r = s1;
  r+=s2;

  return r;
}

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_sample_stats_1d& b)
{
  b.b_write(bfs);
}

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_sample_stats_1d& b)
{
  b.b_read(bfs);
}
