// This is mul/vpdfl/vpdfl_mixture_builder.cxx
//=======================================================================
//
//  Copyright: (C) 2000 Victoria University of Manchester
//
//=======================================================================
#include <sstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "vpdfl_mixture_builder.h"
//:
// \file
// \brief Implements builder for a mixture model PDF.
// \author Tim Cootes
// \date 21-July-98
//
// Modifications
// \verbatim
//    IMS   Converted to VXL 14 May 2000, with redesign
// \endverbatim

#include <vcl_cassert.h>
#include <vcl_compiler.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_binary_loader.h>
#include <vpdfl/vpdfl_mixture.h>
#include <mbl/mbl_data_wrapper.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vnl/vnl_math.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <vul/vul_string.h>
#include <mbl/mbl_exception.h>

// Weights smaller than this are assumed to be zero
const double min_wt = 1e-8;

//=======================================================================
void vpdfl_mixture_builder::init()
{
  min_var_ = 1.0e-6;
  max_its_ = 10;
  weights_fixed_ = false;
  initial_means_.clear();
}

//=======================================================================

vpdfl_mixture_builder::vpdfl_mixture_builder()
{
  init();
}

//=======================================================================

vpdfl_mixture_builder::vpdfl_mixture_builder(const vpdfl_mixture_builder& b):
  vpdfl_builder_base()
{
  init();
  *this = b;
}

//=======================================================================

vpdfl_mixture_builder& vpdfl_mixture_builder::operator=(const vpdfl_mixture_builder& b)
{
  if (&b==this) return *this;

  delete_stuff();

  unsigned int n = b.builder_.size();
  builder_.resize(n);
  for (unsigned int i=0;i<n;++i)
    builder_[i] = b.builder_[i]->clone();

  min_var_ = b.min_var_;
  max_its_ = b.max_its_;
  weights_fixed_ = b.weights_fixed_;
  initial_means_ = b.initial_means_;

  return *this;
}

//=======================================================================

void vpdfl_mixture_builder::delete_stuff()
{
  unsigned int n = builder_.size();
  for (unsigned int i=0;i<n;++i)
    delete builder_[i];
  builder_.resize(0);
  initial_means_.clear();
}

vpdfl_mixture_builder::~vpdfl_mixture_builder()
{
  delete_stuff();
}

//=======================================================================

//: Initialise n builders of type builder
//  Clone taken of builder
void vpdfl_mixture_builder::init(const vpdfl_builder_base& builder, int n)
{
  delete_stuff();
  builder_.resize(n);
  for (int i=0;i<n;++i)
    builder_[i] = builder.clone();
}

//=======================================================================

//: Define maximum number of EM iterations allowed
void vpdfl_mixture_builder::set_max_iterations(int n)
{
  max_its_ = n;
}
//: Define whether weights on components can change or not
void vpdfl_mixture_builder::set_weights_fixed(bool b)
{
  weights_fixed_ = b;
}

//=======================================================================

//: Create empty model
vpdfl_pdf_base* vpdfl_mixture_builder::new_model() const
{
  return new vpdfl_mixture;
}

//=======================================================================

//: Define lower threshold on variance for built models
void vpdfl_mixture_builder::set_min_var(double min_var)
{
  min_var_ = min_var;
}

//=======================================================================

//: Get lower threshold on variance for built models
double vpdfl_mixture_builder::min_var() const
{
  return min_var_;
}

//=======================================================================

//: Build default model with given mean
void vpdfl_mixture_builder::build(vpdfl_pdf_base& /*model*/,
                                  const vnl_vector<double>& /*mean*/) const
{
  std::cerr<<"vpdfl_mixture_builder::build(model,mean) Not yet implemented.\n";
  std::abort();
}

//=======================================================================

//: Build model from data
void vpdfl_mixture_builder::build(vpdfl_pdf_base& model,
                                  mbl_data_wrapper<vnl_vector<double> >& data) const
{
  std::vector<double> wts(int(data.size()), 1.0);
  weighted_build(model,data,wts);
}

//=======================================================================

//: Build model from weighted data
void vpdfl_mixture_builder::weighted_build(vpdfl_pdf_base& base_model,
                                           mbl_data_wrapper<vnl_vector<double> >& data,
                                           const std::vector<double>& wts) const
{
  assert(base_model.is_class("vpdfl_mixture"));
  auto& model = static_cast<vpdfl_mixture&>( base_model);

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

  // Get vectors into an array for rapid access
  const vnl_vector<double>* data_ptr;
  std::vector<vnl_vector<double> > data_array;

  {
    unsigned int n=data.size();
    data.reset();
    data_array.resize(n);
    for (unsigned int i=0;i<n;++i)
    {
      data_array[i] = data.current();
      data.next();
    }

    data_ptr = &data_array[0]/*.begin()*/;
  }

  if (!model_setup || !initial_means_.empty())
    initialise(model,data_ptr,wts);

  std::vector<vnl_vector<double> > probs;

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

static void UpdateRange(vnl_vector<double>& min_vec, vnl_vector<double>& max_vec, const vnl_vector<double>& vec)
{
  unsigned int n=vec.size();
  for (unsigned int i=0;i<n;++i)
  {
    if (vec(i)<min_vec(i))
      min_vec(i)=vec(i);
    else
    if (vec(i)>max_vec(i))
      max_vec(i)=vec(i);
  }
}

//: Assumes means set up.  Estimates starting components.
void vpdfl_mixture_builder::initialise_given_means(vpdfl_mixture& model,
                                                   const vnl_vector<double>* data,
                                                   const std::vector<vnl_vector<double> >& mean,
                                                   const std::vector<double>& wts) const
{
  const unsigned int n_comp = builder_.size();
  const unsigned int n_samples = wts.size();

  // Compute range of data
  vnl_vector<double> min_v(mean[0]);
  vnl_vector<double> max_v(min_v);
  for (unsigned int i=1;i<n_comp;++i)
    UpdateRange(min_v,max_v,mean[i]);

  double mean_sep = vnl_vector_ssd(max_v,min_v)/n_samples;
  if (mean_sep<=1e-6) mean_sep = 1e-6;


  std::vector<double> wts_i(n_samples);

  mbl_data_array_wrapper<vnl_vector<double> > data_array(data,n_samples);

  for (unsigned int i=0;i<n_comp;++i)
  {
    // Compute weights proportional to inverse square to the mean
    double w_sum = 0.0;
    for (unsigned int j=0;j<n_samples;++j)
    {
      wts_i[j] = wts[j]*mean_sep/(mean_sep+ vnl_vector_ssd(data[j], mean[i]));
      w_sum+=wts_i[j];
    }

    // Normalise so weights add to n_samples/n_comp
    double f = n_samples/(n_comp*w_sum);
    for (unsigned int j=0;j<n_samples;++j)
      wts_i[j]*=f;

    // Build i'th component, biasing data toward mean(i)
    builder_[i]->weighted_build(*(model.components()[i]),data_array,wts_i);
  }
}

//=======================================================================

void vpdfl_mixture_builder::initialise_diagonal(vpdfl_mixture& model,
                                                const vnl_vector<double>* data,
                                                const std::vector<double>& wts) const
{
  // Build each component using randomly weighted data
  const unsigned int n_comp = builder_.size();
  const unsigned int n_samples = wts.size();

  // Compute range of data
  vnl_vector<double> min_v(data[0]);
  vnl_vector<double> max_v(min_v);
  for (unsigned int i=1;i<n_samples;++i)
    UpdateRange(min_v,max_v,data[i]);

#if 0 // unused variable
  double mean_sep = vnl_vector_ssd(max_v,min_v)/n_samples;
#endif

  // Create means along diagonal of bounding box
  std::vector<vnl_vector<double> > mean(n_comp);
  for (unsigned int i=0;i<n_comp;++i)
  {
    double f = (i+1.0)/(n_comp+1);
    mean[i] = (1-f)*min_v + f*max_v;
  }

  initialise_given_means(model,data,mean,wts);
}

//=======================================================================

void vpdfl_mixture_builder::initialise_to_regular_samples(vpdfl_mixture& model,
                                                          const vnl_vector<double>* data,
                                                          const std::vector<double>& wts) const
{
  // Build each component using randomly weighted data
  const unsigned int n_comp = builder_.size();
  const unsigned int n_samples = wts.size();

  double f = double(n_samples)/n_comp;

  // Select means from data
  std::vector<vnl_vector<double> > mean(n_comp);
  for (unsigned int i=0;i<n_comp;++i)
  {
    unsigned int j = vnl_math::rnd((i+0.5)*f); // must not be negative!
    if (j>=n_samples) j=n_samples-1;
      mean[i] = data[j];
  }

  initialise_given_means(model,data,mean,wts);
}

void vpdfl_mixture_builder::initialise(vpdfl_mixture& model,
                                       const vnl_vector<double>* data,
                                       const std::vector<double>& wts) const
{
  // Later add a switch to decide on how to initialise
  if (!initial_means_.empty() )
  {
    initialise_given_means(model,data,initial_means_,wts);
  }
  else
  {
    initialise_to_regular_samples(model,data,wts);
  }
}

void vpdfl_mixture_builder::preset_initial_means(const std::vector<vnl_vector<double> >& component_means)
{
    initial_means_ = component_means;
}
//=======================================================================

void vpdfl_mixture_builder::e_step(vpdfl_mixture& model,
                                   std::vector<vnl_vector<double> >& probs,
                                   const vnl_vector<double>* data,
                                   const std::vector<double>& wts) const
{
  const unsigned int n_comp = builder_.size();
  const unsigned int n_egs = wts.size();
  const std::vector<double>& m_wts = model.weights();

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

    double log_wt_i = std::log(m_wts[i]);

    for (unsigned int j=0;j<n_egs;++j)
    {
      p_data[j] = log_wt_i+model.components()[i]->log_p(data[j]);
    }
  }

  // Turn into probabilities and normalise.
  // Normalise so that sum_i probs(i)(j) = 1.0;
  for (unsigned int j=0;j<n_egs;++j)
  {
    // To minimise rounding errors, first find largest value
    double max_log_p=0;
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
      double p = std::exp(probs[i](j)-max_log_p);
      probs[i](j) = p;
      sum+=p;
    }

    // Divide through by sum to normalise
    if (sum>0.0)
      for (unsigned int i=0;i<n_comp;++i)
        probs[i](j)/=sum;

    if (sum<=0)
      std::cerr<<"vpdfl_mixture_builder::e_step() Zero sum for probs!\n";
  }
}

//=======================================================================

double vpdfl_mixture_builder::m_step(vpdfl_mixture& model,
                                     const std::vector<vnl_vector<double> >& probs,
                                     const vnl_vector<double>* data,
                                     const std::vector<double>& wts) const
{
  const unsigned int n_comp = builder_.size();
  const unsigned int n_egs = wts.size();
  std::vector<double> wts_i(n_egs);

  mbl_data_array_wrapper<vnl_vector<double> > data_array(data,n_egs);

  double move = 0.0;
  vnl_vector<double> old_mean;

  if (!weights_fixed_)
  {
    double w_sum = 0.0;
    // update the model weights
    for (unsigned int i=0;i<n_comp;++i)
    {
      model.weights()[i]=probs[i].mean();

      // Eliminate tiny components
      if (model.weights()[i]<min_wt) model.weights()[i]=0.0;

      w_sum += model.weights()[i];
    }

    // Ensure they add up to one
    for (unsigned int i=0;i<n_comp;++i)
    model.weights()[i]/=w_sum;
  }

  for (unsigned int i=0;i<n_comp;++i)
  {
    // Any components with zero weights are ignored.
  // Eventually they should be pruned.
    if (model.weights()[i]<=0.0) continue;

    // Compute weights
    const double* p = probs[i].begin();
    double w_sum = 0.0;
    for (unsigned int j=0;j<n_egs;++j)
    {
      wts_i[j] = wts[j]*p[j];
      w_sum += wts_i[j];
    }

    if (w_sum<=0.0)
      std::cerr<<"m_step: Dubious weights. sum="<<w_sum<<'\n';

    old_mean = model.components()[i]->mean();
    builder_[i]->weighted_build(*(model.components()[i]), data_array, wts_i);

    move += vnl_vector_ssd(old_mean, model.components()[i]->mean());
  }


  return move;
}

//=======================================================================

//: Add Y*v to X
static inline void incXbyYv(vnl_vector<double> *X, const vnl_vector<double> &Y, double v)
{
  assert(X->size() == Y.size());
  int i = ((int)X->size()) - 1;
  double * const pX=X->data_block();
  while (i >= 0)
  {
    pX[i] += Y[i] * v;
    i--;
  }
}

//: Add (Y + Z.*Z)*v to X
static inline void incXbyYplusXXv(vnl_vector<double> *X, const vnl_vector<double> &Y,
                                  const vnl_vector<double> &Z, double v)
{
  assert(X->size() == Y.size());
  int i = ((int)X->size()) - 1;
  double * const pX=X->data_block();
  while (i >= 0)
  {
    pX[i] += (Y[i] + vnl_math::sqr(Z[i]))* v;
    i--;
  }
}


//: Calculate and set the mixture's mean and variance.
void vpdfl_mixture_builder::calc_mean_and_variance(vpdfl_mixture& model)
{
  unsigned int n = model.component(0).mean().size();
  vnl_vector<double> mean(n, 0.0);
  vnl_vector<double> var(n, 0.0);

  for (unsigned int i=0; i<model.n_components(); ++i)
  {
    incXbyYv(&mean, model.component(i).mean(), model.weight(i));
    incXbyYplusXXv(&var, model.component(i).variance(),
                   model.component(i).mean(), model.weight(i));
  }

  for (unsigned int i=0; i<n; ++i)
    var(i) -= vnl_math::sqr(mean(i));

  model.set_mean_and_variance(mean, var);
}

//=======================================================================

std::string vpdfl_mixture_builder::is_a() const
{
  return std::string("vpdfl_mixture_builder");
}

//=======================================================================

bool vpdfl_mixture_builder::is_class(std::string const& s) const
{
  return vpdfl_builder_base::is_class(s) || s==vpdfl_mixture_builder::is_a();
}

//=======================================================================

short vpdfl_mixture_builder::version_no() const
{
  return 1;
}

//=======================================================================

vpdfl_builder_base* vpdfl_mixture_builder::clone() const
{
  return new vpdfl_mixture_builder(*this);
}

//=======================================================================

void vpdfl_mixture_builder::print_summary(std::ostream& os) const
{
  if (weights_fixed_) os<<vsl_indent()<<"Weights fixed"<<'\n';
  else                os<<vsl_indent()<<"Weights may vary"<<'\n';
  os<<vsl_indent()<<"Max iterations: "<<max_its_<<'\n';
  for (unsigned int i=0;i<builder_.size();++i)
  {
    os<<vsl_indent()<<"Builder "<<i<<": ";
    vsl_print_summary(os, builder_[i]); os << '\n';
  }
}

//=======================================================================

void vpdfl_mixture_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,builder_);
  vsl_b_write(bfs,max_its_);
  vsl_b_write(bfs,weights_fixed_);
}

//=======================================================================

void vpdfl_mixture_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  std::string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_mixture_builder &)\n"
             << "           Attempted to load object of type "
             << name <<" into object of type " << is_a() << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
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
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_mixture_builder &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
//: Read initialisation settings from a stream.
// Parameters:
// \verbatim
// {
//   min_var: 1.0e-6
//   n_pdfs: 3
//   // Type of basis pdf
//   basis_pdf: axis_gaussian { min_var: 0.0001 }
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.
void vpdfl_mixture_builder::config_from_stream(std::istream & is)
{
  std::string s = mbl_parse_block(is);

  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  double mv=1.0e-6;
  if (props.find("min_var")!=props.end())
  {
    mv=vul_string_atof(props["min_var"]);
    props.erase("min_var");
  }
  set_min_var(mv);

  unsigned n_pdfs = 2;
  if (props.find("n_pdfs")!=props.end())
  {
    n_pdfs=vul_string_atoi(props["n_pdfs"]);
    props.erase("n_pdfs");
  }

  max_its_=10;
  if (props.find("max_its")!=props.end())
  {
    max_its_=vul_string_atoi(props["max_its"]);
    props.erase("max_its");
  }

  weights_fixed_=false;
  if (props.find("weights_fixed")!=props.end())
  {
    weights_fixed_=vul_string_to_bool(props["weights_fixed"]);
    props.erase("weights_fixed");
  }

  if (props.find("basis_pdf")!=props.end())
  {
    std::istringstream pdf_ss(props["basis_pdf"]);
    std::unique_ptr<vpdfl_builder_base>
            b = vpdfl_builder_base::new_pdf_builder_from_stream(pdf_ss);
    init(*b,n_pdfs);
    props.erase("basis_pdf");
  }

  try
  {
    mbl_read_props_look_for_unused_props(
        "vpdfl_mixture_builder::config_from_stream", props);
  }

  catch(mbl_exception_unused_props &e)
  {
    throw mbl_exception_parse_error(e.what());
  }
}

//==================< end of file: vpdfl_mixture_builder.cxx >====================
