
//:
// \file

#include "vidl_clip.h"

#include <vcl_iostream.h>

#include <vidl/vidl_codec.h>
#include <vidl/vidl_image_list_codec.h>
#include <vidl/vidl_frame.h>

//=========================================================================
//  Methods for vidl_clip.
//_________________________________________________________________________

//------------------------------------------------------------------------
// CONSTRUCTOR(S) AND DESTRUCTOR

//: Constructor. Takes a vidl_codec, start, end and increment frames are optional.
vidl_clip::vidl_clip(
        vidl_codec_sptr codec,
        int start,
        int end,
        int increment) : frames_(codec->length())
{
  for (unsigned int i=0; i<frames_.size(); i++)
    frames_[i] = new vidl_frame(i, codec);

  coder_ = codec;

  init(start, end, increment);
}

//: Constructor. Create a clip from a vector of images. Start, end and increment frames are optional.
vidl_clip::vidl_clip(
        vcl_vector<vil_image_resource_sptr> &images,
        int start,
        int end,
        int increment
        )
{
  int position = 0; // Could not cast the iterator i into (int)
                    // but that would be better
  vidl_image_list_codec_sptr codec = new vidl_image_list_codec(images);

  for (vcl_vector<vil_image_resource_sptr>::iterator i=images.begin(); i!= images.end(); ++i)
    {
      vidl_frame_sptr f = new vidl_frame(position, codec.ptr());
      frames_.push_back(f);
      position++;
    }

  coder_ = codec.ptr();

  init(start, end, increment);
}

//: Constructor. Create a clip from a list of images. Start, end and increment frames are optional.
vidl_clip::vidl_clip(
                     vcl_list<vil_image_resource_sptr> &images,
                     int start,
                     int end,
                     int increment
                    )
{
  int position = 0; // Could not cast the iterator i into (int)
                    // but that would be better
  vidl_image_list_codec_sptr codec = new vidl_image_list_codec(images);

  for (vcl_list<vil_image_resource_sptr>::iterator i=images.begin(); i!= images.end(); ++i)
    {
      vidl_frame_sptr f = new vidl_frame(position, codec.ptr());
      frames_.push_back(f);
      position++;
    }

  coder_ = codec.ptr();

  init(start, end, increment);
}

//: Initialization of the clip. Protected.
void vidl_clip::init(int start, int end, int increment)
{
  // Initialize startframe_, endframe_ and increment_ in the vidl_clip
  //
  // Some sanity checks first
  //

  // Get the actual length
  int len = coder_->length();

  // Check start
  if (start < 0) start = 0;
  if (start >= len) start = len-1;

  // Check end
  if (end == 0)    end = len-1;   // Default
  if (end < start) end = start;
  if (end >= len)  end = len-1;

  // Check increment
  if (increment < 1) increment = 1;

  // Make end equal to start + some multiple of increment
  end = start + (((end-start)/increment)*increment);

  // Now, set the fields of vidl_clip
  increment_  = increment;
  startframe_ = start;
  endframe_   = end;
}


//: Destructor
vidl_clip::~vidl_clip()
{
}


//: Get the frame numbered n inside the range defined by startframe, endframe and increment.
// So, the returned frame is startframe_+n*increment_
vidl_frame_sptr vidl_clip::get_frame(int n)
{
  // Check that the asked frame is in the clip
  if (n>=length() || n < 0)
    {
      vcl_cerr << "vidl_clip::get_frame Frame number " << n << " does not exist." << vcl_endl;
      return 0;
    }

  // return the frame
  return frames_[startframe_+n*increment_];
}

//: Return the number of frames
int vidl_clip::length()
{
  return (endframe_-startframe_)/increment_ + 1;
}

//: Return the horizontal size of the frames in the clip
int vidl_clip::width() const
{ return coder_->width(); }

//: Return the vertical size of the frames in the clip
int vidl_clip::height() const
{ return coder_->height(); }

//: Return the codec.
vidl_codec_sptr vidl_clip::get_codec()
{ return coder_; }

