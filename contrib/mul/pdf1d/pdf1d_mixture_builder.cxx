// This is mul/pdf1d/pdf1d_mixture_builder.cxx
#include "pdf1d_mixture_builder.h"
//:
// \file
// \brief Implements builder for a mixture model PDF.
// \author Tim Cootes and Ian Scott

#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_binary_loader.h>
#include <pdf1d/pdf1d_mixture_sampler.h>
#include <pdf1d/pdf1d_mixture.h>
#include <mbl/mbl_data_wrapper.h>
#include <mbl/mbl_data_array_wrapper.h>

// Weights smaller than this are assumed to be zero
const double min_wt = 1e-8;

//=======================================================================
void pdf1d_mixture_builder::init()
{
  min_var_ = 1.0e-6;
  max_its_ = 10;
  weights_fixed_ = false;
}

//=======================================================================


pdf1d_mixture_builder::pdf1d_mixture_builder()
{
  init();
}

//=======================================================================
pdf1d_mixture_builder::pdf1d_mixture_builder(const pdf1d_mixture_builder& b):
  pdf1d_builder()
{
  init();
  *this = b;
}

//=======================================================================
pdf1d_mixture_builder& pdf1d_mixture_builder::operator=(const pdf1d_mixture_builder& b)
{
  if (&b==this) return *this;

  delete_stuff();

  int n = b.builder_.size();
  builder_.resize(n);
  for (int i=0;i<n;++i)
    builder_[i] = b.builder_[i]->clone();

  min_var_ = b.min_var_;
  max_its_ = b.max_its_;
  weights_fixed_ = b.weights_fixed_;

  return *this;
}

//=======================================================================

void pdf1d_mixture_builder::delete_stuff()
{
  int n = builder_.size();
  for (int i=0;i<n;++i)
    delete builder_[i];
  builder_.resize(0);
}

pdf1d_mixture_builder::~pdf1d_mixture_builder()
{
  delete_stuff();
}

//=======================================================================

//: Initialise n builders of type builder
//  Clone taken of builder
void pdf1d_mixture_builder::init(pdf1d_builder& builder, int n)
{
  delete_stuff();
  builder_.resize(n);
  for (int i=0;i<n;++i)
    builder_[i] = builder.clone();
}

//=======================================================================

//: Define maximum number of EM iterations allowed
void pdf1d_mixture_builder::set_max_iterations(int n)
{
  max_its_ = n;
}

//: Define whether weights on components can change or not
void pdf1d_mixture_builder::set_weights_fixed(bool b)
{
  weights_fixed_ = b;
}

//=======================================================================

//: Create empty model
pdf1d_pdf* pdf1d_mixture_builder::new_model() const
{
  return new pdf1d_mixture;
}

vcl_string pdf1d_mixture_builder::new_model_type() const
{
  return vcl_string("pdf1d_mixture");
}

//=======================================================================

//: Define lower threshold on variance for built models
void pdf1d_mixture_builder::set_min_var(double min_var)
{
  min_var_ = min_var;
}

//=======================================================================

//: Get lower threshold on variance for built models
double pdf1d_mixture_builder::min_var() const
{
  return min_var_;
}

//=======================================================================

//: Build default model with given mean
void pdf1d_mixture_builder::build(pdf1d_pdf& /*model*/, double /*mean*/) const
{
  vcl_cerr<<"pdf1d_mixture_builder::build(model,mean) not yet implemented.\n";
  vcl_abort();
}

//=======================================================================

//: Build model from data
void pdf1d_mixture_builder::build(pdf1d_pdf& model,
                                  mbl_data_wrapper<double>& data) const
{
  vcl_vector<double> wts(data.size());
  vcl_fill(wts.begin(),wts.end(),1.0);
  weighted_build(model,data,wts);
}

//=======================================================================

//: Build model from weighted data
void pdf1d_mixture_builder::weighted_build(pdf1d_pdf& base_model,
            mbl_data_wrapper<double>& data,
            const vcl_vector<double>& wts) const
{
  assert(base_model.is_class("pdf1d_mixture"));
  pdf1d_mixture& model = static_cast<pdf1d_mixture&>(base_model);

  unsigned int n = builder_.size();

  bool model_setup = (model.n_components()==n);

  if (!model_setup)
  {
    // Create default model components
    model.clear();
    model.components().resize(n);
    model.weights().resize(n);
    for (unsigned int i=0;i<n;++i)
    {
      builder_[i]->set_min_var(min_var_);
      model.components()[i] = builder_[i]->new_model();
      model.weights()[i]      = 1.0/n;
    }
  }

  // Get data into an array for rapid access
  const double* data_ptr;
  vcl_vector<double> data_array;

  {
    int n=data.size();
    data.reset();
    data_array.resize(n);
    for (int i=0;i<n;++i)
    {
      data_array[i] = data.current();
      data.next();
    }

    data_ptr = &data_array[0]/*.begin()*/;
  }

  if (!model_setup)
    initialise(model,data_ptr,wts);

  vcl_vector<vnl_vector<double> > probs;

  int n_its = 0;
  double max_move = 1e-6;
  double move = max_move+1;
  while (move>max_move && n_its<max_its_)
  {
    e_step(model,probs,data_ptr,wts);
    move = m_step(model,probs,data_ptr,wts);
    n_its++;
  }
  calc_mean_and_variance(model);
  assert(model.is_valid_pdf());
}

static void UpdateRange(double& min_v, double& max_v, double v)
{
  if (v<min_v) min_v=v;
  else
    if (v>max_v) max_v=v;
}

//=======================================================================
void pdf1d_mixture_builder::initialise(pdf1d_mixture& model,
          const double* data,
          const vcl_vector<double>& wts) const
{
  // Build each component using randomly weighted data
  int n_comp = builder_.size();
  int n_samples = wts.size();

  vcl_vector<double> wts_i(n_samples);

  // Compute range of data
  double min_v = data[0];
  double max_v = min_v;
  for (int i=1;i<n_samples;++i)
    UpdateRange(min_v,max_v,data[i]);

  // Create means equally distributed along range
  vcl_vector<double> mean(n_comp);
  for (int i=0;i<n_comp;++i)
  {
    double f = (i+1.0)/(n_comp+1);
    mean[i] = (1-f)*min_v + f*max_v;
  }

  double mean_sep = (max_v-min_v)/n_samples;

  mbl_data_array_wrapper<double> data_array(data,n_samples);

  for (int i=0;i<n_comp;++i)
  {
    // Compute weights proportional to inverse distance to the mean
    double w_sum = 0.0;
    for (int j=0;j<n_samples;++j)
    {
      wts_i[j] = mean_sep/(mean_sep + vcl_fabs(data[j]-mean[i]));
      w_sum+=wts_i[j];
    }

    // Normalise so weights add to n_samples/n_comp
    double f = double(n_samples)/(n_comp*w_sum);
    for (int j=0;j<n_samples;++j) wts_i[j]*=f;

    // Build i'th component, biasing data toward mean(i)
    builder_[i]->weighted_build(*(model.components()[i]),data_array,wts_i);
  }
}

//=======================================================================
void pdf1d_mixture_builder::e_step(pdf1d_mixture& model,
        vcl_vector<vnl_vector<double> >& probs,
        const double* data,
        const vcl_vector<double>& wts) const
{
  unsigned int n_comp = builder_.size();
  unsigned int n_egs = wts.size();
  const vcl_vector<double>& m_wts = model.weights();

  if (probs.size()!=n_comp) probs.resize(n_comp);

  // Compute log probs
  // probs(i)(j+1) is logProb that e.g. j was drawn from component i
  for (unsigned int i=0;i<n_comp;++i)
  {
    if (probs[i].size()!=n_egs) probs[i].set_size(n_egs);

    // Any components with zero weights are ignored.
    // Eventually they should be pruned.
    if (m_wts[i]<=0) continue;

    double *p_data = probs[i].begin();
    double log_wt_i = vcl_log(model.weights()[i]);

    for (unsigned int j=0;j<n_egs;++j)
      p_data[j] = log_wt_i + model.components()[i]->log_p(data[j]);
  }

  // Turn into probabilities and normalise.
  // Normalise so that sum_i probs(i)(j) = 1.0;
  for (unsigned int j=0;j<n_egs;++j)
  {
    // To minimise rounding errors, first find largest value
    double max_log_p = 0;
    for (unsigned int i=0;i<n_comp;++i)
    {
      if (m_wts[i]<=0) continue;
      if (i==0 || probs[i](j)>max_log_p) max_log_p = probs[i](j);
    }

    // Turn into probabilities and sum
    double sum = 0.0;
    for (unsigned int i=0;i<n_comp;++i)
    {
      if (m_wts[i]<=0) continue;
      double p = vcl_exp(probs[i](j)-max_log_p);
      probs[i](j) = p;
      sum+=p;
    }

    // Divide through by sum to normalise
    if (sum>0.0)
      for (unsigned int i=0;i<n_comp;++i)
        probs[i](j)/=sum;
  }
}

//=======================================================================
double pdf1d_mixture_builder::m_step(pdf1d_mixture& model,
        const vcl_vector<vnl_vector<double> >& probs,
        const double* data,
        const vcl_vector<double>& wts) const
{
  int n_comp = builder_.size();
  int n_egs = wts.size();
  vcl_vector<double> wts_i(n_egs);

  mbl_data_array_wrapper<double> data_array(data,n_egs);

  double move = 0.0;
  double old_mean;

  if (!weights_fixed_)
  {
    double w_sum = 0.0;
    // update the model weights
    for (int i=0;i<n_comp;++i)
    {
      model.weights()[i]=probs[i].mean();

      // Eliminate tiny components
      if (model.weights()[i]<min_wt) model.weights()[i]=0.0;

      w_sum += model.weights()[i];
    }

    // Ensure they add up to one
    for (int i=0;i<n_comp;++i)
      model.weights()[i]/=w_sum;
  }

  for (int i=0;i<n_comp;++i)
  {
    // Any components with zero weights are ignored.
    // Eventually they should be pruned.
    if (model.weights()[i]<=0) continue;

    // Compute weights
    const double* p = probs[i].begin();
    for (int j=0;j<n_egs;++j)
      wts_i[j] = wts[j]*p[j];

    old_mean = model.components()[i]->mean();
    builder_[i]->weighted_build(*(model.components()[i]), data_array, wts_i);

    move += vcl_fabs(old_mean-model.components()[i]->mean());
  }


  return move;
}

//=======================================================================

//: Calculate and set the mixture's mean and variance.
void pdf1d_mixture_builder::calc_mean_and_variance(pdf1d_mixture& model)
{
  double sum = 0;
  double sum2  = 0;

  unsigned i;
  for (i=0; i<model.n_components(); ++i)
  {
    double wi = model.weight(i);
    double mean_i = model.component(i).mean();
    sum += mean_i * wi;
    sum2 += (model.component(i).variance()+mean_i*mean_i)*wi;
  }

  double mean = sum;
  double var = sum2-mean*mean;

  model.set_mean_and_variance(mean, var);
}

//=======================================================================

vcl_string pdf1d_mixture_builder::is_a() const
{
  return vcl_string("pdf1d_mixture_builder");
}

//=======================================================================

bool pdf1d_mixture_builder::is_class(vcl_string const& s) const
{
  return pdf1d_builder::is_class(s) || s==pdf1d_mixture_builder::is_a();
}

//=======================================================================

short pdf1d_mixture_builder::version_no() const
{
  return 1;
}

//=======================================================================

pdf1d_builder* pdf1d_mixture_builder::clone() const
{
  return new pdf1d_mixture_builder(*this);
}

//=======================================================================

void pdf1d_mixture_builder::print_summary(vcl_ostream& os) const
{
  for (unsigned int i=0;i<builder_.size();++i)
  {
    os<<'\n'<<vsl_indent()<<"Builder "<<i<<": ";
    vsl_print_summary(os, builder_[i]);
  }
  os<<vcl_endl;
}

//=======================================================================

void pdf1d_mixture_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,builder_);
  vsl_b_write(bfs,max_its_);
  vsl_b_write(bfs,weights_fixed_);
}

//=======================================================================

void pdf1d_mixture_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  vcl_string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_mixture_builder &)\n"
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
      vsl_b_read(bfs,builder_);
      vsl_b_read(bfs,max_its_);
      vsl_b_read(bfs,weights_fixed_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_mixture_builder &)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//==================< end of file: pdf1d_mixture_builder.cxx >====================
