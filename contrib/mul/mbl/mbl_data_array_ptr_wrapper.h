// This is mul/mbl/mbl_data_array_ptr_wrapper.h
#ifndef mbl_data_array_ptr_wrapper_h
#define mbl_data_array_ptr_wrapper_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Tim Cootes
// \brief A wrapper to provide access to objects through C-arrays of pointers

#include <mbl/mbl_data_wrapper.h>
#include <vcl_vector.h>

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
  // Sets up object to wrap a vcl_vector.
  // The data must be kept in scope, this does not take a copy.
  mbl_data_array_ptr_wrapper(const vcl_vector<const T* > &data);

  //: Initialise to return elements from data[i]
  // Sets up object to return n examples beginning at data[0].
  // The data must be kept in scope, this does not take a copy.
  void set(const T*const* data, unsigned long n);

  //: Default destructor
  virtual ~mbl_data_array_ptr_wrapper();

  //: Number of objects available
  virtual unsigned long size() const;

  //: Return pointer to start of data array
  const T*const* data() const { return data_; }

  //: Reset so that current() returns first object
  virtual void reset();

  //: Return current object
  virtual const T& current();

  //: Move to next object, returning true if is valid
  virtual bool next();

  //: Return current index
  //  First example has index 0
  virtual unsigned long index() const;

  //: Move to element n
  //  First example has index 0
  virtual void set_index(unsigned long n);

  //: Create copy on heap and return base pointer
  virtual mbl_data_wrapper< T >* clone() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: True if this is (or is derived from) class named s
  virtual bool is_class(vcl_string const& s) const;
};

#endif // mbl_data_array_ptr_wrapper_h
