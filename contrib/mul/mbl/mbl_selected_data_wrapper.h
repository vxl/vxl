// This is mul/mbl/mbl_selected_data_wrapper.h
#ifndef mbl_selected_data_wrapper_h
#define mbl_selected_data_wrapper_h
//:
// \file
// \author Ian Scott
// \brief A subset of an existing mbl_data_wrapper.

#include <iostream>
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  ~mbl_selected_data_wrapper() override;

  //: Copy operator.
  mbl_selected_data_wrapper<T>& operator=(const mbl_selected_data_wrapper<T>& b);

  //: Set the wrappers data.
  // This will take its own copy of selection and the data wrapper,
  // but not the underlying data.
  void set(const mbl_data_wrapper<T>& data,
    const std::vector<unsigned> &selection);

  //: Number of objects available.
  unsigned long size() const override;

  //: Reset so that current() returns first object.
  void reset() override;

  //: Return current object.
  const T& current() override;

  //: Move to next object, returning true if is valid.
  bool next() override;

  //: Return current index.
  //  First example has index 0
  unsigned long index() const override;

  //: Move to element n.
  //  First example has index 0
  void set_index(unsigned long n) override;

  //: Create copy on heap and return base pointer.
  // This will create an independent iterator on the underlying data.
  // The original data is not copied.
  // Be careful of destruction of underlying data.
  mbl_data_wrapper< T >* clone() const override ;

  //: Name of the class.
  std::string is_a() const override;

  //: True if this is (or is derived from) class named s
  bool is_class(std::string const& s) const override;
};

#endif // mbl_selected_data_wrapper_h
