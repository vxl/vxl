

#include <vcl_fstream.h>
#include <vcl_vector.h>

#include <vil/vil_save.h>

#include <vidl/vidl_movie_sptr.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_io.h>
#include <vidl/vidl_avicodec.h>
#include <oxp/oxp_vidl_mpeg_codec.h>

int main(int argc, char ** argv)
{
  // ios::sync_with_stdio(false);
  vul_arg<char*> a_filename(0, "Input file");
  vul_arg<int>   a_frame(0, "frame number", 1);
  vul_arg_parse(argc,argv);
  
  if (argc > 1) vul_arg_display_usage_and_exit("Too many arguments\n");

  // Register video codecs
  vidl_io::register_codec(new vidl_avicodec);
  vidl_io::register_codec(new oxp_vidl_mpeg_codec);
  
  vidl_movie moviefile = vidl_io::load_movie(filename(), a_start_frame(), a_end_frame(), increment);
  if (!moviefile || moviefile->width() < 1) {
     vcl_cerr << "glmovie: Couldn't find any movie files. Stopping\n";
     return -1;
  }
  
  // Prime in case it's mpeg
  int s = a_frame() - 14;
  if (s < 0) s = 0;
  moviefile->get_image(s);

  vil_save(moviefile->get_image(a_frame()), );
  vul_printf(vcl_cerr, "glmovie: saved [%s]\n", buf);
  
  return 0;

}
