// This is brl/bbas/vidl2/vidl2_v4l_istream.h
#ifndef vidl2_v4l_istream_h_
#define vidl2_v4l_istream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A class for input video streams from a video-4-linux device
//
// \author Paul Crane and Brendan McCane
// \date 21 Feb 2006

#include "vidl2_istream.h"
// not used? #include <vcl_iostream.h>//for printing to stdout via vcl
// not used? #include <vcl_map.h>//for storage of the devices and if they're being used 
// not used? #include <vcl_fstream.h>//needed to probe for valid devices
// not used? #include <vcl_sstream.h>//to convert from ints to strings
#include <vcl_string.h>//this is for the strings scattered about the place

//this is linux specific
using namespace std;

extern "C" {
#include <linux/videodev.h> //this is the video for linux stuff

#include <sys/ioctl.h> //this is to communicate with the device
#include <sys/mman.h>//for mmap
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>//for reading/writing to the camera
#include <errno.h>//for errno
#include <fcntl.h>//this is for the open, and O_RDWR
#include <pthread.h>//for threaded approach to the asynch capture
};

#include "vidl2_v4l_params.h"

class vidl2_v4l_istream:public vidl2_istream
{
  public:
    //: Constructor
    vidl2_v4l_istream():buf(NULL)
    {
        open("/dev/video0");
    };

    vidl2_v4l_istream(const vcl_string &device_name):buf(NULL)
    {
        open(device_name);
    }

    vidl2_v4l_istream(const vcl_string &device_name, const vidl2_v4l_params p)
        :buf(NULL)
    {
        open(device_name);
        set_params(p);
    };

    //: Destructor
    virtual ~vidl2_v4l_istream();

    //: Return true if the stream is open for reading
    virtual bool is_open() const;

    //: Return true if the stream is in a valid state
    virtual bool is_valid() const;

    //: Return true if the stream support seeking
    virtual bool is_seekable() const {return false;};

    //: Return the current frame number
    virtual unsigned int frame_number() const{return frame_number_;};

    //: Open
    bool open(const vcl_string &device_name);

    //: Close the stream
    virtual void close();

    //: set the params for the device
    bool set_params(const vidl2_v4l_params &p);
    vidl2_v4l_params get_params(){return params_;};

    //: Advance to the next frame (but don't acquire an image)
    virtual bool advance();

    //: Read the next frame from the stream (advance and acquire)
    virtual vidl2_frame_sptr read_frame();

    //: Return the current frame in the stream
    virtual vidl2_frame_sptr current_frame(){return cur_frame_;};

    //: Seek to the given frame number
    // \returns true if successful
    virtual bool seek_frame(unsigned int frame_number){return false;};
  private:
    struct video_capability vc;
    struct video_window vw;
    struct video_picture vp;
    struct video_mbuf vm;
    struct video_mmap mm;
    mutable vidl2_frame_sptr cur_frame_;
    vidl2_v4l_params defaults_;
    vidl2_v4l_params params_;
    int fd_;
    unsigned int frame_number_;
    void *buf;
};

#endif
