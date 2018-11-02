// This is mul/pdf1d/pdf1d_flat.cxx

//:
// \file
// \brief Univariate flat PDF.
// \author Tim Cootes

#include <string>
#include <iostream>
#include <cmath>
#include "pdf1d_flat.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <pdf1d/pdf1d_flat_sampler.h>
#include <pdf1d/pdf1d_sampler.h>

//=======================================================================

pdf1d_flat::pdf1d_flat()
{
  set(0,1);
}

pdf1d_flat::pdf1d_flat(double lo, double hi)
{
  set(lo,hi);
}

//=======================================================================

pdf1d_flat::~pdf1d_flat() = default;

//=======================================================================

//: Initialise
void pdf1d_flat::set(double lo, double hi)
{
  assert(hi>lo);
  lo_ = lo;
  hi_ = hi;

  double w = hi-lo;
  p_  = 1.0/(w);
  log_p_ = std::log(p_);

  pdf1d_pdf::set_mean(0.5*(lo+hi));
  pdf1d_pdf::set_variance(w*w/12.0);
}

//=======================================================================


pdf1d_sampler* pdf1d_flat::new_sampler() const
{
  auto *i = new pdf1d_flat_sampler;
  i->set_model(*this);
  return i;
}

//=======================================================================
//: Probability density at x
double pdf1d_flat::operator()(double x) const
{
  if (x>=lo_ && x<=hi_) return p_;
  return 0;
}


  // Probability densities:
double pdf1d_flat::log_p(double x) const
{
  if (x>=lo_ && x<=hi_) return log_p_;
  return 0;
}

//: Cumulative Probability (P(x'<x) for x' drawn from the distribution)
double pdf1d_flat::cdf(double x) const
{
  if (x<=lo_) return 0;
  if (x>=hi_) return 1;
  return p_*(x-lo_);
}

//: Return true if cdf() uses an analytic implementation
//  Default is false, as the base implementation is to draw samples
//  from the distribution randomly to estimate cdf(x)
bool pdf1d_flat::cdf_is_analytic() const
{
  return true;
}

//=======================================================================


double pdf1d_flat::gradient(double x,
                            double& p) const
{
  if (x>=lo_ && x<=hi_) p=p_;
  else                 p=0.0;
  return 0;
}

// ====================================================================


double pdf1d_flat::log_prob_thresh(double /*pass_proportion*/) const
{
  return log_p_;
}

//=======================================================================

double pdf1d_flat::nearest_plausible(double x, double /*log_p_min*/) const
{
  if (x>hi_) return hi_;
  if (x<lo_) return lo_;
  return x;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string pdf1d_flat::is_a() const
{
  static std::string class_name_ = "pdf1d_flat";
  return class_name_;
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_flat::is_class(std::string const& s) const
{
  return pdf1d_pdf::is_class(s) || s==pdf1d_flat::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short pdf1d_flat::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_pdf* pdf1d_flat::clone() const
{
  return new pdf1d_flat(*this);
}

//=======================================================================
// Method: print
//=======================================================================


void pdf1d_flat::print_summary(std::ostream& os) const
{
  os<<"Range ["<<lo_<<","<<hi_<<"]";
}

//=======================================================================
// Method: save
//=======================================================================

void pdf1d_flat::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,lo_);
  vsl_b_write(bfs,hi_);
}

//=======================================================================
// Method: load
//=======================================================================

void pdf1d_flat::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  std::string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_flat &)\n";
    std::cerr << "           Attempted to load object of type ";
    std::cerr << name <<" into object of type " << is_a() << std::endl;
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,lo_);
      vsl_b_read(bfs,hi_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_flat &)\n";
      std::cerr << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }

  set(lo_,hi_);
}

//==================< end of pdf1d_flat.cxx >====================
