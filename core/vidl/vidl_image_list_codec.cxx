// This is core/vidl/vidl_image_list_codec.cxx
#include "vidl_image_list_codec.h"
//:
// \file

#include <vcl_cassert.h>
#include <vcl_cstdio.h>
#include <vcl_iostream.h>

#include <vul/vul_sprintf.h>

#include <vidl/vidl_codec.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_frame.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

//=========================================================================
//  Methods for vidl_image_list_codec.
//_________________________________________________________________________

char *vidl_image_list_codec::default_initialization_image_type_ = "tiff";

//------------------------------------------------------------------------
// CONSTRUCTOR(S) AND DESTRUCTOR


//: Constructor, from a list of images
vidl_image_list_codec::vidl_image_list_codec(vcl_list<vil_image_resource_sptr>& images)
{
  // Set the image type to the default value
  default_image_type_ = default_initialization_image_type_;

  for (vcl_list<vil_image_resource_sptr>::iterator i=images.begin(); i!= images.end(); ++i)
    images_.push_back(*i);

  if (!init())
    vcl_cerr << "Failed to initialize the ImageList Codec.\n";
}

//: Constructor, from a vector of images
vidl_image_list_codec::vidl_image_list_codec(vcl_vector<vil_image_resource_sptr>& images)
{
  // Set the image type to the default value
  default_image_type_ = default_initialization_image_type_;

  for (vcl_vector<vil_image_resource_sptr>::iterator i=images.begin(); i!= images.end(); ++i)
    images_.push_back(*i);

  if (!init())
    vcl_cerr << "Failed to initialize the ImageList Codec.\n";
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
     vil_image_resource_sptr first = images_[0];

// Come from TargetJr, don't know the vxl equivalent
//   set_format(first->get_format());
//   set_image_class(first->get_image_class());
     set_bits_pixel(vil_pixel_format_sizeof_components(first->pixel_format()) *
                    vil_pixel_format_num_components(first->pixel_format()) * first->nplanes() * 8);
     set_width(first->ni());
     set_height(first->nj());

  return true;
}


//: Get a section of pixels in function of the frame number, position and size.
vil_image_view_base_sptr vidl_image_list_codec::get_view(int position, int x0, int w, int y0, int h) const
{
  return images_[position]->get_view(x0, w, y0, h);
}

//: Put a section of pixels in function of the frame number, position and size.
bool vidl_image_list_codec::put_view(int /*position*/, const vil_image_view_base & /*im*/, int /*x0*/, int /*y0*/)
{
  vcl_cerr << "vidl_image_list_codec::put_section not implemented\n";
  return false;
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
      vil_image_resource_sptr img= vil_load_image_resource(name);

      if (img)
        images_.push_back(img);
      else
        break;
    }

  if (!init())
    {
      vcl_cerr << "Failed to initialize the ImageList Codec.\n";
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
      vil_image_resource_sptr img =  vil_load_image_resource(name);
      if (img)
      {
        images_.push_back(img);
      }
    }

  // Initialize the codec
  if (!init())
    {
      vcl_cerr << "Failed to initialize the ImageList Codec.\n";
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
      vil_image_resource_sptr img =  vil_load_image_resource(name);

      if (img)
        images_.push_back(img);
    }

  // Initialize the codec
  if (!init())
    {
      vcl_cerr << "Failed to initialize the ImageList Codec.\n";
      return NULL;
    }

  // Every thing was all right,
  // return myself
  return this;
}

//: Supposed to check the validity of this codec for a special filename.
// Not so well implemented for this codec.
// This could check if the filename is a valid image type
// by probing all the image types.
bool vidl_image_list_codec::probe(const char*  /*fname*/)
{
  return false;
}

//: Save the given video as a set of images of the default set type.
bool vidl_image_list_codec::save(vidl_movie* movie, const char* fname)
{
  if (!default_image_type_)
  {
    vcl_cerr << "No default image type defined to save the video as a list of images.\n";
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
      vil_image_view_base_sptr image = pframe->get_view();

      // Create a name for the current image to be saved
      const char *currentname = vul_sprintf("%s%05d.%s", fname,
                                            pframe.current_frame_number(),
                                            extension.c_str()).c_str();

      bool saved_image = vil_save(*image, currentname, type);

      if (!saved_image)
        ret = false;
    }

  return ret;
}
