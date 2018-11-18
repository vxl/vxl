// This is mul/vpdfl/vpdfl_gaussian_kernel_pdf_sampler.cxx
//:
// \file
// \author Tim Cootes
// \brief Sampler class for gaussian kernel PDF.

#include "vpdfl_gaussian_kernel_pdf_sampler.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpdfl/vpdfl_gaussian_kernel_pdf.h>

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_gaussian_kernel_pdf_sampler::vpdfl_gaussian_kernel_pdf_sampler():
 rng_(9667566ul)
{
}

//=======================================================================
// Destructor
//=======================================================================

vpdfl_gaussian_kernel_pdf_sampler::~vpdfl_gaussian_kernel_pdf_sampler() = default;


//=======================================================================
// Method: is_a
//=======================================================================

std::string vpdfl_gaussian_kernel_pdf_sampler::is_a() const
{
  return std::string("vpdfl_gaussian_kernel_pdf_sampler");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_gaussian_kernel_pdf_sampler::is_class(std::string const& s) const
{
  return vpdfl_sampler_base::is_class(s) || s==vpdfl_gaussian_kernel_pdf_sampler::is_a();
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_sampler_base* vpdfl_gaussian_kernel_pdf_sampler::clone() const
{
  return new vpdfl_gaussian_kernel_pdf_sampler(*this);
}
//=======================================================================

void vpdfl_gaussian_kernel_pdf_sampler::reseed(unsigned long seed)
{
  rng_.reseed(seed);
}
//=======================================================================

//: Set model for which this is an instance
// Error check that it is an axis gaussian.
void vpdfl_gaussian_kernel_pdf_sampler::set_model(const vpdfl_pdf_base& model)
{
  assert(model.is_class("vpdfl_gaussian_kernel_pdf"));
  // cannot use dynamic_cast<> without rtti - PVr
  // rtti currently turned off
  vpdfl_sampler_base::set_model(model);
}

void vpdfl_gaussian_kernel_pdf_sampler::sample_component(vnl_vector<double>& x,
                                                         int j)
{
  const auto & kpdf = static_cast<const vpdfl_gaussian_kernel_pdf &>(model());

  int n_dims = kpdf.n_dims();
  x.set_size(n_dims);

  const double* m = kpdf.centre()[j].data_block();
  double w = kpdf.width()[j];

  double* x_data = x.data_block();
  for (int i=0;i<n_dims;++i)
    x_data[i] = m[i] + w*rng_.normal();
}


//=======================================================================

void vpdfl_gaussian_kernel_pdf_sampler::sample(vnl_vector<double>& x)
{
  const auto & kpdf = static_cast<const vpdfl_gaussian_kernel_pdf &>(model());
  int n = kpdf.centre().size();

  // Select component
  int j = 0;
  if (n>1) j=rng_.lrand32(0,n-1);

  sample_component(x,j);
}

//: Fill x with samples possibly chosen so as to represent the distribution
//  Sample sequentially from each component.
void vpdfl_gaussian_kernel_pdf_sampler::regular_samples(
                       std::vector<vnl_vector<double> >& x)
{
  const auto & kpdf = static_cast<const vpdfl_gaussian_kernel_pdf &>(model());
  int n_k = kpdf.centre().size();

  int n_samples = x.size();

  for (int i=0;i<n_samples;++i)
  {
    // Ensure sample at centre.  (This biases towards the centres somewhat)
//     if (i<n_k)
//       x[i]=kpdf.centre()[i];
//     else
      sample_component(x[i],i%n_k);
  }
}

//=======================================================================


//: Return a reference to the pdf model
// This is properly cast.
const vpdfl_gaussian_kernel_pdf& vpdfl_gaussian_kernel_pdf_sampler::gaussian_kernel_pdf() const
{
  return static_cast<const vpdfl_gaussian_kernel_pdf&>( model());
}
