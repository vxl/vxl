//:
// \file
// \brief Small application to convert a video file to image files
// \author Fred Wheeler

#include <vcl_compiler.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_list.h>
#include <vcl_algorithm.h>
#include <vul/vul_arg.h>
#include <vul/vul_sprintf.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vxl_config.h>
#include <vidl/vidl_io.h>
#include <vidl/vidl_frame.h>
#include <vidl/vidl_movie.h>

// windows does not seem to get the -DHAS_MPEG2 option from CMake
#if defined(HAS_MPEG2) || defined(VCL_WIN32)
#include <vidl/vidl_mpegcodec.h>
#endif
#ifdef VCL_WIN32
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
"  The following options only affect the decoding of MPEG2 files.  They",
"  are used to specify some parameters that are found in the MPEG2",
"  header, which this program is not capable of parsing.",
"",
"  -greyscale",
"",
"    Indicate that the video is greyscale.  The default is color.",
"",
"  -demux_video",
"",
"    Indicate that the video must be demultiplexed.  The default is",
"    that it need not be demultiplexed.",
"",
"  -pid STRING",
"",
"    Specify the video PID.  The default is 0x0000.  If you have to guess,",
"    another value to try is 0x1023.  Note that this looks like an integer",
"    in hex format but is passed as a string.",
"",
0
};

#if defined(HAS_MPEG2) || defined(VCL_WIN32)

static bool callback_greyscale = false;
static bool callback_demux_video = false;
static vcl_string callback_pid = "0x0000";
static int callback_numframes = -1;

// This callback is needed to provide info necessary to initialize the
// MPEG2 codec.  Normally, this would be done by reading the header,
// but that is not implemented yet.
static void
load_mpegcodec_callback (vidl_codec * vc)
{
  vidl_mpegcodec * mpegcodec = vc->castto_vidl_mpegcodec();
  if ( ! mpegcodec) return;
  mpegcodec->set_grey_scale (callback_greyscale);
  if (callback_demux_video) mpegcodec->set_demux_video();
  mpegcodec->set_pid (callback_pid.c_str());
  mpegcodec->set_number_frames (callback_numframes);
  mpegcodec->init();
}

#endif

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

#if defined(HAS_MPEG2) || defined(VCL_WIN32)
  vul_arg<bool> greyscale
      ("-greyscale", "is the video greyscale (MPEG2 only)?", false);
  vul_arg<bool> demux_video
      ("-demux_video", "should the video be demuxed (MPEG2 only)?", false);
  vul_arg<const char *> pid
      ("-pid", "the video PID (MPEG2 only)", "0x0000");
#endif

  vul_arg_parse( argc, argv);

  if (argc > 1) vcl_exit (1);
  if (help()) print_help_exit (help_text);

  CHECKE( ivfn(), "specify input video file with option -i" );

#if defined(HAS_MPEG2) || defined(VCL_WIN32)
  callback_greyscale = greyscale();
  callback_demux_video = demux_video();
  callback_pid = pid();
  callback_numframes = -1;
#endif

// windows does not seem to get the -DHAS_MPEG2 option from CMake
#if defined(HAS_MPEG2) || defined(VCL_WIN32)
  vidl_io::register_codec (new vidl_mpegcodec);
  V1( "registered MPEG2 codec" );
#endif
#ifdef VCL_WIN32
  vidl_io::register_codec (new vidl_avicodec);
  V1( "registered AVI codec" );
#endif

#if defined(HAS_MPEG2) || defined(VCL_WIN32)
  vidl_io::load_mpegcodec_callback = &load_mpegcodec_callback;
#endif

  vidl_movie_sptr movie = vidl_io::load_movie (ivfn());
  CHECKE2( movie, "could not load file ", ivfn() );

  V1( "number of frames in video: " << movie->length() );

  // the highest frame index to be converted
  int frame_max = * vcl_max_element (frames().begin(), frames().end());

  for (vidl_movie::frame_iterator pframe = movie->begin(); pframe < movie->end(); ++pframe)
  {
    int i = pframe->get_real_frame_index();

    V2( "frame index: " << i );

    if (frames().end() != vcl_find (frames().begin(), frames().end(), i))
    {
      vil_image_view_base_sptr frame0 = pframe->get_view();
      V2( "frame image: " << frame0 );
      CHECKE( 1 == vil_pixel_format_sizeof_components(frame0->pixel_format()),
              "video frames must have 8 bits per component" );
      vil_image_view<vxl_byte> frame (frame0);
      if (oifnt()) {
        vcl_string fn = vul_sprintf (oifnt(), i);
        V2( "writing frame to file " << fn );
        vil_save (frame, fn.c_str());
      }
    }
    else
      V2( "skip frame" );

    // if we've reached the last frame, stop iterating through the video
    if (i >= frame_max)
      break;
  }

  return 0;
} 
