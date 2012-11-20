#include "boxm2_stream_block_cache.h"
#include <vcl_new.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
//:
// \file

boxm2_stream_block_cache::boxm2_stream_block_cache(boxm2_scene_sptr scene,
                                                   const vcl_vector<vcl_string>& data_types,
                                                   const vcl_vector<vcl_string>& identifier_list)
: scene_(scene), data_types_list_(data_types),identifier_list_(identifier_list)
{
}

bool boxm2_stream_block_cache::init(boxm2_block_id id)
{
    clear();
    current_block_id_ = id;
    // First go through each data-type and allocate a big buffer;
    for (unsigned i = 0; i < data_types_list_.size(); i++) {
        unsigned long total_bytes_per_data_type = 0 ;
        unsigned long bytes_per_data_type = 0 ;
        for (unsigned j = 0; j < identifier_list_.size(); j++) {
            vcl_string filename = scene_->data_path() + data_types_list_[i] + "_"+ identifier_list_[j] +"_"+ current_block_id_.to_string() + ".bin";
            if (vul_file::exists(filename))
                bytes_per_data_type= vul_file::size(filename);
        }
        total_bytes_per_data_type= bytes_per_data_type* identifier_list_.size();
        char * buffer = new(std::nothrow) char[total_bytes_per_data_type];

        if (buffer == 0)
           vcl_cout<<"Failed to Allocate Memory"<<vcl_endl;

        unsigned long global_index = 0;

        unsigned long num_bytes = 0;
        for (unsigned j = 0; j < identifier_list_.size(); j++) {
            vcl_string filename = scene_->data_path() + data_types_list_[i] + "_" + identifier_list_[j]+"_"+current_block_id_.to_string() + ".bin";
            if (vul_file::exists(filename))
            {
                unsigned long filesize= vul_file::size(filename);
                num_bytes   =   filesize;
                vcl_ifstream ifs;
                ifs.open(filename.c_str(), vcl_ios::in | vcl_ios::binary);
                if (!ifs) return false;
                ifs.read(&(buffer[global_index]), num_bytes);
                int cnt = (int)ifs.gcount();
                global_index+=cnt;
            }
            else
            {
                //init with zero
                vcl_memset (&(buffer[global_index]), 0, bytes_per_data_type);
                global_index+=bytes_per_data_type;
                vcl_cerr << "For " << id << ", data type: " << data_types_list_[i] + "_" + identifier_list_[j] << " does not exist!\n";
            }
        }

        boxm2_data_base * data_buffer             = new boxm2_data_base(buffer,total_bytes_per_data_type,current_block_id_, true);
        data_types_[data_types_list_[i]]          = data_buffer;
        block_size_in_bytes_[data_types_list_[i]] = num_bytes;
    }
    return true;
}

bool boxm2_stream_block_cache::clear()
{
   vcl_map<data_type, boxm2_data_base *>::iterator mit = data_types_.begin();
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
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_block_cache const& scene) {}
void vsl_b_write(vsl_b_ostream& os, const boxm2_stream_block_cache* &p) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_block_cache_sptr& sptr) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_block_cache_sptr const& sptr) {}

//: Binary load boxm2_cache  from stream
void vsl_b_read(vsl_b_istream& is, boxm2_stream_block_cache &scene) {}
void vsl_b_read(vsl_b_istream& is, boxm2_stream_block_cache* p) {}
void vsl_b_read(vsl_b_istream& is, boxm2_stream_block_cache_sptr& sptr) {}
void vsl_b_read(vsl_b_istream& is, boxm2_stream_block_cache_sptr const& sptr) {}

