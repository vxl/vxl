#ifndef vidl_vil1_io_h
#define vidl_vil1_io_h
//:
// \file
// \author Nicolas Dano, september 1999
//
// \verbatim
//  Modifications
//   Julien ESTEVE, June 2000 -  Ported from TargetJr
//   10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
// \endverbatim


#include <vcl_string.h>
#include <vidl_vil1/vidl_vil1_clip_sptr.h>
#include <vidl_vil1/vidl_vil1_movie_sptr.h>
#include <vidl_vil1/vidl_vil1_codec_sptr.h>
#include <vcl_vector.h>
#include <vcl_list.h>
#include "dll.h"

class vidl_vil1_movie;
class vidl_vil1_codec;
//: Video Input / Output
//   vidl_vil1_io takes care of Input / Output of videos
//   It reads video in from filenames and creates
//   movies or clips. It saves videos into specific
//   codecs
class vidl_vil1_io
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:
  //-----------------------------------------------------
  //   Callbacks for initializing codecs
  //-------------------------------------------------------
  VIDL_VIL1_DLL_DATA static void (* load_mpegcodec_callback)( vidl_vil1_codec*);


  //---------------------------------------------------------
  //   LoadMovie
  //---------------------------------------------------------

  static vidl_vil1_movie_sptr load_movie(
        const char* fname,
        int start, int end,
        int increment,
        char mode = 'r'
        );

  static vidl_vil1_movie_sptr load_movie(
        const vcl_list<vcl_string> &fnames,
        int start,
        int end,
        int increment,
        char mode = 'r'
        );

  static vidl_vil1_movie_sptr load_movie(
        const vcl_vector<vcl_string> &fnames,
        int start,
        int end,
        int increment,
        char mode = 'r'
        );

  static vidl_vil1_movie_sptr load_movie(
        const char* fname,
        char mode = 'r'
        ) { return load_movie(fname, 0, 0, 1, mode); }

  static vidl_vil1_movie_sptr load_movie(
        const vcl_list<vcl_string> &fnames,
        char mode = 'r'
        ) { return load_movie(fnames, 0, 0, 1, mode); }

  static vidl_vil1_movie_sptr load_movie(
        const vcl_vector<vcl_string> &fnames,
        char mode = 'r'
        ) { return load_movie(fnames, 0, 0, 1, mode); }

  //---------------------------------------------------------
  //   LoadClip
  //---------------------------------------------------------

  static vidl_vil1_clip_sptr load_clip(
        const char* fname,
        int start,
        int end,
        int increment,
        char mode = 'r'
        );

  static vidl_vil1_clip_sptr load_clip(
        const vcl_list<vcl_string> &fnames,
        int start, int end,
        int increment,
        char mode = 'r'
        );

  static vidl_vil1_clip_sptr load_clip(
        const vcl_vector<vcl_string> &fnames,
        int start, int end,
        int increment,
        char mode = 'r'
        );

  static vidl_vil1_clip_sptr load_clip(
        const char* fname,
        char mode = 'r'
        ) { return load_clip(fname, 0, 0, 1, mode); }

  static vidl_vil1_clip_sptr load_clip(
        const vcl_list<vcl_string> &fnames,
        char mode = 'r'
        ) { return load_clip(fnames, 0, 0, 1, mode); }

  static vidl_vil1_clip_sptr load_clip(
        const vcl_vector<vcl_string> &fnames,
        char mode = 'r'
        ) { return load_clip(fnames, 0, 0, 1, mode); }

  //---------------------------------------------------------

  static bool save(
        vidl_vil1_movie* movie,
        const char* fname,
        const char* type
        );

  // returns vcl_string names  for supported types
  static vcl_list<vcl_string> supported_types();

  static void register_codec(vidl_vil1_codec* codec); // adds to supported_types list

  static void close(); // Must call this before the MPEG library is deleted, i.e. on exit.

 public:
  static vcl_list<vidl_vil1_codec_sptr> supported_types_;

  // Helpers-------------------------------------------------------------------

  //: Functions when videos are sequences of still images
  // This may go in the public area if some people know
  // they are dealing with images

  static vidl_vil1_clip_sptr load_images(
        const vcl_list<vcl_string> &fnames,
        int start,
        int,
        int increment,
        char mode = 'r'
        );

  static vidl_vil1_clip_sptr load_images(
        const vcl_vector<vcl_string> &fnames,
        int start,
        int,
        int increment,
        char mode = 'r'
        );

  static vidl_vil1_clip_sptr load_images(
        const vcl_list<vcl_string> &fnames,
        char mode = 'r'
        )
        {
        return load_images(fnames, 0, 0, 1, mode);
        }

  static vidl_vil1_clip_sptr load_images(
        const vcl_vector<vcl_string> &fnames,
        char mode = 'r'
        )
        {
        return load_images(fnames, 0, 0, 1, mode);
        }

  static bool save_images(
        vidl_vil1_movie* movie,
        const char* fname,
        const char* type
        );
};

#endif // vidl_vil1_io_h
