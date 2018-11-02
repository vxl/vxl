// This is mul/clsfy/clsfy_rbf_svm.h
// Copyright: (C) 2000 British Telecommunications plc.
#ifndef clsfy_rbf_svm_h_
#define clsfy_rbf_svm_h_
//:
// \file
// \brief Describe a RBF Support Vector Machine
// \author Ian Scott
// \date Jan 2001
// \verbatim
//  Modifications
//   31 May 2001 IMS Converted to VXL
//   31 May 2001 IMS Merged with Finder/IS_OrderedSVM
// \endverbatim

#include <iostream>
#include <cmath>
#include <iosfwd>
#include <clsfy/clsfy_classifier_base.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A Support Vector Machine Binary Classifier.
class clsfy_rbf_svm : public clsfy_classifier_base
{
 protected:
  //: the starting upper bound.
  // If the function sum exceeds this value, the decision will definitely be 1.
  double upper_target_;
  //: the starting lower bound.
  // If the function sum falls below this value, the decision will definitely be 0.
  double lower_target_;

  //: The support vectors.
  std::vector<vnl_vector<double> > supports_;

  //: The Lagrangian multipliers
  // The values have been pre-multiplied by the +/-1.0 depending on the support target.
  std::vector<double> lagrangians_;

  //: The offset bias.
  double bias_;

  //: -1/(2 * sigma*sigma), where sigma = RBF kernel width
  double gamma_;

 public:

  //: Dflt constructor
  clsfy_rbf_svm();

  //: Destructor
  ~clsfy_rbf_svm() override;

  //: Classify the input vector
  // returns 0 to indicate out of (or negative) class and one to indicate in class (or positive.)
  unsigned classify(const vnl_vector<double> &input) const override;

  //: Return the probability the input being in each class.
  // output(i) i<<nClasses, contains the probability that the input
  // is in class i;
  // This are not strict probability values, since SVMs do not give Bayesian
  // outputs. However their properties fit the requirements of a probability.
  void class_probabilities(std::vector<double> &outputs,
                                   const vnl_vector<double> &input) const override;

  //: Log likelihood of being in class (binary classifiers only)
  // class probability = std::exp(logL) / (1+std::exp(logL)
  double log_l(const vnl_vector<double> &input) const override;

  //: Set the internal values defining the classifier.
  // \param supportVectors
  // \param lagrangianAlphas
  // \param labels These should be 0 or 1.
  // \param RBFWidth
  // \param bias
  virtual void set( const std::vector<vnl_vector<double> > & supportVectors,
                    const std::vector<double> & lagrangianAlphas,
                    const std::vector<unsigned> &labels,
                    double RBFWidth, double bias);

  //: The 1st standard deviation width of the RBF kernel.
  // Really this could be better named as the RBF radius.
  double rbf_width() const { return 1/std::sqrt(-2.0*gamma_);}

  //: The number of support vectors.
  unsigned n_support_vectors() const { return supports_.size();}

  //: The kernel function
  // Uses the SVM's current value of RBFWidth.
  double kernel(const vnl_vector<double> &v1, const vnl_vector<double> &v2) const;

  //: The SVM function bias.
  double bias() const {return bias_;}

  //: The Lagrange multipliers.
  // The values corresponding to negative training vectors are pre-multiplied by -1.
  // The array ordering corresponds to supportVectors()
  const std::vector<double> & lagrangians() const {return lagrangians_;}

  //: The support vectors.
  // The array ordering corresponds to lagrangians()
  const std::vector<vnl_vector<double> > & support_vectors() const {return supports_;}


  //: The number of possible output classes.
  unsigned n_classes() const override {return 1;}

  //: The dimensionality of input vectors.
  unsigned n_dims() const override {return supports_[0].size();}

  //: Return the class's IO version number
  short version_no() const;

  //: Name of the class
  std::string is_a() const override ;

  //: Name of the class
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  clsfy_classifier_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

 protected:
  //: Set the private target member values to the correct value.
  void calculate_targets();

  //: Calculate the contribution of a single support vector to the classifier;
  // This local version is used so that the number of calls can be tested.
  double localEuclideanDistanceSq(const vnl_vector<double> &a, const vnl_vector<double> &b) const
  {
#ifdef CLSFY_RBF_SVM_GLOBAL_COUNT_USE_OF_EUCLIDEAN_DISTANCE_SQ
    extern CLSFY_RBF_SVM_GLOBAL_COUNT_USE_OF_EUCLIDEAN_DISTANCE_SQ;
    CLSFY_RBF_SVM_GLOBAL_COUNT_USE_OF_EUCLIDEAN_DISTANCE_SQ ++;
#endif
    return vnl_vector_ssd(a, b);
  }
};

#endif // clsfy_rbf_svm_h_
