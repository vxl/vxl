#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file
// \author Tim Cootes
// \date 12-Apr-2001
// \brief Multi-variate gaussian PDF, with a diagonal covariance matrix

#include <math.h>
#include <cstdlib>
#include <vcl_string.h>
#include <vsl/vsl_indent.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_prob_chi2.h>
#include <mbl/mbl_mz_random.h>

  // Use by ::sample
static mbl_mz_random mz_random(123456);

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_axis_gaussian::vpdfl_axis_gaussian()
{
}

//=======================================================================
// Destructor
//=======================================================================

vpdfl_axis_gaussian::~vpdfl_axis_gaussian()
{
}

//=======================================================================

void vpdfl_axis_gaussian::calcLogK()
{
  const double *v_data = variance().data_block();
  int n = n_dims();
  double log_v_sum = 0.0;
  for (int i=0;i<n;i++)
    log_v_sum+=log(v_data[i]);

  log_k_ = -0.5 * (n*log(2 * 3.14159265) + log_v_sum);
}

void vpdfl_axis_gaussian::calcSD()
{
  sd_ = variance();
  int n = sd_.size();
  for (int i=0;i<n;i++) sd_[i] = sqrt(sd_[i]);
}


void vpdfl_axis_gaussian::set(const vnl_vector<double>& m,
                              const vnl_vector<double>& v)
{
  set_mean(m);
  set_variance(v);

  calcLogK();
  calcSD();
}

// ====================================================================

  // Probability densities:
double vpdfl_axis_gaussian::log_p(const vnl_vector<double>& x)
{
  int n = x.size();
#ifndef NDEBUG
  if (n!=n_dims())
  {
    vcl_cerr<<"vpdfl_axis_gaussian::log_p: Target vector has "
      <<n<<" dimensions, not the required "<<n_dims()<<vcl_endl;
    abort();
  }
#endif

  const double* x_data = x.data_block();
  const double* m_data = mean().data_block();
  const double* v_data = variance().data_block();

  double sum=0.0;
  for (int i=0;i<n;++i)
  {
    double dx=x_data[i]-m_data[i];
    sum+=(dx*dx)/v_data[i];
  }

  return logk() - 0.5*sum;
}

void vpdfl_axis_gaussian::gradient(vnl_vector<double>& g,
                                   const vnl_vector<double>& x,
                                   double& p)
{
  int n = n_dims();
  assert(x.size() == n);

  if (g.size()!=n) g.resize(n);

  double* g_data = g.data_block();
  const double* x_data = x.data_block();
  const double* m_data = mean().data_block();
  const double* v_data = variance().data_block();

  double sum=0.0;

  for (int i=0;i<n;++i)
  {
    double dx=x_data[i]-m_data[i];
    sum+=(dx*dx)/v_data[i];
    g_data[i]= -dx/v_data[i];
  }

  p = exp(logk() - 0.5*sum);

  g*=p;
}

// ====================================================================

  // For generating plausible examples:
void vpdfl_axis_gaussian::sample(vnl_vector<double>& x)
{
  const double *s = sd().data_block();
  const double *m = mean().data_block();
  int n = n_dims();

  if (x.size()!=n) x.resize(n);

  double* x_data = x.data_block();
  for (int i=0;i<n;++i)
    x_data[i] = m[i] + s[i]*mz_random.normal();
}

  //: Reseeds the static random number generator (one per derived class)
void vpdfl_axis_gaussian::reseed(unsigned long seed)
{
  mz_random.reseed(seed);
}



double vpdfl_axis_gaussian::log_prob_thresh(double pass_proportion)
{
  // The mahalanobis distance of n-D gaussian is distributed as Chi^2(n),
  // by definition, Chi^2 is the sum of independedent Normal RVs.
  return logk() - 0.5 * vpdfl_chi2_for_cum_prob (pass_proportion, n_dims());
}



void vpdfl_axis_gaussian::nearest_plausible(vnl_vector<double>& x, double log_p_min)
{
  const double *s = sd_.data_block();
  const double *m = mean().data_block();
  int n = x.size();

  double *x_data = x.data_block();

  // Apply arbitrary limit on sd's regardless of log_p_min
  // Fix this later
  double sd_limit = 3.0;

  for (int i=0;i<n;++i)
  {
    double limit = sd_limit * s[i];
    double lo = m[i] - limit;
    double hi = m[i] + limit;

    if (x_data[i]<lo) x_data[i] = lo;
    else
        if (x_data[i]>hi) x_data[i] = hi;
  }

}
//=======================================================================
// Method: is_a
//=======================================================================

vcl_string  vpdfl_axis_gaussian::is_a() const
{
  return vcl_string("vpdfl_axis_gaussian");
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_axis_gaussian::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_pdf_base* vpdfl_axis_gaussian::clone() const
{
  return new vpdfl_axis_gaussian(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void vpdfl_axis_gaussian::print_summary(ostream& os) const
{
  os<<vcl_endl;
  vpdfl_pdf_base::print_summary(os);
  if (n_dims()==0) return;
  else
  if (n_dims()==1)
    os<<" (Mean: "<<mean()(0)<<" Var: "<<variance()(0)<<")";
  else
  {
    // Show the first few means and variances
    int n = 3;
    if (n>n_dims()) n=n_dims();
    os<<" Var: (";
    for (int i=0;i<n;++i) os<<variance()(i)<<" ";
    if (n_dims()>n) os<<"...";
    os<<")";
  }
}

//=======================================================================
// Method: save
//=======================================================================

void vpdfl_axis_gaussian::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  vpdfl_pdf_base::b_write(bfs);
}

//=======================================================================
// Method: load
//=======================================================================

void vpdfl_axis_gaussian::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vpdfl_pdf_base::b_read(bfs);
      break;
    default:
      vcl_cerr << "vpdfl_axis_gaussian::b_read() ";
      vcl_cerr << "Unexpected version number " << version << vcl_endl;
      abort();
  }

  calcLogK();
  calcSD();
}
