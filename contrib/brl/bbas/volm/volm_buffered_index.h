//This is brl/bbas/volm/volm_buffered_index.h
#ifndef volm_buffered_index_h_
#define volm_buffered_index_h_
//:
// \file
// \brief  A class for binary io of various types of volm indices
//         for each location hypothesis, keep a vector of values
//
//
//
// \author Ozge C. Ozcanli
// \date May 31, 2013
// \verbatim
//   Modifications
//
// \endverbatim
//

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define uchar unsigned char

class volm_buffered_index_params
{
  public:
    volm_buffered_index_params() : layer_size(0) {}

    // text i/o
    bool write_params_file(const std::string& index_file_name);
    bool read_params_file(const std::string& index_file_name);

    // text i/o
    static bool write_size_file(const std::string& index_file_name, unsigned long size);
    static bool read_size_file(const std::string& index_file_name, unsigned long& size);

    //: text i/o for existence histogram parameters
    bool write_ex_param_file(const std::string& index_file_name);
    bool read_ex_param_file(const std::string& index_file_name);

    //: text i/o for 2d configurational index parameters
    bool write_conf_param_file(std::string const& index_file_name_pre);
    bool read_conf_param_file(std::string const& index_file_name_pre);

  public:
    unsigned layer_size;
    //: existence histogram parameters
    std::vector<double> radius;
    unsigned norients;
    unsigned nlands;
    //: 2d configurational index parameters
    double conf_radius;
};


class volm_buffered_index : public vbl_ref_count
{
  public:
    enum mode { READ = 0, WRITE = 1, NOT_INITIALIZED = 2 };

    //: layer_size is the size of index array for each hypothesis, buffer_capacity is the max GBs on RAM for this class to use
    volm_buffered_index(unsigned layer_size, float buffer_capacity);
    ~volm_buffered_index() override;

    //: io as chunks of data to a set of files in the specified folder
    bool initialize_read(const std::string& file_name);
    bool initialize_write(const std::string& file_name);
    bool finalize();

    //: return the max number of indices on active cache
    unsigned int buffer_size() { return buffer_size_; }
    unsigned int current_id() { return current_id_; }
    unsigned int current_global_id() { return current_global_id_; }
    unsigned int layer_size() { return layer_size_; }

    //: just appends to the end of the current active buffer, nothing about which location hypothesis these values correspond is known,
    //  caller is responsible to keep the ordering consistent with the hypotheses ordering
    bool add_to_index(std::vector<uchar>& values);
    //: caller is responsible to pass a valid array of size layer_size
    bool add_to_index(const uchar* values);

    //: retrieve the next index, use the active_cache, if all on the active_cache has been retrieved, read from disc
    //  caller is responsible to resize values array to layer_size before passing to this method
    bool get_next(std::vector<uchar>& values);
    //: caller is responsible to pass a valid array of size at least layer_size, if size>layer_size, fill the rest with zeros
    bool get_next(uchar* values, unsigned size);

    //: binary io
    bool close_file(std::string out_file);

  protected:
    unsigned int read_to_buffer(uchar* buf);

    unsigned int layer_size_;     // number of values in an index, this is given by the spherical shell container
    unsigned int buffer_size_;
    unsigned int current_id_;     // id on the active buffer
    unsigned int current_global_id_;  // global id on the entire index object
    mode m_;
    std::string file_name_;

    uchar *active_buffer_;

    unsigned long file_size_;
    unsigned long read_so_far_;
    unsigned int active_buffer_size_;  // during reading there may be less than buffer_size_ on the active cache

    //std::fstream f_obj_; // had issues on Linux..
    std::ifstream if_obj_;
    std::ofstream of_obj_;
};

#include <vbl/vbl_smart_ptr.h>
typedef vbl_smart_ptr<volm_buffered_index> volm_buffered_index_sptr;

#endif  // volm_buffered_index_h_
