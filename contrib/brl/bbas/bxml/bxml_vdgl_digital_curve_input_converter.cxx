#include <vcl_iostream.h>
#include <vcl_string.h>
#include <dom/DOM_Element.hpp>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vsol/vsol_point_2d.h>
#include <bxml/bxml_vsol_point_2d_input_converter.h>
#include <bxml/bxml_vdgl_digital_curve_input_converter.h>

bxml_vdgl_digital_curve_input_converter::bxml_vdgl_digital_curve_input_converter() {
  _class_name = "vdgl_digital_curve";
  _tag_name = "discrete_curve_3d";
  _ref_tag_name = "discrete_curve_3d_ref";
}

bxml_vdgl_digital_curve_input_converter::~bxml_vdgl_digital_curve_input_converter() {
}

bool bxml_vdgl_digital_curve_input_converter::extract_from_dom(DOM_Node& node) {
  new_or_ref = check_tag(node);

  if (new_or_ref == 0) {
    vcl_cout << "bxml_vdgl_digital_curve_input_converter:: Error, bad tag" << vcl_endl;;
    return false;
  }
  else if (new_or_ref == 2) {
    // ref node
    return (extract_ref_object_atrs(node));
  }
  else {
    extract_object_atrs(node);
  }

  if(_debug)
    vcl_cout << "discrete_curve_3d: _id=" << _id << " _n_points=" << _n_points << vcl_endl;;

  _xs.clear();
  _ys.clear();

  int num_children=0;
  DOM_Node child = node.getFirstChild();
  while (child != 0) {
    int cnode_type = child.getNodeType();
    if (cnode_type == DOM_Node::ELEMENT_NODE) {   
      bxml_vsol_point_2d_input_converter conv;
      if (conv.extract_from_dom(child)) {
	bxml_generic_ptr gp_pt = conv.construct_object();
	vsol_point_2d* pt = (vsol_point_2d*) gp_pt.get_vsol_spatial_object();
	if (!pt) {
	  vcl_cout << "vdgl_digital_curve:Error,  unable to read point_2d" << vcl_endl;;
	  return false;
	}
	_xs.push_back(pt->x());
	_ys.push_back(pt->y());
  vcl_string& s = conv.id();
	num_children++;
      }
      else {
	vcl_cout << "something wrong, no point_2d" << vcl_endl;;
      }
    }
    child = child.getNextSibling();
  }
  if (num_children != _n_points ) {
    vcl_cout << "vdgl_digital_curve:Error num_children=" << num_children << " _n_points=" << _n_points << vcl_endl;;
    return false;
  }

  return true;
}

bxml_generic_ptr bxml_vdgl_digital_curve_input_converter::construct_object()
{
  if (new_or_ref == 1) {
    vdgl_edgel_chain_sptr ec = new vdgl_edgel_chain();
    for (int i=0; i<_n_points; i++)
      ec->add_edgel(vdgl_edgel(_xs[i], _ys[i]));

    vdgl_interpolator_sptr itp = new vdgl_interpolator_linear(ec);
    vdgl_digital_curve* dc = new vdgl_digital_curve(itp);
    bxml_generic_ptr gp(dc);
    if ( !(_id == _null_id) ) {
     _obj_table[_id] = gp;
     dc->ref();
    }
    return gp;
  }
  else {
    bxml_generic_ptr gp = _obj_table[_id];
    return gp;
  }
}

bool bxml_vdgl_digital_curve_input_converter::extract_ref_object_atrs(DOM_Node& node) {
  _id = get_string_attr(node,"id");
  
  return true;
}

bool bxml_vdgl_digital_curve_input_converter::extract_object_atrs(DOM_Node& node) {
  _id = get_string_attr(node,"id");
  _n_points = get_int_attr(node,"n_points");
  
  return true;
}
