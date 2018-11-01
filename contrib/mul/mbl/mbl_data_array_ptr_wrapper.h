// This is mul/mbl/mbl_data_array_ptr_wrapper.h
#ifndef mbl_data_array_ptr_wrapper_h
#define mbl_data_array_ptr_wrapper_h
//:
// \file
// \author Tim Cootes
// \brief A wrapper to provide access to objects through C-arrays of pointers

#include <iostream>
#include <vector>
#include <mbl/mbl_data_wrapper.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A wrapper to provide access to objects through C-arrays of pointers
template<class T>
class mbl_data_array_ptr_wrapper : public mbl_data_wrapper<T>
{
  const T*const* data_;
  unsigned long n_;
  unsigned long index_;
 public:
  //: Default constructor
  mbl_data_array_ptr_wrapper();

  //: Constructor
  // Sets up object to return n examples beginning at data[0].
  // The data must be kept in scope, this does not take a copy.
  mbl_data_array_ptr_wrapper(const T*const* data, unsigned long n);

  //: Constructor
  // Sets up object to wrap a std::vector.
  // The data must be kept in scope, this does not take a copy.
  mbl_data_array_ptr_wrapper(const std::vector<const T* > &data);

  //: Initialise to return elements from data[i]
  // Sets up object to return n examples beginning at data[0].
  // The data must be kept in scope, this does not take a copy.
  void set(const T*const* data, unsigned long n);

  //: Default destructor
  ~mbl_data_array_ptr_wrapper() override;

  //: Number of objects available
  unsigned long size() const override;

  //: Return pointer to start of data array
  const T*const* data() const { return data_; }

  //: Reset so that current() returns first object
  void reset() override;

  //: Return current object
  const T& current() override;

  //: Move to next object, returning true if is valid
  bool next() override;

  //: Return current index
  //  First example has index 0
  unsigned long index() const override;

  //: Move to element n
  //  First example has index 0
  void set_index(unsigned long n) override;

  //: Create copy on heap and return base pointer
  mbl_data_wrapper< T >* clone() const override;

  //: Name of the class
  std::string is_a() const override;

  //: True if this is (or is derived from) class named s
  bool is_class(std::string const& s) const override;
};

#endif // mbl_data_array_ptr_wrapper_h
