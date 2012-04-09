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
#include <boxm2/basic/boxm2_block_id.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vcl_iosfwd.h>

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class boxm2_block_metadata: public vbl_ref_count
{
 public:
  boxm2_block_metadata() {}
  boxm2_block_metadata( boxm2_block_id id,
                        vgl_point_3d<double> local_origin,
                        vgl_vector_3d<double> sub_block_dim,
                        vgl_vector_3d<unsigned> sub_block_num,
                        int init_level,
                        int max_level,
                        double max_mb,
                        double p_init,
                        int version = 1) : id_(id),
                                          local_origin_(local_origin),
                                          sub_block_dim_(sub_block_dim),
                                          sub_block_num_(sub_block_num),
                                          init_level_(init_level),
                                          max_level_(max_level),
                                          max_mb_(max_mb),
                                          p_init_(p_init),version_(version) {}

  boxm2_block_metadata( const boxm2_block_metadata& that)
  : vbl_ref_count ()
  , id_           (that.id_)
  , local_origin_ (that.local_origin_)
  , sub_block_dim_(that.sub_block_dim_)
  , sub_block_num_(that.sub_block_num_)
  , init_level_   (that.init_level_)
  , max_level_    (that.max_level_)
  , max_mb_       (that.max_mb_)
  , p_init_       (that.p_init_)
  , version_      (that.version_)
  {}

  //: id and local origin of the block
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
  int                     version_;
  //: bounding box for this block
  vgl_box_3d<double>      bbox();
  vgl_box_3d<double>      bbox(int x, int y, int z) {
    return vgl_box_3d<double>( local_origin_.x() + x*sub_block_dim_.x(),
                               local_origin_.y() + y*sub_block_dim_.y(),
                               local_origin_.z() + z*sub_block_dim_.z(),
                               local_origin_.x() + (x+1)*sub_block_dim_.x(),
                               local_origin_.y() + (y+1)*sub_block_dim_.y(),
                               local_origin_.z() + (z+1)*sub_block_dim_.z() );
  }
};

//: Smart_Pointer typedef for boxm2_block
typedef vbl_smart_ptr<boxm2_block_metadata> boxm2_block_metadata_sptr;

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_block_metadata const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_block_metadata* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_block_metadata_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, boxm2_block_metadata_sptr const& sptr);

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_block_metadata &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_block_metadata* p);
void vsl_b_read(vsl_b_istream& is, boxm2_block_metadata_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_block_metadata_sptr const& sptr);

#endif
