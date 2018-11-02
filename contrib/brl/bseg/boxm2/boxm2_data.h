#ifndef boxm2_data_h
#define boxm2_data_h
//:
// \file
// \brief base class for different data types class
//
// \author Vishal Jain
// \date nov 17, 2010

#include <iostream>
#include <cstring>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/basic/boxm2_array_1d.h>
#include <boxm2/boxm2_data_traits.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Specific, templated derived class for data blocks
template <boxm2_data_type T>
class boxm2_data: public boxm2_data_base
{
 public:
    //: type of data (float for alpha, bytes for mixture model, etc)
    typedef typename boxm2_data_traits<T>::datatype datatype;

    //: creates boxm2_data object from byte buffer and id
    boxm2_data(char * data_buffer, std::size_t length, boxm2_block_id id);

    //: destructor
    ~boxm2_data() override;

    //: data array accessor
    boxm2_array_1d<datatype>& data() { return data_array_; }

 protected:
    boxm2_array_1d<datatype> data_array_;
};


#endif //boxm2_data_h
