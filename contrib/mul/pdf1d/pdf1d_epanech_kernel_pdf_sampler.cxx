// This is mul/pdf1d/pdf1d_epanech_kernel_pdf_sampler.cxx
#include <string>
#include <iostream>
#include <cmath>
#include <complex>
#include "pdf1d_epanech_kernel_pdf_sampler.h"
//:
// \file
// \brief Implements sampling for an Epanechnikov kernel PDF
// \author Ian Scott

//=======================================================================

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <pdf1d/pdf1d_sampler.h>

//=======================================================================

pdf1d_epanech_kernel_pdf_sampler::pdf1d_epanech_kernel_pdf_sampler():
  rng_(9667566ul)
{
}

pdf1d_epanech_kernel_pdf_sampler::~pdf1d_epanech_kernel_pdf_sampler() = default;

//=======================================================================

const pdf1d_epanech_kernel_pdf& pdf1d_epanech_kernel_pdf_sampler::epanech_kernel_pdf() const
{
  return static_cast<const pdf1d_epanech_kernel_pdf&>( model());
}

// ====================================================================

static const double root5 = 2.23606797749978969641;
static const double root3 = 1.73205080756887729353;

//: Transform a unit uniform distribution x into an Epanech distribution y
// $0 <= x <= 1  =>  -sqrt(5) <= y <= sqrt(5)$
// Matlab found 6 solutions to
// ${\frac {3}{20}}\,\left |{\it Dy}\right |\sqrt {5}\left (1-1/5\,{y}^{2}\right )=1$
// The 6th, which has the correct properties is
// $-1/2\,\sqrt [3]{-10\,\sqrt {5}x+5\,\sqrt {-5+20\,{x}^{2}}}-5/2\,
// {\frac {1}{\sqrt [3]{-10\,\sqrt {5}x+5\,\sqrt {-5+20\,{x}^{2}}}}}-1/2\,
// \sqrt {-1}\sqrt {3}\left (\sqrt [3]{-10\,\sqrt {5}x+5\,\sqrt {-5+20\,
// {x}^{2}}}-5\,{\frac {1}{\sqrt [3]{-10\,\sqrt {5}x+5\,\sqrt {-5+20\,
// {x}^{2}}}}}\right )$
double pdf1d_epanech_kernel_pdf_sampler::epan_transform(double x)
{
  // The Matlab code for the differential equation is
  // d = dsolve('1=std::abs(Dy) * (3 * std::sqrt(5) / 20) *(1-0.2*y*y)')
  x -= 0.5;
  const std::complex<double> z(10.0 * root5 * x, 5 * std::sqrt(5-20*x*x));
  const std::complex<double> cuberoot_z = std::pow(z,(1.0/3.0));
  const std::complex<double> recip_cuberoot_z = 1.0/cuberoot_z;
  const std::complex<double> im(0,1);

// The imaginary terms cancel out in theory, but not necessarily numerically - strip them.
  return std::real(-0.5 * cuberoot_z - 2.5 * recip_cuberoot_z -
                  im * root3 / 2.0 * ( cuberoot_z - 5.0 * recip_cuberoot_z));
}


// For generating plausible examples:
double pdf1d_epanech_kernel_pdf_sampler::sample()
{
  const pdf1d_epanech_kernel_pdf& kpdf = epanech_kernel_pdf();

  int n = kpdf.centre().size();

  int i = 0;
  if (n>1) i=rng_.lrand32(0,n-1);

  double x = epan_transform(rng_.drand32());


  x*=kpdf.width()[i];
  x+=kpdf.centre()[i];

  return x;
}

//: Fill x with samples possibly chosen so as to represent the distribution
//  Samples equal numbers from each kernel.
void pdf1d_epanech_kernel_pdf_sampler::regular_samples(vnl_vector<double>& x)
{
  const pdf1d_epanech_kernel_pdf& kpdf = epanech_kernel_pdf();
  int n = x.size();
  double* x_data = x.data_block();
  int nk = kpdf.centre().size();
  int n_per_k = n/nk;
  double lim = (n_per_k-1)/2.0;

  const double* c = kpdf.centre().data_block();
  const double* w = kpdf.width().data_block();

  for (int i=0;i<n;++i)
  {
    // Select components in order
    int j = i%nk;

    if (n_per_k>5)
    {
       x_data[i] = c[j]+epan_transform(rng_.drand32())*w[j];
    }
    else
    {
      // Spread points about
      // Note that this isn't quite right - should be equally spaced in CDF space
      int a = j/nk;
      double f = double(a)/(n_per_k-1);  // in [0,1]
      x_data[i] = c[j] + lim*(2*f-1)*w[j];
    }
  }
}

//=======================================================================

  //: Reseeds the static random number generator (one per derived class)
void pdf1d_epanech_kernel_pdf_sampler::reseed(unsigned long seed)
{
  rng_.reseed(seed);
}

//=======================================================================

std::string pdf1d_epanech_kernel_pdf_sampler::is_a() const
{
  return std::string("pdf1d_epanech_kernel_pdf_sampler");
}

//=======================================================================

bool pdf1d_epanech_kernel_pdf_sampler::is_class(std::string const& s) const
{
  return pdf1d_sampler::is_class(s) || s==pdf1d_epanech_kernel_pdf_sampler::is_a();
}

//=======================================================================

short pdf1d_epanech_kernel_pdf_sampler::version_no() const
{
  return 1;
}

//=======================================================================

pdf1d_sampler* pdf1d_epanech_kernel_pdf_sampler::clone() const
{
  return new pdf1d_epanech_kernel_pdf_sampler(*this);
}
