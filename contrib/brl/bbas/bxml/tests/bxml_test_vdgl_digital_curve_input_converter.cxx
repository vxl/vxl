// This is brl/bbas/bxml/tests/bxml_test_vdgl_digital_curve_input_converter.cxx
#include <testlib/testlib_test.h>
#include <bxml/bxml_io.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>

void bxml_test_vdgl_digital_curve_input_converter(int argc, char * argv[])
{
  bxml_io::register_input_converters();
  vcl_string test_path = (argc < 2) ? "." : argv[1];
  vcl_string test_file = "/vdgl_digital_curve.xml";
  vcl_string full_test_file_path = test_path + test_file;
  vcl_vector<bxml_generic_ptr> dcs;
  TEST("bxml_io::parse_xml(full_test_file_path, dcs)", bxml_io::parse_xml(full_test_file_path, dcs), true);
  vdgl_edgel_chain_sptr ec;
  for (vcl_vector<bxml_generic_ptr>::iterator cit = dcs.begin();
       cit != dcs.end(); cit++)
    {
      vsol_spatial_object_2d_sptr so  = (*cit).get_vsol_spatial_object();
      vcl_cout << "Spatial Type " << so->spatial_type() << vcl_endl;
      TEST("so->spatial_type()==3", so->spatial_type(), 3);
      vdgl_digital_curve_sptr dc = so->cast_to_curve()->cast_to_digital_curve();
      vdgl_interpolator_sptr intrp = dc->get_interpolator();
      ec = intrp->get_edgel_chain();
      vcl_cout << ec->size() << vcl_endl;
      TEST("ec->size()==27", ec->size(), 27);
    }
  vcl_cout << "Edgels:\n";
  for (int i = 0; i<ec->size(); i++)
    vcl_cout << (*ec)[i] << vcl_endl;
}

TESTMAIN_ARGS(bxml_test_vdgl_digital_curve_input_converter);
