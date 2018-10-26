// This is mul/clsfy/clsfy_binary_pdf_classifier.h
// Copyright: (C) 2000 British Telecommunications PLC
#ifndef clsfy_binary_pdf_classifier_h_
#define clsfy_binary_pdf_classifier_h_
//:
// \file
// \brief Describe a classifier based on a single pdf.
// \author Ian Scott

#include <iostream>
#include <iosfwd>
#include <clsfy/clsfy_classifier_base.h>
#include <vpdfl/vpdfl_pdf_base.h>
#include <vcl_cassert.h>
#include <vcl_compiler.h>

//:  Decisions are based on an explicit multivariate probability distribution
class clsfy_binary_pdf_classifier : public clsfy_classifier_base
{
 protected:
  //: The current distribution model
  vpdfl_pdf_base *pdf_;

  //: The value of log probability density marking the class boundary
  // Above this value the answer is 1, and below it is 0
  double log_prob_limit_;

 public:

  // default constructor
  clsfy_binary_pdf_classifier(): pdf_(nullptr), log_prob_limit_(0.0) {}

  //: A useful constructor
  // Specify the log probability density limit
  clsfy_binary_pdf_classifier(const vpdfl_pdf_base &pdf,
                              double log_prob_limit):
      pdf_(pdf.clone()), log_prob_limit_(log_prob_limit) {}

  // Destructor
  ~clsfy_binary_pdf_classifier() { deleteStuff(); }

  //: Classify the input vector
  // Returns either class1 (Inside PDF mode) or class 0 (Outside PDF mode).
  virtual unsigned classify(const vnl_vector<double> &input) const;

  //: Return the probability the input being in class 0.
  // output(0) contains the probability that the input is in class 1
  virtual void class_probabilities(std::vector<double> &outputs, const vnl_vector<double> &input) const;

  //: Log likelihood of being in class 0, i.e. const + log(P(class=0|data)).
  // The constant is chosen such that the decision boundary is at logL ==0;
  // This function is intended for binary classifiers only.
  // logL is related to class probability as P(class=0|data) = exp(logL) / (1+exp(logL))
  double log_l(const vnl_vector<double> &input) const;

  //: Set the log probability density limit,
  // above which the inputs are in class 1.
  void set_log_prob_limit(double limit)
  {
    log_prob_limit_ = limit;
  }

  //: The log probability density limit,
  // above which the inputs are in class 1.
  double log_prob_limit() const
  {
    return log_prob_limit_;
  }

  //: Set the PDF.
  // The class takes its own deep copy of the PDF.
  void set_pdf(const vpdfl_pdf_base &pdf)
  {
    deleteStuff();
    pdf_ = pdf.clone();
  }

  bool has_pdf() const
  {
    return pdf_!=nullptr;
  }
  //: Get the internal PDFs
  // The object will return a reference to the internal data.
  const vpdfl_pdf_base & pdf() const
  {
    assert(pdf_!=nullptr);
    return *pdf_;
  }

  //: The dimensionality of input vectors.
  virtual unsigned n_dims() const { assert(pdf_!=nullptr); return pdf_->n_dims();}

  //: The number of possible output classes.
  virtual unsigned n_classes() const {return 1;}

  //: Produce a deep copy.
  // client has responsibility for deletion.
  clsfy_classifier_base* clone() const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual std::string is_a() const;

  //: Name of the class
  virtual bool is_class(std::string const& s) const;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

  //: Save class to a binary File Stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load the class from a Binary File Stream
  virtual void b_read(vsl_b_istream& bfs);

 public:

  // Copy constructor
  clsfy_binary_pdf_classifier( const clsfy_binary_pdf_classifier& b ):
    clsfy_classifier_base(), pdf_(nullptr), log_prob_limit_(0.0) { *this = b; }

  //:Assignment operator
  clsfy_binary_pdf_classifier& operator=(const clsfy_binary_pdf_classifier& classifier);

 protected:

  // Delete members
  void deleteStuff();
};

#endif // clsfy_binary_pdf_classifier_h_
