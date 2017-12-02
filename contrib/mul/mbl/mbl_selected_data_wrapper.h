// This is mul/mbl/mbl_selected_data_wrapper.h
#ifndef mbl_selected_data_wrapper_h
#define mbl_selected_data_wrapper_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott
// \brief A subset of an existing mbl_data_wrapper.

#include <iostream>
#include <string>
#include <vector>
#include <vcl_compiler.h>
#include <mbl/mbl_data_wrapper.h>

//: A subset of an existing mbl_data_wrapper.
// This wraps another mbl_data_wrapper, and acts like a rearrangement
// of the original data. The rearrangement does not have to refer
// to all of the original data, and can refer to the same data more than
// once.

template <class T>
class mbl_selected_data_wrapper: public mbl_data_wrapper<T>
{
  mbl_data_wrapper<T> *data_;
  std::vector<unsigned> selection_;
  unsigned index_;
 public:
  //: Copy constructor.
  mbl_selected_data_wrapper(const mbl_selected_data_wrapper<T>& p);

  //: Constructor.
  // This will take its own copy of selection and the data wrapper,
  // but not the underlying data.
  mbl_selected_data_wrapper(const mbl_data_wrapper<T>& data,
                            const std::vector<unsigned> &subset);

  //: Default constructor.
  mbl_selected_data_wrapper();

  //: Default destructor.
  virtual ~mbl_selected_data_wrapper();

  //: Copy operator.
  mbl_selected_data_wrapper<T>& operator=(const mbl_selected_data_wrapper<T>& b);

  //: Set the wrappers data.
  // This will take its own copy of selection and the data wrapper,
  // but not the underlying data.
  void set(const mbl_data_wrapper<T>& data,
    const std::vector<unsigned> &selection);

  //: Number of objects available.
  virtual unsigned long size() const;

  //: Reset so that current() returns first object.
  virtual void reset();

  //: Return current object.
  virtual const T& current();

  //: Move to next object, returning true if is valid.
  virtual bool next();

  //: Return current index.
  //  First example has index 0
  virtual unsigned long index() const;

  //: Move to element n.
  //  First example has index 0
  virtual void set_index(unsigned long n);

  //: Create copy on heap and return base pointer.
  // This will create an independent iterator on the underlying data.
  // The original data is not copied.
  // Be careful of destruction of underlying data.
  virtual mbl_data_wrapper< T >* clone() const ;

  //: Name of the class.
  virtual std::string is_a() const;

  //: True if this is (or is derived from) class named s
  virtual bool is_class(std::string const& s) const;
};

#endif // mbl_selected_data_wrapper_h
