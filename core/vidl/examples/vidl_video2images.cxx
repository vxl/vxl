//:
// \file
// \brief Small application to convert a video file to image files
// \author Fred Wheeler

#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_list.h>
#include <vcl_algorithm.h>
#include <vul/vul_arg.h>
#include <vul/vul_sprintf.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_byte.h>
#include <vil1/vil1_rgb.h>
#include <vidl/vidl_io.h>
#include <vidl/vidl_movie.h>
// windows does not seem to get the -DHAS_MPEG2 option from CMake
#if defined(HAS_MPEG2) || defined(VCL_WIN32)
#include <vidl/vidl_mpegcodec.h>
#endif
#if defined(VCL_WIN32)
#include <vidl/vidl_avicodec.h>
#endif

// assert t is true, otherwise print message m and exit if e is true
#define CHECK_BASE(t,e,m) \
do { \
  if ( ! (t) ) { \
      if (e) vcl_cout << "error: "; \
      else   vcl_cout << "warning: "; \
      vcl_cout << m << vcl_endl; \
      vcl_cout << "use option -help for help\n"; \
      vcl_cout.flush(); \
      if (e) vcl_exit(1); \
  } \
} while (0)

#define CHECKE(t,m0) CHECK_BASE(t,1,m0)
#define CHECKE2(t,m0,m1) CHECK_BASE(t,1,m0<<m1)

// print messages based on the -verbose option setting
#define V1(x) if (verbose() >= 1) vcl_cout << x << vcl_endl
#define V2(x) if (verbose() >= 2) vcl_cout << "  " << x << vcl_endl

inline void
print_help_exit (const char * help_text[])
{
  for (const char ** l = help_text; 0 != *l; l++)
      vcl_cout << *l << vcl_endl;
  vcl_exit(0);
}

// help text printed with -help option
static const char * help_text[] = {
"vidl_video2images",
"",
"Convert selected frames of a video file to individual image files.",
"",
"OPTIONS",
"",
"  -help",
"",
"    Print this help text and exit.",
"",
"  -?",
"",
"    Print a brief description of each option and the default values",
"    for each option.",
"",
"  -verbose INTEGER",
"",
"    Specify the amount of intermediate information printed.  A value",
"    of 0 means print nothing.  More information is printed as the",
"    number increases.  Useful range is 0-2.",
"",
"  -i FILENAME",
"",
"    The input video filename.  Supported formats are MPEG2 and AVI",
"    (AVI in Windows only).",
"",
"  -o FILENAME_TEMPLATE",
"",
"    The output image filename printf-style template.  Each integer",
"    frame number is applied to this template to generate an output",
"    image filename.  The template should have 1 printf escape sequence",
"    that uses an integer.  If only 1 frame is being extracted there is",
"    no need for the escape sequence.  Some examples are:",
"",
"      -o frame%04d.jpg",
"      -o seq/%04d.png",
"",
"  -f INDICES",
"",
"    A comma separated list of frame indices and frame ranges to extract.",
"    Colons (:) are used to specify ranges.  Some examples are:",
"",
"      -f 0,2,4,6,8",
"      -f 0:3,5:8",
"",
0
};

int
main (int argc, char **argv)
{
  vul_arg<bool> help
      ("-help", "print command-line usage help", 0);
  vul_arg<int> verbose
      ("-verbose", "level of informative output", 0);

  vul_arg<const char *> ivfn
      ("-i", "input video filename", 0);
  vul_arg<const char *> oifnt
      ("-o", "output image printf-style filename template", 0);
  vul_arg<vcl_list<int> > frames
      ("-f", "indices of frames to convert");

  vul_arg_parse( argc, argv);

  if (argc > 1) vcl_exit (1);
  if (help()) print_help_exit (help_text);

  CHECKE( ivfn(), "specify input video file with option -i" );

// windows does not seem to get the -DHAS_MPEG2 option from CMake
#if defined(HAS_MPEG2) || defined(VCL_WIN32)
  vidl_io::register_codec (new vidl_mpegcodec);
  V1( "registered MPEG2 codec" );
#endif
#if defined(VCL_WIN32)
  vidl_io::register_codec (new vidl_avicodec);
  V1( "registered AVI codec" );
#endif

  vidl_movie_sptr movie = vidl_io::load_movie (ivfn());
  CHECKE2( movie, "could not load file ", ivfn() );

  V1( "number of frames in video: " << movie->length() );

  // the highest frame index to be converted
  int frame_max = * vcl_max_element (frames().begin(), frames().end());

  vidl_movie::frame_iterator pframe (movie);

  for (pframe = movie->first(); pframe <= movie->last(); pframe = pframe + 1) {

      int i = pframe->get_real_frame_index();

      V2( "frame index: " << i );

      if (frames().end() != vcl_find (frames().begin(), frames().end(), i)) {
          vil1_image frame0 = pframe->get_image();
          V2( "frame image: " << frame0 );
          CHECKE( 3 == frame0.components(),
                  "video frames must have 3 components" );
          CHECKE( 8 == frame0.bits_per_component(),
                  "video frames must have 8 bits per component" );
          vil1_memory_image_of<vil1_rgb<vil1_byte> > frame (frame0);
          if (oifnt()) {
              vcl_string fn = vul_sprintf (oifnt(), i);
              V2( "writing frame to file " << fn );
              vil1_save (frame, fn.c_str());
          }
      }
      else {
          V2( "skip frame" );
      }

      // if we've reached the last frame, stop iterating through the video
      if (i >= frame_max)
          break;
  }

  return 0;
} 
