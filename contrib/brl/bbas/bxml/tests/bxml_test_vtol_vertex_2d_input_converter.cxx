// This is brl/bbas/bxml/tests/bxml_test_vtol_vertex_2d_input_converter.cxx
#include <testlib/testlib_test.h>
#include <bxml/bxml_io.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>

static void bxml_test_vtol_vertex_2d_input_converter(int argc, char * argv[])
{
  bxml_io::register_input_converters();
  vcl_string test_path = (argc < 2) ? "." : argv[1];
  vcl_string test_file = "/vtol_vertex_2d.xml";
  vcl_string full_test_file_path = test_path + test_file;
  vcl_vector<bxml_generic_ptr> pts;
  TEST("bxml_io::parse_xml(full_test_file_path, pts)", bxml_io::parse_xml(full_test_file_path, pts), true);

  for (vcl_vector<bxml_generic_ptr>::iterator pit = pts.begin();
       pit != pts.end(); pit++)
  {
    vsol_spatial_object_2d* so  = (*pit).get_vsol_spatial_object();
    vcl_cout << "Spatial Type " << so->spatial_type() << vcl_endl;
    if (so->spatial_type()==1){
      vtol_topology_object* to = so->cast_to_topology_object();
      vtol_vertex* v = to->cast_to_vertex();
      vtol_vertex_2d_sptr v2d = v->cast_to_vertex_2d();
      vcl_cout << "vertex" << *v2d << vcl_endl;
      TEST_NEAR("v2d->x() == 177.034", v2d->x(), 177.034, 1e-3);
    }
  }
}

TESTMAIN_ARGS(bxml_test_vtol_vertex_2d_input_converter);
