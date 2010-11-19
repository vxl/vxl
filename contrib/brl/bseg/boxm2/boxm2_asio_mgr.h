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
#include <vgl/vgl_point_3d.h>

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
    template <boxm2_data_type data_type> void load_data(vcl_string dir, boxm2_block_id block_id);
    
    //: creates a BAIO object that saves data to disk
    template <boxm2_data_type data_type> void save_data(vcl_string dir, boxm2_block_id block_id , boxm2_data_base * block_data);

    //: returns load list (for updating cache)
    vcl_map<boxm2_block_id, baio> load_list() { return load_list_; }
    
    //: returns save list (for updating cache)
    vcl_map<boxm2_block_id, baio> save_list() { return save_list_; }

  private:

    //: NEED TO KEEP TRACK OF DATA LOAD AND SAVES
    //: list of asynchronous io loads TODO: Make a map to data_types as well
    vcl_map<boxm2_block_id, baio> load_list_;
    vcl_map<boxm2_data_type, vcl_map<boxm2_block_id, baio>  > load_data_list_;

    //: list of asynchronous io saves
    vcl_map<boxm2_block_id, baio> save_list_;
    vcl_map<boxm2_data_type, vcl_map<boxm2_block_id, baio> > save_data_list_;
};

template <boxm2_data_type data_type>
void boxm2_asio_mgr::load_data(vcl_string dir, boxm2_block_id block_id)
{
    //0. open up file for writing
    vcl_ostringstream ns;  // Declare an output string stream.
    ns << dir << boxm2_data_traits<data_type>::prefix() << block_id << ".bin";
    vcl_string filename = ns.str();

    unsigned long buflength=vul_file::size(filename);
    char * buffer=new char[buflength];
    baio aio_reader;
    aio_reader.read(filename,buffer,buflength);
    load_data_list_[data_type][block_id]=baio;
    return;
}

template <boxm2_data_type data_type>
void boxm2_asio_mgr::save_data(vcl_string dir, boxm2_block_id block_id , boxm2_data_base * block_data)
{
    //0. open up file for writing
    vcl_ostringstream ns;  // Declare an output string stream.
    ns << dir << boxm2_data_traits<data_type>::prefix() << block_id << ".bin";
    vcl_string filename = ns.str();

    unsigned long buflength=vul_file::size(filename);
    char * buffer=new char[buflength];
    baio aio_reader;
    aio_reader.write(filename,block_data->data_buffer(),block_data->buffer_length());
    save_data_list_[data_type][block_id]=baio;
    return;
}


#endif // boxm2_asio_mgr_h_
