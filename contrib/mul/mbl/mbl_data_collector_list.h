#ifndef mbl_data_collector_list_h_
#define mbl_data_collector_list_h_

//:
// \file
// \brief Collect data and store them in an list.
// \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include<mbl/mbl_data_collector.h>
#include<mbl/mbl_data_array_wrapper.h>

//: Collect data and store them in an list.
template<class T>
class mbl_data_collector_list : public mbl_data_collector<T> {
private:
  vcl_vector<T> data_;
  mbl_data_array_wrapper<T> wrapper_;

public:
  //: Dflt ctor
  mbl_data_collector_list();

  //: Destructor
  virtual ~mbl_data_collector_list();

  //: Clear any stored data
  virtual void clean();

  //: Hint about how many examples to expect
  virtual void setNSamples(int n);

  //: Record given object
  virtual void record(const T& v);

  //: Return object describing stored data
  virtual mbl_data_wrapper<T >& data_wrapper();
};

#endif // mbl_data_collector_list_h_
