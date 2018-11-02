// This is mul/pdf1d/pdf1d_compare_to_pdf_ks.h
#ifndef pdf1d_compare_to_pdf_ks_h
#define pdf1d_compare_to_pdf_ks_h

//:
// \file
// \author Tim Cootes
// \brief Test if data from a given distribution using Kolmogorov-Smirnov

#include <iostream>
#include <iosfwd>
#include <pdf1d/pdf1d_compare_to_pdf.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// \brief Test if data from a given distribution using Kolmogorov-Smirnov
class pdf1d_compare_to_pdf_ks : public pdf1d_compare_to_pdf
{
 public:

  //: Dflt ctor
  pdf1d_compare_to_pdf_ks();

  //: Destructor
  ~pdf1d_compare_to_pdf_ks() override;

  //: Test whether data came from the given distribution
  double compare(const double* data, int n, const pdf1d_pdf& pdf) override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  pdf1d_compare_to_pdf* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // pdf1d_compare_to_pdf_ks_h
