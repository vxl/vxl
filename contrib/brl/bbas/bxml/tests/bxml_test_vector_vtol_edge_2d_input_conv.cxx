// This is brl/bbas/bxml/tests/bxml_test_vector_vtol_edge_2d_input_conv.cxx
#include <testlib/testlib_test.h>
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

void bxml_test_vector_vtol_edge_2d_input_conv(int argc, char* argv[])
{
  bxml_io::register_input_converters();
  vcl_string test_path = (argc < 2) ? "." : argv[1];
  vcl_string full_test_file_path = test_path + "/xml_edge_test_2.xml";
  vcl_vector<bxml_generic_ptr> edgs;
  vcl_vector<vtol_edge_2d_sptr> edges_2d;
  TEST("bxml_io::parse_xml(full_test_file_path, edgs)", bxml_io::parse_xml(full_test_file_path, edgs), true);
  vcl_cout << "Converted " << edgs.size() << " edges\n";
  TEST("edgs.size()==21", edgs.size(), 21);
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
      vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
      vdgl_interpolator_sptr intrp = dc->get_interpolator();
      vdgl_edgel_chain_sptr ec = intrp->get_edgel_chain();
      vcl_cout << "edgel_chain size =" << ec->size() << vcl_endl;
      for (unsigned int i = 0; i<ec->size(); ++i)
        vcl_cout << (*ec)[i] << vcl_endl;
      vcl_cout << vcl_endl << vcl_endl << vcl_endl;
    }
  }
  bxml_input_converter::clear();
  int j = 0;
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges_2d.begin();
       eit!= edges_2d.end(); eit++, j++)
    vcl_cout << "eout[ " << j << ']' << *(*eit) << vcl_endl;
}

TESTMAIN_ARGS(bxml_test_vector_vtol_edge_2d_input_conv);
