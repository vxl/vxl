#include <bxml/bxml_io.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <bxml/bxml_input_converter.h>
#include <bxml/bxml_vtol_io.h>
#include <bxml/bxml_generic_ptr.h>

bxml_vtol_io::bxml_vtol_io()
{
}

bxml_vtol_io::~bxml_vtol_io()
{
}

bool bxml_vtol_io::read_edges(vcl_string& xml_file_name,
                              vcl_vector<vtol_edge_2d_sptr>& edges )
{
  bxml_io::register_input_converters();
  vcl_vector<bxml_generic_ptr> ptrs;
  if (!bxml_io::parse_xml(xml_file_name, ptrs))
    return false;
  if (!ptrs.size())
    return false;
  for (vcl_vector<bxml_generic_ptr>::iterator pit = ptrs.begin();
       pit != ptrs.end(); pit++)
  {
    vsol_spatial_object_2d* so  = (*pit).get_vsol_spatial_object();
    if (!so) continue;
    vtol_topology_object* to = so->cast_to_topology_object();
    if (to) {
      vtol_edge_2d_sptr e2d = to->cast_to_edge()->cast_to_edge_2d();
      if (e2d)
        edges.push_back(e2d);
      else
        return false;
    }
  }
  bxml_input_converter::clear();
  return edges.size()>0;
}
