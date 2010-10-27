// This is brl/bbas/baio/baio_osx.cxx
#include "baio.h"
#include <vcl_iostream.h> //for vcl_cout

//osx specific includes 


//: baio_info struct: wrapper for status variables
struct baio_info {

  //insert status variable and data buffer
  int status;
  char* buffer; 
};

baio::baio() 
{
  info_ = new baio_info();  
}

baio::~baio()
{
  if(info_)
    delete info_;
}

//: Opens and reads file asynchronously 
void baio::read(vcl_string filename, char* buffer, unsigned BUFSIZE)
{
  vcl_cout<<"baio::read is not yet implemented"<<vcl_endl;
  // need to 
  // - allocate a char* of size BUFSIZE to info_ variable
  // - call asynch read
}

baio_status baio::status() 
{
  vcl_cout<<"baio::status is not yet implemented"<<vcl_endl;
  //need to query info_ variable for current aio status and return 
  //BAIO_IN_PROGRESS, BAIO_FINISEHD, or BAIO_ERROR
}

char* baio::buffer() 
{
  vcl_cout<<"baio::buffer is not yet implemented"<<vcl_endl;
  return info_->buffer;
}

