#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cmath.h>
#include <dom/DOM_Element.hpp>
#include <vdgl/vdgl_digital_curve.h>
#include <vtol/vtol_edge_2d.h>
#include <bdgl/bdgl_curve_algs.h>
#include <bxml/bxml_vtol_zero_chain_2d_input_converter.h>
#include <bxml/bxml_vdgl_digital_curve_input_converter.h>
#include <bxml/bxml_vtol_edge_2d_input_converter.h>

bxml_vtol_edge_2d_input_converter::bxml_vtol_edge_2d_input_converter()
{
  class_name_ = "vtol_edge_2d";
  tag_name_ = "edge_3d";
  ref_tag_name_ = "edge_3d_ref";
  zc_ = 0;
  cur_ = 0;
}

bxml_vtol_edge_2d_input_converter::~bxml_vtol_edge_2d_input_converter()
{
}

vcl_string bxml_vtol_edge_2d_input_converter::get_id(DOM_Node& node)
{
  return get_string_attr(node,"id");
}

bool bxml_vtol_edge_2d_input_converter::extract_from_dom(DOM_Node& node)
{
  new_or_ref = check_tag(node);

  if (new_or_ref == 0) {
    vcl_cout << "bxml_vtol_edge_2d_input_converter:: Error, bad tag\n";
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
    vcl_cout << "vtol_edge_2d: id_=" << id_ << vcl_endl;

  int num_children=0;
  DOM_Node child = node.getFirstChild();
  while (child != 0)
  {
    int cnode_type = child.getNodeType();
    if (cnode_type == DOM_Node::ELEMENT_NODE)
    {
      bxml_vtol_zero_chain_2d_input_converter conv;
      bxml_vdgl_digital_curve_input_converter conv2;
      if (conv.extract_from_dom(child)) {
        bxml_generic_ptr gp_pt = conv.construct_object();
        vsol_spatial_object_2d* so = gp_pt.get_vsol_spatial_object();
        if (!so)
        {
          vcl_cout << "Error vtol_edge_2d unable to read zero_chain_3d\n";
          return false;
        }
        zc_ = so->cast_to_topology_object()->cast_to_zero_chain();
        num_children++;
      }
      else if (conv2.extract_from_dom(child)) {
        bxml_generic_ptr gp_pt = conv2.construct_object();
        vsol_spatial_object_2d* so = gp_pt.get_vsol_spatial_object();
        if (!so)
        {
          vcl_cout << "Error vtol_edge_2d unable to digital_curve\n";
          return false;
        }
        cur_ = so->cast_to_curve()->cast_to_vdgl_digital_curve();
        num_children++;
      }
      else
        vcl_cout << "something wrong, no zero_chain or discrete curve\n";
    }

    child = child.getNextSibling();
  }

  if (num_children != 2) {
    vcl_cout << "vtol_edge_2d:Warning num_children=" << num_children << vcl_endl;
  }

  return true;
}

bxml_generic_ptr bxml_vtol_edge_2d_input_converter::construct_object()
{
  if (new_or_ref == 1)
  {
    if (!zc_) {
      vcl_cout << "bxml_vtol_edge_2d: Note, no zero_chain, constructing edge with one vertex, id=" << id_ << '\n';

      float x = cur_->get_x(0);
      float y = cur_->get_y(0);

      vtol_vertex_2d_sptr  v1 = new vtol_vertex_2d(x,y);
      //why doesn't the vertex get inserted in the object table?
      // for some reason this constructor copies the entire curve??
      vtol_edge_2d* edg = new vtol_edge_2d(v1, v1, cur_->cast_to_curve());

      bxml_generic_ptr gp(edg);
      if ( !(id_ == null_id_) ) {
        obj_table_[id_] = gp;
      }
      return gp;
    }
    else {
      vtol_edge_2d* edg = new vtol_edge_2d(zc_);
      if (cur_)
      {
        double xc0 = cur_->get_x(0), yc0 = cur_->get_y(0);
        double xv1 = zc_->v0()->cast_to_vertex_2d()->x();
        double yv1 = zc_->v0()->cast_to_vertex_2d()->y();
        if (vcl_sqrt((xc0-xv1)*(xc0-xv1) + (yc0-yv1)*(yc0-yv1))>1.0)
          cur_ = bdgl_curve_algs::reverse(cur_);
        vsol_curve_2d_sptr c = cur_->cast_to_curve();
        if (c)
          edg->set_curve(*c);
      }
      bxml_generic_ptr gp(edg);
      if ( !(id_ == null_id_) ) {
        obj_table_[id_] = gp;
      }
      return gp;
    }
  }
  else {
    bxml_generic_ptr gp = obj_table_[id_];
    return gp;
  }
}

bool bxml_vtol_edge_2d_input_converter::extract_object_atrs(DOM_Node& node)
{
  id_ = get_string_attr(node,"id");
  return true;
}
