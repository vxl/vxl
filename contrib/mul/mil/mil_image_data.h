#ifndef mil_image_data_h_
#define mil_image_data_h_
#ifdef __GNUC__
#pragma interface
#endif

//: \file
//  \brief Ref. counted block of data on the heap
//  \author Tim Cootes

#include <vcl_string.h>

class vsl_b_ostream;
class vsl_b_istream;

//: Ref. counted block of data on the heap
//  Image data block used by mil_image_2d_of<T>.
template<class T>
class mil_image_data {
private:
	//: Data
	T *data_;

	//: Number of elements
	unsigned int size_;

	//: Reference count
	int ref_count_;

public:
		//: Dflt ctor
	mil_image_data();

		//: Copy ctor
	mil_image_data(const mil_image_data&);

		//: Copy operator
	mil_image_data<T>& operator=(const mil_image_data&);

		//: Destructor
	~mil_image_data();

	//: Increment reference count
	void ref();

	//: Decrement reference count
	void unref();

	//: Number of objects refering to this data
	int ref_count() const { return ref_count_; }

	//: Pointer to first element of data
	const T* data() const { return data_;}

	//: Pointer to first element of data
	T* data() { return data_;}

	//: Number of elements allocated
	unsigned int size() const { return size_; }

	//: Create space for n elements
	void resize(int n);

	//: Version number for I/O 
	short version_no() const;

		//: Name of the class
	vcl_string is_a() const;

		//: Print class to os
	void print(ostream& os) const;
		
		//: Save class to binary file stream
	void b_write(vsl_b_ostream& bfs) const;

		//: Load class from binary stream
	void b_read(vsl_b_istream& bfs);
};


//: Write data to binary stream
template<class T>
void vsl_b_write(vsl_b_ostream& s, const mil_image_data<T>& v);

//: Read data from binary stream
template<class T>
void vsl_b_read(vsl_b_istream& s, mil_image_data<T>& v);

//: Write  to binary stream
template<class T>
void vsl_b_write(vsl_b_ostream& s, const mil_image_data<T>* p);

//: Read data from binary stream
template<class T>
void vsl_b_read(vsl_b_istream& s, mil_image_data<T>*& v);

#endif // mil_image_data_h_


