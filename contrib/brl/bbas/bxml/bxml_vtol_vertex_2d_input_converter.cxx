#include <vcl_iostream.h> //vcl_cout
#include <vcl_string.h>
#include <dom/DOM_Element.hpp>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <bxml/bxml_vsol_point_2d_input_converter.h>
#include <bxml/bxml_vtol_vertex_2d_input_converter.h>

#include <bxml/bxml_io.h>



bxml_vtol_vertex_2d_input_converter::bxml_vtol_vertex_2d_input_converter() {
  _class_name = "vtol_vertex_2d";
  _tag_name = "vertex_3d";
  _ref_tag_name = "vertex_3d_ref";

  _pt = 0;
}

bxml_vtol_vertex_2d_input_converter::~bxml_vtol_vertex_2d_input_converter() {
}

vcl_string bxml_vtol_vertex_2d_input_converter::get_id(DOM_Node& node) {
  char* tag_name = ((DOM_Element*)&node)->getTagName().transcode();
  vcl_string tname(tag_name);
  if(_debug)
    vcl_cout << "tname=" << tname << vcl_endl;

  return (get_string_attr(node,"id"));
}

bool bxml_vtol_vertex_2d_input_converter::extract_from_dom(DOM_Node& node) {
  new_or_ref = check_tag(node);

  if (new_or_ref == 0) {
    vcl_cout << "bxml_vtol_vertex_2d_input_converter:: Error, bad tag" << vcl_endl;
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
    vcl_cout << "vertex_3d: _id=" << _id << vcl_endl;

  int num_children=0;
  DOM_Node child = node.getFirstChild();
  while (child != 0) {
    int cnode_type = child.getNodeType();
    if (cnode_type == DOM_Node::ELEMENT_NODE) {   
      bxml_vsol_point_2d_input_converter conv;
      if (conv.extract_from_dom(child)) {
	bxml_generic_ptr gp_pt = conv.construct_object();
	_pt = (vsol_point_2d*) gp_pt.get_vsol_spatial_object();
	if (!_pt) {
	  vcl_cout << "Error vertex_3d unable to read point_3d" << vcl_endl;
	  break;
	}
	if(_debug)
	  vcl_cout << "vertex_3d: x=" << _pt->x() << " y=" << _pt->y()
	       << " num_children= " << num_children << vcl_endl; 
	num_children++;
      }
      else {
	vcl_cout << "something wrong, no point_3d" << vcl_endl;
      }
    }
    child = child.getNextSibling();
  }
  if (num_children != 1 ) {
    vcl_cout << "vertex_3d:Error num_children=" << num_children << vcl_endl;
    return false;
  }

  return true;
}

bxml_generic_ptr bxml_vtol_vertex_2d_input_converter::construct_object()
{
  if (new_or_ref == 1) {
    // new
    vtol_vertex_2d* vert = new vtol_vertex_2d(*_pt);
    bxml_generic_ptr gp(vert);
    if ( !(_id == _null_id) ) {
     _obj_table[_id] = gp;
     vert->ref();
    }
    return gp;
  }
  else {
    bxml_generic_ptr gp = _obj_table[_id];
    return gp;
  }
}

bool bxml_vtol_vertex_2d_input_converter::extract_object_atrs(DOM_Node& node) {
  _id = get_string_attr(node,"id");
  
  return true;
}

#if 0
void bxml_vtol_vertex_2d_input_converter::WriteDEXAttributes(int index,DEX_object& obj,DOM_Node& node) {
  if ( !(_id == _null_id) ) {
    _id_map[_id] = index;
  }
  obj.fixed_values.push_back("0");

  int num_children=0;
  DOM_Node child = node.getFirstChild();
  while (child != 0) {
    int cnode_type = child.getNodeType();
    if (cnode_type == DOM_Node::ELEMENT_NODE) {   
      bxml_vsol_point_2d_input_converter conv;
      int pt_index = xml_io::write_object(conv.get_class_name(),child);
      obj.fixed_values.push_back(xml_helper::int_to_string(pt_index));
    }
    child = child.getNextSibling();
  }

  if (num_children != 1 ) {
    vcl_cout << "Error num_children = " << num_children << vcl_endl;
  }
}
#endif
