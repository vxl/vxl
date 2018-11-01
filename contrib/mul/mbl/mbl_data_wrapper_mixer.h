// This is mul/mbl/mbl_data_wrapper_mixer.h
#ifndef mbl_data_wrapper_mixer_h
#define mbl_data_wrapper_mixer_h
//:
// \file
// \author Tim Cootes
// \brief Concatenate together data in two or more mbl_data_wrapper objects

#include <iostream>
#include <vector>
#include <mbl/mbl_data_wrapper.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Concatenate together data in two or more mbl_data_wrapper objects
template<class T>
class mbl_data_wrapper_mixer : public mbl_data_wrapper<T>
{
  std::vector<mbl_data_wrapper<T> *> wrapper_;
  unsigned long n_;
  unsigned long index_;
  unsigned long current_wrapper_;
 public:
  //: Default constructor
  mbl_data_wrapper_mixer();

  //: Constructor
  // Sets up object to return examples from wrapper[i] (i=0..n-1) in turn
  // The wrappers must be kept in scope, this does not take a copy.
  mbl_data_wrapper_mixer(mbl_data_wrapper<T> **wrapper, unsigned long n_wrappers);

  //: Constructor
  // Sets up object to return examples from wrapper[i] (i=0..n-1) in turn
  // The wrappers must be kept in scope, this does not take a copy.
  mbl_data_wrapper_mixer(std::vector<mbl_data_wrapper<T> *> wrapper);

  //: Construct to mix two data wrappers
  // Sets up object to return examples from wrapper1 then wrapper2
  // The wrappers must be kept in scope, this does not take a copy
  // (Pointers are retained).
  mbl_data_wrapper_mixer(mbl_data_wrapper<T>& wrapper1,
                         mbl_data_wrapper<T>& wrapper2);

  //: Sets up object to return examples from wrapper[i] (i=0..n-1) in turn
  // The wrappers must be kept in scope, this does not take a copy.
  void set(mbl_data_wrapper<T> **wrapper, unsigned long n_wrappers);

  //: Default destructor
  ~mbl_data_wrapper_mixer() override;

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

#endif // mbl_data_wrapper_mixer_h
