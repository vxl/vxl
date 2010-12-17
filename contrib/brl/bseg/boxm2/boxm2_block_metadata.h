#ifndef boxm2_block_metadata_h_
#define boxm2_block_metadata_h_
//:
// \file
// \brief  boxm2_block_meta data keeps track of non structure, non visual data
//
//  Block meta data includes: 
//    id, 
//    block origin
//    num_sub_blocks (x,y,z)
//    sub_block_dim (x,y,z)
//    init_level
//    max_level
//    max_mb
//
// \author Andrew Miller
// \date   16 Dec 2010
//
#include <boxm2/boxm2_block_id.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vcl_iosfwd.h>

class boxm2_block_metadata
{
  public: 
    boxm2_block_id          id_;
    vgl_point_3d<double>    local_origin_;

    //: World dimensions of a sub block .e.g 1 meter x 1 meter x 1 meter
    vgl_vector_3d<double>   sub_block_dim_;

    //: number of sub blocks in each dimension
    vgl_vector_3d<unsigned> sub_block_num_;

    //: info about block's trees
    int                     init_level_;   //each sub_blocks's init level (default 1)
    int                     max_level_;    //each sub_blocks's max_level (default 4)
    double                  max_mb_;       //each total block mb
    double                  p_init_;       //initialize occupancy probs with this
}; 

#endif
