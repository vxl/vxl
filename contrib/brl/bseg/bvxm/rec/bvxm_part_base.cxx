//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/08
//
//

#include <rec/bvxm_part_base.h>
#include <rec/bvxm_hierarchy_edge.h>

//: we assume that the part that is added first as the outgoing part is the central part
bvxm_part_base_sptr 
bvxm_part_base::central_part() 
{ 
  if (out_edges().size() == 0) return 0;
  else return (*out_edges_begin())->target(); 
}

//: we assume that the part that is added first as the outgoing part is the central part
bvxm_hierarchy_edge_sptr 
bvxm_part_base::edge_to_central_part() 
{ 
  if (out_edges().size() == 0) return 0;
  else return (*out_edges_begin()); 
}

//: marking receptive field is only possible for bvxm_part_instance class instances
bool bvxm_part_base::mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane)
{
  return true;
}
bool bvxm_part_base::mark_center(vil_image_view<vxl_byte>& img, unsigned plane)
{
  return true;
}

bvxm_part_base* bvxm_part_base::cast_to_base(void)
{
  return this;
}
bvxm_part_gaussian* bvxm_part_base::cast_to_gaussian(void)
{
  return 0;
}
bvxm_part_instance* bvxm_part_base::cast_to_instance(void)
{
  return 0;
}


bvxm_part_gaussian* bvxm_part_instance::cast_to_gaussian(void)
{
  return 0;
}
bvxm_part_instance* bvxm_part_instance::cast_to_instance(void)
{
  return this;
}


bool bvxm_part_instance::mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane)
{
  if (this->out_degree() == 0)
    return false;

  for (edge_iterator eit = this->out_edges_begin(); eit != this->out_edges_end(); eit++) {
    bvxm_part_instance_sptr pi = (*eit)->target()->cast_to_instance();
    pi->mark_receptive_field(img, plane);
  }
  return true;
}

bool bvxm_part_instance::mark_receptive_field(vil_image_view<float>& img, float val)
{
  if (this->out_degree() == 0)
    return false;

  for (edge_iterator eit = this->out_edges_begin(); eit != this->out_edges_end(); eit++) {
    bvxm_part_instance_sptr pi = (*eit)->target()->cast_to_instance();
    pi->mark_receptive_field(img, val);
  }
  return true;
}

bool bvxm_part_instance::mark_center(vil_image_view<vxl_byte>& img, unsigned plane)
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
bvxm_part_instance::direction_vector(void)  // return a unit vector that gives direction of this instance in the image
{
  vnl_vector_fixed<float,2> v;
  v(1) = 1.0f;
  v(2) = 0.0f;
  return v;
}



