// This is oxl/apps/glmovie/vobgetframe.cxx
#include <vil/vil_save.h>

#include <vul/vul_arg.h>
#include <vul/vul_printf.h>

#include <vidl/vidl_movie_sptr.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_io.h>
#ifdef VCL_WIN32
#include <vidl/vidl_avicodec.h>
#endif
#ifdef HAS_MPEG
#include <oxp/oxp_vidl_mpeg_codec.h>
#endif

int main(int argc, char ** argv)
{
  // ios::sync_with_stdio(false);
  vul_arg<char*> a_filename(0, "Input file (e.g. /path/foo for /path/foo.lst)");
  vul_arg<int>   a_frame(0, "frame number", 1);
  vul_arg<char*> a_outfile(0, "Output file");
  vul_arg_parse(argc,argv);

  if (argc > 1)
    vul_arg_display_usage_and_exit("Too many arguments\n");

  // Register video codecs
#ifdef VCL_WIN32
  vidl_io::register_codec(new vidl_avicodec);
#endif
#ifdef HAS_MPEG
  vidl_io::register_codec(new oxp_vidl_mpeg_codec);
#endif

  // Prime in case it's mpeg
  int s = a_frame() - 14;
  if (s < 0) s = 0;

  vidl_movie_sptr moviefile = vidl_io::load_movie(a_filename());
  if (!moviefile || moviefile->width() < 1) {
     vcl_cerr << __FILE__ ": Couldn't find any movie files. Stopping\n";
     return -1;
  }

  // pump-prime
  {
    vcl_clog << "Get frame " << s << vcl_endl;
    unsigned char buf[3];
    moviefile->get_frame(s)->get_image().get_section(buf, 0,0,0,0);
  }

  vil_save(moviefile->get_frame(a_frame())->get_image(), a_outfile());

  vidl_io::close(); // Need to call this to avoid segvs from naughty codecs.

  vul_printf(vcl_clog, __FILE__ ": saved [%s]\n", a_outfile());

  return 0;
}
