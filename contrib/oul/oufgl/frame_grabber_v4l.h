//:
// \file
//
// \brief Classes for video input under linux making use of the video for linux stuff.
// This has been tested using a webcam running under the Philips Web Cam driver.
//
// Copyright (c) 2002 Brendan McCane
// University of Otago, Dunedin, New Zealand
// Reproduction rights limited as described in the COPYRIGHT file.
//----------------------------------------------------------------------

#ifndef OTAGO_frame_grabber_v4l__h_INCLUDED
#define OTAGO_frame_grabber_v4l__h_INCLUDED

#include <vil1/vil1_memory_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vxl_config.h>
#include "asyncio.h"
#include "frame_grabber.h"
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/videodev.h>
#include <vcl_iostream.h>

//----------------------------------------------------------------------
//: The FrameGrabberV4lGrey class
//
// This class provides an interface with a v4l camera on a
// linux system. I have not attempted at all to provide
// cross-compatibility with linux and windows and indeed some
// difficult questions are raised when hardware issues become
// involved.
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------


class FrameGrabberV4lGrey : public FrameGrabber
{
 public:
  typedef vxl_byte ImageContents;
  typedef vil1_memory_image_of< ImageContents > ImageGrey;

  FrameGrabberV4lGrey(int width_=384, int height_=288, bool debug=false,
                      char *devname="/dev/video0");
  virtual ~FrameGrabberV4lGrey();
  // returns the current acquired frame
  inline vil1_memory_image *get_current_frame()
  {
    if (debug)
    {
      vcl_cout << "get_current_frame called\n"
               << "current = " << current
               << " im = " << im[current] << vcl_endl;
    }
    return im[current];
  }
  // acquire a new frame synchronously (ie don't return until completed)
  void acquire_frame_synch();
  // acquire a new frame asynchronously (ie start acquiring and
  // return immediately )
  void acquire_frame_asynch();
  // return the current frame and start acquiring the next one. This
  // function is only useful if asynchronous acquires are going to
  // be used.
  vil1_memory_image *get_current_and_acquire();
 private:
  ImageContents *contents[2];
  ImageGrey *im[2];
  int current;
  AsyncIO *aio;
  int width, height;
  int fd;
  void flip_current();
  bool debug;
};

#endif
