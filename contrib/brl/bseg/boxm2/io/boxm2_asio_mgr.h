#ifndef boxm2_asio_mgr_h_
#define boxm2_asio_mgr_h_
//:
// \file
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/basic/boxm2_block_id.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bbas/baio/baio.h>
#include <vul/vul_file.h>

//: disk level storage class.
//  handles all of the asynchronous IO read and write requests
//  In order to request a block from disk asynchronously use the following syntax: '
//    boxm2_asio_mgr mgr;
//    mgr.load_block(dir, block_id);
//    <continue processing other stuff>
//    std::map<boxm2_block_id, boxm2_block*> mgr.get_loaded_blocks();
//  now you have a pointer to newly allocated blocks
class boxm2_asio_mgr
{
  public:
    //: map typedefs
    typedef std::map<boxm2_block_id, baio*>                block_list_t;
    typedef std::map<boxm2_block_id, boxm2_block*>         block_return_t;
    typedef std::map<boxm2_block_id, boxm2_data_base*>     data_ptr_list_t;
    typedef std::map<boxm2_block_id, boxm2_block_metadata> metadata_list_t;
    typedef std::map<std::string, block_list_t>             data_list_t;
    typedef std::map<std::string, data_ptr_list_t >         data_return_t;

    //: destructor
    ~boxm2_asio_mgr();

    //: creates a BAIO object that loads block data from disk
     void load_block(const std::string& dir, const boxm2_block_id& block_id,const boxm2_block_metadata& mdata);

    //: creates a BAIO object that saves block data to disk
    void save_block(const std::string& dir, boxm2_block* block );

    //: creates a BAIO object that loads data from disk
    template <boxm2_data_type data_type>
    void load_block_data(std::string dir, boxm2_block_id block_i);
    void load_block_data_generic(const std::string& dir, const boxm2_block_id& block_id, const std::string& type);

    //: creates a BAIO object that saves data to disk
    template <boxm2_data_type data_type>
    void save_block_data(std::string dir, boxm2_block_id block_id , boxm2_data_base * block_data);

    //: Access the completed loads
    // \returns a map of block pointers (completed)
    block_return_t  get_loaded_blocks();

    //: access completed data loads
    // \returns a map of data pointers (specific typed pointers)
    template <boxm2_data_type data_type>
    std::map<boxm2_block_id, boxm2_data<data_type>* > get_loaded_data();

    //: access completed data loads
    // \returns a map of data pointers (generic pointers)
    std::map<boxm2_block_id, boxm2_data_base*> get_loaded_data_generic(const std::string& prefix);

  private:

    //: list of asynchronous io loads
    // NEED TO KEEP TRACK OF DATA LOAD AND SAVES
    // \todo Make a map to data_types as well
    block_list_t load_list_;

    //: data list, first index by data_type string, then by block_id
    data_list_t load_data_list_;
    metadata_list_t load_metadata_list_;

    //: list of asynchronous io saves
    block_list_t save_list_;
    data_list_t save_data_list_;
};


//------------------------------------------------------------------------------
// Templated functions for loading and saving data
//------------------------------------------------------------------------------

//: load_block_data creates and stores async request for data of data_type with block_id
template <boxm2_data_type data_type>
void boxm2_asio_mgr::load_block_data(std::string dir, boxm2_block_id block_id)
{
  // if map for this particular data type doesn't exist, initialize it
  if ( load_data_list_.find(boxm2_data_traits<data_type>::prefix()) == load_data_list_.end() )
  {
    std::map<boxm2_block_id, baio*> bmap;
    load_data_list_[boxm2_data_traits<data_type>::prefix()] = bmap;
  }

  //get reference to specific data map
  std::map<boxm2_block_id, baio*>& data_map = load_data_list_[boxm2_data_traits<data_type>::prefix()];

  //create BAIO object only if this data block is not already loading
  if ( data_map.find(block_id) == data_map.end())
  {
    // construct filename
    std::string filename = dir + boxm2_data_traits<data_type>::prefix() + "_" + block_id.to_string() + ".bin";
    //std::cout<<"boxm2_asio_mgr:: data load requested from file:"<<filename<<std::endl;

    // get file size
    unsigned long buflength = vul_file::size(filename);

    // allocate buffer and read to it, store aio object in list
    char * buffer = new char[buflength];
    baio* aio = new baio();
    aio->read(filename, buffer, buflength);

    //store the async request
    data_map[block_id] = aio;
  }
}

//: creates async object that saves block
template <boxm2_data_type data_type>
void boxm2_asio_mgr::save_block_data(std::string dir, boxm2_block_id block_id , boxm2_data_base * block_data)
{
    //0. open up file for writing
    std::string filename = dir + boxm2_data_traits<data_type>::prefix() + "_" +  block_id.to_string() + ".bin";

    // create baio object
    baio *aio = new baio();
    aio->write(filename, block_data->data_buffer(), block_data->buffer_length());
    save_data_list_[boxm2_data_traits<data_type>::prefix()][block_id] = aio;
}


//: get block data of type this function is templated over
template <boxm2_data_type data_type>
std::map<boxm2_block_id, boxm2_data<data_type>* > boxm2_asio_mgr::get_loaded_data()
{
  // map to return
  std::map<boxm2_block_id, boxm2_data<data_type>* > toReturn;

  // see if there even exists a sub-map with this particular data_type
  if ( load_data_list_.find(boxm2_data_traits<data_type>::prefix()) != load_data_list_.end() )
  {
    // iterate over map of current loads
    std::map<boxm2_block_id, baio*>& data_list = load_data_list_[boxm2_data_traits<data_type>::prefix()]; //needs to be a reference, you idiot.
    std::map<boxm2_block_id, baio*>::iterator iter;
    std::vector<std::map<boxm2_block_id, baio*>::iterator > to_delete;
    for (iter=data_list.begin(); iter!=data_list.end(); ++iter)
    {
      // get baio object and block id
      baio*           aio = (*iter).second;
      boxm2_block_id  id  = (*iter).first;
      if ( aio->status() == BAIO_FINISHED )
      {
        // close baio file
        aio->close_file();

        // instantiate new block
        boxm2_data<data_type>* dat = new boxm2_data<data_type>(aio->buffer(), aio->buffer_size(), id);
        toReturn[id] = dat;

        // remove iter from the load list/delete aio
        to_delete.push_back(iter);
        delete aio;
      }
    }

    //delete loaded entries from data list after iterating through the list
    for (auto i : to_delete)
      data_list.erase(i);
  }
  return toReturn;
}


#endif // boxm2_asio_mgr_h_
