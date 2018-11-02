// This is mul/mbl/mbl_ar_process.h
#ifndef mbl_ar_process_h_
#define mbl_ar_process_h_
//:
// \file
// \brief Compute the parameters of a second order autoregressive process.
// \author Franck Bettinger

#include <vector>
#include <string>
#include <iostream>
#include <iosfwd>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vsl/vsl_binary_io.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vnl_random;

//: Compute the parameters of a second order autoregressive process.
template<class T>
class mbl_ar_process
{
  vnl_matrix<T> A_2,A_1,B_0;
  vnl_vector<T> Xm; // mean

 public:

  //: Constructor
  mbl_ar_process();

  //: Destructor
  ~mbl_ar_process();

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const;

  //: Print class to os
  void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);

  //: Dynamic learning
  void learn(std::vector<vnl_vector<T> >& data);

  //: Learning using Burg's algorithm
  void learn_burg(std::vector<vnl_vector<T> >& data);

  //: Prediction
  // of a vector given the two previous vectors
  // \param rng Use an externally provided random number generator,
  // rather than one statically local to the function.
  vnl_vector<T> predict(vnl_vector<T>& Xm1, vnl_vector<T>& Xm2, vnl_random *rng=nullptr);
};

//: Write  to binary stream
template<class T>
void vsl_b_write(vsl_b_ostream& s, const mbl_ar_process<T>* p);

//: Read data from binary stream
template<class T>
void vsl_b_read(vsl_b_istream& s, mbl_ar_process<T>* & v);

//: Print class to os
template<class T>
void vsl_print_summary(std::ostream& os, const mbl_ar_process<T>* p);

#endif // mbl_ar_process_h_
