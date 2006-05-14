// This is mul/pdf1d/pdf1d_compare_to_pdf_ks.h
#ifndef pdf1d_compare_to_pdf_ks_h
#define pdf1d_compare_to_pdf_ks_h

//:
// \file
// \author Tim Cootes
// \brief Test if data from a given distribution using Kolmogorov-Smirnov

#include <pdf1d/pdf1d_compare_to_pdf.h>
#include <vcl_iosfwd.h>

// \brief Test if data from a given distribution using Kolmogorov-Smirnov
class pdf1d_compare_to_pdf_ks : public pdf1d_compare_to_pdf
{
 public:

  //: Dflt ctor
  pdf1d_compare_to_pdf_ks();

  //: Destructor
  virtual ~pdf1d_compare_to_pdf_ks();

  //: Test whether data came from the given distribution
  virtual double compare(const double* data, int n, const pdf1d_pdf& pdf);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_compare_to_pdf* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // pdf1d_compare_to_pdf_ks_h
