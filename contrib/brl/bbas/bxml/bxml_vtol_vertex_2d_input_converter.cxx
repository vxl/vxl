#include <vcl_iostream.h> //vcl_cout
#include <vcl_string.h>
#include <dom/DOM_Element.hpp>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <bxml/bxml_vsol_point_2d_input_converter.h>
#include <bxml/bxml_vtol_vertex_2d_input_converter.h>
#include <bxml/bxml_io.h>

bxml_vtol_vertex_2d_input_converter::bxml_vtol_vertex_2d_input_converter()
{
  class_name_ = "vtol_vertex_2d";
  tag_name_ = "vertex_3d";
  ref_tag_name_ = "vertex_3d_ref";

  pt_ = 0;
}

bxml_vtol_vertex_2d_input_converter::~bxml_vtol_vertex_2d_input_converter()
{
}

vcl_string bxml_vtol_vertex_2d_input_converter::get_id(DOM_Node& node)
{
  char* tag_name = ((DOM_Element*)&node)->getTagName().transcode();
  vcl_string tname(tag_name);
  if (debug_)
    vcl_cout << "tname=" << tname << vcl_endl;

  return get_string_attr(node,"id");
}

bool bxml_vtol_vertex_2d_input_converter::extract_from_dom(DOM_Node& node)
{
  new_or_ref = check_tag(node);

  if (new_or_ref == 0) {
    vcl_cout << "bxml_vtol_vertex_2d_input_converter:: Error, bad tag\n";
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
    vcl_cout << "vertex_3d: id_=" << id_ << vcl_endl;

  int num_children=0;
  DOM_Node child = node.getFirstChild();
  while (child != 0)
  {
    int cnode_type = child.getNodeType();
    if (cnode_type == DOM_Node::ELEMENT_NODE)
    {
      bxml_vsol_point_2d_input_converter conv;
      if (conv.extract_from_dom(child)) {
        bxml_generic_ptr gp_pt = conv.construct_object();
        pt_ = gp_pt.get_vsol_spatial_object()->cast_to_point();
        if (!pt_) {
          vcl_cout << "Error vertex_3d unable to read point_3d\n";
          break;
        }
        if (debug_)
          vcl_cout << "vertex_3d: x=" << pt_->x() << " y=" << pt_->y()
                   << " num_children= " << num_children << vcl_endl;
        num_children++;
      }
      else {
        vcl_cout << "something wrong, no point_3d\n";
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
    vtol_vertex_2d* vert = new vtol_vertex_2d(*pt_);
    bxml_generic_ptr gp(vert);
    if ( !(id_ == null_id_) ) {
     obj_table_[id_] = gp;
     vert->ref();
    }
    return gp;
  }
  else {
    bxml_generic_ptr gp = obj_table_[id_];
    return gp;
  }
}

bool bxml_vtol_vertex_2d_input_converter::extract_object_atrs(DOM_Node& node)
{
  id_ = get_string_attr(node,"id");

  return true;
}

#if 0
void bxml_vtol_vertex_2d_input_converter::WriteDEXAttributes(int index,DEX_object& obj,DOM_Node& node)
{
  if ( !(id_ == null_id_) ) {
    id_map_[id_] = index;
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
#endif // 0
