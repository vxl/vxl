vidl1 - The VXL video library.

Registering your codecs is no longer needed.
Use vidl1_io to load a movie.  Here's a program 
which prints info about a movie:

#include <vidl1/vidl1_io.h>

int main(int argc, char** argv) 
{

  vidl1_movie_sptr movie = vidl1_io::load_movie(argv[1]);
  vcl_cerr << "Length = " << movie->length() << vcl_endl;
}
