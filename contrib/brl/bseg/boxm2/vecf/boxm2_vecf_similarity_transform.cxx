#include <utility>
#include "boxm2_vecf_similarity_transform.h"
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_point_3d.h>


// instantiate the template code in the .txx file
#include "boxm2_vecf_vector_field.hxx"
BOXM2_VECF_VECTOR_FIELD_INSTANTIATE(boxm2_vecf_similarity_transform_mapper);

boxm2_vecf_similarity_transform_mapper::
boxm2_vecf_similarity_transform_mapper(vgl_rotation_3d<double>  rot,
                                       vgl_vector_3d<double> const& trans,
                                       vgl_vector_3d<double> const& scale)
  : rot_(std::move(rot)), trans_(trans), scale_(scale)
{}

vgl_point_3d<double> boxm2_vecf_similarity_transform_mapper::operator() (vgl_point_3d<double> const& x) const
{
  const vgl_point_3d<double> tmp = rot_ * x;
  // no vgl_vector_3d * vgl_point_3d operator, so we have to do this.
  return vgl_point_3d<double>(tmp.x()*scale_.x(), tmp.y()*scale_.y(), tmp.z()*scale_.z()) + trans_;
}


boxm2_vecf_similarity_transform::boxm2_vecf_similarity_transform(vgl_rotation_3d<double>  rot,
                                                                 vgl_vector_3d<double> const& trans,
                                                                 vgl_vector_3d<double> const& scale)
  : rot_(std::move(rot)), trans_(trans), scale_(scale)
{
}

//: Create a function object that maps source pts to target pts.
boxm2_vecf_similarity_transform_mapper
boxm2_vecf_similarity_transform::make_forward_mapper(boxm2_scene_sptr  /*source*/, boxm2_block_id const&  /*blk_id*/)
{
  return boxm2_vecf_similarity_transform_mapper(rot_, trans_, scale_);
}

//: Create a function object that maps target pts to source pts.
boxm2_vecf_similarity_transform_mapper
boxm2_vecf_similarity_transform::make_inverse_mapper(boxm2_scene_sptr  /*source*/, boxm2_block_id const&  /*blk_id*/)
{
  vgl_rotation_3d<double> inv_rot = rot_.inverse();
  vgl_vector_3d<double> inv_scale( 1.0/scale_.x(), 1.0/scale_.y(), 1.0/scale_.z() );
  vgl_vector_3d<double> tmp = inv_rot * -trans_;
  vgl_vector_3d<double> inv_trans(tmp.x() / scale_.x(), tmp.y()/scale_.y(), tmp.z()/scale_.z());
  return boxm2_vecf_similarity_transform_mapper(inv_rot, inv_trans, inv_scale);
}
