#include <boxm2/vecf/ocl/boxm2_vecf_ocl_orbit_vector_field.h>
bool boxm2_vecf_ocl_orbit_vector_field::compute_forward_transform(boxm2_scene_sptr  /*source*/,
                                 boxm2_block_id const&  /*blk_id*/,
                                 bocl_mem*  /*pts_source*/, // in
                                 bocl_mem*  /*pts_target*/, // out
                                 cl_command_queue & /*queue*/){return true;}
bool boxm2_vecf_ocl_orbit_vector_field::compute_inverse_transform(boxm2_scene_sptr  /*target*/,
                                 boxm2_block_id const&  /*blk_id*/,
                                 bocl_mem*  /*pts_target*/, // in
                                 bocl_mem*  /*pts_source*/, // out
                                 cl_command_queue & /*queue*/){return true;}
