
//:
// \file

#include "vidl_image_list_codec.h"

#include <vcl_cassert.h>
#include <vcl_cstdio.h>
#include <vcl_iostream.h>

#include <vul/vul_sprintf.h>

#include <vidl/vidl_codec.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_frame.h>
#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

//=========================================================================
//  Methods for vidl_image_list_codec.
//_________________________________________________________________________


char *vidl_image_list_codec::default_initialization_image_type_ = "TIFFImage";

//------------------------------------------------------------------------
// CONSTRUCTOR(S) AND DESTRUCTOR


//: Constructor, from a list of images
vidl_image_list_codec::vidl_image_list_codec(vcl_list<vil_image>& images)
{
  // Set the image type to the default value
  default_image_type_ = default_initialization_image_type_;

  for (vcl_list<vil_image>::iterator i=images.begin(); i!= images.end(); ++i)
    images_.push_back(*i);

  if (!init())
    vcl_cerr << "Failed to initialize the ImageList Codec." << vcl_endl;
}

//: Constructor, from a vector of images
vidl_image_list_codec::vidl_image_list_codec(vcl_vector<vil_image>& images)
{
  // Set the image type to the default value
  default_image_type_ = default_initialization_image_type_;

  for (vcl_vector<vil_image>::iterator i=images.begin(); i!= images.end(); ++i)
    images_.push_back(*i);

  if (!init())
    vcl_cerr << "Failed to initialize the ImageList Codec." << vcl_endl;
}

//: Basic constructor. Should not be called unless we initialize the codec by some ways.
vidl_image_list_codec::vidl_image_list_codec()
{
  // Set the image type to the default value
  default_image_type_ = default_initialization_image_type_;

  // Nothing to do, here
  // Caution, a call to this constructor
  // creates an instance of this class in bad shape
}

// Destructor
vidl_image_list_codec::~vidl_image_list_codec()
{
}

//: Initialize
bool vidl_image_list_codec::init()
{
  if (images_.empty())
    return false;

//   unfinished !!!!! TODO

     set_number_frames(images_.size());
     vil_image first = images_[0];

// Come from TargetJr, don't know the vxl equivalent
//   set_format(first->get_format());
//   set_image_class(first->get_image_class());
     set_bits_pixel(first.bits_per_component() * first.components());
     set_width(first.width());
     set_height(first.height());

  return true;
}


//: Get a section of pixels in function of the frame number, position and size.
bool vidl_image_list_codec::get_section(int position, void* ib, int x0, int y0, int w, int h) const
{
  return images_[position].get_section(ib, x0, y0, w, h);
}

//: Put a section of pixels in function of the frame number, position and size.
int vidl_image_list_codec::put_section(int /*position*/, void* /*ib*/, int /*x0*/, int /*y0*/, int /*w*/, int /*h*/)
{
  vcl_cerr << "vidl_image_list_codec::put_section not implemented" << vcl_endl;
  return 0;
}

//: Load from a file name.
vidl_codec_sptr vidl_image_list_codec::load(const char* fname, char mode)
{
  // will try and load as many images as possible starting with
  //   index 0 and stopping when we run out of images
  assert(mode == 'r');

  for ( int i=0; true; i++)
    {
      const char *name = vul_sprintf( fname, i).c_str();
      vil_image img= vil_load(name);

      if (img!=0)
        images_.push_back(img);
      else
        break;
    }

  if (!init())
    {
      vcl_cerr << "Failed to initialize the ImageList Codec." << vcl_endl;
      return NULL;
    }

  return this;
}

//: Load a 'movie' from a list of filenames, return a codec.
vidl_codec_sptr vidl_image_list_codec::load(const vcl_list<vcl_string> &fnames, char mode)
{
  // Makes sure image loaders are registered
  //register_image_loaders();
  assert(mode == 'r');

  for (vcl_list<vcl_string>::const_iterator i = fnames.begin(); i!=fnames.end(); ++i)
    {
      const char* name = (*i).c_str();
      vil_image img =  vil_load(name);
      if (img!=0)
      {
        images_.push_back(img);
      }
    }

  // Initialize the codec
  if (!init())
    {
      vcl_cerr << "Failed to initialize the ImageList Codec." << vcl_endl;
      return NULL;
    }

  // Every thing was all right,
  // return myself
  return this;
}

//: Load a 'movie' from a vector of filenames, return a codec.
vidl_codec_sptr vidl_image_list_codec::load(const vcl_vector<vcl_string> &fnames, char mode)
{
  // Make sure image loaders are registered
  //register_image_loaders();
  assert(mode == 'r');

  for (vcl_vector<vcl_string>::const_iterator i = fnames.begin(); i!=fnames.end(); ++i)
    {
      const char* name = (*i).c_str();
      vil_image img =  vil_load(name);

      if (img!=0)
        images_.push_back(img);
    }

  // Initialize the codec
  if (!init())
    {
      vcl_cerr << "Failed to initialize the ImageList Codec." << vcl_endl;
      return NULL;
    }

  // Every thing was all right,
  // return myself
  return this;
}

//: Supposed to check the validity of this codec for a special filname.
// Not so well implemented for this codec.
// This could check if the filename is a valid image type
// by probing all the image types.
bool vidl_image_list_codec::probe(const char* fname)
{
  return false;
}

//: Save the given video as a set of images of the default set type.
bool vidl_image_list_codec::save(vidl_movie* movie, const char* fname)
{
  if (!default_image_type_)
  {
    vcl_cerr << "No default image type defined to save the video as a list of images." << vcl_endl;
    return false;
  }

  return save(movie, fname, default_image_type_);
}

//: Save the given video as a set of images of the type given.
bool vidl_image_list_codec::save(
        vidl_movie* movie,
        const char* fname,
        const char* type
        )
{
  // The value to be returned
  bool ret = true;

  // Create the extension for filenames
  vcl_string extension = vcl_string(type);
  extension = extension.substr(0, extension.size()-5); // To get rid of "Image" vcl_string

  for (vidl_movie::frame_iterator pframe = movie->begin();
       pframe <= movie->last();
       ++pframe)
    {
      // Get the image from the frame
      vil_image image = pframe->get_image();

      // Create a name for the current image to be saved
      char currentname [100];
      vcl_sprintf (currentname, "%s%05d.%s",
          fname, pframe.current_frame_number(), extension.c_str());

      bool saved_image = vil_save(image, currentname, type);

      if (!saved_image)
        ret = false;
    }

  return ret;
}

