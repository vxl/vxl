// This is brl/bbas/bxml/tests/bxml_test_vsol_point_2d_input_converter.cxx
#include <testlib/testlib_test.h>
#include <bxml/bxml_io.h>
#include <vsol/vsol_point_2d.h>

void bxml_test_vsol_point_2d_input_converter(int argc, char * argv[])
{
  bxml_io::register_input_converters();
  vcl_string test_path = (argc < 2) ? "." : argv[1];
  vcl_string test_file = "/vsol_point_2d.xml";
  vcl_string full_test_file_path = test_path + test_file;
  vcl_vector<bxml_generic_ptr> pts;
  TEST("bxml_io::parse_xml(full_test_file_path, pts)", bxml_io::parse_xml(full_test_file_path, pts), true);

  for (vcl_vector<bxml_generic_ptr>::iterator pit = pts.begin();
       pit != pts.end(); pit++)
  {
    vsol_spatial_object_2d_sptr so  = (*pit).get_vsol_spatial_object();
    vcl_cout << "Spatial Type " << so->spatial_type() << vcl_endl;
    TEST("so->spatial_type()==2", so->spatial_type(), 2);
  }
}

TESTMAIN_ARGS(bxml_test_vsol_point_2d_input_converter);
