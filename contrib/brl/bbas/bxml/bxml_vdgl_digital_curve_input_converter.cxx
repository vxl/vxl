#include <vcl_iostream.h>
#include <dom/DOM_Element.hpp>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vsol/vsol_point_2d.h>
#include <bxml/bxml_vsol_point_2d_input_converter.h>
#include <bxml/bxml_vdgl_digital_curve_input_converter.h>

bxml_vdgl_digital_curve_input_converter::bxml_vdgl_digital_curve_input_converter()
{
  class_name_ = "vdgl_digital_curve";
  tag_name_ = "discrete_curve_3d";
  ref_tag_name_ = "discrete_curve_3d_ref";
}

bxml_vdgl_digital_curve_input_converter::~bxml_vdgl_digital_curve_input_converter()
{
}

bool bxml_vdgl_digital_curve_input_converter::extract_from_dom(DOM_Node& node)
{
  new_or_ref = check_tag(node);

  if (new_or_ref == 0) {
    vcl_cout << "bxml_vdgl_digital_curve_input_converter: Error, bad tag\n";
    return false;
  }
  else if (new_or_ref == 2) {
    // ref node
    return extract_ref_object_atrs(node);
  }
  else {
    extract_object_atrs(node);
  }

  if (debug_)
    vcl_cout << "discrete_curve_3d: id_=" << id_ << " n_points_=" << n_points_
             << '\n';

  xs_.clear();
  ys_.clear();

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
        vsol_point_2d_sptr pt= gp_pt.get_vsol_spatial_object()->cast_to_point();
        if (!pt) {
          vcl_cout << "vdgl_digital_curve: Error, unable to read point_2d\n";
          return false;
        }
        xs_.push_back(pt->x());
        ys_.push_back(pt->y());
        //vcl_string& s = conv.id();
        num_children++;
      }
      else {
        vcl_cout << "something wrong, no point_2d\n";
      }
    }
    child = child.getNextSibling();
  }
  if (num_children != n_points_ ) {
    vcl_cout << "vdgl_digital_curve:Error num_children=" << num_children << " n_points_=" << n_points_ << vcl_endl;
    return false;
  }

  return true;
}

bxml_generic_ptr bxml_vdgl_digital_curve_input_converter::construct_object()
{
  if (new_or_ref == 1) {
    vdgl_edgel_chain_sptr ec = new vdgl_edgel_chain();
    for (int i=0; i<n_points_; i++)
      ec->add_edgel(vdgl_edgel(xs_[i], ys_[i]));

    vdgl_interpolator_sptr itp = new vdgl_interpolator_linear(ec);
    vdgl_digital_curve* dc = new vdgl_digital_curve(itp);
    bxml_generic_ptr gp(dc);
    if ( !(id_ == null_id_) ) {
     obj_table_[id_] = gp;
     //     vsol_curve_2d* c = dc->cast_to_curve();
     dc->ref();
    }
    return gp;
  }
  else {
    bxml_generic_ptr gp = obj_table_[id_];
    return gp;
  }
}

bool bxml_vdgl_digital_curve_input_converter::extract_ref_object_atrs(DOM_Node& node)
{
  id_ = get_string_attr(node,"id");

  return true;
}

bool bxml_vdgl_digital_curve_input_converter::extract_object_atrs(DOM_Node& node)
{
  id_ = get_string_attr(node,"id");
  n_points_ = get_int_attr(node,"n_points");

  return true;
}
