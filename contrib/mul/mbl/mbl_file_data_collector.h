// This is mul/mbl/mbl_file_data_collector.h
#ifndef mbl_file_data_collector_h_
#define mbl_file_data_collector_h_
//:
// \file
// \brief Collect data objects and store them in a file.
// \author dac

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include <mbl/mbl_data_collector.h>
#include <mbl/mbl_file_data_wrapper.h>


//: Collect data objects and store them in a file.
template<class T>
class mbl_file_data_collector : public mbl_data_collector<T>
{
  //: binary file stream where the data is stored
  vsl_b_ofstream *bfs_;

  //: path to binary data stream
  vcl_string path_;

  //: wrapper object point to same file (return reference to this object!)
  mbl_file_data_wrapper<T>* wrapper_;

 public:
 
  //: Constructor
  mbl_file_data_collector( const vcl_string & path );

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
  virtual ~mbl_file_data_collector();

  //: Clear any stored data
  virtual void clear();

  //: delete stuff
  void delete_stuff();

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

#endif // mbl_file_data_collector_h_
