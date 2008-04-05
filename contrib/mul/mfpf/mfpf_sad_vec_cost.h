#ifndef mfpf_sad_vec_cost_h_
#define mfpf_sad_vec_cost_h_
//:
// \file
// \brief Computes weighted sum of absolute differences to a mean
// \author Tim Cootes

#include <mfpf/mfpf_vec_cost.h>
#include <vcl_iosfwd.h>

//: Computes weighted sum of absolute differences to a mean
class mfpf_sad_vec_cost : public mfpf_vec_cost
{
 private:
  //: Mean for vector 
  vnl_vector<double> mean_;

  //: Weights to apply to each element in sum
  vnl_vector<double> wts_;

 public:

  //: Dflt ctor
  mfpf_sad_vec_cost();

  //: Destructor
  virtual ~mfpf_sad_vec_cost();

  //: Define mean and weights
  void set(const vnl_vector<double>& mean,
           const vnl_vector<double>& wts);

  //: Mean for vector
  const vnl_vector<double>& mean() const { return mean_; }

  //: Weights to apply to each element in sum
  const vnl_vector<double>& wts() const { return wts_; }

  //: Evaluate weighted sum of absolute difference from mean
  virtual double evaluate(const vnl_vector<double>& v);

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
