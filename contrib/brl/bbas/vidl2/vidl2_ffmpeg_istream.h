// This is contrib/brl/bbas/vidl2/vidl2_ffmpeg_istream.h
#ifndef vidl2_ffmpeg_istream_h_
#define vidl2_ffmpeg_istream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A video input stream from a list of images
//
// \author Matt Leotta 
// \date 21 Dec 2005
//
// \verbatim
//  Modifications
//    Matt Leotta   21 Dec 2005   Adapted from code by Amitha Perera
// \endverbatim

#include "vidl2_istream.h"
#include <vcl_string.h>
#include <vil/vil_image_resource_sptr.h>


//: A video input stream from a list of images
class vidl2_ffmpeg_istream
  : public vidl2_istream
{
  public:
    //: Constructor - default
    vidl2_ffmpeg_istream();

    //: Constructor - from a filename
    vidl2_ffmpeg_istream(const vcl_string& filename);

    //: Destructor
    virtual ~vidl2_ffmpeg_istream();

    //: Open a new stream using a filename
    virtual bool open(const vcl_string& filename);

    //: Close the stream
    virtual void close();


    //: Return true if the stream is open for reading
    virtual bool is_open() const;

    //: Return true if the stream is in a valid state
    virtual bool is_valid() const;

    //: Return true if the stream support seeking
    virtual bool is_seekable() const;

    //: Return the current frame number
    virtual unsigned int frame_number() const;

    //: Advance to the next frame (but don't acquire an image)
    virtual bool advance();

    //: Read the next frame from the stream (advance and acquire)
    virtual vil_image_resource_sptr read_frame();

    //: Return the current frame in the stream
    virtual vil_image_resource_sptr current_frame();

    //: Seek to the given frame number
    // \returns true if successful
    virtual bool seek_frame(unsigned int frame_number);


  private:
    //: The private implementation (PIMPL) details
    // this isolates the clients from the ffmpeg details
    struct pimpl;
    pimpl* is_;
};

#endif // vidl2_ffmpeg_istream_h_

