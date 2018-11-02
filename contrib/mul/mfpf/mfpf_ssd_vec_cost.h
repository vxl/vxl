#ifndef mfpf_ssd_vec_cost_h_
#define mfpf_ssd_vec_cost_h_
//:
// \file
// \brief Computes weighted sum of square differences to a mean
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_vec_cost.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Computes weighted sum of square differences to a mean
class mfpf_ssd_vec_cost : public mfpf_vec_cost
{
 private:
  //: Mean for vector
  vnl_vector<double> mean_;

  //: Weights to apply to each element in sum
  vnl_vector<double> wts_;

 public:

  //: Dflt ctor
  mfpf_ssd_vec_cost();

  //: Destructor
  ~mfpf_ssd_vec_cost() override;

  //: Define mean and weights
  void set(const vnl_vector<double>& mean,
           const vnl_vector<double>& wts);

  //: Mean for vector
  const vnl_vector<double>& mean() const { return mean_; }

  //: Weights to apply to each element in sum
  const vnl_vector<double>& wts() const { return wts_; }

  //: Evaluate weighted sum of absolute difference from mean
  double evaluate(const vnl_vector<double>& v) override;

  //: Return the mean
  void get_average(vnl_vector<double>& v) const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mfpf_vec_cost* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif
