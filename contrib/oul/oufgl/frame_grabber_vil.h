#ifndef __frame_grabber__
#define __frame_grabber__ 1

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/videodev.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <iostream>
#include <cstdio>
#include <vil/vil_image_view.h>
#include <vcl_iostream.h>
#include <errno.h>

// The framegrabber class. Very easy to use. Uses the video-4-linux
// interface. Simply create an object of type FrameGrabber and then
// call grab_frame which returns a greyscale image.

using namespace std;

class FrameGrabberVil
{
public:
    FrameGrabberVil(const string &device_name="/dev/video0",
                 int width=320, int height=240);
    ~FrameGrabberVil();
    FrameGrabberVil(const FrameGrabberVil &cpy)
    { vcl_cerr << "Can't copy framegrabber\n"; exit(-1);};
    FrameGrabberVil &operator=(const FrameGrabberVil &cpy)
    { vcl_cerr << "Can't copy framegrabber\n"; exit(-1);};
    vil_image_view<vxl_byte> *grab_frame();

private:
    int fd; // the file descriptor for the v4l device
    struct video_mmap mm;
    vil_image_view<vxl_byte> **frame;
    unsigned char *bigbuf;
    struct video_mbuf vm;
};

#endif
