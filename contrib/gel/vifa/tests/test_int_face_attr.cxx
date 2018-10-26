// This is gel/vifa/tests/test_int_face_attr.cxx
#include <iostream>
#include <ostream>
#include <vcl_compiler.h>
#include <vxl_config.h>
#include <testlib/testlib_test.h>
#include <sdet/sdet_detector_params.h>
#include <vdgl/vdgl_fit_lines_params.h>
#include <sdet/sdet_region_proc.h>
#include <sdet/sdet_region_proc_params.h>
#include <vifa/vifa_int_face_attr.h>
#include <vifa/vifa_typedefs.h>
#include <vil/vil_image_view.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_image_as.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_vil.h>


static void test_int_face_attr(int argc, char* argv[])
{
  // Get the image
  vil1_image  test_img_raw = vil1_load(argc<2 ? "testimg.tif" : argv[1]);

  if (test_img_raw)
  {
    // Convert the image to greyscale
    vil1_image      test_img = vil1_image_as_byte(test_img_raw);

    // Get a vil-compatible version of the greyscale image
    // (for normalization)
    vil1_memory_image_of<vxl_byte>  test_img_mem(test_img);
    vil_image_view_base* vil_test_img
      = new vil_image_view<vxl_byte>(vil1_to_vil_image_view(test_img_mem));

    // Set up the image's normalization
    vifa_norm_params np(0, 0.05f, 0, 0.95f);
    np.get_norm_bounds(vil_test_img,
                       np.plow, np.phigh,
                       np.ilow, np.ihigh);
    np.recompute();

    std::cout << "Normalization params:\n";
    np.print_info();

    // Set up the line fitting parameters
    vdgl_fit_lines_params  flp(5);

    std::cout << flp;

    // Get the detector & region processor parameters
    sdet_detector_params  dp;
    sdet_region_proc_params  rpp(dp, false, false, 1);
    std::cout << rpp;

    // Instantiate a region processor & attach the test image
    sdet_region_proc  rp(rpp);
    rp.set_image(test_img);

    // Segment the image
    rp.extract_regions();

    // Get the intensity face list
    iface_list&  region_list = rp.get_regions();

    std::cout << region_list.size() << " intensity faces found:\n";

    // For each intensity face...
    iface_iterator  ifi = region_list.begin();
    int  i = 1;
    for (; ifi != region_list.end(); ifi++, i++)
    {
      vtol_intensity_face_sptr  face = (*ifi);

      std::cout << "  Intensity Face #" << i << " at ("
               << face->Xo() << ", " << face->Yo() << "): " << (*face);

      // Compute the attributes.  Set the normalization params
      // for the image, but use defaults otherwise.
      vifa_int_face_attr  ifa(face, &flp, nullptr, nullptr, &np);
      if (ifa.ComputeAttributes())
      {
        // Retrieve the attribute vector
        std::vector<float>    attrs;
        std::vector<std::string>  attr_names;
        ifa.GetAttributes(attrs);
        ifa.GetAttributeNames(attr_names);

        std::cout << attrs.size() << " attributes, "
                 << attr_names.size() << " attribute names:\n";

        // Dump the attribute vector
        std::vector<float>::iterator      ai = attrs.begin();
        std::vector<std::string>::iterator  ani = attr_names.begin();
        for (; (ai != attrs.end()) && (ani != attr_names.end()); ai++, ani++)
          std::cout << (*ani) << ": " << (*ai) << std::endl;
      }
      else
        std::cout << "vifa_int_face_attr::ComputeAttributes() failed for face #"
                 << i << "!\n";
    }
  }
  else
    std::cout << "Could not load image -- aborting!\n";
}


TESTMAIN_ARGS(test_int_face_attr);
