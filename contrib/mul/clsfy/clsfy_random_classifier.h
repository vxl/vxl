// This is mul/clsfy/clsfy_random_classifier.h
#ifndef clsfy_random_classifier_h_
#define clsfy_random_classifier_h_
// Copyright: (C) 2001 British Telecommunications plc
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Describe a random classifier
// \author Ian Scott
// \date 2000/05/10
// \verbatim
//  Modifications
//  2 May 2001 IMS Converted to VXL
// \endverbatim

#include "clsfy_classifier_base.h"
#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <mbl/mbl_mz_random.h>
#include <vsl/vsl_binary_io.h>

//:  A common interface for 1-out-of-N classifiers
// This class takes a vector and classifies into one of
// N classes.
//
// And derived classes with binary in the name indicates that
// the classifier works with only two classes, 0 and 1.

class clsfy_random_classifier : public clsfy_classifier_base
{
 public:
  // Dflt constructor
  clsfy_random_classifier();

  // Destructor
  virtual ~clsfy_random_classifier() {}

  //: Return the probability the input being in each class.
  // output(i) i<nClasses, contains the probability that the input is in class i
  virtual void class_probabilities(vcl_vector<double> &outputs, const vnl_vector<double> &input) const;

  //: Log likelihood of being in class (binary classifiers only)
  // class probability = 1 / (1+exp(-log_l))
  // Operation of this method is undefined for multiclass classifiers
  virtual double log_l(const vnl_vector<double> &input) const;

  //: The number of possible output classes.
  virtual unsigned n_classes() const;

  //: The dimensionality of input vectors.
  virtual unsigned n_dims() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_classifier_base* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: The probabilities of returning a value in each class.
  const vcl_vector<double> & probs() const;

  //: Set the prior probabilities of each class
  // The values are normalised to sum to 1.
  void set_probs(const vcl_vector<double> &);

  //: Set the number of dimensions the classifier reports that it uses.
  // The classifier itself pays no attention to this value, but it
  // may be useful for other error checking code which calls n_dims()
  void set_n_dims(unsigned);

  //: The mean confidence noise added to class probabilities.
  double confidence() const;

  //: Set the mean of confidence noise added to class probabilities.
  // If the value is 0.0, then the probability of the winning class, will
  // only be just large enough to guarantee the win. Larger values will allow
  // more confident winning class probabilities - however each actual
  // confidence increase is random.
  void set_confidence(double);

  //: Reseeds the internal random number generator
  // To achieve quasi-random initialisation use
  // \code
  // #include <vcl_ctime.h>
  // ..
  // sampler.reseed(vcl_time(0));
  // \endcode
  virtual void reseed(unsigned long);

 private:
  //: Calculate the minimum value each class probability needs to be biased by to win
  void calc_min_to_win();

  //: The probabilities of each class.
  // The values will always sum to 1.
  // If the vector is empty then the builder will use the prior probability
  vcl_vector<double> probs_;

  //: The mean confidence noise added to class probabilities
  double confidence_;

  //: The classifier may get asked this
  unsigned n_dims_;

  //: Give the same answers if the same vector is presented twice in a row
  mutable vnl_vector<double> last_inputs_;

  //: The last class probabilities calculated.
  mutable vcl_vector<double> last_outputs_;

  //: The random number generator used to sample classes.
  mutable mbl_mz_random rng_;

  //: The minimum value each class probability needs to be biased by to win.
  vcl_vector<double> min_to_win_;
};

#endif // clsfy_random_classifier_h_
