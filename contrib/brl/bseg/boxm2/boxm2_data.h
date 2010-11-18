#ifndef boxm2_data_h
#define boxm2_data_h
//:
// \file
// \brief base class for different data types class
//
// \author Vishal Jain
// \date nov 17, 2010
#include <boxm2/boxm2_block_id.h>
#include <boxm2/basic/boxm2_array_1d.h>
#include <boxm2/boxm2_data_traits.h>
#include <vcl_cstring.h>
class boxm2_data_base
{
public:
    boxm2_data_base(unsigned char * data_buffer, 
                    vcl_size_t length,
                    boxm2_block_id id):data_buffer_(data_buffer),buffer_length_(length),id_(id){};
    ~boxm2_data_base(){delete [] data_buffer_;}

protected:

    boxm2_block_id id_;
    unsigned char * data_buffer_;
    vcl_size_t buffer_length_;

};
template <boxm2_data_type T>
class boxm2_data: boxm2_data_base
{
public:
    typedef typename boxm2_data_traits<T>::datatype datatype;

    boxm2_data(unsigned char * data_buffer, 
                                vcl_size_t length, 
                                boxm2_block_id id);
    ~boxm2_data();

    boxm2_array_1d<datatype> * data(){return data_array_;}

protected:
    boxm2_array_1d<datatype> * data_array_;
};
#endif //boxm2_data_h