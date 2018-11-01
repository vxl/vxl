// This is core/vidl/vidl_v4l_istream.h
#ifndef vidl_v4l_istream_h_
#define vidl_v4l_istream_h_
//:
// \file
// \brief A class for input video streams from a video-4-linux device
//
// \author Paul Crane and Brendan McCane
// \date 21 Feb 2006

#include <string>
#include <cerrno>
#include "vidl_istream.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// this is linux specific
using namespace std;

extern "C" {
#include <linux/videodev.h> // this is the video for linux stuff

#include <sys/ioctl.h> // this is to communicate with the device
#include <sys/mman.h>  // for mmap
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h> // for reading/writing to the camera
#include <fcntl.h> // this is for the open, and O_RDWR
#include <pthread.h> // for threaded approach to the asynch capture
};

#include "vidl_v4l_params.h"

class vidl_v4l_istream:public vidl_istream
{
  public:
    // Constructor
    vidl_v4l_istream():buf(NULL)
    {
        open("/dev/video0");
    }

    vidl_v4l_istream(const std::string &device_name):buf(NULL)
    {
        open(device_name);
    }

    vidl_v4l_istream(const std::string &device_name, const vidl_v4l_params p)
        :buf(NULL)
    {
        open(device_name);
        set_params(p);
    }

    // Destructor
    virtual ~vidl_v4l_istream();

    //: Return true if the stream is open for reading
    virtual bool is_open() const;

    //: Return true if the stream is in a valid state
    virtual bool is_valid() const;

    //: Return true if the stream support seeking
    virtual bool is_seekable() const {return false;}

    //: Return the number of frames if known
    //  returns -1 for non-seekable streams
    virtual int num_frames() const { return -1; }

    //: Return the width of each frame
    virtual unsigned int width() const;

    //: Return the height of each frame
    virtual unsigned int height() const;

    //: Return the pixel format
    virtual vidl_pixel_format format() const;

    //: Return the frame rate (FPS, 0.0 if unspecified)
    virtual double frame_rate() const;

    //: Return the duration in seconds (0.0 if unknown)
    virtual double duration() const { return 0.0; }

    //: Return the current frame number
    virtual unsigned int frame_number() const { return frame_number_; }

    //: Open
    bool open(const std::string &device_name);

    //: Close the stream
    virtual void close();

    //: set the params for the device
    bool set_params(const vidl_v4l_params &p);
    vidl_v4l_params get_params() { return params_; }

    //: Advance to the next frame (but don't acquire an image)
    virtual bool advance();

    //: Read the next frame from the stream (advance and acquire)
    virtual vidl_frame_sptr read_frame();

    //: Return the current frame in the stream
    virtual vidl_frame_sptr current_frame() { return cur_frame_; }

    //: Seek to the given frame number
    // \returns true if successful
    virtual bool seek_frame(unsigned int /*frame_nr*/) { return false; }
  private:
    struct video_capability vc;
    struct video_window vw;
    struct video_picture vp;
    struct video_mbuf vm;
    struct video_mmap mm;
    mutable vidl_frame_sptr cur_frame_;
    vidl_v4l_params defaults_;
    vidl_v4l_params params_;
    int fd_;
    unsigned int frame_number_;
    void *buf;
};

#endif
