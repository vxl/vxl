#ifndef mbl_data_collector_list_h_
#define mbl_data_collector_list_h_
//:
// \file
// \brief Collect data and store them in an list.
// \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include <mbl/mbl_data_collector.h>
#include <mbl/mbl_data_array_wrapper.h>

//: Collect data and store them in an list.
template<class T>
class mbl_data_collector_list : public mbl_data_collector<T>
{
  vcl_vector<T> data_;
  mbl_data_array_wrapper<T> wrapper_;

public:
  //: Dflt ctor
  mbl_data_collector_list();

  //: Destructor
  virtual ~mbl_data_collector_list();

  //: Clear any stored data
  virtual void clear();

  //: Hint about how many examples to expect
  virtual void set_n_samples(int n);

  //: Record given object
  virtual void record(const T& v);

  //: Return object describing stored data
  virtual mbl_data_wrapper<T >& data_wrapper();


  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual mbl_data_collector_base* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // mbl_data_collector_list_h_
