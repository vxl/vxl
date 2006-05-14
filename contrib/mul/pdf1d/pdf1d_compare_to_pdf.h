// This is mul/pdf1d/pdf1d_compare_to_pdf.h
#ifndef pdf1d_compare_to_pdf_h
#define pdf1d_compare_to_pdf_h

//:
// \file
// \brief Base for classes with test whether data could come from a given pdf.
// \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include <pdf1d/pdf1d_pdf.h>
#include <pdf1d/pdf1d_builder.h>
#include <vnl/vnl_fwd.h>
#include <mbl/mbl_cloneable_ptr.h>

//=======================================================================

//: Base for classes with test whether data could come from a given pdf.
// Functions are available to test whether a set of samples are likely
// to have been drawn from either a particular distribution or a given
// form of distribution.
class pdf1d_compare_to_pdf
{
 protected:
  //: Workspace for PDFs
  mbl_cloneable_ptr<pdf1d_pdf> pdf_;
 public:

  //: Dflt ctor
  pdf1d_compare_to_pdf();

  //: Destructor
  virtual ~pdf1d_compare_to_pdf();

  //: Test whether data came from the given distribution
  virtual double compare(const double* data, int n, const pdf1d_pdf& pdf) =0;

  //: Test whether data came from the given distribution, using bootstrap
  //  Repeatedly resamples n values from data[0..n-1] and compares with
  //  the given pdf.  Individual comparisons are returned in B.
  //  \return Mean of B
  virtual double bootstrap_compare(vnl_vector<double>& B,
                                   const double* data, int n,
                                   const pdf1d_pdf& pdf, int n_trials);

  //: Test whether data has form of the given distribution
  //  Default behaviour is to build pdf from data and then compare data with pdf
  virtual double compare_form(const double* data, int n,
                              const pdf1d_builder& builder);

  //: Test whether data has form of the given distribution
  //  Repeatedly resamples n values from data[0..n-1] and
  //  calls compare_form().
  //  Individual comparisons are returned in B.
  //  \return Mean of B
  virtual double bootstrap_compare_form(vnl_vector<double>& B,
                              const double* data, int n,
                              const pdf1d_builder& builder, int n_trials);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_compare_to_pdf* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;
};

//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "bfs>>base_ptr;".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const pdf1d_compare_to_pdf& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const pdf1d_compare_to_pdf& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, pdf1d_compare_to_pdf& b);

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const pdf1d_compare_to_pdf& b);

//: Stream output operator for class pointer
void vsl_print_summary(vcl_ostream& os,const pdf1d_compare_to_pdf* b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const pdf1d_compare_to_pdf& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const pdf1d_compare_to_pdf* b);

#endif // pdf1d_compare_to_pdf_h
