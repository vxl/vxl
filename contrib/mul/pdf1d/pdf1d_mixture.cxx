// This is mul/pdf1d/pdf1d_mixture.cxx
#include "pdf1d_mixture.h"
//:
// \file
// \brief Implements a mixture model (a set of individual pdfs + weights)
// \author Tim Cootes and Ian Scott

//=======================================================================

#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <pdf1d/pdf1d_mixture_sampler.h>
#include <vcl_cassert.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/vnl_c_vector.h>

//=======================================================================

void pdf1d_mixture::init()
{
}

pdf1d_mixture::pdf1d_mixture()
{
  init();
}

pdf1d_mixture::pdf1d_mixture(const pdf1d_mixture& m):
  pdf1d_pdf()
{
  init();
  *this = m;
}

pdf1d_mixture& pdf1d_mixture::operator=(const pdf1d_mixture& m)
{
  if (this==&m) return *this;

  delete_stuff();

  pdf1d_pdf::operator=(m);

  int n = m.component_.size();
  component_.resize(n);
  for (int i=0;i<n;++i)
    component_[i] = m.component_[i]->clone();

  weight_ = m.weight_;

  return *this;
}


//=======================================================================

void pdf1d_mixture::delete_stuff()
{
  int n = component_.size();
  for (int i=0;i<n;++i)
    delete component_[i];
  component_.resize(0);
  weight_.resize(0);
}

pdf1d_mixture::~pdf1d_mixture()
{
  delete_stuff();
}


//: Return instance of this PDF
pdf1d_sampler* pdf1d_mixture::new_sampler() const
{
  pdf1d_mixture_sampler* i = new pdf1d_mixture_sampler;
  i->set_model(*this);

  return i;
}

//: Initialise to use n components of type comp_type
void pdf1d_mixture::init(const pdf1d_pdf& comp_type, int n)
{
  delete_stuff();
  component_.resize(n);
  weight_.resize(n);
  for (int i=0;i<n;++i)
  {
    component_[i] = comp_type.clone();
    weight_[i] = 1.0/n;
  }
}

//=======================================================================

void pdf1d_mixture::add_component(const pdf1d_pdf& comp)
{
  vcl_vector<pdf1d_pdf*> old_comps = component_;
  vcl_vector<double> old_wts = weight_;
  unsigned int n = component_.size();
  assert(n == weight_.size());

  component_.resize(n+1);
  weight_.resize(n+1);

  for (unsigned int i=0;i<n;++i)
  {
    component_[i] = old_comps[i];
    weight_[i] = old_wts[i];
  }

  weight_[n] = 0.0;
  component_[n] = comp.clone();
}

//=======================================================================

void pdf1d_mixture::clear()
{
  delete_stuff();
}

//=======================================================================

//: Return true if the object represents a valid PDF.
// This will return false, if n_dims() is 0, for example just ofter
// default construction.
bool pdf1d_mixture::is_valid_pdf() const
{
  if (!pdf1d_pdf::is_valid_pdf()) return false;
  const unsigned n = n_components();
    // the number of components should be consistent
  if (weight_.size() != n || component_.size() != n || n < 1) return false;
    // weights should sum to 1.
  double sum = vnl_c_vector<double>::sum(&weight_[0]/*.begin()*/, n);
  if (vcl_fabs(1.0 - sum) > 1e-10 ) return false;
    // the number of dimensions should be consistent
  for (unsigned i=0; i<n; ++i)
  {
    if (!components()[i]->is_valid_pdf()) return false;
  }
  return true;
}

//: Set the whole pdf mean and variance values.
void pdf1d_mixture::set_mean_and_variance(double m, double v)
{
  set_mean(m);
  set_variance(v);
}


//=======================================================================

vcl_string pdf1d_mixture::is_a() const
{
  return vcl_string("pdf1d_mixture");
}

//=======================================================================

bool pdf1d_mixture::is_class(vcl_string const& s) const
{
  return pdf1d_pdf::is_class(s) || s==pdf1d_mixture::is_a();
}

//=======================================================================

short pdf1d_mixture::version_no() const
{
  return 1;
}

//=======================================================================

pdf1d_pdf* pdf1d_mixture::clone() const
{
  return new pdf1d_mixture(*this);
}


//=======================================================================


void pdf1d_mixture::print_summary(vcl_ostream& os) const
{
  os<<vcl_endl<<vsl_indent();
  pdf1d_pdf::print_summary(os);
  os<<vcl_endl;
  for (unsigned int i=0;i<component_.size();++i)
  {
    os<<vsl_indent()<<"Component "<<i<<" :  Wt: "<<weight_[i] <<vcl_endl
      <<vsl_indent()<<"PDF: " << component_[i]<<vcl_endl;
  }
}

//=======================================================================

void pdf1d_mixture::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, is_a());
  vsl_b_write(bfs, version_no());
  pdf1d_pdf::b_write(bfs);
  vsl_b_write(bfs, component_);
  vsl_b_write(bfs, weight_);
}

//=======================================================================

void pdf1d_mixture::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  vcl_string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_mixture &)\n"
             << "           Attempted to load object of type "
             << name <<" into object of type " << is_a() << vcl_endl;
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  delete_stuff();

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      pdf1d_pdf::b_read(bfs);
      vsl_b_read(bfs, component_);
      vsl_b_read(bfs, weight_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_mixture &)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//=======================================================================


double pdf1d_mixture::operator()(double x) const
{
  return vcl_exp(log_p(x));
}

//=======================================================================

double pdf1d_mixture::log_p(double x) const
{
  int n = n_components();

  vcl_vector<double> log_ps(n);
  double max_log_p = 0;
  for (int i=0;i<n;++i)
  {
    if (weight_[i]>0.0)
    {
      log_ps[i] = component_[i]->log_p(x);
      if (i==0 || log_ps[i]>max_log_p) max_log_p = log_ps[i];
    }
  }

  double sum=0.0;

  for (int i=0;i<n;i++)
  {
    if (weight_[i]>0.0)
      sum += weight_[i] * vcl_exp(log_ps[i]-max_log_p);
  }

  return vcl_log(sum) + max_log_p;
}

//: Cumulative Probability (P(x'<x) for x' drawn from the distribution)
double pdf1d_mixture::cdf(double x) const
{
  double sum = 0;

  int n = n_components();
  for (int i=0;i<n;++i)
    sum += weight_[i]*component_[i]->cdf(x);

  return sum;
}

//: Return true if cdf() uses an analytic implementation
bool pdf1d_mixture::cdf_is_analytic() const
{
  return true;
}

//=======================================================================

double pdf1d_mixture::gradient(double x, double& p) const
{
  double p1;
  double g1 = component_[0]->gradient(x,p1);
  double g = g1*weight_[0];
  p = p1*weight_[0];

  for (unsigned int i=1;i<n_components();i++)
  {
    g1 = component_[i]->gradient(x,p1);
    g += g1*weight_[i];
    double p_comp = p1*weight_[i];
    p += p_comp;
  }

  return g;
}


//=======================================================================

unsigned pdf1d_mixture::nearest_comp(double x) const
{
  assert(component_.size()>=1);

  int n = n_components();
  if (n==1) return 0;

  int best_i=0;
  double min_d2 = vcl_fabs(x-component_[0]->mean());

  for (int i=1;i<n;i++)
  {
    double d2 = vcl_fabs(x-component_[i]->mean());
    if (d2<min_d2)
    {
      best_i=i;
      min_d2=d2;
    }
  }

  return best_i;
}

//=======================================================================


//: Compute nearest point to x which has a density above a threshold
//  If log_p(x)>log_p_min then x unchanged.  Otherwise x is moved
//  (typically up the gradient) until log_p(x)>=log_p_min.
// \param x This may be modified to the nearest plausible position.
double pdf1d_mixture::nearest_plausible(double /*x*/, double /*log_p_min*/) const
{
  vcl_cerr << "ERROR: pdf1d_mixture::nearest_plausible NYI\n";
  vcl_abort();
  return 0.0; // dummy return
}


//==================< end of file: pdf1d_mixture.cxx >====================
