//=======================================================================
//
//  Copyright: (C) 1998 Victoria University of Manchester
//
//=======================================================================

//:
// \file
// \brief Implements a mixture model (a set of individual pdfs + weights)
// \author Tim Cootes
// \date 21-July-98
//
// Modifications
// \verbatim
//    IMS   Converted to VXL 12 May 2000
// \endverbatim

//=======================================================================

#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <vpdfl/vpdfl_mixture.h>
#include <vpdfl/vpdfl_mixture_sampler.h>
#include <vcl_cassert.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/vnl_c_vector.h>

//=======================================================================

void vpdfl_mixture::init()
{
}

vpdfl_mixture::vpdfl_mixture()
{
  init();
}

vpdfl_mixture::vpdfl_mixture(const vpdfl_mixture& m)
{
  init();
  *this = m;
}

vpdfl_mixture& vpdfl_mixture::operator=(const vpdfl_mixture& m)
{
  if (this==&m) return *this;

  delete_stuff();

  vpdfl_pdf_base::operator=(m);

  int n = m.component_.size();
  component_.resize(n);
  for (int i=0;i<n;++i)
    component_[i] = m.component_[i]->clone();

  weight_ = m.weight_;

  return *this;
}

//=======================================================================

void vpdfl_mixture::delete_stuff()
{
  int n = component_.size();
  for (int i=0;i<n;++i)
    delete component_[i];
  component_.resize(0);
  weight_.resize(0);
}

vpdfl_mixture::~vpdfl_mixture()
{
  delete_stuff();
}


//: Return instance of this PDF
vpdfl_sampler_base* vpdfl_mixture::new_sampler() const
{
  vpdfl_mixture_sampler* i = new vpdfl_mixture_sampler;
  i->set_model(*this);

  return i;
}

//: Initialise to use n components of type comp_type
void vpdfl_mixture::init(const vpdfl_pdf_base& comp_type, int n)
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

void vpdfl_mixture::add_component(const vpdfl_pdf_base& comp)
{
  vcl_vector<vpdfl_pdf_base*> old_comps = component_;
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

void vpdfl_mixture::clear()
{
  delete_stuff();
}

//=======================================================================

//: Return true if the object represents a valid PDF.
// This will return false, if n_dims() is 0, for example just ofter
// default construction.
bool vpdfl_mixture::is_valid_pdf() const
{
  if (!vpdfl_pdf_base::is_valid_pdf()) return false;
  const unsigned n = n_components();
    // the number of components should be consistent
  if (weight_.size() != n || component_.size() != n || n < 1) return false;
    // weights should sum to 1.
  double sum =vnl_c_vector<double>::sum(&weight_[0]/*.begin()*/, n);
  if (vcl_fabs(1.0 - sum) > 1e-10 ) return false;
    // the number of dimensions should be consistent
  for (unsigned i=0; i<n; ++i)
  {
    if (!components()[i]->is_valid_pdf()) return false;
    if (components()[i]->n_dims() != n_dims()) return false;
  }
  return true;
}

//: Set the whole pdf mean and variance values.
void vpdfl_mixture::set_mean_and_variance(vnl_vector<double>&m, vnl_vector<double>&v)
{
  assert(m.size() == v.size());

  set_mean(m);
  set_variance(v);
}

//=======================================================================

vcl_string vpdfl_mixture::is_a() const
{
  return vcl_string("vpdfl_mixture");
}

//=======================================================================

bool vpdfl_mixture::is_class(vcl_string const& s) const
{
  return vpdfl_pdf_base::is_class(s) || s==vpdfl_mixture::is_a();
}

//=======================================================================

short vpdfl_mixture::version_no() const
{
  return 1;
}

//=======================================================================

vpdfl_pdf_base* vpdfl_mixture::clone() const
{
  return new vpdfl_mixture(*this);
}

//=======================================================================

void vpdfl_mixture::print_summary(vcl_ostream& os) const
{
  os<<vcl_endl<<vsl_indent();
  vpdfl_pdf_base::print_summary(os);
  os<<vcl_endl;
  for (unsigned int i=0;i<component_.size();++i)
  {
    os<<vsl_indent()<<"Component "<<i<<" :  Wt: "<<weight_[i] <<vcl_endl;
    os<<vsl_indent()<<"PDF: " << component_[i]<<vcl_endl;
  }
}

//=======================================================================

void vpdfl_mixture::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, is_a());
  vsl_b_write(bfs, version_no());
  vpdfl_pdf_base::b_write(bfs);
  vsl_b_write(bfs, component_);
  vsl_b_write(bfs, weight_);
}

//=======================================================================

void vpdfl_mixture::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  vcl_string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_mixture &) \n";
    vcl_cerr << "           Attempted to load object of type ";
    vcl_cerr << name <<" into object of type " << is_a() << vcl_endl;
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  delete_stuff();

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vpdfl_pdf_base::b_read(bfs);
      vsl_b_read(bfs, component_);
      vsl_b_read(bfs, weight_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_mixture &) \n";
      vcl_cerr << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//=======================================================================

double vpdfl_mixture::operator()(const vnl_vector<double>& x) const
{
  return vcl_exp(log_p(x));
}

//=======================================================================

double vpdfl_mixture::log_p(const vnl_vector<double>& x) const
{
  int n = n_components();

  vnl_vector<double>& log_ps = ws_;
  log_ps.resize(n);

  double max_log_p = 0.0; // initialise just to make the compiler happy
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

//=======================================================================

void vpdfl_mixture::gradient(vnl_vector<double>& g,
                                     const vnl_vector<double>& x,
                                     double& p) const
{
  vnl_vector<double>& g1 = ws_;

  double p1;
  component_[0]->gradient(g1,x,p1);
  g = g1*weight_[0];
  p = p1*weight_[0];

  for (unsigned int i=1;i<n_components();i++)
  {
    component_[i]->gradient(g1,x,p1);
    g += g1*weight_[i];
    double p_comp = p1*weight_[i];
    p += p_comp;
  }
}

//=======================================================================

unsigned vpdfl_mixture::nearest_comp(const vnl_vector<double>& x) const
{
  assert(component_.size()>=1);

  int n = n_components();
  if (n==1) return 0;

  int best_i=0;
  double min_d2 = vnl_vector_ssd(x, component_[0]->mean());;

  for (int i=1;i<n;i++)
  {
    double d2 = vnl_vector_ssd(x, component_[i]->mean());
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
void vpdfl_mixture::nearest_plausible(vnl_vector<double>& /*x*/, double /*log_p_min*/) const
{
  vcl_cerr << "ERROR: vpdfl_mixture::nearest_plausible NYI" << vcl_endl;
  vcl_abort();
}

//==================< end of file: vpdfl_mixture.cxx >====================
