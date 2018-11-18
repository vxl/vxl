#include <new>
#include <iostream>
#include <algorithm>
#include <utility>
#include "boxm2_stream_block_cache.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//:
// \file

boxm2_stream_block_cache::boxm2_stream_block_cache(const boxm2_scene_sptr& scene,
                                                   std::vector<std::string>  data_types,
                                                   std::vector<std::string>  identifier_list)
: scene_(scene), data_types_list_(std::move(data_types)),identifier_list_(std::move(identifier_list))
{
}

bool boxm2_stream_block_cache::init(const boxm2_block_id& id)
{
    clear();
    current_block_id_ = id;
    // First go through each data-type and allocate a big buffer;
    for (const auto & i : data_types_list_) {
        unsigned long total_bytes_per_data_type = 0 ;
        unsigned long bytes_per_data_type = 0 ;
        for (const auto & j : identifier_list_) {
            std::string filename = scene_->data_path() + i + "_"+ j +"_"+ current_block_id_.to_string() + ".bin";
            if (vul_file::exists(filename))
                bytes_per_data_type= vul_file::size(filename);
        }
        total_bytes_per_data_type= bytes_per_data_type* identifier_list_.size();
        char * buffer = new(std::nothrow) char[total_bytes_per_data_type];

        if (buffer == nullptr)
           std::cout<<"Failed to Allocate Memory"<<std::endl;

        unsigned long global_index = 0;

        unsigned long num_bytes = 0;
        for (const auto & j : identifier_list_) {
            std::string filename = scene_->data_path() + i + "_" + j+"_"+current_block_id_.to_string() + ".bin";
            if (vul_file::exists(filename))
            {
                unsigned long filesize= vul_file::size(filename);
                num_bytes   =   filesize;
                std::ifstream ifs;
                ifs.open(filename.c_str(), std::ios::in | std::ios::binary);
                if (!ifs) return false;
                ifs.read(&(buffer[global_index]), num_bytes);
                int cnt = (int)ifs.gcount();
                global_index+=cnt;
            }
            else
            {
                //init with zero
                std::memset (&(buffer[global_index]), 0, bytes_per_data_type);
                global_index+=bytes_per_data_type;
                std::cerr << "For " << id << ", data type: " << i + "_" + j << " does not exist!\n";
            }
        }

        boxm2_data_base * data_buffer             = new boxm2_data_base(buffer,total_bytes_per_data_type,current_block_id_, true);
        data_types_[i]          = data_buffer;
        block_size_in_bytes_[i] = num_bytes;
    }
    return true;
}

bool boxm2_stream_block_cache::clear()
{
   auto mit = data_types_.begin();
   for (; mit != data_types_.end(); ++mit) {
      boxm2_data_base* ptr = mit->second;
      delete ptr;
   }
   data_types_.clear();
   return true;
}

//: hidden destructor (private so it cannot be called -- forces the class to be singleton)
boxm2_stream_block_cache::~boxm2_stream_block_cache()
{
   clear();
}

// in iterative mode, the files need to be closed and re-opened

//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_stream_block_cache const&  /*scene*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, const boxm2_stream_block_cache* & /*p*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_stream_block_cache_sptr&  /*sptr*/) {}
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_stream_block_cache_sptr const&  /*sptr*/) {}

//: Binary load boxm2_cache  from stream
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_stream_block_cache & /*scene*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_stream_block_cache*  /*p*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_stream_block_cache_sptr&  /*sptr*/) {}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_stream_block_cache_sptr const&  /*sptr*/) {}
