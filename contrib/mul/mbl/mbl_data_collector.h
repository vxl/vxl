#ifndef mbl_data_collector_h_
#define mbl_data_collector_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief Templated base class for objects which collect sets of data.
// \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include<mbl/mbl_data_collector_base.h>
#include<mbl/mbl_data_wrapper.h>

//: Templated base class for objects which collect sets of data.
//  They are primed with the total number of samples to be presented,
//  then the record() function is called with each example.
//  They can return a mbl_data_wrapper<T> object to iterate through
//  the saved examples.
template<class T>
class mbl_data_collector : public mbl_data_collector_base {
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
  virtual mbl_data_wrapper<T >& data_wrapper() = 0;
};

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
