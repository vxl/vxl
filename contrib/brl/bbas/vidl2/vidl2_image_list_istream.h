// This is contrib/brl/bbas/vidl2/vidl2_image_list_istream.h
#ifndef vidl2_image_list_istream_h_
#define vidl2_image_list_istream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A video input stream from a list of images
//
// \author Matt Leotta 
// \date 19 Dec 2005

#include "vidl2_istream.h"
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_image_resource_sptr.h>


//: A video input stream from a list of images
class vidl2_image_list_istream
  : public vidl2_istream
{
  public:
    //: Constructor - default
    vidl2_image_list_istream();

    //: Constructor - from a vector of image resources
    vidl2_image_list_istream(const vcl_vector<vil_image_resource_sptr>& images);

    //: Constructor - from a file glob string
    vidl2_image_list_istream(const vcl_string& glob);

    //: Constructor - from a vector of file paths
    vidl2_image_list_istream(const vcl_vector<vcl_string>& paths);

    //: Destructor
    virtual ~vidl2_image_list_istream() { close(); }


    //: Open a new stream using an existing vector of images
    virtual bool open(const vcl_vector<vil_image_resource_sptr>& images);

    //: Open a new stream using a file glob (see vul_file_iterator)
    // \note files are loaded in alphanumeric order by path name
    virtual bool open(const vcl_string& glob);

    //: Open a new stream using a vector of file paths
    virtual bool open(const vcl_vector<vcl_string>& paths);

    //: Close the stream
    virtual void close() { images_.clear(); }


    //: Return true if the stream is open for reading
    virtual bool is_open() const { return !images_.empty(); }

    //: Return true if the stream is in a valid state
    virtual bool is_valid() const { return is_open() &&
                                     index_ < images_.size(); }

    //: Return true if the stream support seeking
    virtual bool is_seekable() const { return true; }

    //: Return the current frame number
    virtual unsigned int frame_number() const { return index_; }


    //: Advance to the next frame (but don't acquire an image)
    virtual bool advance();

    //: Read the next frame from the stream
    virtual vil_image_resource_sptr read_frame();

    //: Return the current frame in the stream
    virtual vil_image_resource_sptr current_frame();

    //: Seek to the given frame number
    // \returns true if successful
    virtual bool seek_frame(unsigned int frame_number);


  private:
    //: The vector of images
    vcl_vector<vil_image_resource_sptr> images_;

    //: The current index
    unsigned int index_;
};

#endif // vidl2_image_list_istream_h_

