#include "boxm2_stream_scene_cache.h"
//:
// \file
#include <vgl/vgl_box_3d.h>
#include <vcl_new.h>
#include <vcl_iostream.h>

boxm2_stream_scene_cache::boxm2_stream_scene_cache(boxm2_scene_sptr scene,
                                                   vcl_vector<vcl_string> data_types,
                                                   vcl_vector<vcl_string> identifiers)
: blk_buffer_(0), scene_(scene), data_types_(data_types), identifiers_(identifiers)
{
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  total_bytes_per_block_ = 0;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    vcl_string filename= scene_->data_path() + blk_iter->first.to_string() + ".bin";
    unsigned long filesize= vul_file::size(filename);
    char * temp_buff = new char[filesize];
    vcl_ifstream ifs;
    ifs.open(filename.c_str(), vcl_ios::in | vcl_ios::binary);
    if (!ifs)  continue;
    ifs.read(temp_buff, filesize);
    ifs.close();
    boxm2_block_metadata mdata =  scene_->get_block_metadata(blk_iter->first);
    boxm2_block blk(blk_iter->first,mdata, temp_buff);
    unsigned long cnt = blk.tree_buff_length();
    total_bytes_per_block_ += cnt;
  }

  blk_buffer_ = new(std::nothrow)  uchar16[total_bytes_per_block_];
  if (blk_buffer_ == 0)
  {
    vcl_cout<<"Failed to Allocate Memory"<<vcl_endl;
    return ;
  }
  total_bytes_per_block_*=16;
  unsigned long global_index = 0;
  blk_iter = blocks.begin();
  vgl_box_3d<int> bounding_blk_ids = scene_->bounding_box_blk_ids();
  for (int i = bounding_blk_ids.min_x(); i <= bounding_blk_ids.max_x(); ++i) {
    for (int j = bounding_blk_ids.min_y(); j <= bounding_blk_ids.max_y(); ++j) {
      for (int k = bounding_blk_ids.min_z(); k <= bounding_blk_ids.max_z(); ++k)
      {
        boxm2_block_id id(i,j,k);
        vcl_string filename= scene_->data_path() + id.to_string() + ".bin";
        unsigned long filesize= vul_file::size(filename);
        char * temp_buff = new char[filesize];

        vcl_ifstream ifs;
        ifs.open(filename.c_str(), vcl_ios::in | vcl_ios::binary);
        if (!ifs) continue;
        ifs.read(temp_buff, filesize);
        ifs.close();
        boxm2_block_metadata mdata =  scene_->get_block_metadata(blk_iter->first);
        boxm2_block blk(blk_iter->first,mdata, temp_buff);

        int cnt = (int)blk.tree_buff_length();

        vcl_cout.flush();          vcl_memcpy(&blk_buffer_[global_index],blk.trees().data_block(),cnt*16);
        blk_offsets_.push_back(global_index);
        global_index+=cnt;
      }
    }
  }
  for (unsigned data_type_index=0;data_type_index!=data_types.size();++data_type_index)
  {
    unsigned long total_bytes_per_data_type = 0;

    vcl_string identifier = identifiers_[data_type_index];
    vcl_string data_type  = data_types_[data_type_index];
    for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
    {
      boxm2_block_id id = blk_iter->first;
      vcl_string filename = scene_->data_path() + data_type  + "_" + identifier
                          + (identifier=="" ? "" : "_") + blk_iter->first.to_string() + ".bin";
      total_bytes_per_data_type += vul_file::size(filename);
    }

    total_bytes_per_data_[data_type]=total_bytes_per_data_type;
    char * data_buffer = new(std::nothrow)  char[total_bytes_per_data_type];
    if (data_buffer == 0)
    {
      vcl_cout<<"Failed to Allocate Memory"<<vcl_endl;
      return ;
    }
    unsigned long global_index = 0;
    vcl_vector<unsigned long> offsets;
    for (int i = bounding_blk_ids.min_x(); i <= bounding_blk_ids.max_x(); ++i) {
      for (int j = bounding_blk_ids.min_y(); j <= bounding_blk_ids.max_y(); ++j) {
        for (int k = bounding_blk_ids.min_z(); k <= bounding_blk_ids.max_z(); ++k)
        {
          boxm2_block_id id(i,j,k);
          vcl_string filename = scene_->data_path() + data_type  + "_" + identifier
                              + (identifier=="" ? "" : "_") + id.to_string() + ".bin";
          unsigned long filesize= vul_file::size(filename);
          vcl_ifstream ifs;
          ifs.open(filename.c_str(), vcl_ios::in | vcl_ios::binary);
          if (!ifs) continue;
          ifs.read(&data_buffer[global_index], (int) filesize);
          int cnt = (int)ifs.gcount();
          ifs.close();
          offsets.push_back(global_index);
          global_index+=cnt;
        }
      }
    }
    data_buffers_[data_types[data_type_index]]= data_buffer;
    offsets_[data_types[data_type_index]] = offsets;
  }
}


boxm2_stream_scene_cache::~boxm2_stream_scene_cache()
{
  delete [] blk_buffer_;
  vcl_map<data_type, char *>::iterator iter;
  for (iter = data_buffers_.begin(); iter!= data_buffers_.end(); ++iter)
    delete [] iter->second;
}

// in iterative mode, the files need to be closed and re-opened

//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_scene_cache const& scene) {}
void vsl_b_write(vsl_b_ostream& os, const boxm2_stream_scene_cache* &p) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_scene_cache_sptr& sptr) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_scene_cache_sptr const& sptr) {}

//: Binary load boxm2_cache  from stream
void vsl_b_read(vsl_b_istream& is, boxm2_stream_scene_cache &scene) {}
void vsl_b_read(vsl_b_istream& is, boxm2_stream_scene_cache* p) {}
void vsl_b_read(vsl_b_istream& is, boxm2_stream_scene_cache_sptr& sptr) {}
void vsl_b_read(vsl_b_istream& is, boxm2_stream_scene_cache_sptr const& sptr) {}

