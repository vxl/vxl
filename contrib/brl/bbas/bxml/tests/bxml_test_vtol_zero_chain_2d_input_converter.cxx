// This is brl/bbas/bxml/tests/bxml_test_vtol_zero_chain_2d_input_converter.cxx
#include <testlib/testlib_test.h>
#include <bxml/bxml_io.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>

static void bxml_test_vtol_zero_chain_2d_input_converter(int argc, char * argv[])
{
  bxml_io::register_input_converters();
  vcl_string test_path = (argc < 2) ? "." : argv[1];
  vcl_string test_file = "/vtol_zero_chain_2d.xml";
  vcl_string full_test_file_path = test_path + test_file;
  vcl_vector<bxml_generic_ptr> pts;
  TEST("bxml_io::parse_xml(full_test_file_path, pts)", bxml_io::parse_xml(full_test_file_path, pts), true);

  for (vcl_vector<bxml_generic_ptr>::iterator pit = pts.begin();
       pit != pts.end(); pit++)
  {
    vsol_spatial_object_2d* so  = (*pit).get_vsol_spatial_object();
    vcl_cout << "Spatial Type " << so->get_name() << vcl_endl;
    vtol_topology_object* to = so->cast_to_topology_object();
    if (to)
    {
      vtol_zero_chain_sptr zc = to->cast_to_zero_chain();
      vcl_cout << "zero_chain " << *zc << vcl_endl;
      vcl_vector<vtol_vertex_sptr>* verts = zc->vertices();
      TEST("verts!=0", !verts, false);
      if (verts)
      {
        vtol_vertex_2d_sptr v2d = zc->v0()->cast_to_vertex_2d();
        vcl_cout << "v0" << *(zc->v0()->cast_to_vertex_2d()) << vcl_endl;
        TEST_NEAR("v2d->x() == 191.468", v2d->x(), 191.468, 1e-3);
      }
    }
  }
}

TESTMAIN_ARGS(bxml_test_vtol_zero_chain_2d_input_converter);
