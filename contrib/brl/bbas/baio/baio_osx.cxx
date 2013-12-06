// This is brl/bbas/baio/baio_osx.cxx
#include "baio.h"
//:
// \file
#include <vcl_iostream.h> //for vcl_cout
#include <vcl_cstdlib.h>  //includes malloc
#include <vcl_cstdio.h>   // for std::perror
#include <vcl_cerrno.h>   // for EINPROGRESS int

//UNIX specific includes
#include <aio.h>     //for aio_read
#include <fcntl.h>   //for open
#include <strings.h> //for bzero
#include <unistd.h>  //for close


//: baio_info struct: wrapper for status variables
struct baio_info
{
  aiocb my_aiocb;
};

baio::baio()
{
  info_ = new baio_info();
}

baio::~baio()
{
  if (info_)
    delete info_;
}

//: Opens and reads file asynchronously
bool baio::read(vcl_string filename, char* buff, long BUFSIZE)
{
  // 1. call c open to get standard file handle
  int fhandle = open(filename.c_str(), O_RDONLY);
  if (fhandle < 0) {
    vcl_cerr<<"baio (linux)::read could not open file"<<filename<<vcl_endl;
    vcl_perror("open");
  }

  // 2. Zero out the aiocb structure (recommended)
  bzero( (char *) &(info_->my_aiocb), sizeof(struct aiocb) );

  // 3. Allocate a data buffer for the aiocb request
  info_->my_aiocb.aio_buf = buff;
  if (!info_->my_aiocb.aio_buf) {
    vcl_cerr<<"baio (linux)::read could not assign buffer of size "<<BUFSIZE<<vcl_endl;
    vcl_perror("malloc");
  }

  //4.  Initialize the necessary fields in the aiocb
  info_->my_aiocb.aio_fildes = fhandle;
  info_->my_aiocb.aio_nbytes = BUFSIZE;
  info_->my_aiocb.aio_offset = 0;

  //5.  Call AIO_READ using my_aiocb struct
  int STATUS = aio_read( &(info_->my_aiocb) );
  if (STATUS < 0) {
    vcl_cerr<<"baio (linux)::read throws error on aio_read: "<<STATUS<<vcl_endl;
    vcl_perror("aio_read");
  }
  return true;
}

//: opens and writes file asynchronously
bool baio::write(vcl_string filename, char* buff, long BUFSIZE)
{
  // 1. call c open to get standard file handle
  int fhandle = open(filename.c_str(), O_WRONLY | O_CREAT, 0666);
  if (fhandle < 0) {
    vcl_cerr<<"baio (linux)::write could not open file: "<<filename<<vcl_endl;
    vcl_perror("open");
  }

  // 2. Zero out the aiocb structure (recommended)
  bzero( (char *) &(info_->my_aiocb), sizeof(struct aiocb) );

  // 3. Allocate a data buffer for the aiocb request
  info_->my_aiocb.aio_buf = buff;
  if (!info_->my_aiocb.aio_buf) {
    vcl_cerr<<"baio (linux)::write could not assign buffer of size "<<BUFSIZE<<vcl_endl;
    vcl_perror("malloc");
  }

  //4.  Initialize the necessary fields in the aiocb
  info_->my_aiocb.aio_fildes = fhandle;
  info_->my_aiocb.aio_nbytes = BUFSIZE;
  info_->my_aiocb.aio_offset = 0;

  //5.  Call AIO_READ using my_aiocb struct
  int STATUS = aio_write( &(info_->my_aiocb) );
  if (STATUS < 0) {
    vcl_cerr<<"baio (linux)::write throws error on aio_read: "<<STATUS<<vcl_endl;
    vcl_perror("aio_read");
  }
  return true;
}

//: closes file handle
void baio::close_file()
{
  close(info_->my_aiocb.aio_fildes);
}

baio_status baio::status()
{
  int status = aio_error( &(info_->my_aiocb) );
  if (status == EINPROGRESS)    return BAIO_IN_PROGRESS;
  else if (status == 0)         return BAIO_FINISHED;
  else if (status == ECANCELED) return BAIO_FINISHED;
  else if (status < 0 )         return BAIO_ERROR;
  else                          return BAIO_ERROR;
}

char* baio::buffer()
{
  return (char*) (info_->my_aiocb.aio_buf);
}

long baio::buffer_size()
{
  return (long) (info_->my_aiocb.aio_nbytes);
}
