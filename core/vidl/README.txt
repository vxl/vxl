vidl - The VXL video library.

To get started, register your codecs and use vidl_io to
load a movie.  Here's a program which prints info about 
a movie:

#include <vidl/vidl_io.h>
#include <vidl/vidl_avicodec.h>

int main(int argc, char** argv) 
{
  // Register video codec
  vidl_io::register_codec(new vidl_avicodec);

  vidl_movie_sptr movie = vidl_io::load_movie(argv[1]);
  vcl_cerr << "Length = " << movie->length() << vcl_endl;
}
