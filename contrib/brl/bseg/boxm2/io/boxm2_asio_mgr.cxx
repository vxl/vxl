#include "boxm2_asio_mgr.h"
//:
// \file

boxm2_asio_mgr::~boxm2_asio_mgr()
{
#if 0
  // flush unfinished block requests
  std::vector<boxm2_block*> flush_list;
  typedef std::map<boxm2_block_id, boxm2_block*> maptype;
  while (load_list_.size() > 0)  {
    maptype lmap = this->get_loaded_blocks();
    maptype::iterator iter;
    for (iter = lmap.begin(); iter != lmap.end(); ++iter)
      flush_list.push_back(iter->second);
  }
  for (unsigned int i=0; i<flush_list.size(); ++i) {
    if (flush_list[i]){
        std::cout<<"deleting "<<flush_list[i]->block_id()<<std::endl;
       delete flush_list[i];
    }
  }

  //flush unfinished data requests
  std::vector<boxm2_data_base*> flush_data_list;
  typedef std::map<std::string, std::map<boxm2_block_id, baio*> > data_table_t;
  data_table_t::iterator map_i;
  for (map_i = load_data_list_.begin(); map_i != load_data_list_.end(); ++map_i)
  {
    std::string prefix = map_i->first;
    std::map<boxm2_block_id, baio*>& data_map = map_i->second;
    while (data_map.size() > 0)  {
      typedef std::map<boxm2_block_id, boxm2_data_base*> data_map_t;
      data_map_t lmap = this->get_loaded_data_generic(prefix);
      data_map_t::iterator iter;
      for (iter = lmap.begin(); iter != lmap.end(); ++iter)
        flush_data_list.push_back(iter->second);
    }
    for (unsigned int i=0; i<flush_data_list.size(); ++i) {
      if (flush_data_list[i]) {
        std::cout<<"deleting "<<flush_data_list[i]->block_id()<<std::endl;
        delete flush_data_list[i];
      }
    }
  }
#endif // 0
}


//: creates a BAIO object that loads/saves block data from disk
// Make sure asio_mgr doesn't try to load a block that's already loading
void boxm2_asio_mgr::load_block(const std::string& dir, const boxm2_block_id& block_id,const boxm2_block_metadata& mdata)
{
  //if it's not already loading...
  if ( load_list_.find(block_id) == load_list_.end())
  {
    std::string filepath = dir + block_id.to_string() + ".bin";
    //std::cout<<"boxm2_asio_mgr:: load requested from file:"<<filepath<<std::endl;

    //get file size
    unsigned long numBytes = vul_file::size(filepath);

    //read bytes asynchronously, store aio object in aio list
    char * bytes = new char[numBytes];
    baio* aio = new baio();
    aio->read(filepath, bytes, numBytes);
    load_list_[block_id] = aio;
    load_metadata_list_[block_id] = mdata;
  }
}


//: method of saving block
void boxm2_asio_mgr::save_block(const std::string& dir, boxm2_block* block)
{
  std::string filepath = dir + block->block_id().to_string() + ".bin";
  std::cout<<"boxm2_asio_mgr::write save to file: "<<filepath<<std::endl;

  // get block id
  boxm2_block_id id = block->block_id();

  // make sure bytes are written to the buffer
  char * bytes = block->buffer();
  block->b_write(bytes);

  // async write to disk
  baio* aio = new baio();
  aio->write(filepath, bytes, block->byte_count());
  save_list_[id] = aio;

  // TODO go through save list and find completed requests
  //    1. close file
  //    2. delete aio objects
}


//: returns a map of pointers to block pointers
std::map<boxm2_block_id, boxm2_block*> boxm2_asio_mgr::get_loaded_blocks()
{
  std::vector<std::map<boxm2_block_id, baio*>::iterator> to_delete;

  std::map<boxm2_block_id, boxm2_block*> toReturn;
  std::map<boxm2_block_id, baio*>::iterator iter;
  for (iter=load_list_.begin(); iter!=load_list_.end(); ++iter)
  {
    // get baio object and block id
    baio*           aio = (*iter).second;
    boxm2_block_id  id  = (*iter).first;

    if ( aio->status() == BAIO_FINISHED )
    {
      // close baio file
      aio->close_file();

      // instantiate new block
      auto*  blk = new boxm2_block(id, load_metadata_list_[id],aio->buffer());
      toReturn[id] = blk;

      // remove iter from the load list/delete aio
      //load_list_.erase(iter);
      to_delete.push_back(iter);
      delete aio;
    }
  }

  for (auto i : to_delete)
    load_list_.erase(i);

  return toReturn;
}


//: generic get loaded data
std::map<boxm2_block_id, boxm2_data_base*> boxm2_asio_mgr::get_loaded_data_generic(const std::string& prefix)
{
  std::map<boxm2_block_id, boxm2_data_base*> toReturn;

  // see if there even exists a sub-map with this particular data_type
  if ( load_data_list_.find(prefix) != load_data_list_.end() )
  {
    // iterate over map of current loads
    std::map<boxm2_block_id, baio*>& data_list = load_data_list_[prefix]; //needs to be a reference
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
        boxm2_data_base* dat = new boxm2_data_base(aio->buffer(), aio->buffer_size(), id);
        toReturn[id] = dat;

        // remove iter from the load list/delete aio
        to_delete.push_back(iter);
        delete aio;
      }
    }

    //delete loaded entries from data list
    for (auto i : to_delete)
      data_list.erase(i);
  }
  return toReturn;
}



//: load_block_data creates and stores async request for data of data_type with block_id
void boxm2_asio_mgr::load_block_data_generic(const std::string& dir, const boxm2_block_id& block_id, const std::string& type)
{
  // if map for this particular data type doesn't exist, initialize it
  if ( load_data_list_.find(type) == load_data_list_.end() )
  {
    std::map<boxm2_block_id, baio*> bmap;
    load_data_list_[type] = bmap;
  }

  //get reference to specific data map
  std::map<boxm2_block_id, baio*>& data_map = load_data_list_[type];

  //create BAIO object only if this data block is not already loading
  if ( data_map.find(block_id) == data_map.end())
  {
    // construct filename
    std::string filename = dir + type + "_" + block_id.to_string() + ".bin";
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
