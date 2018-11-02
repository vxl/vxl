//:
// \file
//
// Copyright (c) 2002 Brendan McCane
// University of Otago, Dunedin, New Zealand
// Reproduction rights limited as described in the COPYRIGHT file.
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//: constructor
//
// @param int width: the width of the image (default 384)
// @param int height: the height of the image (default 288)
// @param char *devname: the name of the device (default /dev/video0
//
// @status Under Development
// @author Brendan McCane
// @library oul/oufgl
//----------------------------------------------------------------------

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include "frame_grabber_v4l.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

FrameGrabberV4lGrey::FrameGrabberV4lGrey(int width_, int height_,
                                         bool debug_, const char * const devname)
  : current(0), width(width_), height(height_), debug(debug_)
{
  if (debug)
    std::cout << "V4lGrey constructor, current = " << current << std::endl;
  // now setup the input stream
  fd = open(devname, O_RDWR);
  if (fd<0)
  {
    std::cerr << "Error: couldn't open device " << devname << std::endl;
    std::perror("Couldn't open device");
    std::exit(-1);
  }
  // check the video capability stuff
  struct video_capability vcap;
  ioctl(fd, VIDIOCGCAP, &vcap);
  if (debug)
  {
    std::cout << "camera name = " << vcap.name << std::endl
             << "max image size = " << vcap.maxwidth << ' '
             << vcap.maxheight << std::endl;
  }

  // now set the default parameters for the camera set the format to
  // YUV420P - easy to extract greyscale that way. What we really
  // need here is to poll through all possible palettes and choose
  // the most appropriate for grey scale, then convert as needed -
  // but what a pain. Actually it appears that the phillips web cam
  // driver only supports YUV420P, so to do colour capturing we need
  // to convert to RGB.
  struct video_picture vp;
  ioctl(fd, VIDIOCGPICT, &vp);
  if (debug)
    std::cout << "vp.pallette = " << vp.palette << std::endl;
  vp.palette = VIDEO_PALETTE_YUV420P;
  if (ioctl(fd, VIDIOCSPICT, &vp)>=0)
    std::cout << "Successfully set palette to YUV420P\n";
  else
  {
    std::cerr << "Error setting pallette\n"
             << "Capture may not work\n";
  }

  struct video_window vw;
  ioctl(fd, VIDIOCGWIN, &vw);
  vw.x = vw.y = 0;
  vw.width = width;
  vw.height = height;
  if (debug)
    std::cout << "trying to set to window = " << vw.x << ' ' << vw.y
             << ' ' << vw.width << ' ' << vw.height << std::endl;
#if 0
  // set the fps to 30 fps
  vw.flags &= ~PWC_FPS_FRMASK;
  vw.flags |= (30 << PWC_FPS_SHIFT);
#endif // 0
  // try setting the image size
  ioctl(fd, VIDIOCSWIN, &vw);
  // now read the actual size back
  ioctl(fd, VIDIOCGWIN, &vw);
  if (debug)
    std::cout << "actually setting window to = " << vw.x << ' ' << vw.y
             << ' ' << vw.width << ' ' << vw.height << std::endl;
  // set the size to the actual size
  width = vw.width;
  height = vw.height;

  // allocate the buffers for the image
  contents[0] = new ImageContents[width*height*3];
  contents[1] = new ImageContents[width*height*3];
  im[0] = new ImageGrey(contents[0], width, height);
  im[1] = new ImageGrey(contents[1], width, height);

  // aio = new AsyncIO(fd);
  if (debug)
    std::cout << "V4lGrey constructor end, current = " << current << std::endl;
}

//----------------------------------------------------------------------
//: destructor
//
// .status Under Development
// .author Brendan McCane
// .library oul/oufgl
//----------------------------------------------------------------------

FrameGrabberV4lGrey::~FrameGrabberV4lGrey()
{
  delete im[0];
  delete im[1];
  delete contents[0];
  delete contents[1];
//delete aio;
  close(fd);
}

//----------------------------------------------------------------------
//: acquire_frame_synch
//
// acquire a new frame synchronously (ie don't return until completed)
// You probably shouldn't mix asynch and synchronous calls as this
// tends to screw up the notion of the current frame.
//
// .status Under Development
// .author Brendan McCane
// .library oul/oufgl
//----------------------------------------------------------------------
void FrameGrabberV4lGrey::acquire_frame_synch()
{
  if (debug)
    std::cout << "acquire_frame_synch, current = " << current << std::endl;
  if (!read(fd, (void*)contents[current], width*height*3))
    return;
  if (debug)
    std::cout << "acquire_frame_synch end, current = " << current << std::endl;
#if 0
  current = (current+1)%2;
  aio->read(contents[current], width*height*sizeof(ImageContents));
  aio->wait_for_completion();

  // Alessandro reckons the following is needed for compatibility with
  // >0.28 versions of the driver, but it seems to work fine without it.

  struct Px_BufControl bufcontrol;
  ioctl(aio->getFd(), PX_IOCINFOBUF, &bufcontrol);
  ioctl(aio->getFd(), PX_IOCDONEBUF, &bufcontrol);

  flip_current();
#endif // 0
}

//----------------------------------------------------------------------
//: acquire_frame_asynch
//
// acquire a new frame asynchronously (ie start acquiring and
// return immediately )
//
// actually the asynch bit is currently not working. So at the moment
// it is actually acquiring synchronously.
//
// .status Under Development
// .author Brendan McCane
// .library oul/oufgl
//----------------------------------------------------------------------

void FrameGrabberV4lGrey::acquire_frame_asynch()
{
  if (debug)
    std::cout << "acquire_frame_asynch: currently equivalent to synch\n";
  if (!read(fd, (void*)contents[current], width*height*3))
    return;
  if (debug)
    std::cout << "acquire_frame_asynch end\n";
#if 0
  // first need to wait for any previous acquire to finish
  aio->wait_for_completion();
  // now start reading the next one
  aio->read(contents[current], width*height*sizeof(ImageContents));

  struct Px_BufControl bufcontrol;
  ioctl(aio->getFd(), PX_IOCINFOBUF, &bufcontrol);
  ioctl(aio->getFd(), PX_IOCDONEBUF, &bufcontrol);

  // move the current frame to the frame previously completed
  current = (current+1)%2;
  // now need to flip the image so that it is the right way up
  flip_current();
#endif // 0
}


//----------------------------------------------------------------------
//: get_current_and_acquire
//
// start acquiring the next frame asynchronously and return the
// current frame
//
// .status Under Development
// .author Brendan McCane
// .library oul/oufgl
//----------------------------------------------------------------------

vil1_memory_image *FrameGrabberV4lGrey::get_current_and_acquire()
{
  if (debug)
    std::cout << "get_current_and_acquire\n";
  acquire_frame_asynch();
  return get_current_frame();
  if (debug)
    std::cout << "get_current_and_acquire end\n";
}

//----------------------------------------------------------------------
//: flip the current frame around a horizontal axis
//
// .status Under Development
// .author Brendan McCane
// .library oul/oufgl
//----------------------------------------------------------------------

void FrameGrabberV4lGrey::flip_current()
{
  int limit = im[current]->height()/2;
  int length = im[current]->width()*sizeof(ImageContents);
  int width = im[current]->width();
  int height = im[current]->height();
  ImageContents *temp_mem = new ImageContents[im[current]->width()];
  for (int i=0; i<limit; i++)
  {
    std::memcpy(temp_mem, contents[current]+i*width, length);
    std::memcpy(contents[current]+i*width, contents[current]+(height-i-1)*width, length);
    std::memcpy(contents[current]+(height-i-1)*width, temp_mem, length);
  }
  delete[] temp_mem;
}
