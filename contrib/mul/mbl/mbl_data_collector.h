#ifndef mbl_data_collector_h_
#define mbl_data_collector_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author Tim Cootes
// \brief Templated base class for objects which collect sets of data.

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include<mbl/mbl_data_wrapper.h>

//: Templated base class for objects which collect sets of data.
//  They are primed with the total number of samples to be presented,
//  then the record() function is called with each example.
//  They can return a mbl_data_wrapper<T> object to iterate through
//  the saved examples.
template<class T>
class mbl_data_collector {
public:
  //: Dflt ctor
  mbl_data_collector();

  //: Destructor
  virtual ~mbl_data_collector();

  //: Clear any stored data
  virtual void clean() =0;

  //: Hint about how many examples to expect
  virtual void setNSamples(int n) = 0;

  //: Record given object
  virtual void record(const T& v) =0;

  //: Return object describing stored data
  virtual mbl_data_wrapper<T >& vectorData() = 0;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const = 0;

  //: Create a copy on the heap and return base class pointer
  virtual mbl_data_collector* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const = 0;

  //: Save class to binary file stream
  //!in: bfs: Target binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  //!out: bfs: Target binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;
};

//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(bfs,base_ptr);".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
template<class T>
void vsl_add_to_binary_loader(const mbl_data_collector<T>& b);

//: Binary file stream output operator for class reference
template<class T>
void vsl_b_write(vsl_b_ostream& bfs, const mbl_data_collector<T>& b);

//: Binary file stream output operator for pointer to class
template<class T>
void vsl_b_write(vsl_b_ostream& bfs, const mbl_data_collector<T>* b);

//: Binary file stream input operator for class reference
template<class T>
void vsl_b_read(vsl_b_istream& bfs, mbl_data_collector<T>& b);

//: Stream output operator for class reference
template<class T>
vcl_ostream& operator<<(vcl_ostream& os,const mbl_data_collector<T>& b);

//: Stream output operator for class pointer
template<class T>
vcl_ostream& operator<<(vcl_ostream& os,const mbl_data_collector<T>* b);


//: Copy all the data from a mbl_data_wrapper<vnl_vector<double> > into a mbl_data_collector
// This function will change the position of the iterator in the mbl_data_wrapper<vnl_vector<double> >,
// but will not modify any of the data
// The function returns the number of objects copied.
template<class T>
unsigned long CopyAllData(mbl_data_collector<T> &dest, mbl_data_wrapper<T> &src);


//: Merge all the data from two mbl_data_wrapper-s into one mbl_data_collector
// This function will change the position of the iterator in src1 and src2,
// but will not modify any of the data therein. Existing vectors in dest are
// preserved, as is the vector ordering within src1 and src2.
// The function returns the number of vectors copied.
template<class T>
unsigned long MergeAllData(mbl_data_collector<T > &dest,
                           mbl_data_wrapper<T > &src1,
                           mbl_data_wrapper<T > &src2);

#endif // mbl_data_collector_h_
