// This is mul/mbl/mbl_data_wrapper.h
#ifndef mbl_data_wrapper_h
#define mbl_data_wrapper_h
//:
// \file
// \author Tim Cootes
// \brief A wrapper to provide access to sets of objects

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
//  vnl_vector<double> my_sum(mbl_data_wrapper<vnl_vector<double> >& data)
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

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template<class T>
class mbl_data_wrapper
{
 public:
  //: Default constructor
  mbl_data_wrapper();

  //: Default destructor
  virtual ~mbl_data_wrapper();

  //: Number of objects available
  virtual unsigned long size() const = 0;

  //: Reset so that current() returns first object
  virtual void reset() = 0;

  //: Return current object
  virtual const T& current() = 0;

  //: Move to next object, returning true if is valid
  virtual bool next() = 0;

  //: Return current index
  //  First example has index 0
  virtual unsigned long index() const =0;

  //: Move to element n
  //  First example has index 0
  virtual void set_index(unsigned long n);

  //: Create copy on heap and return base pointer
  // This will create an independent iterator on the underlying data.
  // The original data is not copied.
  // Be careful of destruction of underlying data.
  virtual mbl_data_wrapper< T >* clone() const = 0;

  //: Name of the class
  virtual std::string is_a() const =0;

  //: True if this is (or is derived from) class named s
  virtual bool is_class(std::string const& s) const =0;
};

#endif // mbl_data_wrapper_h
