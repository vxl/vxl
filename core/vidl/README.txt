vidl - The VXL video library.

Registering your codecs is no longer needed.
Use vidl_io to load a movie.  Here's a program 
which prints info about a movie:

#include <vidl/vidl_io.h>

int main(int argc, char** argv) 
{

  vidl_movie_sptr movie = vidl_io::load_movie(argv[1]);
  vcl_cerr << "Length = " << movie->length() << vcl_endl;
}
