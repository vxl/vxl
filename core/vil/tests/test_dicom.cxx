// Minimal direct-construction smoke test for the vil_dicom reader.
// Exercises vil_dicom_file_format::make_input_image against the
// tiny (988-byte) 16-bit DICOM fixture already shipped in
// file_read_data/, bypassing the multi-format probe in vil_load.
#include <vil/vil_config.h>
#if HAS_DCMTK

#  include <iostream>
#  include <string>

#  include <vil/vil_open.h>
#  include <vil/vil_stream.h>
#  include <vil/vil_image_view.h>
#  include <vil/vil_pixel_format.h>
#  include <vil/file_formats/vil_dicom.h>

#  include "testlib/testlib_test.h"

static void
test_dicom(int argc, char * argv[])
{
  const std::string base = (argc > 1 ? argv[1] : ".");
  const std::string path = base + "/ff_grey16bit_uncompressed.dcm";

  vil_stream * vs = vil_open(path.c_str(), "r");
  TEST("opened DICOM fixture", vs && vs->ok(), true);
  if (!vs)
    return;
  vs->ref();

  vil_dicom_file_format fmt;
  const vil_image_resource_sptr ir = fmt.make_input_image(vs);
  TEST("vil_dicom_file_format::make_input_image succeeded", static_cast<bool>(ir), true);
  if (!ir)
  {
    vs->unref();
    return;
  }

  TEST("pixel format is 16-bit unsigned", ir->pixel_format(), VIL_PIXEL_FORMAT_UINT_16);
  TEST("nplanes == 1", ir->nplanes(), 1u);
  TEST("ni > 0", ir->ni() > 0, true);
  TEST("nj > 0", ir->nj() > 0, true);

  const vil_image_view_base_sptr view = ir->get_view(0, ir->ni(), 0, ir->nj());
  TEST("decoded a view", static_cast<bool>(view), true);

  vs->unref();
}

TESTMAIN_ARGS(test_dicom);

#else // HAS_DCMTK

#  include <iostream>
#  include "testlib/testlib_test.h"

static void
test_dicom(int /*argc*/, char * /*argv*/[])
{
  std::cout << "DCMTK support not enabled; vil_dicom test skipped\n";
}

TESTMAIN_ARGS(test_dicom);

#endif // HAS_DCMTK
