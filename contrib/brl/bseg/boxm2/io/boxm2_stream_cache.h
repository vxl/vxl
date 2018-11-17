#ifndef boxm2_stream_cache_h_
#define boxm2_stream_cache_h_
//:
// \file
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

class boxm2_stream_cache_helper : public vbl_ref_count
{
  public:
    boxm2_stream_cache_helper() : index_(-1), buf_(nullptr) {}
    ~boxm2_stream_cache_helper() override;

    bool open_file(const std::string& filename);
    void read(unsigned long size, const boxm2_block_id& id);
    void close_file();

    //: return num cells on the buf
    int num_cells(std::size_t cell_size);

    //: return the byte buffer that contains ith cell, i is with respect to the global file
    char *get_cell(int i, std::size_t cell_size, const boxm2_block_id& id);

    int index_;  // index of the data point at the beginning of buf_
    std::ifstream ifs_;
    boxm2_data_base *buf_;
};

typedef vbl_smart_ptr<boxm2_stream_cache_helper> boxm2_stream_cache_helper_sptr;

class boxm2_stream_cache_datatype_helper : public vbl_ref_count
{
  public:
    boxm2_stream_cache_datatype_helper() : current_index_(-1), cell_cnt_(-1) {}

    int current_index_;        // global index in the file
    boxm2_block_id current_block_;

    //: the size of datatype that is stored in each cell
    std::size_t cell_size_;

    //: the size of chunks that will be read from the file, computed based on cell size and available memory
    unsigned long buf_size_;

    //: number of cells in the data files, computed based on file size and cell size
    int cell_cnt_;
};

typedef vbl_smart_ptr<boxm2_stream_cache_datatype_helper> boxm2_stream_cache_datatype_helper_sptr;

class boxm2_stream_cache_destroyer;

class boxm2_stream_cache: public vbl_ref_count
{
  public:
    //: hidden constructor (singleton class)
    boxm2_stream_cache(const boxm2_scene_sptr& scene,
                       const std::vector<std::string>& data_types,
                       const std::vector<std::string>& identifier_list,
                       float num_giga = 1.0f);

    //: return the next cells in the streams of each data block given by the identifier list for this datatype, pass index if available to check synchronization
    template <boxm2_data_type T> std::vector<typename boxm2_data_traits<T>::datatype> get_next(boxm2_block_id id, int index = -1);

    //: random access in the stream
    template <boxm2_data_type T> std::vector<typename boxm2_data_traits<T>::datatype> get_random_i(boxm2_block_id id, unsigned int index);
    //: in iterative mode, the files need to be closed and re-opened
    void close_streams();

    template <boxm2_data_type T> int exists(boxm2_block_id bid);

  protected:

    //: hidden destructor (singleton class)
    ~boxm2_stream_cache() override;

    boxm2_stream_cache_datatype_helper_sptr get_helper(std::string& data_type);

    template <boxm2_data_type T> bool open_streams( boxm2_stream_cache_datatype_helper_sptr h);
#if 0
    //: singleton instance of boxm2_stream_cache
    static boxm2_stream_cache* instance_;
#endif
    //: boxm2_scene needs to be around to get path of the files
    boxm2_scene_sptr scene_;

    std::vector<std::string> identifier_list_;

    //: available RAM size in bytes
    unsigned long mem_size_;

    //: map to store various info about each datatype
    std::map<std::string, boxm2_stream_cache_datatype_helper_sptr > data_types_;

    //: for each data type, there is a list for each identifier
    std::map<std::string, std::vector<boxm2_stream_cache_helper_sptr> > data_streams_;
};

typedef vbl_smart_ptr<boxm2_stream_cache> boxm2_stream_cache_sptr;


//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_cache const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_stream_cache* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_cache_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_cache_sptr const& sptr);

//: Binary load boxm2_cache  from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_stream_cache &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_stream_cache* p);
void vsl_b_read(vsl_b_istream& is, boxm2_stream_cache_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_stream_cache_sptr const& sptr);


#endif //boxm2_stream_cache_h_
