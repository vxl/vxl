// This is oul/oufgl/frame_grabber_pxc200.cxx
#include "frame_grabber_pxc200.h"
//:
// \file
#include <vcl_cstring.h> // for memcpy()
#include <vcl_cstdio.h> // for fopen(), fclose(), fprintf()
#include <vcl_cstdlib.h> // for exit()

//----------------------------------------------------------------------
//: constructor
//
// .param int width: the width of the image (default 384)
// .param int height: the height of the image (default 288)
//
// .status Under Development
// .author Brendan McCane
// .library OTAGO-classes
//----------------------------------------------------------------------

FrameGrabberPxc200RGB::FrameGrabberPxc200RGB(int width_, int height_,
                                             char *devname)
  : current(0), width(width_), height(height_)
{
  // first allocate the buffers for the image
  contents[0] = new ImageContents[width*height];
  contents[1] = new ImageContents[width*height];
  im[0] = new ImageRGB(contents[0], width, height);
  im[1] = new ImageRGB(contents[1], width, height);
  // now setup the input stream
  devfile = vcl_fopen(devname, "r");
  if (!devfile)
  {
    vcl_fprintf(stderr, "Error: couldn't open device %s\n", devname);
    vcl_exit(-1);
  }
  fd = fileno(devfile);
  aio = new AsyncIO(fd);
}

//----------------------------------------------------------------------
//: destructor
//
// .status Under Development
// .author Brendan McCane
// .library OTAGO-classes
//----------------------------------------------------------------------

FrameGrabberPxc200RGB::~FrameGrabberPxc200RGB()
{
  delete im[0];
  delete im[1];
  delete contents[0];
  delete contents[1];
  delete aio;
  vcl_fclose(devfile);
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
// .library OTAGO-classes
//----------------------------------------------------------------------
void FrameGrabberPxc200RGB::acquire_frame_synch()
{
  current = (current+1)%2;
  aio->read(contents[current], width*height*sizeof(vil1_rgb<vxl_byte>));
  aio->wait_for_completion();
  flip_current();
}

//----------------------------------------------------------------------
//: acquire_frame_asynch
//
// acquire a new frame asynchronously (ie start acquiring and
// return immediately )
//
// .status Under Development
// .author Brendan McCane
// .library OTAGO-classes
//----------------------------------------------------------------------

void FrameGrabberPxc200RGB::acquire_frame_asynch()
{
  // first need to wait for any previous acquire to finish
  aio->wait_for_completion();
  // now start reading the next one
  aio->read(contents[current], width*height*sizeof(vil1_rgb<vxl_byte>));
  // move the current frame to the frame previously completed
  current = (current+1)%2;
  // now need to flip the image so that it is the right way up
  flip_current();
}


//----------------------------------------------------------------------
//: get_current_and_acquire
//
// start acquiring the next frame asynchronously and return the
// current frame
//
// .status Under Development
// .author Brendan McCane
// .library OTAGO-classes
//----------------------------------------------------------------------

vil1_memory_image *FrameGrabberPxc200RGB::get_current_and_acquire()
{
  acquire_frame_asynch();
  return get_current_frame();
}

//----------------------------------------------------------------------
//: flip the current frame around a horizontal axis
//
// .status Under Development
// .author Brendan McCane
// .library OTAGO-classes
//----------------------------------------------------------------------

void FrameGrabberPxc200RGB::flip_current()
{
  int limit = im[current]->height()/2;
  int length = im[current]->width()*sizeof(ImageContents);
  int width = im[current]->width();
  int height = im[current]->height();
  ImageContents *temp_mem = new ImageContents[im[current]->width()];
  for (int i=0; i<limit; i++)
  {
    vcl_memcpy(temp_mem, contents[current]+i*width, length);
    vcl_memcpy(contents[current]+i*width, contents[current]+(height-i-1)*width,
               length);
    vcl_memcpy(contents[current]+(height-i-1)*width, temp_mem, length);
  }
  delete [] temp_mem;
}
