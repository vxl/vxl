// This is mul/vpdfl/vpdfl_pdf_base.cxx
//:
// \file
// \author Tim Cootes
// \date 12-Apr-2001
// \brief Base class for Multi-Variate Probability Density Function classes.

#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <queue>
#include "vpdfl_pdf_base.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

#include <vpdfl/vpdfl_sampler_base.h>

//=======================================================================

vpdfl_pdf_base::vpdfl_pdf_base() = default;

//=======================================================================

vpdfl_pdf_base::~vpdfl_pdf_base() = default;

//=======================================================================

double vpdfl_pdf_base::operator()(const vnl_vector<double>& x) const
{
  return std::exp(log_p(x));
}


double vpdfl_pdf_base::log_prob_thresh(double pass_proportion) const
{
  assert(pass_proportion >= 0.0);
  assert(pass_proportion < 1.0);

  // The number of samples on the less likely side of the boundary.
  // Increase the number for greater reliabililty
  constexpr unsigned n_stat = 20;

  double /* above, */ below, lP;
  unsigned int nSamples, i;
  vnl_vector<double> x;

  vpdfl_sampler_base *sampler = new_sampler();
  if (pass_proportion > 0.5)
  {
    std::priority_queue<double, std::vector<double>, std::less<double> > pq;
    //We want at n_stat samples outside the cut-off.
    nSamples = (unsigned)(((double)n_stat / (1.0 - pass_proportion)) + 0.5);

    for (i = 0; i < n_stat+1; i++)
    {
      sampler->sample(x);
      pq.push(log_p(x));
    }

    for (; i < nSamples; i++)
    {
      sampler->sample(x);
      lP = log_p(x);
      // pq.top() should be the greatest value in the queue
      if (lP < pq.top())
      {
        pq.pop();
        pq.push(lP);
      }
    }
    // get two values either side of boundary;
#if 0
    above = pq.top();
#endif
    pq.pop();
    below = pq.top();
  }
  else
  {
    std::priority_queue<double, std::vector<double>, std::greater<double> > pq;
    //We want at n_stat samples inside the cut-off.
    nSamples = (unsigned)(((double)n_stat / pass_proportion) + 0.5);

    for (i = 0; i < n_stat+1; i++)
    {
      sampler->sample(x);
      pq.push(log_p(x));
    }

    for (; i < nSamples; i++)
    {
      sampler->sample(x);
      lP = log_p(x);
      if (lP > pq.top())  // pq.top() should be the smallest value in the queue.
      {
        pq.pop();
        pq.push(lP);
      }
    }
    // get two values either side of boundary;
#if 0
    above = pq.top();
#endif
    pq.pop();
    below = pq.top();
  }

  delete sampler;

  // Find geometric mean of probability densities to get boundary (arithmetic mean of logProbs.)
#if 0
  return (above + below)/2.0;
#else
  return below;
#endif
}

//: Gradient of log(p(x)) at x
//  Computes gradient df/dx of f(x)=log(p(x)) at x.
//  Default baseclass implementation uses gradient() to compute grad/p
void vpdfl_pdf_base::gradient_logp(vnl_vector<double>& g,
                                   const vnl_vector<double>& x) const
{
  double p;
  gradient(g,x,p);
  if (p==0.0)
    g.fill(0.0);  // Avoid division by zero.
  else
    g/=p;
}


//=======================================================================

bool vpdfl_pdf_base::is_valid_pdf() const
{
  return mean_.size() == var_.size() && mean_.size() > 0;
}


//=======================================================================

short vpdfl_pdf_base::version_no() const
{
  return 1;
}

//=======================================================================

void vsl_add_to_binary_loader(const vpdfl_pdf_base& b)
{
  vsl_binary_loader<vpdfl_pdf_base>::instance().add(b);
}

//=======================================================================

std::string vpdfl_pdf_base::is_a() const
{
  static std::string class_name_ = "vpdfl_pdf_base";
  return class_name_;
}

//=======================================================================

bool vpdfl_pdf_base::is_class(std::string const& s) const
{
  return s==vpdfl_pdf_base::is_a();
}

//=======================================================================

static void ShowStartVec(std::ostream& os, const vnl_vector<double>& v)
{
  unsigned int n = 3;
  if (n>v.size()) n=v.size();
  os<<'(';
  for (unsigned int i=0;i<n;++i) os<<v(i)<<' ';
  if (v.size()>n) os<<"...";
  os<<')';
}


  // required if data is present in this base class
void vpdfl_pdf_base::print_summary(std::ostream& os) const
{
  os <<  "N. Dims: "<< mean_.size();
  os <<  "  Mean: "; ShowStartVec(os, mean_);
  os <<  "  Variance: "; ShowStartVec(os, var_);
}

//=======================================================================

  // required if data is present in this base class
void vpdfl_pdf_base::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no());
  vsl_b_write(bfs, mean_);
  vsl_b_write(bfs, var_);
}

//=======================================================================

  // required if data is present in this base class
void vpdfl_pdf_base::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      vsl_b_read(bfs,mean_);
      vsl_b_read(bfs,var_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_pdf_base &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const vpdfl_pdf_base& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, vpdfl_pdf_base& b)
{
  b.b_read(bfs);
}

//=======================================================================

void vsl_print_summary(std::ostream& os,const vpdfl_pdf_base& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

void vsl_print_summary(std::ostream& os,const vpdfl_pdf_base* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No vpdfl_pdf_base defined.";
}

//=======================================================================

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const vpdfl_pdf_base& b)
{
  vsl_print_summary(os,b);
  return os;
}

//=======================================================================

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const vpdfl_pdf_base* b)
{
  vsl_print_summary(os,b);
  return os;
}
