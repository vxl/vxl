// This is mul/pdf1d/pdf1d_compare_to_pdf_bhat.h
#ifndef pdf1d_compare_to_pdf_bhat_h
#define pdf1d_compare_to_pdf_bhat_h

//:
// \file
// \author Tim Cootes
// \brief Test if data from a given distribution using Bhattacharyya overlap

#include <iostream>
#include <iosfwd>
#include <pdf1d/pdf1d_compare_to_pdf.h>
#include <mbl/mbl_cloneable_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Test if data from a given distribution using Bhattacharyya overlap
//  Uses builder() to build a pdf from supplied data.  This is then
//  tested against the target distribution using an estimate of the
//  Bhattacharyya overlap.
class pdf1d_compare_to_pdf_bhat : public pdf1d_compare_to_pdf
{
  //: Define method of building pdf from data
  mbl_cloneable_ptr<pdf1d_builder> builder_;

  //: Number of samples per data-point used in estimating overlap
  int n_per_point_;
 public:

  //: Dflt ctor
  pdf1d_compare_to_pdf_bhat();

  //: Construct and define method of building pdf from data
  // \param n_per_point : Number of samples per data-point used in estimating overlap
  pdf1d_compare_to_pdf_bhat(const pdf1d_builder& builder, int n_per_point);

  //: Destructor
  ~pdf1d_compare_to_pdf_bhat() override;

  //: Define method of building pdf from data
  void set_builder(const pdf1d_builder&);

  //: Method of building pdf from data
  const pdf1d_builder& builder() const { return builder_; }

  //: Number of samples per data-point used in estimating overlap
  int n_per_point() const { return n_per_point_; }

  //: Number of samples per data-point used in estimating overlap
  void set_n_per_point(int n);


  //: Test whether data came from the given distribution
  double compare(const double* data, int n, const pdf1d_pdf& pdf) override;

  //: Test whether data has form of the given distribution
  //  Repeatedly resamples n values from data[0..n-1] and
  //  calls compare_form().
  //  Individual comparisons are returned in B.
  //  \return Mean of B
  double bootstrap_compare_form(vnl_vector<double>& B,
                                        const double* data, int n,
                                        const pdf1d_builder& builder, int n_trials) override;

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

#endif // pdf1d_compare_to_pdf_bhat_h
