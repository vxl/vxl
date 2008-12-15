//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/08
//
//

#include "brec_part_base.h"
#include "brec_hierarchy_edge.h"

#include <bxml/bxml_find.h>

//: we assume that the part that is added first as the outgoing part is the central part
brec_part_base_sptr 
brec_part_base::central_part() 
{ 
  if (out_edges().size() == 0) return 0;
  else return (*out_edges_begin())->target(); 
}

//: we assume that the part that is added first as the outgoing part is the central part
brec_hierarchy_edge_sptr 
brec_part_base::edge_to_central_part() 
{ 
  if (out_edges().size() == 0) return 0;
  else return (*out_edges_begin()); 
}

//: marking receptive field is only possible for brec_part_instance class instances
bool brec_part_base::mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane)
{
  return true;
}
bool brec_part_base::mark_center(vil_image_view<vxl_byte>& img, unsigned plane)
{
  return true;
}

bxml_data_sptr brec_part_base::xml_element()
{
  bxml_element* data = new bxml_element("base");
  data->set_attribute("layer",layer_);
  data->set_attribute("type",type_);
  data->set_attribute("det_thres",detection_threshold_);
  data->append_text("\n ");
  return data;
}

bool brec_part_base::xml_parse_element(bxml_data_sptr data)
{
  bxml_element query("base");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  
  if (!base_root)
    return false;

  if (base_root->type() == bxml_data::ELEMENT) {
    return (((bxml_element*)base_root.ptr())->get_attribute("layer", layer_) &&
            ((bxml_element*)base_root.ptr())->get_attribute("type", type_) &&
            ((bxml_element*)base_root.ptr())->get_attribute("det_thres", detection_threshold_));
  } else
    return false;
}

brec_part_base* brec_part_base::cast_to_base(void)
{
  return this;
}
brec_part_gaussian* brec_part_base::cast_to_gaussian(void)
{
  return 0;
}
brec_part_instance* brec_part_base::cast_to_instance(void)
{
  return 0;
}


brec_part_gaussian* brec_part_instance::cast_to_gaussian(void)
{
  return 0;
}
brec_part_instance* brec_part_instance::cast_to_instance(void)
{
  return this;
}


bool brec_part_instance::mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane)
{
  if (this->out_degree() == 0)
    return false;

  for (edge_iterator eit = this->out_edges_begin(); eit != this->out_edges_end(); eit++) {
    brec_part_instance_sptr pi = (*eit)->target()->cast_to_instance();
    pi->mark_receptive_field(img, plane);
  }
  return true;
}

bool brec_part_instance::mark_receptive_field(vil_image_view<float>& img, float val)
{
  if (this->out_degree() == 0)
    return false;

  for (edge_iterator eit = this->out_edges_begin(); eit != this->out_edges_end(); eit++) {
    brec_part_instance_sptr pi = (*eit)->target()->cast_to_instance();
    pi->mark_receptive_field(img, val);
  }
  return true;
}

bool brec_part_instance::mark_center(vil_image_view<vxl_byte>& img, unsigned plane)
{
  if (img.nplanes() <= plane)
    return false;

  int ni = (int)img.ni();
  int nj = (int)img.nj();

  int ic = (int)vcl_floor(x_ + 0.5f);
  int jc = (int)vcl_floor(y_ + 0.5f);
  if (ic >= 0 && jc >= 0 && ic < ni && jc < nj)
    img(ic, jc, plane) = (vxl_byte)(strength_*255);

  return true;
}

//: this method should be overwritten by inheriting classes so should never be called
vnl_vector_fixed<float,2> 
brec_part_instance::direction_vector(void)  // return a unit vector that gives direction of this instance in the image
{
  vnl_vector_fixed<float,2> v;
  v(1) = 1.0f;
  v(2) = 0.0f;
  return v;
}


bxml_data_sptr brec_part_instance::xml_element()
{
  bxml_data_sptr data_super = brec_part_base::xml_element();
  
  bxml_element* data = new bxml_element("instance");
  data->set_attribute("kind",kind_);
  data->set_attribute("x",x_);
  data->set_attribute("y",y_);
  data->set_attribute("strength",strength_);
  data->append_text("\n ");
  data->append_data(data_super);
  data->append_text("\n ");

  return data;
}

bool brec_part_instance::xml_parse_element(bxml_data_sptr data)
{
  bxml_element query("instance");
  bxml_data_sptr ins_root = bxml_find_by_name(data, query);
  
  if (!ins_root)
    return false;

  if (ins_root->type() == bxml_data::ELEMENT) {
    bool found = (((bxml_element*)ins_root.ptr())->get_attribute("kind", kind_) &&
                  ((bxml_element*)ins_root.ptr())->get_attribute("x", x_) &&
                  ((bxml_element*)ins_root.ptr())->get_attribute("y", y_) &&
                  ((bxml_element*)ins_root.ptr())->get_attribute("strength", strength_));
    if (!found)
      return false;

    return brec_part_base::xml_parse_element(ins_root);
  } else
    return false;
}






