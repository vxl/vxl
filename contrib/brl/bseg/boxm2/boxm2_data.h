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

//: Generic, untemplated base class for data blocks
class boxm2_data_base
{
 public:
    //: Constructor - beware that the data_buffer becomes OWNED (and will be deleted) by this class!
    boxm2_data_base(char * data_buffer, vcl_size_t length, boxm2_block_id id)
     : id_(id), data_buffer_(data_buffer),  buffer_length_(length) {}

    //: This destructor is correct - by our design the original data_buffer becomes OWNED by the data_base class
    virtual ~boxm2_data_base() { if (data_buffer_) delete [] data_buffer_; }

    //: accessor for low level byte buffer kept by the data_base
    char *            data_buffer()    { return data_buffer_; }
    vcl_size_t        buffer_length()  { return buffer_length_; }
    boxm2_block_id&   block_id()       { return id_; }

 protected:
    //: id for this particular block
    boxm2_block_id id_;

    //: byte buffer and it's size
    char * data_buffer_;
    vcl_size_t buffer_length_;
};

//: Specific, templated derived class for data blocks
template <boxm2_data_type T>
class boxm2_data: public boxm2_data_base
{
 public:
    //: type of data (float for alpha, bytes for mixture model, etc)
    typedef typename boxm2_data_traits<T>::datatype datatype;

    //: creats boxm2_data object from byte buffer and id
    boxm2_data(char * data_buffer, vcl_size_t length, boxm2_block_id id);

    //: destructor
    virtual ~boxm2_data();

    //: data array accessor
    boxm2_array_1d<datatype> data() {return data_array_;}

 protected:
    boxm2_array_1d<datatype> data_array_;
};

#endif //boxm2_data_h
