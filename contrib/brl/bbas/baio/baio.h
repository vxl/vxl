// This is brl/bbas/baio/baio.h
#ifndef baio_h
#define baio_h
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//-----------------------------------------------------------------------------
//:
// \file
// \author Andrew Miller
// \brief  Portable wrapper for simple, single-threaded asynchronous I/O
// \verbatim
//  Modifications
//   Initial version Oct 25, 2010
// \endverbatim
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
//: brown asynchronous IO:
//  Asynch IO wrapper class that handles aio in an object oriented, platform
//  independent way.  Usage example:
//
//    char* buffer
//    baio aio; aio.read("file.bin", BUFSIZE);
//    /* other processing not blocked by I/O */
//    //check to make sure aio is done
//    while (aio.status() == BAIO_IN_PROGRESS) {}
//    char* data = aio.buffer();
//------------------------------------------------------------------------------

//: forward declare baio_info struct, keeping a pointer to it in this class
//  This is forward declared because the AIO mechanism is platform dependent,
//  thus baio_info must be defined in the specific baio_<platform>.cxx files.
//  the baio_info struct keeps track of two things:
//    - status (whether or not IO has returned)
//    - buffer (the data being read from file)
struct baio_info;

// BAIO status for returning
enum baio_status
{
  BAIO_IN_PROGRESS = 0, // baio object is either reading or writing
  BAIO_FINISHED    = 1, // baio object is done and idle
  BAIO_ERROR       = 2  // baio object has hit an error
};

class baio
{
  public:
    baio();
    ~baio();

    //: read bytes from file
    bool read(const std::string& filename, char* buff, long BUFSIZE);

    bool write(const std::string& filename, char* buff, long BUFSIZE);

    void close_file();

    //: get object's status
    baio_status status();

    //: get data (assumes aio object is done)
    char* buffer();

    //: get buffer size (assumes aio object is done)
    long buffer_size();

  private:
    //: member variable needs to persist between read and status calls
    baio_info * info_;
};


#endif //baio_h
