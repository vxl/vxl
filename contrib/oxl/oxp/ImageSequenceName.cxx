#ifdef __GNUC__
#pragma implementation
#endif

#include "ImageSequenceName.h"

#include <vcl_iostream.h>

ImageSequenceName::ImageSequenceName(char const* s,
                                     char const* read_or_write,
                                     char const* default_extension):
  SequenceFileName(s, read_or_write)
{
  init(read_or_write, default_extension);
}

ImageSequenceName::ImageSequenceName(char const* s, int start_frame, int step,
                                     char const* read_or_write,
                                     char const* default_extension):
  SequenceFileName(s, start_frame, step, read_or_write)
{
  init(read_or_write, default_extension);
}

void ImageSequenceName::init(char const* read_or_write, char const* default_extension)
{
  // Check loading if input_file
  if (read_or_write[0] == 'r') {
    if (!ext_.length()) {
      vcl_cerr << "ImageSequenceName: Searching for extension\n";
      static char const * extensions[] = {
        "",
        "", // Space for default
        ".png",  // Color first
        ".ppm",  // Color first
        ".tif",
        ".jpg",
        ".rgb",
        // Then mono
        ".pgm",
        ".mit",
        0
      };
      ok_ = false;
      if (default_extension)
        extensions[0] = default_extension;
      for (char const* const* p = extensions; *p; ++p)
        if (exists(fmt_, *p, get_start_frame())) {
          ok_ = true;
          ext_ = *p;
          break;
        }
    }
  }
}
