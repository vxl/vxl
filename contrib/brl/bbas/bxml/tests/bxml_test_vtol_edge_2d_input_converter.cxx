// This is brl/bbas/bxml/tests/bxml_test_vtol_edge_2d_input_converter.cxx
#include <testlib/testlib_test.h>
#include <bxml/bxml_io.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vtol/vtol_topology_object.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>

void bxml_test_vtol_edge_2d_input_converter(int argc, char * argv[])
{
  bxml_io::register_input_converters();
  vcl_string test_path = (argc < 2) ? "." : argv[1];
  vcl_string test_file = "/vtol_edge_2d.xml";
  vcl_string full_test_file_path = test_path + test_file;
  vcl_vector<bxml_generic_ptr> pts;
  TEST("bxml_io::parse_xml(full_test_file_path, pts)", bxml_io::parse_xml(full_test_file_path, pts), true);

  for (vcl_vector<bxml_generic_ptr>::iterator pit = pts.begin();
       pit != pts.end(); pit++)
  {
    vsol_spatial_object_2d* so  = (*pit).get_vsol_spatial_object();
    vcl_cout << "Spatial Type " << so->spatial_type() << vcl_endl;
    if (so->spatial_type()==1)
    {
      vtol_topology_object* to = so->cast_to_topology_object();
      vtol_edge* e = to->cast_to_edge();
      vtol_edge_2d_sptr e2d = e->cast_to_edge_2d();
      vcl_cout << "edge:" << *(e2d->v1()) << *(e2d->v2()) << vcl_endl;
      vsol_curve_2d_sptr c = e2d->curve();
      vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
      vdgl_interpolator_sptr intrp = dc->get_interpolator();
      vdgl_edgel_chain_sptr ec = intrp->get_edgel_chain();
      vcl_cout << "edgel_chain size =" << ec->size() << vcl_endl;
      double xdc = dc->get_x(0), xv1=e2d->v1()->cast_to_vertex_2d()->x();
      vcl_cout << "xdc = " << xdc << " xv1 = " << xv1 << vcl_endl;
      TEST("ec->size()==27", ec->size(), 27);
      TEST_NEAR("|xdc-xv1|<=1", xdc, xv1, 1.0);
    }
  }
}

TESTMAIN_ARGS(bxml_test_vtol_edge_2d_input_converter);
