#include <vcl_iostream.h> //vcl_cout
#include <vcl_string.h>
#include <dom/DOM_Element.hpp>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_zero_chain.h>
#include <bxml/bxml_vtol_vertex_2d_input_converter.h>
#include <bxml/bxml_vtol_zero_chain_2d_input_converter.h>

bxml_vtol_zero_chain_2d_input_converter::bxml_vtol_zero_chain_2d_input_converter()
{
  class_name_ = "vtol_zero_chain_2d";
  tag_name_ = "zero_chain_3d";
  ref_tag_name_ = "zero_chain_3d_ref";
}

bxml_vtol_zero_chain_2d_input_converter::~bxml_vtol_zero_chain_2d_input_converter()
{
}

vcl_string bxml_vtol_zero_chain_2d_input_converter::get_id(DOM_Node& node)
{
  return get_string_attr(node,"id");
}

bool bxml_vtol_zero_chain_2d_input_converter::extract_from_dom(DOM_Node& node)
{
  new_or_ref = check_tag(node);

  if (new_or_ref == 0) {
    return false;
  }
  else if (new_or_ref == 2) {
    // ref node
    return extract_object_atrs(node);
  }
  else {
    extract_object_atrs(node);
  }

  if (debug_)
    vcl_cout << "vtol_zero_chain_2d: id_=" << id_ << vcl_endl;

  // clear out list in case converter is called multiple times
  verts_.clear();

  int num_children=0;
  DOM_Node child = node.getFirstChild();
  while (child != 0)
  {
    int cnode_type = child.getNodeType();
    if (cnode_type == DOM_Node::ELEMENT_NODE)
    {
      bxml_vtol_vertex_2d_input_converter conv;
      if (conv.extract_from_dom(child)) {
        bxml_generic_ptr gp_pt = conv.construct_object();
        vtol_vertex_2d* vert = gp_pt.get_vsol_spatial_object()->cast_to_topology_object()->cast_to_vertex()->cast_to_vertex_2d();
        if (!vert) {
          vcl_cout << "Error vtol_zero_chain_2d unable to read vertex_3d\n";
          break;
        }
        if (debug_)
          vcl_cout << "vtol_zero_chain_2d: vert=" << (*vert)
                   << " num_children= " << num_children << vcl_endl;
        verts_.push_back(vert);
        num_children++;
      }
      else {
        vcl_cout << "something wrong, no vertex\n";
      }
    }
    child = child.getNextSibling();
  }
  return true;
}

bxml_generic_ptr bxml_vtol_zero_chain_2d_input_converter::construct_object()
{
  if (new_or_ref == 1) {
    vtol_zero_chain* zc = new vtol_zero_chain(verts_);
    bxml_generic_ptr gp(zc);
    if ( !(id_ == null_id_) ) {
      obj_table_[id_] = gp;
      //      zc->ref();
     }
    return gp;
  }
  else {
    bxml_generic_ptr gp = obj_table_[id_];
    return gp;
  }
}

bool bxml_vtol_zero_chain_2d_input_converter::extract_object_atrs(DOM_Node& node)
{
  id_ = get_string_attr(node,"id");

  return true;
}
