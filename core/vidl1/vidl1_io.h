// This is core/vidl/vidl_io.h
#ifndef vidl_io_h
#define vidl_io_h
//:
// \file
// \author Nicolas Dano, september 1999
//
// \verbatim
//  Modifications
//   Julien ESTEVE, June 2000 -  Ported from TargetJr
//   10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
//   10/7/2003 Matt Leotta (Brown) Converted vil1 to vil
// \endverbatim


#include <vcl_string.h>
#include <vidl/vidl_clip_sptr.h>
#include <vidl/vidl_movie_sptr.h>
#include <vcl_vector.h>
#include <vcl_list.h>
#include "dll.h"
class vidl_movie;
class vidl_codec;

//: Video Input / Output
//   vidl_io takes care of Input / Output of videos
//   It reads video in from filenames and creates
//   movies or clips. It saves videos into specific
//   codecs
class vidl_io
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:
  //-----------------------------------------------------
  //   Callbacks for initializing codecs
  //-------------------------------------------------------
  VIDL_DLL_DATA static void (* load_mpegcodec_callback)( vidl_codec*);

  //---------------------------------------------------------
  //   LoadMovie
  //---------------------------------------------------------

  static vidl_movie_sptr load_movie(
        vcl_string const& fname,
        int start, int end,
        int increment,
        char mode = 'r'
        );

  static vidl_movie_sptr load_movie(
        const vcl_list<vcl_string> &fnames,
        int start,
        int end,
        int increment,
        char mode = 'r'
        );

  static vidl_movie_sptr load_movie(
        const vcl_vector<vcl_string> &fnames,
        int start,
        int end,
        int increment,
        char mode = 'r'
        );

  static vidl_movie_sptr load_movie(
        vcl_string const& fname,
        char mode = 'r'
        ) { return load_movie(fname, 0, 0, 1, mode); }

  static vidl_movie_sptr load_movie(
        const vcl_list<vcl_string> &fnames,
        char mode = 'r'
        ) { return load_movie(fnames, 0, 0, 1, mode); }

  static vidl_movie_sptr load_movie(
        const vcl_vector<vcl_string> &fnames,
        char mode = 'r'
        ) { return load_movie(fnames, 0, 0, 1, mode); }

  //---------------------------------------------------------
  //   LoadClip
  //---------------------------------------------------------

  static vidl_clip_sptr load_clip(
        vcl_string const& fname,
        int start,
        int end,
        int increment,
        char mode = 'r'
        );

  static vidl_clip_sptr load_clip(
        const vcl_list<vcl_string> &fnames,
        int start, int end,
        int increment,
        char mode = 'r'
        );

  static vidl_clip_sptr load_clip(
        const vcl_vector<vcl_string> &fnames,
        int start, int end,
        int increment,
        char mode = 'r'
        );

  static vidl_clip_sptr load_clip(
        vcl_string const& fname,
        char mode = 'r'
        ) { return load_clip(fname, 0, 0, 1, mode); }

  static vidl_clip_sptr load_clip(
        const vcl_list<vcl_string> &fnames,
        char mode = 'r'
        ) { return load_clip(fnames, 0, 0, 1, mode); }

  static vidl_clip_sptr load_clip(
        const vcl_vector<vcl_string> &fnames,
        char mode = 'r'
        ) { return load_clip(fnames, 0, 0, 1, mode); }

  //---------------------------------------------------------

  static bool save(vidl_movie_sptr movie,
                   vcl_string const& fname,
                   vcl_string const& type);

  // returns vcl_string names  for supported types
  static vcl_list<vcl_string> supported_types();

  // This is no longer used by the new vidl approach of loading all
  // available codecs. This function is present simply to prevent code
  // breakage, and will eventually be removed.
  static void register_codec( vidl_codec* ) {}

  // This is no longer used by the new vidl approach of loading all
  // available codecs. This function is present simply to prevent code
  // breakage, and will eventually be removed.
  static void close() {}

 public:

  // Helpers-------------------------------------------------------------------

  //: Functions when videos are sequences of still images
  // This may go in the public area if some people know
  // they are dealing with images

  static vidl_clip_sptr load_images(
        const vcl_list<vcl_string> &fnames,
        int start,
        int,
        int increment,
        char mode = 'r'
        );

  static vidl_clip_sptr load_images(
        const vcl_vector<vcl_string> &fnames,
        int start,
        int,
        int increment,
        char mode = 'r'
        );

  static vidl_clip_sptr load_images(
        const vcl_list<vcl_string> &fnames,
        char mode = 'r'
        )
        {
          return load_images(fnames, 0, 0, 1, mode);
        }

  static vidl_clip_sptr load_images(
        const vcl_vector<vcl_string> &fnames,
        char mode = 'r'
        )
        {
          return load_images(fnames, 0, 0, 1, mode);
        }

  static bool save_images(vidl_movie_sptr movie,
                          vcl_string const& fname,
                          vcl_string const& type);
};

#endif // vidl_io_h
