#include <vcl_iostream.h> //vcl_cout
#include <vcl_string.h>
#include <vcl_cmath.h>
#include <dom/DOM_Element.hpp>
#include <vdgl/vdgl_digital_curve.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_edge_2d.h>
#include <bdgl/bdgl_curve_algs.h>
#include <bxml/bxml_vtol_zero_chain_2d_input_converter.h>
#include <bxml/bxml_vdgl_digital_curve_input_converter.h>
#include <bxml/bxml_vtol_edge_2d_input_converter.h>


bxml_vtol_edge_2d_input_converter::bxml_vtol_edge_2d_input_converter() {
  _class_name = "vtol_edge_2d";
  _tag_name = "edge_3d";
  _ref_tag_name = "edge_3d_ref";
  _zc = 0;
  _cur = 0;
}

bxml_vtol_edge_2d_input_converter::~bxml_vtol_edge_2d_input_converter() {
}

vcl_string bxml_vtol_edge_2d_input_converter::get_id(DOM_Node& node) {
  return (get_string_attr(node,"id"));
}

bool bxml_vtol_edge_2d_input_converter::extract_from_dom(DOM_Node& node) {
  new_or_ref = check_tag(node);

  if (new_or_ref == 0) {
    vcl_cout << "bxml_vtol_edge_2d_input_converter:: Error, bad tag" << vcl_endl;
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
    vcl_cout << "vtol_edge_2d: _id=" << _id << vcl_endl;

  int num_children=0;
  DOM_Node child = node.getFirstChild();
  while (child != 0) {
    int cnode_type = child.getNodeType();
    if (cnode_type == DOM_Node::ELEMENT_NODE) {   
      bxml_vtol_zero_chain_2d_input_converter conv;
      bxml_vdgl_digital_curve_input_converter conv2;
      if (conv.extract_from_dom(child)) {
        bxml_generic_ptr gp_pt = conv.construct_object();
        vsol_spatial_object_2d* so = gp_pt.get_vsol_spatial_object();
        if(!so)
          {
            vcl_cout << "Error vtol_edge_2d unable to read zero_chain_3d" 
                     << vcl_endl;
            return false;
          }
        _zc = so->cast_to_topology_object()->cast_to_zero_chain();
        num_children++;
      }
      else if (conv2.extract_from_dom(child)) {
        bxml_generic_ptr gp_pt = conv2.construct_object();
        vsol_spatial_object_2d* so = gp_pt.get_vsol_spatial_object();
        if(!so)
          {
            vcl_cout << "Error vtol_edge_2d unable to digital_curve" 
                     << vcl_endl;
            return false;
          }
        _cur = so->cast_to_curve()->cast_to_digital_curve();
        num_children++;
      }
      else 
        vcl_cout << "something wrong, no zero_chain or discrete curve" 
                 << vcl_endl;
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
  if (new_or_ref == 1) {
    if (!_zc) {
      vcl_cout << "bxml_vtol_edge_2d: Note, no zero_chain, constructing edge with one vertex, id=" << _id << vcl_endl;

      float x = _cur->get_x(0);
      float y = _cur->get_y(0);

      vtol_vertex_2d*  v1 = new vtol_vertex_2d(x,y);
      // for some reason this constructor copies the entire curve??
      vtol_edge_2d* edg = new vtol_edge_2d(*v1, *v1, _cur->cast_to_curve());

      bxml_generic_ptr gp(edg);
      if ( !(_id == _null_id) ) {
        _obj_table[_id] = gp;
        //FIXME
       //        edg->ref(); // we should add this when we do adjacent faces
      }
      return gp;
    }
    else {
      vtol_edge_2d* edg = new vtol_edge_2d(*_zc);
      if (_cur)
        {
          double xc0 = _cur->get_x(0), yc0 = _cur->get_y(0);
          double xv1 = _zc->v0()->cast_to_vertex_2d()->x();
          double yv1 = _zc->v0()->cast_to_vertex_2d()->y();
          if(vcl_sqrt((xc0-xv1)*(xc0-xv1) + (yc0-yv1)*(yc0-yv1))>1.0)
            _cur = bdgl_curve_algs::reverse(_cur);
          vsol_curve_2d* c = _cur->cast_to_curve();
          if(c)
            edg->set_curve(*c);
        //FIXME
       //        edg->ref(); // we should add this when we do adjacent faces
        }
      bxml_generic_ptr gp(edg);
      if ( !(_id == _null_id) ) {
        _obj_table[_id] = gp;
      }
      return gp;
    }
  }
  else {
    bxml_generic_ptr gp = _obj_table[_id];
    return gp;
  }
}

bool bxml_vtol_edge_2d_input_converter::extract_object_atrs(DOM_Node& node) {
  _id = get_string_attr(node,"id");
  
  return true;
}
