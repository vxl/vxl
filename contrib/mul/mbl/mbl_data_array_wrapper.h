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

//: A wrapper to provide access to C-arrays of objects
template<class T>
class mbl_data_array_wrapper : public mbl_data_wrapper<T> {
private:
	const T* data_;
	int n_;
	int index_;
public:
	//: Default constructor
	mbl_data_array_wrapper();

		//: Constructor
		// Sets up object to return n examples beginning at data[0].
		// The data must be kept in scope, this does not take a copy.
	mbl_data_array_wrapper(const T* data, int n);

		//: Initialise to return elements from data[i]
		// Sets up object to return n examples beginning at data[0].
		// The data must be kept in scope, this does not take a copy.
	void set(const T* data, int n);


	//: Default destructor
	virtual ~mbl_data_array_wrapper();

	//: Number of objects available
	virtual int size() const;

	//: Reset so that current() returns first object
	virtual void reset();

	//: Return current object
	virtual const T& current();

	//: Move to next object, returning true if is valid
	virtual bool next();

		//: Return current index
		//  First example has index 0
	virtual int index() const;

		//: Move to element n
		//  First example has index 0
	virtual void set_index(int n);

		//: Create copy on heap and return base pointer
	virtual mbl_data_array_wrapper< T >* clone() const;

};

#endif // mbl_data_array_wrapper_h
