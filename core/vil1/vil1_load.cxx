
#include "vil_load.h"
#include <vil/vil_file_format.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_image.h>

vil_image vil_load(char const* filename)
{
  vil_stream_fstream* is = new vil_stream_fstream(filename, "r");
  if (!is->ok()) {
    delete is;
    return 0;
  }
  
  for(vil_file_format** p = vil_file_format::all(); *p; ++p) {
#if 0 // debugging
    cerr << __FILE__ " : trying \'" << (*p)->tag() << "\'" << endl;
#endif
    is->seek(0);
    vil_image i = (*p)->make_input_image(is);
    if (i)
      return i;
  }

  // failed.
  cerr << "vil_load: Failed to load [" << filename << "]" << endl
       << "vil_load: Tried";
  for (vil_file_format** p = vil_file_format::all(); *p; ++p)
    // 'flush' in case of segfault next time through loop. Else, we
    // will not see those printed tags still in the stream buffer.
    cerr << " \'" << (*p)->tag() << "\'" << flush; 
  cerr << endl;

  return 0;
}
