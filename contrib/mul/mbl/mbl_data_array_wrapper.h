#ifndef mbl_data_array_wrapper_h
#define mbl_data_array_wrapper_h
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author Tim Cootes
// \brief A wrapper to provide access to C-arrays of objects

#include <mbl/mbl_data_wrapper.h>
#include <vcl_vector.h>

//: A wrapper to provide access to C-arrays of objects
template<class T>
class mbl_data_array_wrapper : public mbl_data_wrapper<T> {
private:
  const T* data_;
  unsigned n_;
  unsigned index_;
public:
  //: Default constructor
  mbl_data_array_wrapper();

  //: Constructor
  // Sets up object to return n examples beginning at data[0].
  // The data must be kept in scope, this does not take a copy.
  mbl_data_array_wrapper(const T* data, unsigned n);

  //: Constructor
  // Sets up object to wrap a vcl_vector.
  // The data must be kept in scope, this does not take a copy.
  mbl_data_array_wrapper(const vcl_vector<T > &data);

  //: Initialise to return elements from data[i]
  // Sets up object to return n examples beginning at data[0].
  // The data must be kept in scope, this does not take a copy.
  void set(const T* data, unsigned n);

  //: Default destructor
  virtual ~mbl_data_array_wrapper();

  //: Number of objects available
  virtual unsigned size() const;

	//: Return pointer to start of data array
	const T* data() const { return data_; }


  //: Reset so that current() returns first object
  virtual void reset();

  //: Return current object
  virtual const T& current();

  //: Move to next object, returning true if is valid
  virtual bool next();

  //: Return current index
  //  First example has index 0
  virtual unsigned index() const;

  //: Move to element n
  //  First example has index 0
  virtual void set_index(unsigned n);

  //: Create copy on heap and return base pointer
  virtual mbl_data_wrapper< T >* clone() const;

  //: Name of the class
  virtual vcl_string is_a() const;

};

#endif // mbl_data_array_wrapper_h
