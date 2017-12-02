// This is mul/mbl/mbl_data_wrapper_mixer.h
#ifndef mbl_data_wrapper_mixer_h
#define mbl_data_wrapper_mixer_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Tim Cootes
// \brief Concatenate together data in two or more mbl_data_wrapper objects

#include <iostream>
#include <vector>
#include <mbl/mbl_data_wrapper.h>
#include <vcl_compiler.h>

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
  virtual ~mbl_data_wrapper_mixer();

  //: Number of objects available
  virtual unsigned long size() const;

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
  virtual std::string is_a() const;

  //: True if this is (or is derived from) class named s
  virtual bool is_class(std::string const& s) const;
};

#endif // mbl_data_wrapper_mixer_h
