// This is mul/vil2/vil2_save.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif


#include "vil2_save.h"

#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vil/vil_open.h>
#include <vil/vil_rgb.h>
#include <vil/vil_property.h>
#include <vil2/vil2_new.h>
#include <vil2/vil2_image_data.h>


//: Send vil2_image to disk.
bool vil2_save(const vil2_image_view_base &i, char const* filename, char const* file_format)
{
  vil_stream* os = vil_open(filename, "w");
  if (!os->ok()) {
    vcl_cerr << __FILE__ ": Invalid stream for \"" << filename << "\"\n";
    return false;
  }
  vil2_image_data_sptr out = vil2_new_image_data(os, i.nx(), i.ny(), i.nplanes(),
    i.pixel_format(), file_format);
  if (!out) {
    vcl_cerr << __FILE__ ": (vil2_save) Cannot save to type [" << file_format << "]\n";
    return false;
  }

  return out->put_view(i, 0, 0, 0);

#ifdef VIL2_TO_BE_FIXED
  bool top_first, bgr;
  if (out.get_property(vil2_property_top_row_first, &top_first) && !top_first)
    i = vil2_flipud(i);
  if (i.components() == 3 && out.get_property(vil2_property_component_order_is_BGR, &bgr) && bgr)
    i = vil2_flip_components(i);
#endif
}



static
char const *guess_file_format(char const* filename)
{
  char const *file_format = 0;

  // find last "."
  char const *dot = vcl_strrchr(filename, '.');
  if (!dot) {
    // filename doesn't end in ".anything"
    vcl_cerr << __FILE__ ": assuming pnm format for \'" << filename << "\'\n";
    file_format = "pnm";
  }
  else {
    // translate common extensions into known file formats.
    if (false) { }
#define macro(ext, fmt) else if (!vcl_strcmp(dot, "." #ext)) file_format = #fmt
    macro(bmp, bmp);
    macro(pbm, pnm);
    macro(pgm, pnm);
    macro(ppm, pnm);
    macro(pnm, pnm);
    macro(jpg, jpeg);
    macro(tif, tiff);
    macro(mit, mit);
    macro(gif, gif);
    macro(png, png);
#undef macro
    else {
      //file_format = dot+1; // hope it works.
      vcl_cerr << __FILE__ ": assuming pnm format for \'" << filename << "\'\n";
      file_format = "pnm";
    }
  }

  return file_format;
}

//: save to file, deducing format from filename.
bool vil2_save(const vil2_image_view_base & i, char const* filename)
{
  return vil2_save(i, filename, guess_file_format(filename));
}

