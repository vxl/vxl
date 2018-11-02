#ifndef bstm_multi_space_time_scene_hxx_
#define bstm_multi_space_time_scene_hxx_

#include <iostream>
#include <algorithm>
#include <limits>
#include <map>
#include <string>
#include "space_time_scene.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_vector_3d.h>

#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <vpgl/xio/vpgl_xio_lvcs.h>
#include <vsl/vsl_basic_xml_element.h>

#include <vsph/vsph_camera_bounds.h>

#include <bstm/bstm_scene_parser.h>

template <typename Block>
space_time_scene<Block>::space_time_scene(std::string data_path,
                                          vgl_point_3d<double> const &origin,
                                          int version) {
  local_origin_ = origin;
  data_path_ = data_path;
  xml_path_ = data_path_ + "/scene.xml";
  version_ = version;
}

//: initializes Scene from XML file
template <typename Block>
space_time_scene<Block>::space_time_scene(std::string filename) {
  // xml parser
  xml_path_ = filename;
  scene_parser parser;
  if (filename.size() > 0) {
    std::FILE *xmlFile = std::fopen(filename.c_str(), "r");
    if (!xmlFile) {
      std::cerr << filename.c_str() << " error on opening\n";
      return;
    }
    if (!parser.parseFile(xmlFile)) {
      std::cerr << XML_ErrorString(parser.XML_GetErrorCode()) << " at line "
               << parser.XML_GetCurrentLineNumber() << '\n';
      return;
    }
    std::fclose(xmlFile);
  }

  // store data path
  data_path_ = parser.path();
  xml_path_ = data_path_ + "scene.xml";

  // lvcs, origin, block dimension
  parser.lvcs(lvcs_);
  local_origin_ = parser.origin();
  rpc_origin_ = parser.origin();

  // store BLOCKS
  blocks_ = parser.blocks();

  // store list of appearances
  appearances_ = parser.appearances();
  version_ = parser.version();
}

template <typename Block>
std::vector<bstm_block_id> space_time_scene<Block>::get_block_ids() const {
  typename std::map<bstm_block_id, block_metadata>::const_iterator iter;
  std::vector<bstm_block_id> block_ids;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    block_ids.push_back(iter->first);
  }
  return block_ids;
}

template <typename Block>
std::vector<bstm_block_id>
space_time_scene<Block>::get_block_ids(vgl_box_3d<double> bb,
                                       float time) const {
  typename std::map<bstm_block_id, block_metadata>::const_iterator iter;
  std::vector<bstm_block_id> block_ids;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {

    vgl_box_3d<double> block_bb = (iter->second).bbox();
    double local_time;
    if (!vgl_intersection(bb, block_bb).is_empty() &&
        (iter->second).contains_t(time, local_time))
      block_ids.push_back(iter->first);
  }
  return block_ids;
}

template <typename Block>
typename space_time_scene<Block>::block_metadata
space_time_scene<Block>::get_block_metadata(const bstm_block_id &id) const {
  typename std::map<bstm_block_id, block_metadata>::const_iterator iter =
      blocks_.find(id);
  if (iter == blocks_.end()) {
    return block_metadata();
  } else {
    return iter->second;
  }
}

template <typename Block>
std::vector<bstm_block_id>
space_time_scene<Block>::get_vis_blocks(vpgl_generic_camera<double> *cam) {
  std::vector<bstm_block_id> vis_order;
  std::vector<space_time_dist_id_pair> distances;
  if (!cam) {
    std::cout << "null camera in boxm2_scene::get_vis_blocks(.)\n";
    return vis_order;
  }
  typename std::map<bstm_block_id, block_metadata>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    double min_depth = 1e10;
    std::vector<vgl_point_3d<double> > vertices = (iter->second).vertices();
    std::vector<vgl_point_3d<double> >::iterator pt_iter;
    for (pt_iter = vertices.begin(); pt_iter != vertices.end(); ++pt_iter) {
      double u, v;
      cam->project(pt_iter->x(), pt_iter->y(), pt_iter->z(), u, v);
      if (u >= 0 && v >= 0 && u < cam->cols() && v < cam->rows()) {
        vgl_point_3d<double> ro = cam->ray(u, v).origin();
        double depth = (ro - *pt_iter).length();
        if (depth < min_depth) {
          min_depth = depth;
        }
      }
    }
    if (min_depth < 1e10) {
      distances.push_back(space_time_dist_id_pair(min_depth, iter->first));
    }
  }

  // sort distances
  std::sort(distances.begin(), distances.end());

  // put blocks in "vis_order"
  std::vector<space_time_dist_id_pair>::iterator di;
  for (di = distances.begin(); di != distances.end(); ++di)
    vis_order.push_back(di->id_);
  return vis_order;
}

template <typename Block>
std::vector<bstm_block_id>
space_time_scene<Block>::get_vis_blocks(vpgl_perspective_camera<double> *cam) {
  std::vector<bstm_block_id> vis_order;
  if (!cam) {
    std::cout << "null camera in space_time_scene<Block>::get_vis_blocks(.)\n";
    return vis_order;
  }

  //---------------------------------------
  // find intersection box
  //---------------------------------------
  vgl_box_3d<double> sceneBB = this->bounding_box();
  vgl_box_2d<double> lowBox, highBox;
  vsph_camera_bounds::planar_bounding_box(*cam, lowBox, sceneBB.min_z());
  vsph_camera_bounds::planar_bounding_box(*cam, highBox, sceneBB.max_z());
  vgl_box_2d<double> camBox;
  camBox.add(lowBox);
  camBox.add(highBox);

  // grab visibility order from camera center
  vgl_point_3d<double> cam_center = cam->camera_center();
  return get_vis_order_from_pt(cam_center, camBox);
}

template <typename Block>
std::vector<bstm_block_id>
space_time_scene<Block>::get_vis_order_from_pt(vgl_point_3d<double> const &pt,
                                               vgl_box_2d<double> camBox) {
  // Map of distance, id
  std::vector<bstm_block_id> vis_order;
  std::vector<space_time_dist_id_pair> distances;

  // get camera center and order blocks distance from the cam center
  // for non-projective cameras there may not be a single center of projection
  // so instead get the ray origin farthest from the scene origin.
  typename std::map<bstm_block_id, block_metadata>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    vgl_point_3d<double> &blk_o = (iter->second).local_origin_;
    vgl_vector_3d<double> &blk_dim = (iter->second).sub_block_dim_;
    vgl_vector_3d<unsigned> &blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double> length(blk_dim.x() * blk_num.x(),
                                 blk_dim.y() * blk_num.y(),
                                 blk_dim.z() * blk_num.z());

    // make sure there is a non empty intersection here
    vgl_point_3d<double> blk_center = blk_o + length / 2.0;

    double dist = vgl_distance(blk_center, pt);
    distances.push_back(bstm_dist_id_pair(dist, iter->first));
  }

  // sort distances
  std::sort(distances.begin(), distances.end());

  // put blocks in "vis_order"
  std::vector<space_time_dist_id_pair>::iterator di;
  for (di = distances.begin(); di != distances.end(); ++di)
    vis_order.push_back(di->id_);
  return vis_order;
}

//: find the block containing the specified point, else return false
//  Local coordinates are also returned
template <typename Block>
bool space_time_scene<Block>::contains(vgl_point_3d<double> const &p,
                                       bstm_block_id &bid,
                                       vgl_point_3d<double> &local_coords,
                                       double const t,
                                       double &local_time) const {
  std::vector<bstm_block_id> block_ids = this->get_block_ids();
  for (auto & block_id : block_ids) {
    block_metadata md = this->get_block_metadata_const(block_id);
    vgl_vector_3d<double> dims(md.sub_block_dim_.x() * md.sub_block_num_.x(),
                               md.sub_block_dim_.y() * md.sub_block_num_.y(),
                               md.sub_block_dim_.z() * md.sub_block_num_.z());

    vgl_point_3d<double> lorigin = md.local_origin_;
    vgl_box_3d<double> bbox(lorigin, lorigin + dims);
    if (bbox.contains(p.x(), p.y(), p.z())) {

      // now check for time
      if (md.contains_t(t, local_time)) {
        bid = block_id;
        double local_x = (p.x() - md.local_origin_.x()) / md.sub_block_dim_.x();
        double local_y = (p.y() - md.local_origin_.y()) / md.sub_block_dim_.y();
        double local_z = (p.z() - md.local_origin_.z()) / md.sub_block_dim_.z();
        local_coords.set(local_x, local_y, local_z);
        return true;
      }
    }
  }
  return false;
}

//  Local coordinates are also returned
template <typename Block>
bool space_time_scene<Block>::local_time(double const t,
                                         double &local_time) const {
  typename std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    if ((iter->second).contains_t(t, local_time))
      return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

//: save scene (xml file)
template <typename Block> void space_time_scene<Block>::save_scene() {
  // write out to XML file
  std::ofstream xmlstrm(xml_path_.c_str());
  x_write(xmlstrm, (*this), "scene");
  xmlstrm.close();
}

template <typename Block>
void space_time_scene<Block>::bounding_box_t(double &min_t,
                                             double &max_t) const {
  min_t = 10e10;
  max_t = -10e10;

  typename std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    double blk_min, blk_max;
    (iter->second).bbox_t(blk_min, blk_max);
    if (blk_min < min_t)
      min_t = blk_min;

    if (blk_max > max_t)
      max_t = blk_max;
  }
}

//: gets a tight bounding box of the block ids
template <typename Block>
void space_time_scene<Block>::blocks_ids_bounding_box_t(
    unsigned &min_block_id, unsigned &max_block_id) const {
  min_block_id = std::numeric_limits<unsigned>::max();
  max_block_id = std::numeric_limits<unsigned>::min();

  typename std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    if (iter->first.t_ < (int)min_block_id)
      min_block_id = iter->first.t_;

    if (iter->first.t_ > (int)max_block_id)
      max_block_id = iter->first.t_;
  }
}

template <typename Block>
vgl_box_3d<double> space_time_scene<Block>::bounding_box() const {
  double xmin = 10e10, xmax = -10e10;
  double ymin = 10e10, ymax = -10e10;
  double zmin = 10e10, zmax = -10e10;

  // iterate through each block
  typename std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    // determine xmin, ymin, zmin using block_o
    vgl_point_3d<double> blk_o = (iter->second).local_origin_;
    if (blk_o.x() < xmin)
      xmin = blk_o.x();
    if (blk_o.y() < ymin)
      ymin = blk_o.y();
    if (blk_o.z() < zmin)
      zmin = blk_o.z();

    // get block max point
    vgl_vector_3d<double> blk_dim = (iter->second).sub_block_dim_;
    vgl_vector_3d<unsigned> blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double> length(blk_dim.x() * blk_num.x(),
                                 blk_dim.y() * blk_num.y(),
                                 blk_dim.z() * blk_num.z());
    vgl_point_3d<double> blk_max = blk_o + length;
    if (blk_max.x() > xmax)
      xmax = blk_max.x();
    if (blk_max.y() > ymax)
      ymax = blk_max.y();
    if (blk_max.z() > zmax)
      zmax = blk_max.z();
  }

  //: Construct from ranges in \a x,y,z (take care with order of inputs).
  //  The \a x range is given by the 1st and 4th coordinates,
  //  the \a y range is given by the 2nd and 5th coordinates,
  //  the \a z range is given by the 3rd and 6th coordinates.
  return {xmin, ymin, zmin, xmax, ymax, zmax};
}

template <typename Block>
vgl_box_3d<int> space_time_scene<Block>::bounding_box_blk_ids() const {
  vgl_box_3d<int> bbox;
  // iterate through each block
  typename std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter)
    bbox.add(
        vgl_point_3d<int>(iter->first.i(), iter->first.j(), iter->first.k()));

  return bbox;
}

template <typename Block>
vgl_vector_3d<unsigned int> space_time_scene<Block>::scene_dimensions() const {
  std::vector<bstm_block_id> ids = this->get_block_ids();

  if (ids.empty())
    return {0, 0, 0};

  int max_i = ids[0].i(), max_j = ids[0].j(), max_k = ids[0].k();
  int min_i = ids[0].i(), min_j = ids[0].j(), min_k = ids[0].k();

  for (auto & id : ids) {
    if (id.i() > max_i)
      max_i = id.i();
    if (id.j() > max_j)
      max_j = id.j();
    if (id.k() > max_k)
      max_k = id.k();

    if (id.i() < min_i)
      min_i = id.i();
    if (id.j() < min_j)
      min_j = id.j();
    if (id.k() < min_k)
      min_k = id.k();
  }
  max_i++;
  max_j++;
  max_k++;

  return {
      static_cast<unsigned int>((max_i - min_i)), static_cast<unsigned int>((max_j - min_j)), static_cast<unsigned int>((max_k - min_k))};
}

//: returns true if the scene has specified data type (simple linear search)
template <typename Block>
bool space_time_scene<Block>::has_data_type(const std::string &data_type) const {
  for (unsigned int i = 0; i < appearances_.size(); ++i)
    if (appearances_[i] == data_type)
      return true;
  return false;
}

//---------------------------------------------------------------------
// NON CLASS FUNCTIONS
//---------------------------------------------------------------------
//------------XML WRITE------------------------------------------------
// NOTE: This uses the same XML tags as bstm_scene_parser, since except for
// block-specific info, all 4D scenes should have the same metadata.
template <typename Block>
void x_write(std::ostream &os, space_time_scene<Block> &scene, std::string name) {
  typedef typename Block::metadata block_metadata;
  // open root tag
  vsl_basic_xml_element scene_elm(name);
  scene_elm.x_write_open(os);

  // write lvcs information
  vpgl_lvcs lvcs = scene.lvcs();
  x_write(os, lvcs, LVCS_TAG);
  x_write(os, scene.local_origin(), LOCAL_ORIGIN_TAG);

  // write scene path for (needs to know where blocks are)
  std::string path = scene.data_path();
  vsl_basic_xml_element paths(SCENE_PATHS_TAG);
  paths.add_attribute("path", path + '/');
  paths.x_write(os);

  vsl_basic_xml_element ver(VERSION_TAG);
  ver.add_attribute("number", scene.version());
  ver.x_write(os);

  // write list of appearance models

  std::vector<std::string> apps = scene.appearances();
  for (const auto & app : apps) {
    vsl_basic_xml_element apms(APM_TAG);
    apms.add_attribute("apm", app);
    apms.x_write(os);
  }

  // write block information for each block
  std::map<bstm_block_id, block_metadata> blocks = scene.blocks();
  typename std::map<bstm_block_id, block_metadata>::iterator iter;
  for (iter = blocks.begin(); iter != blocks.end(); ++iter) {
    const block_metadata &data = iter->second;
    // creat tag from metadata & write to stream
    vsl_basic_xml_element block(BLOCK_TAG);
    data.to_xml(block);
    block.x_write(os);
  }

  // close up tag
  scene_elm.x_write_close(os);
}

//------------IO Stream------------------------------------------------
template <typename Block>
std::ostream &operator<<(std::ostream &s, space_time_scene<Block> &scene) {
  typedef typename space_time_scene<Block>::block_metadata block_metadata;

  s << "--- bstm_scene -----------------------------\n"
    << "xml_path:         " << scene.xml_path() << '\n'
    << "data_path:        " << scene.data_path() << '\n'
    << "world origin:     " << scene.rpc_origin() << '\n'
    << "list of APMs:     " << '\n';

  // list appearance models for this scene
  std::vector<std::string> apps = scene.appearances();
  for (const auto & app : apps)
    s << "    " << app << ", ";
  s << '\n';
  vpgl_lvcs lvcs = scene.lvcs();
  s << lvcs << '\n';

  vgl_box_3d<double> bb = scene.bounding_box();
  vgl_point_3d<double> minp = bb.min_point();
  vgl_point_3d<double> maxp = bb.max_point();
  s << "bounds : ( " << minp.x() << ' ' << minp.y() << ' ' << minp.z()
    << " )==>( " << maxp.x() << ' ' << maxp.y() << ' ' << maxp.z() << " )\n";

  // list of block ids for this scene....
  vgl_vector_3d<unsigned> dims = scene.scene_dimensions();
  s << "block array dims(" << dims.x() << ' ' << dims.y() << ' ' << dims.z()
    << ")\n";
  std::map<bstm_block_id, block_metadata> &blk = scene.blocks();
  s << " blocks:==>\n";
  for (typename std::map<bstm_block_id, block_metadata>::iterator bit =
           blk.begin();
       bit != blk.end();
       ++bit) {
    s << bit->second << "\n";
  }
  s << "<=====:end blocks\n";
  return s;
}

#endif // bstm_multi_space_time_scene_hxx_
