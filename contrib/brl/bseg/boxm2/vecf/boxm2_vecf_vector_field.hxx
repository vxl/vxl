#ifndef boxm2_vecf_vector_field_hxx_included_
#define boxm2_vecf_vector_field_hxx_included_

#include "boxm2_vecf_vector_field.h"
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_point_3d.h>


template <class T>
bool boxm2_vecf_vector_field<T>::
compute_forward_transform(boxm2_scene_sptr source,
                          boxm2_block_id const& blk_id,
                          const boxm2_data_traits<BOXM2_POINT>::datatype *source_pts,
                          boxm2_data_traits<BOXM2_POINT>::datatype *target_pts)
{
  T fwd_mapper = make_forward_mapper(source, blk_id);
  return compute_transformed_pts(source, blk_id, fwd_mapper, source_pts, target_pts);
}


template <class T>
bool boxm2_vecf_vector_field<T>::
compute_inverse_transform(boxm2_scene_sptr target,
                          boxm2_block_id const& blk_id,
                          const boxm2_data_traits<BOXM2_POINT>::datatype *target_pts,
                          boxm2_data_traits<BOXM2_POINT>::datatype *source_pts)
{
  T inv_mapper = make_inverse_mapper(target, blk_id);
  return compute_transformed_pts(target, blk_id, inv_mapper, target_pts, source_pts);
}

template <class T>
bool boxm2_vecf_vector_field<T>::
compute_transformed_pts(boxm2_scene_sptr scene, boxm2_block_id const& blk_id,
                        T &f,
                        const boxm2_data_traits<BOXM2_POINT>::datatype *input_pts,
                        boxm2_data_traits<BOXM2_POINT>::datatype *output_pts)
{
  // get block data
  boxm2_block *blk = boxm2_cache::instance()->get_block(scene, blk_id);
  unsigned ncells = blk->num_cells();
  typedef boxm2_data_traits<BOXM2_POINT>::datatype data_t;
  const data_t* in_p = input_pts;
  data_t* out_p = output_pts;
  for (unsigned n=0; n<ncells; ++n) {
    const vgl_point_3d<double> x((*in_p)[0],
                                 (*in_p)[1],
                                 (*in_p)[2]);
    ++in_p;

    vgl_point_3d<double> y = f(x);

    *out_p++ = vnl_vector_fixed<float,4>((float)y.x(), (float)y.y(), (float)y.z(), 1.0f);
  }
  return true;
}

#undef  BOXM2_VECF_VECTOR_FIELD_INSTANTIATE
#define BOXM2_VECF_VECTOR_FIELD_INSTANTIATE(T) \
template class boxm2_vecf_vector_field<T >;

#endif
