//:
// \file
//  This example program shows a typical use of the vipl_convert IP class on
//  any image.  The input image (argv[1]) is converted to a ubyte image
//  and written to argv[2] which is always a PGM file image.
//  When the input image is RGB, its intensity is extracted.
//  To this end an explicit converter from vil_rgb<ubyte> to ubyte is necessary.
//  When it is short, int or float, an implicit mapping is done to 0--255.
//  Uses vipl_convert<vil_image,vil_image,*,ubyte>.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   29 may 1998
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_convert.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>

typedef unsigned char ubyte;
typedef vil_rgb<ubyte> rgbcell;

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example_convert file_in file_out\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);

  // The output image:
  vil_memory_image_of<ubyte> out(in);

  if (in.component_format() == VIL_COMPONENT_FORMAT_UNSIGNED_INT) {
    if (in.planes() == 1 && in.components() == 1) { // monochrome
      if (in.bits_per_component() == 8) {
        // no conversion necessary
        out = in;
        vcl_cout << "vipl_converted ubyte image to PGM image " << argv[2] << vcl_endl;
      } else if (in.bits_per_component() == 16) {
        vipl_convert<vil_image,vil_image,short,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
        op.put_in_data_ptr(&in); op.put_out_data_ptr(&out); op.filter();
        vcl_cout << "vipl_converted short image to PGM image " << argv[2] << vcl_endl;
      } else if (in.bits_per_component() == 32) {
        vipl_convert<vil_image,vil_image,int,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
        op.put_in_data_ptr(&in); op.put_out_data_ptr(&out); op.filter();
        vcl_cout << "vipl_converted int image to PGM image " << argv[2] << vcl_endl;
      } else if (in.bits_per_component() == 64) {
        vcl_cerr << "Please instantiate the vipl_convert IP filter for type long\n";
        return 1;
      } else {
        vcl_cerr<<"Don't know an integer type with "<<in.bits_per_component()<<" bits\n";
        return 1;
      }
    } else if (in.planes() == 1 && in.components() == 3) { // colour (RGB)
      if (in.bits_per_component() == 8) {
        vipl_convert<vil_image,vil_image,rgbcell,ubyte> op;
        op.put_in_data_ptr(&in); op.put_out_data_ptr(&out); op.filter();
        vcl_cout << "vipl_converted RGB image to PGM image " << argv[2] << vcl_endl;
      } else {
        vcl_cerr<<"Don't know a colour type with "<<in.bits_per_component()<<" bits per component\n";
        return 1;
      }
    }
    else {
      vcl_cerr << "Cannot handle image with "<< in.planes() <<" planes and "<< in.components() <<" components\n";
      return 1;
    }
  }
  else if (in.component_format() == VIL_COMPONENT_FORMAT_IEEE_FLOAT) {
    if (in.components() == 1 && in.planes() == 1) { // monochrome
      if (in.bits_per_component() == 32) {
        vipl_convert<vil_image,vil_image,float,ubyte> op;
        op.put_in_data_ptr(&in); op.put_out_data_ptr(&out); op.filter();
        vcl_cout << "vipl_converted float image to PGM image " << argv[2] << vcl_endl;
      } else if (in.bits_per_component() == 64) {
        vipl_convert<vil_image,vil_image,double,ubyte> op;
        op.put_in_data_ptr(&in); op.put_out_data_ptr(&out); op.filter();
        vcl_cout << "vipl_converted double image to PGM image " << argv[2] << vcl_endl;
      } else {
        vcl_cerr << "Cannot handle float image with "<< in.bits_per_component() <<" bits per component\n";
        return 1;
      }
    } else {
      vcl_cerr << "Cannot handle float image with "<< in.planes() <<" planes and "<< in.components() <<" components\n";
      return 1;
    }
  } else if (in.component_format() == VIL_COMPONENT_FORMAT_COMPLEX) {
      vcl_cerr << "Cannot currently handle complex pixel type images\n";
      return 1;
#if 0
  } else if (in.component_format() == VIL_COMPONENT_FORMAT_PYRAMID) {
      vcl_cerr << "Cannot currently handle pyramid images\n";
      return 1;
  } else if (in.component_format() == VIL_COMPONENT_FORMAT_BANDED) {
      vcl_cerr << "Cannot currently handle banded images\n";
      return 1;
#endif
  }
  else {
    vcl_cerr << "Never heared of image format " << int(in.component_format()) << vcl_endl;
    return 1;
  }

  vil_save(out, argv[2], "pnm");
  return 0;
}
