#ifndef boxm2_vecf_similarity_transform_h_included_
#define boxm2_vecf_similarity_transform_h_included_

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>

#include "boxm2_vecf_vector_field.h"

class boxm2_vecf_similarity_transform : public boxm2_vecf_vector_field
{
  public:
    boxm2_vecf_similarity_transform(vgl_rotation_3d<double> rot,
                                    vgl_vector_3d<double> trans,
                                    vgl_vector_3d<double> scale);

    //: write vector field to source's BOXM2_VEC3D data
    virtual bool compute_forward_transform(boxm2_scene_sptr source,
                                           boxm2_block_id const& blk_id,
                                           boxm2_data_traits<BOXM2_VEC3D>::datatype *vec_field);
    
    //: write vector field to target's BOXM2_VEC3D data
    virtual bool compute_inverse_transform(boxm2_scene_sptr target,
                                           boxm2_block_id const& blk_id,
                                           boxm2_data_traits<BOXM2_VEC3D>::datatype *vec_field);

  private:
    vgl_rotation_3d<double> rot_;
    vgl_vector_3d<double> trans_;
    vgl_vector_3d<double> scale_;
};

#endif
