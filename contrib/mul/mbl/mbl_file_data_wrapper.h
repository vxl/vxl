// This is mul/mbl/mbl_file_data_wrapper.h
#ifndef mbl_file_data_wrapper_h
#define mbl_file_data_wrapper_h
//:
// \file
// \author Tim Cootes
// \brief A wrapper to provide access to sets of objects

#include <iostream>
#include <string>
#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_data_wrapper.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Base class for objects which can return a set of objects, one at a time
//  This is an iterator-like object.  However, unlike STL, generality is
//  obtained by deriving from the class rather than providing a similar
//  interface.
//
//  In particular one can query the object to find out how many examples
//  to expect.
//
//  A typical use is to allow clients to build
//  models from sets of data without having to hold all the data in memory
//  at once.
//
//  Example:
//  \code
//  vnl_vector<double> my_sum(mbl_file_data_wrapper<vnl_vector<double> >& data)
//  {
//    data.reset();
//    vnl_vector<double> sum = data.current();
//    while (data.next())
//      sum += data.current();
//  }
//  \endcode
//
//  Note: It should be fairly simple to provide an iterator type object
//  which steps through the examples in a standard STL way by using this class.

template<class T>
class mbl_file_data_wrapper : public mbl_data_wrapper<T>
{
  // file stream
  vsl_b_ifstream bfs_;

  // path to file holding all data
  std::string path_;

  // data
  T d_;

  // current index of data
  unsigned long index_;

  // size of data
    unsigned long size_;

 public:
#if 0
  //: Default constructor
  mbl_file_data_wrapper();
#endif

  // constructors
  mbl_file_data_wrapper(const std::string path);

  //: Default destructor
  ~mbl_file_data_wrapper() override;

  //: count number of data items
  void calc_data_size();

  //: Number of objects available
  unsigned long size() const override;

  //: Reset so that current() returns first object
  void reset() override;

  //: Return current object
  const T& current() override;

  //: Move to next object, returning true if is valid
  bool next() override;

  //: Return current index
  //  First example has index 0
  unsigned long index() const override;

#if 0
  //: Move to element n
  //  First example has index 0
  virtual void set_index(unsigned long n);
#endif

  //: Create copy on heap and return base pointer
  // This will create an independent iterator on the underlying data.
  // The original data is not copied.
  // Be careful of destruction of underlying data.
  mbl_data_wrapper< T >* clone() const override;

  //: Name of the class
  std::string is_a() const override;

  //: True if this is (or is derived from) class named s
  bool is_class(std::string const& s) const override;
};

#endif // mbl_file_data_wrapper_h
