//:
// \file
#include <bxml/bxml_input_converter.h>
#include <vcl_iostream.h>
#include <vsol/vsol_spatial_object_2d.h>

typedef vcl_map<vcl_string,bxml_generic_ptr,vcl_less<vcl_string> > OTAB;
OTAB bxml_input_converter::obj_table_;

//: Constructor
bxml_input_converter::bxml_input_converter()
{
  debug_=false;
  null_id_ = "p00";
}

//: Destructor
bxml_input_converter::~bxml_input_converter()
{
}

vcl_string bxml_input_converter::get_DOM_tag(DOM_Node& node)
{
  int node_type = node.getNodeType();
  if (node_type != DOM_Node::ELEMENT_NODE) {
    vcl_cout << "In bxml_input_converter::get_DOM_tag: node_type="
             << node_type << vcl_endl;
    return "";
  }
  char* tag_name = ((DOM_Element*)&node)->getTagName().transcode();
  vcl_string tname(tag_name);
  return tname;
}

int bxml_input_converter::check_tag(DOM_Node& node)
{
  int node_type = node.getNodeType();
  if (node_type != DOM_Node::ELEMENT_NODE) {
    vcl_cout << "bxml_input_converter::check_tag() Error: node_type="
             << node_type << vcl_endl;
    return 0;
  }

  char* tag_name = ((DOM_Element*)&node)->getTagName().transcode();
  vcl_string tname(tag_name);
  if (debug_)
    vcl_cout<< "bxml_input_converter::check_tag() tag_name from DOM: "
            << tname << ", tag_name expected: " << get_tag_name()
            << vcl_endl << vcl_flush;
  // virtual method get_tag_name()
  if ( tname == get_tag_name() ) {
    return 1;
  }
  else if ( tname == get_ref_tag_name() ) {
    return 2;
  }
  else {
    // not found
    if (debug_)
      vcl_cout<< "bxml_input_converter::check_tag() tag not found\n";
    return 0;
  }
}

bool bxml_input_converter::getNextElementSibling(DOM_Node& node)
{
  node = node.getNextSibling();

  bool found = false;
  while (!found && node != 0) {
    int node_type = node.getNodeType();
    if (node_type == DOM_Node::ELEMENT_NODE) {
      found = true;
    }
    else {
      node = node.getNextSibling();
    }
  }
  return found;
}

DOM_Node bxml_input_converter::getChild(DOM_Node& node,vcl_string tname)
{
  DOM_NodeList param_list =
    ((DOM_Element*)&node)->getElementsByTagName(tname.c_str());
  if (param_list == 0) {
    return DOM_Node();
  }
  else if (param_list.getLength() == 0) {
    return DOM_Node();
  }
  else if (param_list.getLength() != 1) {
    return DOM_Node();
  }
  return param_list.item(0);
}

bool bxml_input_converter::has_attr(DOM_Node& node, vcl_string attr_name)
{
  DOM_Attr attr = ((DOM_Element*)&node)->getAttributeNode(attr_name.c_str());
  return attr != NULL;
}

vcl_string bxml_input_converter::get_string_attr(DOM_Node& node,
                                                 vcl_string attr_name)
{
  DOMString sx = ((DOM_Element*)&node)->getAttribute(attr_name.c_str());
  char* cx = sx.transcode(); //transcode does new
  vcl_string str_x(cx);
  return str_x;
}

int bxml_input_converter::get_int_attr(DOM_Node& node,vcl_string attr_name)
{
  vcl_string cx = get_string_attr(node,attr_name) ;
  int x = atoi(cx.c_str());
  return x;
}

double bxml_input_converter::get_double_attr(DOM_Node& node,
                                             vcl_string attr_name)
{
  vcl_string cx = get_string_attr(node,attr_name) ;
  double x = atof(cx.c_str());
  return x;
}

bool bxml_input_converter::get_bool_attr(DOM_Node& node,vcl_string attr_name)
{
  vcl_string cx = get_string_attr(node,attr_name) ;
  bool x = ( (cx == "true") || (cx == "1") );
  return x;
}

//:unref spatial object instances in the object table
void bxml_input_converter::clear()
{
  for (vcl_map<vcl_string,bxml_generic_ptr,vcl_less<vcl_string> >::iterator
       pit =obj_table_.begin(); pit != obj_table_.end(); pit++)
  {
    bxml_generic_ptr gp = (*pit).second;
    vsol_spatial_object_2d* so = gp.get_vsol_spatial_object();
    if (so)
    {
      vcl_cout << "so(" << so->get_name()<<") refcnt "<< so->get_references();
#if 0
      if (so->cast_to_topology_object())
      {
        if (so->cast_to_topology_object()->cast_to_vertex())
          vcl_cout << " vertex\n";
        if (so->cast_to_topology_object()->cast_to_zero_chain())
          vcl_cout << " zero chain\n";
        if (so->cast_to_topology_object()->cast_to_edge())
          vcl_cout << " edge\n";
      }
      if (so->cast_to_curve())
        vcl_cout << " curve\n";
#endif
      if (so->cast_to_point())
      {
        vcl_cout << " point\n";
        so->unref();
      }
    }
  }
  obj_table_.clear();
}
