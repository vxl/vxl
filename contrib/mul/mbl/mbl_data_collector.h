#ifndef mbl_data_collector_h_
#define mbl_data_collector_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief Templated base class for objects which collect sets of data.
// \author Tim Cootes
// \author Ian Scott

#include <vcl_vector.h>
#include <mbl/mbl_data_collector_base.h>
#include <mbl/mbl_data_wrapper.h>

//: Templated base class for objects which collect sets of data.
//  They are primed with the total number of samples to be presented,
//  then the record() function is called with each example.
//  They can return a mbl_data_wrapper<T> object to iterate through
//  the saved examples.

template<class T>
class mbl_data_collector : public mbl_data_collector_base
{
public:
  //: Dflt ctor
  mbl_data_collector();

  //: Destructor
  virtual ~mbl_data_collector();

  //: Clear any stored data
  virtual void clear() =0;

  //: Hint about how many examples to expect
  virtual void set_n_samples(int n) = 0;

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
unsigned long mbl_data_collector_copy_all(mbl_data_collector<T> &dest, mbl_data_wrapper<T> &src);


//: Merge all the data from two mbl_data_wrapper-s into one mbl_data_collector
// This function will change the position of the iterator in src1 and src2,
// but will not modify any of the data therein. Existing vectors in dest are
// preserved, as is the vector ordering within src1 and src2.
// The function returns the number of vectors copied.
// \param order If specified, this vector will contains 0 in every position
// where a value from src0 has been copied into dest, and a 1 where a
// valud from src1 has been copied into dest.
template<class T>
unsigned long mbl_data_collector_merge_all(mbl_data_collector<T > &dest,
                           mbl_data_wrapper<T > &src0,
                           mbl_data_wrapper<T > &src1,
                           vcl_vector<unsigned> *order = 0);

#endif // mbl_data_collector_h_
