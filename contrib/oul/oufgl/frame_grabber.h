//-*-c++-*--------------------------------------------------------------
//
// \file
// \brief A base class for frame grabbers
//
// Copyright (c) 2001 Brendan McCane
// University of Otago, Dunedin, New Zealand
// Reproduction rights limited as described in the COPYRIGHT file.
//----------------------------------------------------------------------

#ifndef OTAGO_frame_grabber__h_INCLUDED
#define OTAGO_frame_grabber__h_INCLUDED
//----------------------------------------------------------------------
// This is the base class for frame grabbers. It is a pure virtual
// class that acts as an interface really. Subclasses should provide
// methods for acquiring new frames both synchronously and
// asynchronously.
//
// \status Complete
// \author Brendan McCane
//----------------------------------------------------------------------
class FrameGrabber
{
 public:
  virtual ~FrameGrabber() {}
  // returns the current acquired frame
  virtual vil1_memory_image *get_current_frame()=0;
  // acquire a new frame synchronously (ie don't return until completed)
  virtual void acquire_frame_synch()=0;
  // acquire a new frame asynchronously (ie start acquiring and
  // return immediately )
  virtual void acquire_frame_asynch()=0;
  // return the current frame and start acquiring the next one. This
  // function is only useful if asynchronous acquires are going to
  // be used.
  virtual vil1_memory_image *get_current_and_acquire()=0;
};

#endif // OTAGO_frame_grabber__h_INCLUDED
