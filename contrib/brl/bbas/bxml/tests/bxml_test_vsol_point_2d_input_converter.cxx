// This is brl/bbas/bxml/tests/bxml_test_vsol_point_2d_input_converter.cxx
#include <bxml/bxml_io.h>
#include <vsol/vsol_point_2d.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

int main(int argc, char * argv[])
{
  int success=0, failures=0;

  // we want to test the methods on bxml_vsol_point_2d_input_converter
  vcl_cout << "Testing bxml_vsol_point_2d_input_converter" << vcl_endl;

  bxml_io::register_input_converters();
  vcl_string test_path = (argc < 2) ? "" : argv[1];
  vcl_string test_file = "vsol_point_2d.xml";
  vcl_string full_test_file_path = test_path + test_file;
  vcl_vector<bxml_generic_ptr> pts;
  Assert(bxml_io::parse_xml(full_test_file_path, pts));

  for (vcl_vector<bxml_generic_ptr>::iterator pit = pts.begin();
       pit != pts.end(); pit++)
    {
      vsol_spatial_object_2d_sptr so  = (*pit).get_vsol_spatial_object();
      vcl_cout << "Spatial Type " << so->spatial_type() << vcl_endl;
      Assert(so->spatial_type()==2);
    }
  vcl_cout << "finished testing vxml_vsol_point_2d_input_converter\n";
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
