// .NAME example_convert
// .SECTION Description
//  This example program shows a typical use of the vipl_convert IP class on
//  any image.  The input image (argv[1]) is converted to a ubyte image
//  and written to argv[2] which is always a PGM file image.
//  When the input image is RGB, its intensity is extracted.
//  To this end an explicit converter from vil_rgb<ubyte> to ubyte is necessary.
//  When it is short, int or float, an implicit mapping is done to 0--255.
//  Uses vipl_convert<vil_image,vil_image,*,ubyte>.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven, ESAT/PSI, 29 may 1998
//
// .SECTION Modifications
//   Peter Vanroose, Aug.2000 - adapted to vxl
//
#include <vil/vil_memory_image_of.h>
#include <vipl/vipl_convert.h>
#include <vil/vil_rgb.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl/vcl_iostream.h>

typedef unsigned char ubyte;
typedef vil_rgb<ubyte> rgbcell;

int
main(int argc, char** argv) {
  if (argc < 3) { cerr << "Syntax: example_convert file_in file_out\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);

  // The output image:
  vil_memory_image_of<ubyte> out_grey(in);
  vil_memory_image_of<rgbcell> out_rgb(in);

  if (in.component_format() == VIL_COMPONENT_FORMAT_UNSIGNED_INT) {
    if (in.planes() == 1 && in.components() == 1) { // monochrome
      if (in.bits_per_component() == 8) {
        cerr<<"Warning: no conversion necessary\n";
        vipl_convert<vil_image,vil_image,ubyte,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
        op.put_in_data_ptr(&in); op.put_out_data_ptr(&out_grey); op.filter();
        cout << "vipl_converted ubyte image to PGM image " << argv[2] << endl;
      } else if (in.bits_per_component() == 16) {
        vipl_convert<vil_image,vil_image,short,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
        op.put_in_data_ptr(&in); op.put_out_data_ptr(&out_grey); op.filter();
        cout << "vipl_converted short image to PGM image " << argv[2] << endl;
      } else if (in.bits_per_component() == 32) {
        vipl_convert<vil_image,vil_image,int,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
        op.put_in_data_ptr(&in); op.put_out_data_ptr(&out_grey); op.filter();
        cout << "vipl_converted int image to PGM image " << argv[2] << endl;
      } else if (in.bits_per_component() == 64) {
        cerr << "Please instantiate the vipl_convert IP filter for type long\n";
      } else {
        cerr<<"Don't know an integer type with "<<in.bits_per_component()<<" bits\n";
      }
    } else if (in.planes() == 1 && in.components() == 3) { // colour (RGB)
      if (in.bits_per_component() == 8) {
        vipl_convert<vil_image,vil_image,rgbcell,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
        op.put_in_data_ptr(&in); op.put_out_data_ptr(&out_rgb); op.filter();
        cout << "vipl_converted RGB image to PPM image " << argv[2] << endl;
      } else {
        cerr<<"Don't know a colour type with "<<in.bits_per_component()<<" bits per component\n";
      }
    }
    else cerr << "Cannot handle image with "<< in.planes() <<" planes and "<< in.components() <<" components\n";
  }
  else if (in.component_format() == VIL_COMPONENT_FORMAT_IEEE_FLOAT) {
    if (in.components() == 1 && in.planes() == 1) { // monochrome
      if (in.bits_per_component() == 32) {
        vipl_convert<vil_image,vil_image,float,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
        op.put_in_data_ptr(&in); op.put_out_data_ptr(&out_grey); op.filter();
        cout << "vipl_converted float image to PGM image " << argv[2] << endl;
      } else if (in.bits_per_component() == 64) {
        vipl_convert<vil_image,vil_image,double,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
        op.put_in_data_ptr(&in); op.put_out_data_ptr(&out_grey); op.filter();
        cout << "vipl_converted double image to PGM image " << argv[2] << endl;
      } else
       	cerr << "Cannot handle float image with "<< in.bits_per_component() <<" bits per component\n";
    } else
      cerr << "Cannot handle float image with "<< in.planes() <<" planes and "<< in.components() <<" components\n";
  } else if (in.component_format() == VIL_COMPONENT_FORMAT_COMPLEX) {
      cerr << "Cannot currently handle complex pixel type images\n";
#if 0
  } else if (in.component_format() == VIL_COMPONENT_FORMAT_PYRAMID) {
      cerr << "Cannot currently handle pyramid images\n";
  } else if (in.component_format() == VIL_COMPONENT_FORMAT_BANDED) {
      cerr << "Cannot currently handle banded images\n";
#endif
  }
  else cerr << "Never heared of image format " << in.component_format() << endl;

  if (in.planes() == 1 && in.components() == 1) 
    vil_save(out_grey, argv[2], "pnm");
  else
    vil_save(out_rgb, argv[2], "pnm");

  return 0;
}
