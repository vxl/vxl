#ifndef mbl_stochastic_data_collector_h_
#define mbl_stochastic_data_collector_h_
//:
// \file
// \brief Describe class that collects random subset of arbitrary length data.
// \author Ian Scott
//=======================================================================

#include <vector>
#include <iostream>
#include <iosfwd>
#include <mbl/mbl_data_collector.h>
#include <mbl/mbl_data_array_wrapper.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>

//=======================================================================


//: Collects vectors, but only stores a subsample of them.
// This is useful if you are unsure how many vectors examples a routine might store,
// but you only want a randomly selected fixed number of them.
// The stored vectors are (in the limit) unbiased w.r.t. the order in which they were presented,
//
// If calculating the values to be stored is expensive, this class
// can be used as follows.
// \code
// mbl_stochastic_data_collector<double> c(100);
// while (..)
// {
//   if (c.store_next()) c.force_record(f());
// }
// \endcode
template <class T>
class mbl_stochastic_data_collector: public mbl_data_collector<T>
{
 private:
  //: Recorded samples are stored here
  std::vector<T > samples_;

  //: Provides iterator access to the data via data_wrapper()
  mbl_data_array_wrapper<T > v_data_;

  //: The number of samples presented to record() so far.
  unsigned long nPresented_;

  //: Random number generator used to decide whether to store a particular vector.
  vnl_random rand;

 public:

  //: Dflt ctor
  mbl_stochastic_data_collector();

  //: Set number of samples to be stored.
  // This is the number of vectors that can be actually retrieved.
  explicit mbl_stochastic_data_collector(unsigned n);

  //: Destructor
  ~mbl_stochastic_data_collector() override;

  //: Clear any stored data
  void clear() override;

  //: Set number of samples to be stored
  // If not set, the value defaults to 1000.
  // Calling this function implicitly calls clean().
  void set_n_samples(int n) override;

  //: Record given value
  void record(const T& v) override;

  //: Force recording of this given value
  // This does not increment n_presented()
  // Used with next(), to avoid calculating values that will not be stored.
  void force_record(const T& v);

  //: Will decide whether to store the next value
  // This will increment n_presented()
  // \return true if the value was actually stored.
  bool store_next();

  //: Return object describing stored data
  mbl_data_wrapper<T >& data_wrapper() override;

  //: Reseed the internal random number generator.
  void  reseed (unsigned long seed);

  //: The number of vectors that have been presented so far.
  unsigned long n_presented() const {return nPresented_;}

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  mbl_data_collector_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // mbl_stochastic_data_collector_h_
