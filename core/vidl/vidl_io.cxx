//:
// \file

#include "vidl_io.h"
#include <vcl_compiler.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_clip.h>
#include <vidl/vidl_image_list_codec.h>

#include <vul/vul_file.h>
#include <vul/vul_sequence_filename_map.h>
#include <vul/vul_file_iterator.h>

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_list.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#ifdef HAS_MPEG2
# include <vidl/vidl_mpegcodec.h>
void (* vidl_io::load_mpegcodec_callback)(vidl_codec*) = 0;
#endif


static bool looks_like_a_file_list(const char* fname);
static vidl_clip_sptr load_from_file_list(vcl_string const& fname);

static vidl_clip_sptr load_from_directory(vcl_string const& fname)
{
  vcl_list<vcl_string> filenames;

  vcl_string s(fname);
  s += "/*.*";
  for (vul_file_iterator fit = s;fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.push_back(fit());
  }

  // Call load_images and return the result
  return vidl_io::load_images(filenames, 'r');
}


//-----------------------------------------------------------------------------

//: Load a movie, takes a file name and return the created movie.
// A starting frame, ending frame and increment number are optionals
vidl_movie_sptr vidl_io::load_movie(
        vcl_string const& fname,
        int start,
        int end,
        int increment,
        char mode
        )
{
  vidl_clip_sptr clip = load_clip(fname, start, end, increment, mode);

  // Stop here if the clip was not created
  if (!clip)
    return 0;

  vidl_movie_sptr movie = new vidl_movie(clip);

  return movie;
}

//: Loads and creates movie from a list of image file names.
// A starting frame, ending frame and increment number are optionals
vidl_movie_sptr  vidl_io::load_movie(
        const vcl_list<vcl_string> &fnames,
        int start,
        int end,
        int increment,
        char mode
        )
{
  vidl_clip_sptr clip = load_clip(fnames, start, end, increment, mode);

  // Stop here if the clip was not created
  if (!clip)
    return 0;

  vidl_movie_sptr movie = new vidl_movie(clip);

  return movie;
}

//: Loads and creates movie from a vector of image file names.
// A starting frame, ending frame and increment number are optionals
vidl_movie_sptr  vidl_io::load_movie(
        const vcl_vector<vcl_string> &fnames,
        int start,
        int end,
        int increment,
        char mode
        )
{
  vidl_clip_sptr clip = load_clip(fnames, start, end, increment, mode);

  // Stop here if the clip was not created
  if (!clip)
    return 0;

  vidl_movie_sptr movie = new vidl_movie(clip);

  return movie;
}

//: Load a clip, takes a file name and return the created clip.
// A starting frame, ending frame and increment number are optionals
vidl_clip_sptr  vidl_io::load_clip(
        vcl_string const& fname,
        int start,
        int end,
        int increment,
        char mode
       )
{
  // make sure that fname is sane
  if (fname == "") { return 0; }

  // test if fname is a directory
  if (vul_file::is_directory(fname))
    return load_from_directory(fname);
    //    return load_from_directory(fname);
  else if (looks_like_a_file_list(fname.c_str()))
    return load_from_file_list(fname);

  // The file is not a directory,
  // Let us try all the known video formats,
  // hoping to find the good one
  for (vidl_codec_sptr* i = vidl_codec::all_codecs(); *i; ++i)
  {
    if ((*i)->probe(fname))
    {
      vidl_codec_sptr codec = (*i)->load(fname, mode);
      if (!codec)
        return 0;

#ifdef HAS_MPEG2
      //this calls the dialog box necessary for initialization
      //of the mpeg codec.
      vidl_mpegcodec * vmp = (*i)->castto_vidl_mpegcodec();
      if (vmp) {
        assert (load_mpegcodec_callback);
        load_mpegcodec_callback(vmp);
      }
#endif

      vidl_clip_sptr clip = new vidl_clip(codec, start, end, increment);
      vcl_cout << "vidl_io::load_move. just got a new clip.\n";
      return clip;
    }
  }

  // We did not find a codec corresponding
  // to this file name
  // Return error
  return 0;
}

//: Loads and creates clip from a list of image file names.
// A starting frame, ending frame and increment number are optionals
vidl_clip_sptr vidl_io::load_clip(
        const vcl_list<vcl_string> &fnames,
        int start,
        int end,
        int increment,
        char mode
       )
{
  // Check if the input is correct
  if (fnames.empty())
    return 0;

  // If we have only one filename, we run the special function written for that
  if (fnames.size()==1)
    return load_clip((*fnames.begin()).c_str(), start, end, increment, mode);

  // We have severall files, so we suppose that the video is a set of images.
  return load_images(fnames, start, end, increment, mode);
}

//: Loads and creates clip from a list of image file names.
// A starting frame, ending frame and increment number are optionals
vidl_clip_sptr  vidl_io::load_clip(
        const vcl_vector<vcl_string> &fnames,
        int start,
        int end,
        int increment,
        char mode
       )
{
  // Check if the input is correct
  if (fnames.empty())
    return 0;

  // If we have only one filename, we run the special function written for that
  if (fnames.size()==1)
    return load_clip((*fnames.begin()).c_str(), start, end, increment, mode);

  // We have severall files, so we suppose that the video is a set of images.
  return load_images(fnames, start, end, increment, mode);
}

//: load a list of images as a clip.
// This function should not be called unless
// you are sure you are dealing with images
vidl_clip_sptr  vidl_io::load_images(
        const vcl_list<vcl_string> &fnames,
        int start,
        int end,
        int increment,
        char mode
       )
{
  // This should be a video represented by a set of images
  vidl_image_list_codec_sptr ImCodec = new vidl_image_list_codec();

  vidl_codec_sptr codec = ImCodec->load(fnames, mode);
  if (!codec)
    return 0;
  vidl_clip_sptr clip = new vidl_clip(codec, start, end, increment);
  return clip;
}

//: load a vector of images as a clip.
// This function should not be called unless
// you are sure you are dealing with images
vidl_clip_sptr  vidl_io::load_images(
        const vcl_vector<vcl_string> &fnames,
        int start,
        int end,
        int increment,
        char mode
       )
{
  // This should be a video represented by a set of images
  vidl_image_list_codec_sptr ImCodec = new vidl_image_list_codec();

  vidl_codec_sptr codec = ImCodec->load(fnames, mode);
  if (!codec)
    return 0;
  vidl_clip_sptr clip = new vidl_clip(codec, start, end, increment);
  return clip;
}

//: Save a video into a file "fname" as type "type"
bool vidl_io::save(vidl_movie_sptr movie, vcl_string const& fname, vcl_string const& type)
{
  // Go along the vcl_list of supported videoCODECs,
  // find the one of the type asked if it does exist.
  vidl_codec_sptr* i = vidl_codec::all_codecs();
  while ((*i) && (*i)->type() != type)
  {
#ifdef DEBUG
    vcl_cout << "debug : " << (*i)->type() << " type : " << type << vcl_endl;
#endif
    ++i;
  }

  // Check if the type asked really exists in the context
  // If it does not, Try the vcl_list of images mode.
  if (!(*i))
    return save_images(movie, fname, type);

  // Try to save it
  return (*i)->save(movie.ptr(), fname);
}

// This function should be removed and integrated in save()
bool vidl_io::save_images(vidl_movie_sptr movie, vcl_string const& fname,  vcl_string const& type)
{
  vidl_image_list_codec codec;

  // Try to save and return success or failure
  return codec.save(movie.ptr(), fname, type);
}

//: Return the list of the supported video coder/decoder types
vcl_list<vcl_string> vidl_io::supported_types()
{
  // Create the vcl_list with type() for all the codecs
  vcl_list<vcl_string> ret;
  for (vidl_codec_sptr* i = vidl_codec::all_codecs(); *i; ++i)
    ret.push_back((*i)->type().c_str());

  // Return the vcl_list of type supported codecs
  return ret;
}


static bool looks_like_a_file_list(const char* fname)
{
  while (*fname) {
    if (*fname == '%' || *fname == '#')
      return true;
    ++fname;
  }
  return false;
}

static vidl_clip_sptr load_from_file_list(vcl_string const& fname)
{
  // Declare the vcl_list of image filenames
  vcl_list<vcl_string> filenames;

  vul_sequence_filename_map map(fname);

  for (int i = 0;i < map.get_nviews(); ++i)
  {
    vcl_string fullpath = map.image_name(i);
    // check to see if file is a directory.
    if (vul_file::is_directory(fullpath))
      continue;
    filenames.push_back(fullpath);
  }

  // Call load_images and return the result
  return vidl_io::load_images(filenames, 'r');
}
