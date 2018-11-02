#ifndef mbl_data_collector_list_h_
#define mbl_data_collector_list_h_
//:
// \file
// \brief Collect data and store them in a list.
// \author Tim Cootes

#include <string>
#include <iostream>
#include <iosfwd>
#include <mbl/mbl_data_collector.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Collect data and store them in a list.
template<class T>
class mbl_data_collector_list : public mbl_data_collector<T>
{
  std::vector<T> data_;
  mbl_data_array_wrapper<T> wrapper_;

 public:
  //: Dflt ctor
  mbl_data_collector_list();

  //: Destructor
  ~mbl_data_collector_list() override;

  //: Clear any stored data
  void clear() override;

  //: Hint about how many examples to expect
  void set_n_samples(int n) override;

  //: Record given object
  void record(const T& v) override;

  //: Return object describing stored data
  mbl_data_wrapper<T >& data_wrapper() override;


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

#endif // mbl_data_collector_list_h_
