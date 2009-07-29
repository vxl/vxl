#ifndef mfpf_log_lin_class_cost_h_
#define mfpf_log_lin_class_cost_h_
//:
// \file
// \brief Computes log prob based on output of a linear classifier
// \author Tim Cootes

#include <mfpf/mfpf_vec_cost.h>
#include <vcl_iosfwd.h>

//: Computes log prob based on output of a linear classifier
//  Linear classifier gives output z = w.x - bias
//  This returns -log(minp + (1-minp)/(1+exp(-z)))
//  minp defines the minimum prob output by classifier,
//  which should be non-zero to avoid danger of log(0)
class mfpf_log_lin_class_cost : public mfpf_vec_cost
{
 private:
  //: Classifier weights
  vnl_vector<double> wts_;

  //: Classifier bias
  double bias_;

  //: Minimum probability allowed in response
  double min_p_;
 public:

  //: Dflt ctor
  mfpf_log_lin_class_cost();

  //: Destructor
  virtual ~mfpf_log_lin_class_cost();

  //: Define weights, bias and minp
  void set(const vnl_vector<double>& wts, double bias, double min_p=1e-6);

  //: Weights to apply to each element in sum
  const vnl_vector<double>& wts() const { return wts_; }

  //: Returns -log(minp + (1-minp)/(1+exp(-(x.wts-bias)))
  virtual double evaluate(const vnl_vector<double>& x);

  //: Return the weights
  virtual void get_average(vnl_vector<double>& v) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_vec_cost* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif
