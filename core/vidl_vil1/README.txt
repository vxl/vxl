vidl_vil1 - The VXL video library (using vil1).

To get started, register your codecs and use vidl_vil1_io to
load a movie.  Here's a program which prints info about 
a movie:

#include <vidl_vil1/vidl_vil1_io.h>
#include <vidl_vil1/vidl_vil1_avicodec.h>

int main(int argc, char** argv) 
{
  // Register video codec
  vidl_vil1_io::register_codec(new vidl_vil1_avicodec);

  vidl_vil1_movie_sptr movie = vidl_vil1_io::load_movie(argv[1]);
  vcl_cerr << "Length = " << movie->length() << vcl_endl;
}
