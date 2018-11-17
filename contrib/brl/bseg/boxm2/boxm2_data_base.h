#ifndef boxm2_data_base_h
#define boxm2_data_base_h
//:
// \file
// \brief base class for different data types class
//
// \author Vishal Jain
// \date nov 17, 2010
#include <iostream>
#include <cstring>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/basic/boxm2_array_1d.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_block_metadata.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//smart ptr includes
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//
#include <vsl/vsl_binary_io.h>


//: Generic, untemplated base class for data blocks
class boxm2_data_base : public vbl_ref_count
{
 public:
    //: Constructor - beware that the data_buffer becomes OWNED (and will be deleted) by this class!
    boxm2_data_base(char * data_buffer, std::size_t length, boxm2_block_id id, bool read_only = true)
    : read_only_(read_only), id_(id), data_buffer_(data_buffer), buffer_length_(length) {}

    //: initializes empty data buffer
    boxm2_data_base(boxm2_block_metadata data, const std::string& type, bool read_only = true);

    void set_default_value(const std::string& data_type, boxm2_block_metadata data);

    //: This destructor is correct - by our design the original data_buffer becomes OWNED by the data_base class
    ~boxm2_data_base() override { if (data_buffer_) delete [] data_buffer_; }

    //: accessor for low level byte buffer kept by the data_base
    char *            data_buffer()    { return data_buffer_; }
    std::size_t        buffer_length()  const { return buffer_length_; }
    boxm2_block_id&   block_id()       { return id_; }
    //: accessor to a portion of the byte buffer
    char *            cell_buffer(int i, std::size_t cell_size);

    //: setter for swapping out data buffer

    //: by default data is read-only, i.e. cache doesn't save it before destroying it
    bool read_only_;
    void enable_write() { read_only_ = false; }
    void disable_write() { read_only_ = true; }

 protected:
    //: id for this particular block
    boxm2_block_id id_;

    //: byte buffer and its size
    char * data_buffer_;
    std::size_t buffer_length_;
};

//: Smart_Pointer typedef for boxm2_data_base
typedef vbl_smart_ptr<boxm2_data_base> boxm2_data_base_sptr;

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_data_base* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base_sptr const& sptr);


//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_data_base &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_data_base* p);
void vsl_b_read(vsl_b_istream& is, boxm2_data_base_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_data_base_sptr const& sptr);


#endif //boxm2_data_base_h
