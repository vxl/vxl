#ifndef bstm_block_metadata_h_
#define bstm_block_metadata_h_
//:
// \file
// \brief  bstm_block_metadata data keeps track of non structure, non visual data
//
//  Block meta data includes:
//    id,
//    block origin
//    num_sub_blocks (x,y,z,t)
//    sub_block_dim (x,y,z,t)
//    init_level, init_level_t
//    max_level, max_level_t
//    max_mb
//
// \author Ali Osman Ulusoy
// \date   03 Aug 2012
//
#include <iostream>
#include <iosfwd>
#include <utility>
#include <bstm/basic/bstm_block_id.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

#include <vsl/vsl_basic_xml_element.h>

#include <boxm2/boxm2_block_metadata.h>
class bstm_block_metadata:  public vbl_ref_count
{
 public:
  bstm_block_metadata() = default;
  bstm_block_metadata(  bstm_block_id id,
                        vgl_point_3d<double> local_origin,
                        float local_t,
                        vgl_vector_3d<double> sub_block_dim,
                        double sub_block_dim_t,
                        vgl_vector_3d<unsigned> sub_block_num,
                        unsigned num_t,
                        int init_level,
                        int max_level,
                        double max_mb,
                        double p_init,
                        int version = 1,
                        int init_level_t=1,
                        int max_level_t=6)
  :  id_(id),
     local_origin_(local_origin),
     local_origin_t_(local_t),
     sub_block_dim_t_(sub_block_dim_t),
     sub_block_num_t_(num_t),
     sub_block_dim_(sub_block_dim),
     sub_block_num_(sub_block_num),
     init_level_(init_level),
     max_level_(max_level),
     init_level_t_(init_level_t),
     max_level_t_(max_level_t),
     max_mb_(max_mb),
     p_init_(p_init),
     version_(version) {}

  bstm_block_metadata( const bstm_block_metadata& that)
  : vbl_ref_count ()
  , id_           (that.id_)
  , local_origin_ (that.local_origin_)
  , local_origin_t_ (that.local_origin_t_)
  , sub_block_dim_t_(that.sub_block_dim_t_)
  , sub_block_num_t_(that.sub_block_num_t_)
  , sub_block_dim_(that.sub_block_dim_)
  , sub_block_num_(that.sub_block_num_)
  , init_level_   (that.init_level_)
  , max_level_    (that.max_level_)
  , init_level_t_ (that.init_level_t_)
  , max_level_t_  (that.max_level_t_)
  , max_mb_       (that.max_mb_)
  , p_init_       (that.p_init_)
  , version_      (that.version_)
  {}

  //: id and local origin of the block
  bstm_block_id          id_;
  vgl_point_3d<double>    local_origin_;

  //: origin of time axis
  double   local_origin_t_;

  //: dimension of time axis, e.g., 1s
  double   sub_block_dim_t_;

  //: number of sub-blocks time dimension
  unsigned   sub_block_num_t_;

  //: World dimensions of a sub block .e.g 1 meter x 1 meter x 1 meter
  vgl_vector_3d<double>   sub_block_dim_;

  //: number of sub blocks in each dimension
  vgl_vector_3d<unsigned> sub_block_num_;

  //: info about block's trees
  int                     init_level_;    //each sub_blocks's init level (default 1)
  int                     max_level_;     //each sub_blocks's max_level (default 4)
  int                     init_level_t_;  //time tree's initial level(default 1)
  int                     max_level_t_;   //time tree's maximum level(default 6)

  double                  max_mb_;       //each total block mb
  double                  p_init_;       //initialize occupancy probs with this
  int                     version_;

  //: checks if this block contains the given t, if yes, returns local_time
  bool                    contains_t  (double const t, double& local_time) const;

  bstm_block_id id() const { return id_; }
  //: bounding box for this block
  vgl_box_3d<double>      bbox() const;
  void   bbox_t(double& min_t, double& max_t) const {min_t = local_origin_t_; max_t = local_origin_t_ + sub_block_num_t_*sub_block_dim_t_; };
  std::pair<double, double> bbox_t() const { std::pair<double,double> p; bbox_t(p.first, p.second); return p; }
  std::pair<vgl_vector_3d<double>,double> resolution() const {
    return std::pair<vgl_vector_3d<double>,double>(sub_block_dim_ / 8.0, sub_block_dim_t_ / 32.0);
  }

  bool operator==(bstm_block_metadata const& m) const;
  bool operator==(boxm2_block_metadata const& m) const;

  //: Writes this block's metadata to an XML element which can later
  //be written to a file, e.g. as part of a scene.
  void to_xml(vsl_basic_xml_element& block) const;
  static bstm_block_metadata from_xml(const char **atts);
};

//: Smart_Pointer typedef for bstm_block
typedef vbl_smart_ptr<bstm_block_metadata> bstm_block_metadata_sptr;

std::ostream &operator<<(std::ostream &s, const bstm_block_metadata &metadata);

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& os, bstm_block_metadata const& scene);
void vsl_b_write(vsl_b_ostream& os, const bstm_block_metadata* &p);
void vsl_b_write(vsl_b_ostream& os, bstm_block_metadata_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, bstm_block_metadata_sptr const& sptr);

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& is, bstm_block_metadata &scene);
void vsl_b_read(vsl_b_istream& is, bstm_block_metadata* p);
void vsl_b_read(vsl_b_istream& is, bstm_block_metadata_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, bstm_block_metadata_sptr const& sptr);

#endif
