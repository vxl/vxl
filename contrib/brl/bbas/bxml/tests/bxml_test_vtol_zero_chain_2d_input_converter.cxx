// This is brl/bbas/bxml/tests/bxml_test_vtol_zero_chain_2d_input_converter.cxx
#include <vcl_cmath.h>
#include <bxml/bxml_io.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>

bool near_eq(double x, double y)
{
  double d = x-y;
  double er = vcl_abs(d);
  return er<1e-03;
}


#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

int main(int argc, char * argv[])
{
  int success=0, failures=0;

  // we want to test the methods on bxml_vtol_zero_chain_2d_input_converter
  vcl_cout << "Testing bxml_vtol_zero_chain_2d_input_converter" << vcl_endl;

  bxml_io::register_input_converters();
  vcl_string test_path = (argc < 2) ? "" : argv[1];
  vcl_string test_file = "vtol_zero_chain_2d.xml";
  vcl_string full_test_file_path = test_path + test_file;
  vcl_vector<bxml_generic_ptr> pts;
  Assert(bxml_io::parse_xml(full_test_file_path, pts));

  for (vcl_vector<bxml_generic_ptr>::iterator pit = pts.begin();
       pit != pts.end(); pit++)
  {
    vsol_spatial_object_2d* so  = (*pit).get_vsol_spatial_object();
    vcl_cout << "Spatial Type " << so->spatial_type() << vcl_endl;
    if (so->spatial_type()==1)
    {
      vtol_topology_object* to = so->cast_to_topology_object();
      vtol_zero_chain* zc = to->cast_to_zero_chain();
      vtol_zero_chain_sptr zc2d = zc->cast_to_zero_chain();
      vcl_cout << "zero_chain " << *zc2d << vcl_endl;
      vcl_vector<vtol_vertex_sptr>* verts = zc->vertices();
      Assert(verts!=0);
      if (verts)
        {
          vtol_vertex_2d_sptr v2d = zc2d->v0()->cast_to_vertex_2d();
          vcl_cout << "v0" << *(zc2d->v0()->cast_to_vertex_2d()) << vcl_endl;
          Assert(near_eq(v2d->x(), 191.468));
        }
    }
  }
  vcl_cout << "finished testing vxml_vtol_zero_chain_2d_input_converter\n";
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
