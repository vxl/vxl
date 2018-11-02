// This is mul/mbl/mbl_file_data_collector.h
#ifndef mbl_file_data_collector_h_
#define mbl_file_data_collector_h_
//:
// \file
// \brief Collect data objects and store them in a file.
// \author dac

#include <string>
#include <iostream>
#include <iosfwd>
#include <mbl/mbl_data_collector.h>
#include <mbl/mbl_file_data_wrapper.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Collect data objects and store them in a file.
template<class T>
class mbl_file_data_collector : public mbl_data_collector<T>
{
  //: binary file stream where the data is stored
  vsl_b_ofstream *bfs_;

  //: path to binary data stream
  std::string path_;

  //: wrapper object point to same file (return reference to this object!)
  mbl_file_data_wrapper<T>* wrapper_;

 public:

  //: Constructor
  mbl_file_data_collector( const std::string & path );

//#if 0
  // Copy constructor
  mbl_file_data_collector(const mbl_data_collector_base& c);
  // Copy constructor
  mbl_file_data_collector(const mbl_file_data_collector & c);

  //: Copy operator
  //virtual mbl_file_data_collector<T>& operator=( const mbl_data_collector_base& c);
  virtual mbl_file_data_collector<T>& operator=( const mbl_file_data_collector & c);
//#endif

  //: Destructor
  ~mbl_file_data_collector() override;

  //: Clear any stored data
  void clear() override;

  //: delete stuff
  void delete_stuff();

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

#endif // mbl_file_data_collector_h_
