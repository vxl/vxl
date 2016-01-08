// This is core/vidl/vidl_v4l2_istream.h
#ifndef vidl_v4l2_istream_h_
#define vidl_v4l2_istream_h_
//:
// \file
// \brief Adapter for class vidl_istream
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//   30 Apr 2008 Created (A. Garrido)
//\endverbatim

#include "vidl_istream.h"
#include "vidl_v4l2_device.h"
#include "vidl_v4l2_pixel_format.h"

//: A video input stream using a v4l2 device
class vidl_v4l2_istream: public vidl_istream
{
    //vidl_v4l2_device_sptr dev;
    vidl_v4l2_device& dev;
    mutable vidl_frame_sptr cur_frame_;
    unsigned int frame_number_;

    void update_frame();

  public:
    //: Constructor
    vidl_v4l2_istream(vidl_v4l2_device& device);

    //: Destructor
    virtual ~vidl_v4l2_istream() {dev.stop_capturing();}

    //: Return true if the stream is open for reading.
    virtual bool is_open() const { return true; }

    //: Return true if the stream is in a valid state
    virtual bool is_valid() const { return cur_frame_ && dev; }

    //: Return true if the stream support seeking
    virtual bool is_seekable() const {return false;}

    //: Return the current frame number.
    virtual unsigned int frame_number() const{return frame_number_;}

    //: Return the number of frames if known
    //  returns -1 for non-seekable streams
    virtual int num_frames() const { return -1; }

    //: Return the width of each frame
    virtual unsigned int width() const { return (unsigned int) dev.get_width(); }

    //: Return the height of each frame
    virtual unsigned int height() const { return (unsigned int) dev.get_height(); }

    //: Return the pixel format
    virtual vidl_pixel_format format() const { return v4l2_to_vidl(dev.get_v4l2_format()); }

    //: Return the frame rate (0.0 if unspecified)
    virtual double frame_rate() const { return dev.get_frame_rate(); }

    //: Return the duration in seconds (0.0 if unknown)
    virtual double duration() const { return 0.0; }

    //: Return the current frame number. does this works?
    unsigned int frame_number_device() const { return dev.sequence(); }

    //: Close the stream
    virtual void close() { dev.stop_capturing(); } // why to close?

    //: Advance to the next frame (but don't acquire an image)
    virtual bool advance();

    //: Read the next frame from the stream (advance and acquire)
    virtual vidl_frame_sptr read_frame();

    //: Return the current frame in the stream
    virtual vidl_frame_sptr current_frame() { return cur_frame_; }

    //: Seek to the given frame number
    // \returns true if successful
    virtual bool seek_frame(unsigned int /*frame_number*/) { return false; }
};


#endif // vidl_v4l2_istream_h_
