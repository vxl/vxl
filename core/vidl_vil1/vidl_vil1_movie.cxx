//:
// \file

#include "vidl_vil1_movie.h"

#include <vcl_iostream.h>

#include <vidl_vil1/vidl_vil1_frame.h>
#include <vidl_vil1/vidl_vil1_clip.h>

//=========================================================================
//  Methods for vidl_vil1_movie.
//_________________________________________________________________________

//------------------------------------------------------------------------
// CONSTRUCTOR(S) AND DESTRUCTOR

//: Constructor
vidl_vil1_movie::vidl_vil1_movie()
  : frame_rate_ ( 30 ) {}

//: Constructor, build a movie with the single given clip
vidl_vil1_movie::vidl_vil1_movie(vidl_vil1_clip_sptr clip)
  : frame_rate_ ( 30 )
{
  // Check validity of initialisation
  if (!clip_.empty())
    vcl_cerr << "Bad initialisation of the movie."<< vcl_endl;

  add_clip(clip);
}

//: destructor
vidl_vil1_movie::~vidl_vil1_movie()
{
}

//: Get the frame numbered n (frames are numbered from 0 to total-1)
vidl_vil1_frame_sptr vidl_vil1_movie::get_frame(int n)
{
  vidl_vil1_frame_sptr ret_frame = NULL;

  vcl_list<vidl_vil1_clip_sptr>::iterator i = clip_.begin();

  while ((i!=clip_.end()) && (!(ret_frame=(*i)->get_frame(n))))
  {
    n = n - (**i).length();
    i++;
  }

  // one can note that if the frame was not in the clip (n too big),
  // Then, NULL is returned.
  return ret_frame;
}

vil1_image vidl_vil1_movie::get_image(int n)
{
  return get_frame(n)->get_image();
}

//: Add a clip at the end of the movie
void vidl_vil1_movie::add_clip(vidl_vil1_clip_sptr clip)
{
  clip_.push_back(clip);
}


//: Give back the number of frames of the movie
int vidl_vil1_movie::length() const
{
  int number = 0;
  for (vcl_list<vidl_vil1_clip_sptr>::const_iterator i=clip_.begin(); i!= clip_.end(); ++i)
    number += (*i)->length();

  return number;
}

//: Return the horizontal size of the frames in the movie
// Check that all the movies do have the same size, output
// an error if not
int vidl_vil1_movie::width() const
{
  // Get the size X of the first clip
  int sizeX = 0;
  vcl_list<vidl_vil1_clip_sptr>::const_iterator i=clip_.begin();
  sizeX = (*i)->width();

  // Check that the (eventually) other clips have the same size
  for (; i!= clip_.end(); ++i)
    if ((*i)->width() != sizeX)
    {
      vcl_cerr << "SizeX of the movie asked. But the different clips have different sizes.\n";
      return 0;
    }

  // Return the size X
  return sizeX;
}

//: Return the vertical size of the frames in the movie
// Check that all the movies do have the same size, output
// an error if not
int vidl_vil1_movie::height() const
{
  // Get the size Y of the first clip
  int sizeY = 0;
  vcl_list<vidl_vil1_clip_sptr>::const_iterator i = clip_.begin();
  sizeY = (*i)->height();

  // Check that the (eventually) other clips have the same size
  for (; i!= clip_.end(); ++i)
    if ((*i)->height() != sizeY)
    {
      vcl_cerr << "SizeY of the movie asked. But the different clips have different sizes.\n";
      return 0;
    }

  // Return the size Y
  return sizeY;
}
