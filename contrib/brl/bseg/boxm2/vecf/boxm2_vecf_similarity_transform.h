#ifndef boxm2_vecf_similarity_transform_h_included_
#define boxm2_vecf_similarity_transform_h_included_

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>

#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include "boxm2_vecf_vector_field.h"

class boxm2_vecf_similarity_transform_mapper
{
  public:
  boxm2_vecf_similarity_transform_mapper(vgl_rotation_3d<double>  rot,
                                         vgl_vector_3d<double> const& trans,
                                         vgl_vector_3d<double> const& scale);

  vgl_point_3d<double> operator() (vgl_point_3d<double> const& x) const;

  private:
    const vgl_rotation_3d<double> rot_;
    const vgl_vector_3d<double> trans_;
    const vgl_vector_3d<double> scale_;
};

class boxm2_vecf_similarity_transform : public boxm2_vecf_vector_field<boxm2_vecf_similarity_transform_mapper>
{
  public:
    boxm2_vecf_similarity_transform(vgl_rotation_3d<double>  rot,
                                    vgl_vector_3d<double> const& trans,
                                    vgl_vector_3d<double> const& scale);

  private:
    typedef boxm2_vecf_similarity_transform_mapper MAPPER_T;

    //: Create a function object that maps source pts to target pts.
    MAPPER_T make_forward_mapper(boxm2_scene_sptr source, boxm2_block_id const& blk_id) override;
    //: Create a function object that maps target pts to source pts.
    MAPPER_T make_inverse_mapper(boxm2_scene_sptr target, boxm2_block_id const& blk_id) override;

    //: data
    const vgl_rotation_3d<double> rot_;
    const vgl_vector_3d<double> trans_;
    const vgl_vector_3d<double> scale_;
};

#endif
