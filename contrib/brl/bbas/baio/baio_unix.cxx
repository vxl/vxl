// This is brl/bbas/baio/baio_unix.cxx
#include "baio.h"
#include <vcl_iostream.h> //for vcl_cout


//UNIX specific includes 
#include <aio.h>     //for aio_read
#include <fcntl.h>   // for open (not really necessary   
#include <iostream>  //for cout
#include <strings.h> //includes bzero
#include <stdlib.h>  //includes malloc
#include <stdio.h>   // for perror
#include <errno.h>   // for EINPROGRESS int


//: baio_info struct: wrapper for status variables
struct baio_info {
  aiocb my_aiocb; 
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
void baio::read(vcl_string filename, unsigned BUFSIZE)
{

  // 1. call c open to get standard file handle
  int fhandle = open(filename.c_str(), O_RDONLY);
  if (fhandle < 0) { 
    vcl_cerr<<"baio (linux)::read could not open file"<<filename<<vcl_endl;
    perror("open");
  }
  
  // 2. Zero out the aiocb structure (recommended) 
  bzero( (char *) &(info_->my_aiocb), sizeof(struct aiocb) );

  // 3. Allocate a data buffer for the aiocb request 
  info_->my_aiocb.aio_buf = malloc(BUFSIZE+1);
  if (!info_->my_aiocb.aio_buf) {
    vcl_cerr<<"baio (linux)::read could not allocate buffer of size "<<BUFSIZE<<vcl_endl;
    perror("malloc");
  }
  
  //4.  Initialize the necessary fields in the aiocb 
  info_->my_aiocb.aio_fildes = fhandle;
  info_->my_aiocb.aio_nbytes = BUFSIZE;
  info_->my_aiocb.aio_offset = 0;

  //5.  Call AIO_READ using my_aiocb struct
  int STATUS = aio_read( &(info_->my_aiocb) );
  if(STATUS < 0) {
    vcl_cerr<<"baio (linux)::read throws error on aio_read: "<<STATUS<<vcl_endl;
    perror("aio_read");
  }
}

baio_status baio::status() 
{
  int status = aio_error( &(info_->my_aiocb) );
  if(status == EINPROGRESS) {
    return BAIO_IN_PROGRESS; 
  }
  if(status == 0 ) {
    return BAIO_FINISHED; 
  }
  if(status == ECANCELED) {
    return BAIO_FINISHED; 
  }
  if(status < 0 ) {
    return BAIO_ERROR; 
  }
}

char* baio::buffer() 
{
  return (char*) (info_->my_aiocb.aio_buf);
}

