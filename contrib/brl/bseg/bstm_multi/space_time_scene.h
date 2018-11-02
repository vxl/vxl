#ifndef bstm_multi_space_time_scene_h_
#define bstm_multi_space_time_scene_h_

//: \file space_time_scene.h
//
// \brief space_time_scene is a scene that represents a 3D
// volume of space over time. It contains a 4D grid of blocks that
// each model a region of that space. The scene data structure is
// templated over block type, and different types of blocks may use
// different implementations for representing the space itself, such
// as space-time trees (e.g. BSTM) or something else.
//
// This is closely based off of bstm/bstm_scene.h. Really the only
// difference is the substitution of template parameters where
// necessary, and that the implementation does not make use of
// bstm_block_metadata's internal members (i.e. only calls functions
// such as bbox()).
//
// \author Raphael Kargon
//
// \date 26 Jul 2017
//

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_lvcs.h>
#include <vul/vul_file.h>

// smart pointer stuff
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
// vpgl camera
#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <bstm/basic/bstm_block_id.h>
#include <bstm_multi/space_time_scene_parser.hxx>

//: space_time_scene: simple scene model that maintains (in world coordinates)
//      - scene origin
//      - number of blocks in each dimension
//      - size of each block in each dimension
//      - lvcs information
//      - xml path on disk and data path (directory) on disk
// NOTE: This uses bstm_block_id as a generic ID for 4D blocks. The block type
// itself must implement:
// - Block::metadata - type alias for corresponding metadata type
// - Block::metadata must contain:
//     - a method vgl_box_3d<double> bbox()
//     - a method 'void to_xml(vsl_basic_xml_element&) const' that fills a
//     <block></block> XML tag with the metadata as attributes.
//     - a static method 'Block::metadata from_xml(const char **atts)' that
//     creates a metadata object from a given XML <block></block> tag.
template <typename Block> class space_time_scene : public vbl_ref_count {
public:
  typedef typename Block::metadata_t block_metadata;
  typedef space_time_scene_parser<Block> scene_parser;
  typedef vbl_smart_ptr<space_time_scene<Block> > sptr;

  //: empty scene, needs to be initialized manually
  space_time_scene() = default;

  space_time_scene(std::string data_path,
                   vgl_point_3d<double> const &origin,
                   int version = 2);

  //: initializes scene from xmlFile
  space_time_scene(std::string filename);

  //: destructor
  ~space_time_scene() override = default;

  //: save scene xml file
  void save_scene();

  //: return a vector of block ids in visibility order
  std::vector<bstm_block_id> get_vis_blocks(vpgl_generic_camera<double> *cam);
  std::vector<bstm_block_id>
  get_vis_blocks(vpgl_perspective_camera<double> *cam);
  std::vector<bstm_block_id> get_vis_blocks(vpgl_camera_double_sptr &cam) {
    if (cam->type_name() == "vpgl_generic_camera")
      return this->get_vis_blocks((vpgl_generic_camera<double> *)cam.ptr());
    else if (cam->type_name() == "vpgl_perspective_camera")
      return this->get_vis_blocks((vpgl_perspective_camera<double> *)cam.ptr());
    else
      std::cout
          << "space_time_scene::get_vis_blocks doesn't support camera type "
          << cam->type_name() << std::endl;
    // else return empty
    std::vector<bstm_block_id> empty;
    return empty;
  }
  //: visibility order from point, blocks must intersect with cam box
  std::vector<bstm_block_id>
  get_vis_order_from_pt(vgl_point_3d<double> const &pt,
                        vgl_box_2d<double> camBox = vgl_box_2d<double>());

  bool block_exists(bstm_block_id id) const {
    return blocks_.find(id) != blocks_.end();
  }
  bool block_on_disk(bstm_block_id id) const {
    return vul_file::exists(data_path_ + id.to_string() + ".bin");
  }
  bool data_on_disk(bstm_block_id id, std::string data_type) {
    return vul_file::exists(data_path_ + data_type + "_" + id.to_string() +
                            ".bin");
  }

  //: a list of block metadata...
  std::map<bstm_block_id, block_metadata> &blocks() { return blocks_; }
  const std::map<bstm_block_id, block_metadata> &blocks() const {
    return blocks_;
  }
  unsigned num_blocks() const { return (unsigned)blocks_.size(); }

  //: returns a copy of the metadata of the given block ID.
  block_metadata get_block_metadata(const bstm_block_id &id) const;

  std::vector<bstm_block_id> get_block_ids() const;

  //: returns the block ids of blocks that intersect the given bounding box at
  // given time, as well as the local time
  std::vector<bstm_block_id> get_block_ids(vgl_box_3d<double> bb,
                                          float time) const;

  //: gets a tight bounding box for the scene
  vgl_box_3d<double> bounding_box() const;
  //: gets a tight bounding box for the scene
  vgl_box_3d<int> bounding_box_blk_ids() const;

  //: gets tight time bounds for the scene
  void bounding_box_t(double &min_t, double &max_t) const;

  //: gets a tight time bounds of the block ids
  void blocks_ids_bounding_box_t(unsigned &min_block_id,
                                 unsigned &max_block_id) const;

  // returns the dimesnsion of the scene grid where each grid element is a block
  vgl_vector_3d<unsigned int> scene_dimensions() const;

  //: If a block contains a 3-d point, set the block id, else return false. The
  // local coordinates of the point are also returned
  bool contains(vgl_point_3d<double> const &p,
                bstm_block_id &bid,
                vgl_point_3d<double> &local_coords,
                double const t,
                double &local_time) const;

  //: returns the local time if t is contained in scene
  bool local_time(double const t, double &local_time) const;

  //: scene dimensions accessors
  vgl_point_3d<double> local_origin() const { return local_origin_; }
  vgl_point_3d<double> rpc_origin() const { return rpc_origin_; }
  vpgl_lvcs lvcs() const { return lvcs_; }

  //: scene path accessors
  std::string xml_path() const { return xml_path_; }
  std::string data_path() const { return data_path_; }

  //: appearance model accessor
  std::vector<std::string> appearances() const { return appearances_; }
  bool has_data_type(const std::string &data_type) const;

  //: scene version number
  int version() const { return version_; }
  void set_version(int v) { version_ = v; }

  //: scene mutators
  void set_local_origin(vgl_point_3d<double> org) { local_origin_ = org; }
  void set_rpc_origin(vgl_point_3d<double> rpc) { rpc_origin_ = rpc; }
  void set_lvcs(vpgl_lvcs lvcs) { lvcs_ = lvcs; }
  void set_blocks(const std::map<bstm_block_id, block_metadata> &blocks) {
    blocks_ = blocks;
  }
  void set_appearances(const std::vector<std::string> &appearances) {
    appearances_ = appearances;
  }

  //: scene path mutators
  void set_xml_path(std::string path) { xml_path_ = path; }
  void set_data_path(std::string path) { data_path_ = path + "/"; }

private:
  //: world scene information
  vpgl_lvcs lvcs_;
  vgl_point_3d<double> local_origin_;
  vgl_point_3d<double> rpc_origin_;

  //: location on disk of xml file and data/block files
  std::string data_path_, xml_path_;

  //: list of block meta data available to this scene
  std::map<bstm_block_id, block_metadata> blocks_;

  //: list of appearance models/observation models used by this scene
  std::vector<std::string> appearances_;
  int version_;
};

//: utility class for sorting id's by their distance
class space_time_dist_id_pair {
public:
  space_time_dist_id_pair(double dist, bstm_block_id id)
      : dist_(dist), id_(id) {}
  double dist_;
  bstm_block_id id_;

  inline bool operator<(space_time_dist_id_pair const &v) const {
    return dist_ < v.dist_;
  }
};

//: scene output stream operator
template <typename Block>
std::ostream &operator<<(std::ostream &s, space_time_scene<Block> &scene);

//: scene xml write function
template <typename Block>
void x_write(std::ostream &os, space_time_scene<Block> &scene, std::string name);

#endif // bstm_multi_space_time_scene_h_
