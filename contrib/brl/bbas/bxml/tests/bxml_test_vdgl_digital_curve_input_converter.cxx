// This is brl/bbas/bxml/tests/bxml_test_vdgl_digital_curve_input_converter.cxx
#include <bxml/bxml_io.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>


#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

#define test_path "$VXLROOT/contrib/brl/bbas/bxml/tests/"

int main(int, char **)
{
  int success=0, failures=0;

  // we want to test the methods on bxml_vdgl_digital_curve_input_converter
  vcl_cout << "Testing bxml_vdgl_digital_curve_input_converter\n";

  bxml_io::register_input_converters();
  vcl_string test_file = "vdgl_digital_curve.xml";
  vcl_string full_test_file_path = test_path + test_file;
  vcl_vector<bxml_generic_ptr> dcs;
  Assert(bxml_io::parse_xml(full_test_file_path, dcs));
  vdgl_edgel_chain_sptr ec;
  for (vcl_vector<bxml_generic_ptr>::iterator cit = dcs.begin();
       cit != dcs.end(); cit++)
    {
      vsol_spatial_object_2d_sptr so  = (*cit).get_vsol_spatial_object();
      vcl_cout << "Spatial Type " << so->spatial_type() << vcl_endl;
      Assert(so->spatial_type()==3);
      vdgl_digital_curve_sptr dc = so->cast_to_curve()->cast_to_digital_curve();
      vdgl_interpolator_sptr intrp = dc->get_interpolator();
      ec = intrp->get_edgel_chain();
      vcl_cout << ec->size() << vcl_endl;
      Assert(ec->size()==27);
    }
  vcl_cout << "Edgels:\n";
  for (int i = 0; i<ec->size(); i++)
    vcl_cout << (*ec)[i] << vcl_endl;

  vcl_cout << "finished testing vxml_vdgl_digital_curve_input_converter\n";
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
