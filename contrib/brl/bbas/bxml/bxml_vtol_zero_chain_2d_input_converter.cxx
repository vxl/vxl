
#include <vcl_iostream.h> //vcl_cout
#include <vcl_string.h>
#include <dom/DOM_Element.hpp>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_zero_chain.h>
#include <bxml/bxml_vtol_vertex_2d_input_converter.h>
#include <bxml/bxml_vtol_zero_chain_2d_input_converter.h>



bxml_vtol_zero_chain_2d_input_converter::bxml_vtol_zero_chain_2d_input_converter() {
  _class_name = "vtol_zero_chain_2d";
  _tag_name = "zero_chain_3d";
  _ref_tag_name = "zero_chain_3d_ref";
}

bxml_vtol_zero_chain_2d_input_converter::~bxml_vtol_zero_chain_2d_input_converter() {
}

vcl_string bxml_vtol_zero_chain_2d_input_converter::get_id(DOM_Node& node) {
  return (get_string_attr(node,"id"));
}

bool bxml_vtol_zero_chain_2d_input_converter::extract_from_dom(DOM_Node& node) {
  new_or_ref = check_tag(node);

  if (new_or_ref == 0) {
    return false;
  }
  else if (new_or_ref == 2) {
    // ref node
    return (extract_object_atrs(node));
  }
  else {
    extract_object_atrs(node);
  }

  if(_debug)
    vcl_cout << "vtol_zero_chain_2d: _id=" << _id << vcl_endl;

  // clear out list in case converter is called multiple times
  _verts.clear();

  int num_children=0;
  DOM_Node child = node.getFirstChild();
  while (child != 0) {
    int cnode_type = child.getNodeType();
    if (cnode_type == DOM_Node::ELEMENT_NODE) {   
      bxml_vtol_vertex_2d_input_converter conv;
      if (conv.extract_from_dom(child)) {
	bxml_generic_ptr gp_pt = conv.construct_object();
	vtol_vertex_2d* vert = (vtol_vertex_2d*) gp_pt.get_vsol_spatial_object();
	if (!vert) {
	  vcl_cout << "Error vtol_zero_chain_2d unable to read vertex_3d" << vcl_endl;
	  break;
	}
	if(_debug)
	  vcl_cout << "vtol_zero_chain_2d: vert=" << (*vert) <<
	    " num_children= " << num_children << vcl_endl;
	_verts.push_back(vert);
	num_children++;
      }
      else {
	vcl_cout << "something wrong, no vertex" << vcl_endl;
      }
    }
    child = child.getNextSibling();
  }
  return true;
}

bxml_generic_ptr bxml_vtol_zero_chain_2d_input_converter::construct_object()
{
  if (new_or_ref == 1) {
    vtol_zero_chain* zc = new vtol_zero_chain(_verts);
    bxml_generic_ptr gp(zc);
    if ( !(_id == _null_id) ) {
      _obj_table[_id] = gp;
      zc->ref();
     }
    return gp;
  }
  else {
    bxml_generic_ptr gp = _obj_table[_id];
    return gp;
  }
}

bool bxml_vtol_zero_chain_2d_input_converter::extract_object_atrs(DOM_Node& node) {
  _id = get_string_attr(node,"id");
  
  return true;
}
