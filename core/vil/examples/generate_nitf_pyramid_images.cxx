// This is core/vil/examples/generate_nitf_pyramid_images.cxx
#include <vil/file_formats/vil_nitf.h>
//:
// \file
// Generate pyramid images for NITF files.
//     NOTE: Currently, vil_nitf::construct_pyramid_images only works for 16 bit images.
//
// \author    laymon@crd.ge.com
// \date: 2004/1/8
//
// \verbatim
//  Modifications:
//   none yet
// \endverbatim

#include <vcl_cstdlib.h>

#include <vil/vil_open.h>
#include <vil/vil_property.h>
#include <vil/vil_stream.h>

const int DEFAULT_LEVELS = 6;

// ######################################################################
int main(int argc, char **argv)
{
    static vcl_string method_name = "vil/examples/generate_nitf_pyramid_images.cxx: ";

    unsigned int levels = argc>2 ?  vcl_atoi(argv[2]) : DEFAULT_LEVELS;

    const char * test_input_file = argc>1 ? argv[1] :  "schen9_29a.nitf";
    vcl_cout << "argc = " << argc << vcl_endl
             << "input file = <" << test_input_file
             << ">  # levels = " << levels << vcl_endl;

    if (argc < 2) {
      vcl_cout << "Usage: test_generate_pyramid_images.exe <input_file_name> <num_levels>\n";
      vcl_exit(-1);
    }

    vil_stream * input_stream = vil_open(test_input_file, "r");
    vcl_cout << "input opened OK.\n";

    input_stream->ref();
    input_stream->seek(0);  // RESET STREAM TO BEGINNING

    vcl_cout << "create vil_nitf_file_format\n";
    vil_nitf_file_format * file_format = new vil_nitf_file_format();

    vcl_cout << "call vil_nitf_file_format::make_input_image\n";
    vil_image_resource_sptr nitf_image = file_format->make_input_image(input_stream);

    if (nitf_image != static_cast<vil_image_resource_sptr>(0))
    {
      const char * tag_name = vil_property_quantisation_depth;
      unsigned int bits_per_component = 0;
      bool got_property = nitf_image->get_property(tag_name, &bits_per_component);

      if (got_property == false) {
        vcl_cout << "WARNING: failed to get property <" << tag_name << ">\n";
      }

      vcl_cout << method_name << "vil_nitf_image parameters:\n"
               << "    ni = " << nitf_image->ni()
               << "  nj = " << nitf_image->nj()
               << "  nplanes = " << nitf_image->nplanes()
               << vcl_endl
               << "    pixel_format = " << nitf_image->pixel_format()
               << "  vil_property_quantisation_depth = " << bits_per_component
               << vcl_endl;

      // NEED A vil_nitf_image POINTER SO WE CAN INVOKE vil_nitf_image
      // SPECIFIC METHODS, LIKE get_rational_camera.

      vil_nitf_image * temp_nitf_image =
            dynamic_cast<vil_nitf_image*>(nitf_image.as_pointer());

      if (temp_nitf_image == 0) {
        vcl_cerr << method_name << "dynamic cast to vil_nitf_image failed.\n";
      }
      else {
        vcl_string base_file_name = test_input_file;
        vcl_string dir_name = "";

        temp_nitf_image->construct_pyramid_images(levels, base_file_name, dir_name);
      }
    }

    input_stream->unref();  // NO NEED TO EXPLICITLY CLOSE vil_stream.  Just call unref.

    return 0;
}  // end main
