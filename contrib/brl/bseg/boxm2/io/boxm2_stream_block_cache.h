#ifndef boxm2_stream_block_cache_h_
#define boxm2_stream_block_cache_h_
//:
// \file
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/basic/boxm2_block_id.h>

#include <vcl_fstream.h>

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class boxm2_stream_block_cache: public vbl_ref_count
{

    typedef vcl_string data_type;
    typedef vcl_string identifier;

    typedef int identifier_index;

  public:
    //: hidden constructor (singleton class)
    boxm2_stream_block_cache(boxm2_scene_sptr scene,
                             const vcl_vector<vcl_string>& data_types,
                             const vcl_vector<vcl_string>& identifier_list);

    bool init(boxm2_block_id id);


    bool clear();

    ~boxm2_stream_block_cache();

    //: map to store various info about each datatype
    vcl_map<data_type, boxm2_data_base *> data_types_;

    //: for each data type, there is a list for each identifier
    vcl_map<data_type, unsigned long > block_size_in_bytes_;

  protected:

    boxm2_scene_sptr scene_;

    boxm2_block_id current_block_id_;
    vcl_vector<vcl_string> data_types_list_;
    vcl_vector<vcl_string> identifier_list_;
};

typedef vbl_smart_ptr<boxm2_stream_block_cache> boxm2_stream_block_cache_sptr;


//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_block_cache const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_stream_block_cache* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_block_cache_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_block_cache_sptr const& sptr);

//: Binary load boxm2_cache  from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_stream_block_cache &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_stream_block_cache* p);
void vsl_b_read(vsl_b_istream& is, boxm2_stream_block_cache_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_stream_block_cache_sptr const& sptr);


#endif //boxm2_stream_block_cache_h_
