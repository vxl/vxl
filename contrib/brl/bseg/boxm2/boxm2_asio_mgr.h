#ifndef boxm2_asio_mgr_h_
#define boxm2_asio_mgr_h_
//:
// \file
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_data.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <bbas/baio/baio.h>
#include <vul/vul_file.h>

//: disk level storage class.
//  handles all of the asynchronous IO read and write requests
class boxm2_asio_mgr
{
  public:

    //: creates a BAIO object that loads block data from disk
    void load_block(vcl_string dir, boxm2_block_id block_id);

    //: creates a BAIO object that saves block data to disk
    void save_block(vcl_string dir, boxm2_block* block );

    //: creates a BAIO object that loads data from disk
    template <boxm2_data_type data_type>
    void load_block_data(vcl_string dir, boxm2_block_id block_id, baio & aio_reader);

    //: creates a BAIO object that saves data to disk
    template <boxm2_data_type data_type>
    void save_block_data(vcl_string dir, boxm2_block_id block_id , boxm2_data_base * block_data,baio & aio_writer);

    //: returns a map of block pointers
    vcl_map<boxm2_block_id, boxm2_block*> get_loaded_blocks();

    //: returns a map of data pointers
    vcl_map<vcl_string, vcl_map<boxm2_block_id, boxm2_data_base*> > get_loaded_data_blocks();

    //: returns load list (for updating cache)
    vcl_map<boxm2_block_id, baio> load_list() { return load_list_; }

    //: returns save list (for updating cache)
    vcl_map<boxm2_block_id, baio> save_list() { return save_list_; }

  private:

    //: list of asynchronous io loads
    // NEED TO KEEP TRACK OF DATA LOAD AND SAVES
    // TODO: Make a map to data_types as well
    vcl_map<boxm2_block_id, baio> load_list_;
    vcl_map<vcl_string, vcl_map<boxm2_block_id, baio>  > load_data_list_;

    //: list of asynchronous io saves
    vcl_map<boxm2_block_id, baio> save_list_;
    vcl_map<vcl_string, vcl_map<boxm2_block_id, baio> > save_data_list_;
};


template <boxm2_data_type data_type>
void boxm2_asio_mgr::load_block_data(vcl_string dir, boxm2_block_id block_id, baio & aio_reader)
{
    //0. open up file for writing
    vcl_ostringstream ns;  // Declare an output string stream.
    ns << dir << boxm2_data_traits<data_type>::prefix() << block_id << ".bin";
    vcl_string filename = ns.str();

    unsigned long buflength=vul_file::size(filename);
    char * buffer=new char[buflength];
    aio_reader.read(filename,buffer,buflength);
    //load_data_list_[boxm2_data_traits<data_type>::prefix()][block_id]=aio_reader;
    return ;
}

template <boxm2_data_type data_type>
void boxm2_asio_mgr::save_block_data(vcl_string dir, boxm2_block_id block_id , boxm2_data_base * block_data,baio & aio_writer )
{
    //0. open up file for writing
    vcl_ostringstream ns;  // Declare an output string stream.
    ns << dir << boxm2_data_traits<data_type>::prefix() << block_id << ".bin";
    vcl_string filename = ns.str();

    aio_writer.write(filename,block_data->data_buffer(),block_data->buffer_length());
    //save_data_list_[boxm2_data_traits<data_type>::prefix()][block_id]=aio_writer;
    return ;//aio_writer;
}

#endif // boxm2_asio_mgr_h_
