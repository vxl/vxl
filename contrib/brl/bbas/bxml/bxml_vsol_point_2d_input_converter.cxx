#include <vcl_iostream.h> //cout
#include <vcl_string.h>
#include <dom/DOM_Element.hpp>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <bxml/bxml_vsol_point_2d_input_converter.h>


bxml_vsol_point_2d_input_converter::bxml_vsol_point_2d_input_converter() {
  _class_name = "vsol_point_2d";
  _tag_name = "point_3d";
  _ref_tag_name = "point_3d_ref";
}

bxml_vsol_point_2d_input_converter::~bxml_vsol_point_2d_input_converter() {
}

bool bxml_vsol_point_2d_input_converter::extract_ref_object_atrs(DOM_Node& node) {
  _id = get_string_attr(node,"id");

  return true;
}

bool bxml_vsol_point_2d_input_converter::extract_object_atrs(DOM_Node& node) {
  _x  = get_double_attr(node,"x");
  _y  = get_double_attr(node,"y");
  _id = get_string_attr(node,"id");

  return true;
}

bool bxml_vsol_point_2d_input_converter::extract_from_dom(DOM_Node& node) {
  new_or_ref = check_tag(node);

  if (new_or_ref == 0) {
    vcl_cout << "bxml_vsol_point_2d_input_converter:: Error, bad tag" 
             << vcl_endl;
    return false;
  }
  else if (new_or_ref == 2) {
    return (extract_ref_object_atrs(node));
  }
  else {
    extract_object_atrs(node);
  }

  return true;
}
//:
// Construct the object if it is not already in the object table
// It will be in the object table if it has already been constructed 
// and the current occurence is just a ref in the xml file.
// WARNING!!
// There is an issue with the use of the generic pointer, since we 
// have no way to reference count the pointer. We might be able to 
// deal with the problem in the destructor of the generic pointer
bxml_generic_ptr bxml_vsol_point_2d_input_converter::construct_object()
{
  if (new_or_ref == 1) {
    vsol_point_2d* p = new vsol_point_2d(_x, _y);
    bxml_generic_ptr gp(p);
    if ( !(_id == _null_id) ) {
     _obj_table[_id] = gp;
     p->ref();//Keep the point alive until it is used
    }
    return gp;
  }
  else {
    bxml_generic_ptr gp = _obj_table[_id];
    return gp;
  }
}
