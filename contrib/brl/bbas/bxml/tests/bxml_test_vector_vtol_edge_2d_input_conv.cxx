// This is brl/bbas/bxml/tests/bxml_test_vector_vtol_edge_2d_input_conv.cxx
#include <vcl_cmath.h>
#include <bxml/bxml_io.h>
#include <bxml/bxml_input_converter.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vtol/vtol_topology_object.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>

bool near_eq(double x, double y)
{
  double d = x-y;
  double er = vcl_abs(d);
  return er<1e-03;
}


#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

#define test_path "$VXLROOT/contrib/brl/bbas/bxml/tests/"
//#define test_path "c:/vxl/vxl/contrib/brl/bbas/bxml/tests/"

int main(int, char **)
{
  int success=0, failures=0;

  // we want to test the methods on bxml_vtol_edge_2d_input_converter
  vcl_cout << "Testing bxml_vector_vtol_edge_2d_input_converter\n";

  bxml_io::register_input_converters();
  vcl_string test_file = "xml_edge_test_2.xml";
  vcl_string full_test_file_path = test_path + test_file;
  vcl_vector<bxml_generic_ptr> edgs;
  vcl_vector<vtol_edge_2d_sptr> edges_2d;
  Assert(bxml_io::parse_xml(full_test_file_path, edgs));
  vcl_cout << "Converted " << edgs.size() << " edges\n";
  Assert(edgs.size()==21);
  for (vcl_vector<bxml_generic_ptr>::iterator eit = edgs.begin();
       eit != edgs.end(); eit++)
  {
    vsol_spatial_object_2d* so  = (*eit).get_vsol_spatial_object();
    vcl_cout << "Spatial Type " << so->spatial_type() << vcl_endl;
    if (so->spatial_type()==1)
    {
      vtol_topology_object* to = so->cast_to_topology_object();
      vtol_edge* e = to->cast_to_edge();
      vtol_edge_2d_sptr e2d = e->cast_to_edge_2d();
      edges_2d.push_back(e2d);
      vcl_cout << "edge:" << *(e2d->v1()) << *(e2d->v2()) << vcl_endl;
      vsol_curve_2d_sptr c = e2d->curve();
      vdgl_digital_curve_sptr dc = c->cast_to_digital_curve();
      vdgl_interpolator_sptr intrp = dc->get_interpolator();
      vdgl_edgel_chain_sptr ec = intrp->get_edgel_chain();
      vcl_cout << "edgel_chain size =" << ec->size() << vcl_endl;
      for (int i = 0; i<ec->size(); i++)
        vcl_cout << (*ec)[i] << vcl_endl;
      vcl_cout << vcl_endl << vcl_endl << vcl_endl ;
    }
  }
  bxml_input_converter::clear();
  int j = 0;
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges_2d.begin();
       eit!= edges_2d.end(); eit++, j++)
    vcl_cout << "eout[ " << j << "]" << *(*eit) << vcl_endl;

  vcl_cout << "finished testing vxml_vtol_edge_2d_input_converter\n";

  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
