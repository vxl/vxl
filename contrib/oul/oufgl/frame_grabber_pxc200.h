// This is oul/oufgl/frame_grabber_pxc200.h
#ifndef OTAGO_frame_grabber_pxc__h_INCLUDED
#define OTAGO_frame_grabber_pxc__h_INCLUDED
//:
// \file
// \brief frame_grabber_pxc.h: Classes for the pxc200 frame grabber
//
// Copyright (c) 2001 Brendan McCane
// University of Otago, Dunedin, New Zealand
// Reproduction rights limited as described in the COPYRIGHT file.
//----------------------------------------------------------------------

#include <vcl_cstdio.h>
#include <vil1/vil1_memory_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>
#include <vxl_config.h>
#include "pxc200.h"
#include "asyncio.h"
#include "frame_grabber.h"

//----------------------------------------------------------------------
//: The FrameGrabberPxcRGB class 
//
// This class provides an interface with the pxc200 framegrabber on a
// linux system. I have not attempted at all to provide
// cross-compatibility with linux and windows and indeed some
// difficult questions are raised when hardware issues become
// involved. 
//
// \status Complete
// \author Brendan McCane
//----------------------------------------------------------------------
class FrameGrabberPxc200RGB : public FrameGrabber
{
 public:
  typedef vil1_rgb<vxl_byte> ImageContents; 
  typedef vil1_memory_image_of< ImageContents > ImageRGB;

 private:
  ImageContents *contents[2];
  ImageRGB *im[2];
  int current;
  vcl_FILE *devfile;
  AsyncIO *aio;
  int width, height;
  int fd;
  void flip_current();

 public:
  FrameGrabberPxc200RGB(int width_=384, int height_=288, 
                        char *devname="/dev/pxc0rgb");
  virtual ~FrameGrabberPxc200RGB();
  // returns the current acquired frame
  inline vil1_memory_image *get_current_frame(){return im[current];}
  // acquire a new frame synchronously (ie don't return until completed)
  void acquire_frame_synch();
  // acquire a new frame asynchronously (ie start acquiring and
  // return immediately )
  void acquire_frame_asynch();
  // return the current frame and start acquiring the next one. This
  // function is only useful if asynchronous acquires are going to
  // be used.
  vil1_memory_image *get_current_and_acquire();
};

#endif // OTAGO_frame_grabber_pxc__h_INCLUDED
