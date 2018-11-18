// This is mul/pdf1d/pdf1d_compare_to_pdf_ks.cxx

//:
// \file
// \author Tim Cootes
// \brief Test if data from a given distribution using Kolmogorov-Smirnov

#include <string>
#include <iostream>
#include <cmath>
#include "pdf1d_compare_to_pdf_ks.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_qsort.h>
#include <vnl/vnl_vector.h>

#include <pdf1d/pdf1d_prob_ks.h>

//=======================================================================
// Dflt ctor
//=======================================================================

pdf1d_compare_to_pdf_ks::pdf1d_compare_to_pdf_ks() = default;

//=======================================================================
// Destructor
//=======================================================================

pdf1d_compare_to_pdf_ks::~pdf1d_compare_to_pdf_ks() = default;

//=======================================================================

//: Test whether data came from the given distribution
double pdf1d_compare_to_pdf_ks::compare(const double* data, int n,
                                        const pdf1d_pdf& pdf)
{
  if (!pdf.cdf_is_analytic())
  {
    std::cerr<<"Warning: pdf1d_compare_to_pdf_ks::compare() "
            <<"Incorrectly assuming an analytic form for CDF.\n";
    // Should use slightly different implementation when estimating CDF using samples
    // Not yet implemented though.
  }

  // Sort the data
  vnl_vector<double> sorted_data(data,n);
  double *s = sorted_data.data_block();

  vbl_qsort_ascending(s,n);

  double d_max=0.0;
  double cdf_last = 0;

  for (int i=0;i<n;++i)
  {
    double cdf = pdf.cdf(s[i]);
    double data_cdf = (1.0+i)/n;
    double d = std::fabs(cdf-data_cdf);
    if (d>d_max) d_max=d;
    d = std::fabs(cdf_last-data_cdf);
    if (d>d_max) d_max=d;

    cdf_last = cdf;
  }

  double root_n = std::sqrt(double(n));
  return pdf1d_prob_ks((root_n+0.12+0.11/root_n)*d_max);
}


//=======================================================================
// Method: is_a
//=======================================================================

std::string pdf1d_compare_to_pdf_ks::is_a() const
{
  return std::string("pdf1d_compare_to_pdf_ks");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_compare_to_pdf_ks::is_class(std::string const& s) const
{
  return pdf1d_compare_to_pdf::is_class(s) || s==pdf1d_compare_to_pdf_ks::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short pdf1d_compare_to_pdf_ks::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_compare_to_pdf* pdf1d_compare_to_pdf_ks::clone() const
{
  return new pdf1d_compare_to_pdf_ks(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void pdf1d_compare_to_pdf_ks::print_summary(std::ostream& /*os*/) const
{
  std::cerr << "pdf1d_compare_to_pdf_ks::print_summary() NYI\n";
}

//=======================================================================
// Method: save
//=======================================================================

void pdf1d_compare_to_pdf_ks::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
}

//=======================================================================
// Method: load
//=======================================================================

void pdf1d_compare_to_pdf_ks::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_compare_to_pdf_ks &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
