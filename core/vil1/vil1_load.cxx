
#include <vil/vil_load.h>
#include <vil/vil_file_format.h>
#include <vil/vil_stream_fstream.h>

vil_generic_image* vil_load(char const* filename)
{
  vil_stream_fstream* is = new vil_stream_fstream(filename, "r");
  
  for(vil_file_format** p = vil_file_format::all(); *p; ++p) {
#if 1 // debugging
    cerr << __FILE__ " : trying \'" << (*p)->tag() << "\'" << endl;
#endif
    is->seek(0);
    vil_generic_image* i = (*p)->make_input_image(is);
    if (i)
      return i;
  }
  cerr << "vil_load: Failed to load [" << filename << "]\n";
  return 0;
}
