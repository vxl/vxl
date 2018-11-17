#ifndef boxm2_stream_scene_cache_h_
#define boxm2_stream_scene_cache_h_
//:
// \file
// \brief This class aggregates data of one type across all the blocks of the scene into an array.
#include <iostream>
#include <fstream>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/basic/boxm2_block_id.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class boxm2_stream_scene_cache: public vbl_ref_count
{
    typedef std::string data_type;
    typedef std::string identifier;

    typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  public:
    //: hidden constructor (singleton class)
    boxm2_stream_scene_cache(const boxm2_scene_sptr& scene,
                             std::vector<data_type> data_types,
                             std::vector<identifier> identifiers);

    bool clear();

    ~boxm2_stream_scene_cache() override;

    //: map to store various info about each datatype
    std::map<data_type, char *> data_buffers_;
    std::map<data_type, std::vector<unsigned long> > offsets_;

    uchar16 * blk_buffer_;
    std::vector<unsigned long> blk_offsets_;

    unsigned long total_bytes_per_block_;
    std::map<data_type,unsigned long> total_bytes_per_data_;

    boxm2_scene_sptr scene(){return scene_ ;}

  protected:
    boxm2_scene_sptr scene_;
    std::vector<data_type> data_types_;
    std::vector<identifier> identifiers_;
};

typedef vbl_smart_ptr<boxm2_stream_scene_cache> boxm2_stream_scene_cache_sptr;


//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_scene_cache const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_stream_scene_cache* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_scene_cache_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_scene_cache_sptr const& sptr);

//: Binary load boxm2_cache  from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_stream_scene_cache &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_stream_scene_cache* p);
void vsl_b_read(vsl_b_istream& is, boxm2_stream_scene_cache_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_stream_scene_cache_sptr const& sptr);


#endif //boxm2_stream_scene_cache_h_
