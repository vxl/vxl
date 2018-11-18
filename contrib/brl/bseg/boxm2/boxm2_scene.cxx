#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstddef>
#include "boxm2_scene.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
/* xml includes */
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <boxm2/boxm2_scene_parser.h>
#include <vpgl/xio/vpgl_xio_lvcs.h>

//vgl includes
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>

#include <vgl/vgl_intersection.h>

//vsph include
#include <vsph/vsph_camera_bounds.h>

//vul include
#include <vul/vul_file.h>


boxm2_scene::boxm2_scene(std::string const& data_path, vgl_point_3d<double> const& origin, int version)
{
  local_origin_=origin;
  data_path_   = data_path;
  xml_path_  = data_path_ + "/scene.xml";
  num_illumination_bins_ = -1;
  version_ = version;
  id_ = boxm2_scene::get_count();
  boxm2_scene::get_count()++;
}
//create a scene with one block
boxm2_scene::boxm2_scene(std::string const& scene_dir, std::string const& scene_name, std::string const& data_path, std::vector<std::string> const& prefixes,
            vgl_box_3d<double> const& scene_box, double sub_block_len, int init_level,
                         int max_level, double  /*max_mb*/, double  /*p_init*/, int n_illum_bins, int version){
  num_illumination_bins_ = n_illum_bins;
  version_ = version;
  id_ = boxm2_scene::get_count();
  boxm2_scene::get_count()++;
  boxm2_block_id bid(0,0,0);
  boxm2_block_metadata md;
  md.id_ = bid;
  vgl_point_3d<double> origin = scene_box.min_point();
  auto dim_x = static_cast<unsigned>(std::ceil(scene_box.width()/sub_block_len));
  auto dim_y = static_cast<unsigned>(std::ceil(scene_box.height()/sub_block_len));
  auto dim_z = static_cast<unsigned>(std::ceil(scene_box.depth()/sub_block_len));
  md.local_origin_ = origin;
  md.sub_block_dim_ = vgl_vector_3d<double>(sub_block_len, sub_block_len, sub_block_len);
  md.sub_block_num_ = vgl_vector_3d<unsigned>(dim_x, dim_y, dim_z);
  md.init_level_ = init_level;
  md.max_level_ = max_level;
  md.max_mb_ = 400;
  md.p_init_ = .001;
  md.version_ = version;
  blocks_[bid]=md;
  appearances_ = prefixes;
  this->set_local_origin(origin);
  this->set_rpc_origin(origin);
  vpgl_lvcs lvcs;
  this->set_lvcs(lvcs);
  this->set_xml_path(scene_dir + scene_name + ".xml");
  this->set_data_path(scene_dir + data_path );
}
boxm2_scene::boxm2_scene(const char* buffer)
{
  boxm2_scene_parser parser;
  if (!parser.parseString(buffer)) {
    std::cerr << XML_ErrorString(parser.XML_GetErrorCode()) << " at line "
             << parser.XML_GetCurrentLineNumber() << '\n';
    return;
  }

  //store data path
  data_path_ = parser.path();
  xml_path_  = data_path_ + "scene.xml";

  //lvcs, origin, block dimension
  parser.lvcs(lvcs_);
  local_origin_ = parser.origin();
  rpc_origin_   = parser.origin();

  //store BLOCKS
  blocks_ = parser.blocks();

  //store list of appearances
  appearances_ = parser.appearances();
  num_illumination_bins_ = parser.num_illumination_bins();
  version_ = parser.version();
  id_ = boxm2_scene::get_count();
  boxm2_scene::get_count()++;
}

//: initializes Scene from XML file
boxm2_scene::boxm2_scene(std::string const& filename)
{
  std::ifstream ifs;
  // we must throw an exception on failure in a constructor
  ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  ifs.open(filename.c_str());
  std::stringstream buffer;
  buffer << ifs.rdbuf();

  //xml parser
  xml_path_ = filename;
  boxm2_scene_parser parser;

  if (!parser.parseString(buffer.str().c_str())) {
    std::cerr << XML_ErrorString(parser.XML_GetErrorCode()) << " at line "
             << parser.XML_GetCurrentLineNumber() << '\n';
    throw std::ifstream::failure("Error parsing file.");
  }

  //store data path
  if(parser.is_model_local_to_scene_path()) {
    // to make the model (data) path relative to the scene file,
    // set the 'is_model_local' bool attribute of the <scene_paths> tag
    std::string basepath = vul_file::dirname(filename); // cant return an empty string
    data_path_ = basepath + "/" + parser.path(); // not normalized, but thats ok
  }
  else {
    // the data path is relative to the current working directory of the process
    data_path_ = parser.path();
  }

  //lvcs, origin, block dimension
  parser.lvcs(lvcs_);
  local_origin_ = parser.origin();
  rpc_origin_   = parser.origin();

  //store BLOCKS
  blocks_ = parser.blocks();

  //store list of appearances
  appearances_ = parser.appearances();
  num_illumination_bins_ = parser.num_illumination_bins();
  version_ = parser.version();
  id_ = boxm2_scene::get_count();
  boxm2_scene::get_count()++;
}
// pretty much a straignt copy but provide a unique id
boxm2_scene::boxm2_scene(boxm2_scene const& other_scene){
  lvcs_ = other_scene.lvcs();
  local_origin_ = other_scene.local_origin();
  rpc_origin_ = other_scene.rpc_origin();
  data_path_ = other_scene.data_path();
  xml_path_ = other_scene.xml_path();
  auto& non_const_scene = const_cast<boxm2_scene&>(other_scene);
  blocks_ = non_const_scene.blocks();
  appearances_ = other_scene.appearances();
  num_illumination_bins_ = other_scene.num_illumination_bins();
  version_ = other_scene.version();
  id_ = boxm2_scene::get_count();
  boxm2_scene::get_count()++;
}

boxm2_scene_sptr boxm2_scene::clone_no_disk(){
  auto* clone = new boxm2_scene();
  clone->set_lvcs(this->lvcs_);
  clone->set_local_origin(this->local_origin_);
  clone->set_rpc_origin(this->rpc_origin_);
  clone->set_data_path("");
  clone->set_xml_path("");
  clone->set_blocks(this->blocks_);
  clone->set_appearances(this->appearances_);
  clone->set_num_illumination_bins(this->num_illumination_bins_);
  clone->set_version(this->version_);
  return clone;
}

//: add a block meta data...
void boxm2_scene::add_block_metadata(boxm2_block_metadata data)
{
  if ( blocks_.find(data.id_) != blocks_.end() )
  {
    std::cout<<"Boxm2 SCENE: Overwriting block metadata for id: "<<data.id_<<std::endl;
  }
  blocks_[data.id_] = data;
}

std::vector<boxm2_block_id> boxm2_scene::get_block_ids() const
{
  std::map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter;
  std::vector<boxm2_block_id> block_ids;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    block_ids.push_back(iter->first);
  }
  return block_ids;
}

boxm2_block_metadata boxm2_scene::
get_block_metadata_const(boxm2_block_id const& id) const
{
  std::map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter)
    if ((*iter).first == id)
      return (*iter).second;
  return boxm2_block_metadata();
}

#include <boxm2/boxm2_blocks_vis_graph.h>

std::vector<boxm2_block_id> boxm2_scene::get_vis_blocks(vpgl_generic_camera<double>* cam, double  /*dist*/)
{
  boxm2_block_vis_graph g(blocks_,*cam);
  std::vector<boxm2_block_id> vis_order = g.get_ordered_ids();
  return vis_order;
}

std::vector<boxm2_block_id> boxm2_scene::get_vis_blocks(vpgl_affine_camera<double>* cam)
{
  std::vector<boxm2_block_id> vis_order;
  if (!cam) {
    std::cout << "null camera in boxm2_scene::get_vis_blocks(.)\n";
    return vis_order;
  }
  vgl_homg_point_3d<double> cam_center_ideal = cam->camera_center();
  vgl_vector_3d<double> ray_dir(cam_center_ideal.x(), cam_center_ideal.y(), cam_center_ideal.z());

  vgl_box_3d<int> idx_bbox;
  for (auto & block : blocks_) {
    boxm2_block_id const& id = block.first;
    idx_bbox.add(vgl_point_3d<int>(id.i_, id.j_, id.k_));
  }
  int closest_i = ray_dir.x() > 0? idx_bbox.min_x() : idx_bbox.max_x();
  int closest_j = ray_dir.y() > 0? idx_bbox.min_y() : idx_bbox.max_y();
  int closest_k = ray_dir.z() > 0? idx_bbox.min_z() : idx_bbox.max_z();

  // visibility ordering is based on manhattan distance of block index from that of closest block.
  std::vector<boxm2_dist_id_pair> manhattan_distances;
  for (auto & block : blocks_)
  {
    // dec: we would perform a visibility test here if we had ni,nj.
    //if(!this->is_block_visible(iter->second,*cam,ni,nj))
    //  continue;

    int manhattan_distance = std::abs(block.first.i_ - closest_i) +
                             std::abs(block.first.j_ - closest_j) +
                             std::abs(block.first.k_ - closest_k);

    manhattan_distances.emplace_back(manhattan_distance, block.first );
  }

    //sort distances
    std::sort(manhattan_distances.begin(), manhattan_distances.end());
    //put blocks in "vis_order"
    std::vector<boxm2_dist_id_pair>::iterator di;
    for (di = manhattan_distances.begin(); di != manhattan_distances.end(); ++di) {
        vis_order.push_back(di->id_);
    }
    return vis_order;
}


std::vector<boxm2_block_id> boxm2_scene::get_vis_blocks(vpgl_perspective_camera<double>* cam, double dist)
{
  std::vector<boxm2_block_id> vis_order;
  if (!cam) {
    std::cout << "null camera in boxm2_scene::get_vis_blocks(.)\n";
    return vis_order;
  }

  //---------------------------------------
  //find intersection box
  //---------------------------------------
  vgl_box_3d<double> sceneBB = this->bounding_box();
  vgl_box_2d<double> lowBox, highBox;
  vsph_camera_bounds::planar_bounding_box(*cam, lowBox, sceneBB.min_z());
  vsph_camera_bounds::planar_bounding_box(*cam, highBox, sceneBB.max_z());
  vgl_box_2d<double> camBox;
  camBox.add(lowBox);
  camBox.add(highBox);

  //grab visibility order from camera center
  vgl_point_3d<double> cam_center = cam->camera_center();


  return get_vis_order_from_pt(cam_center, camBox, dist);
}
std::vector<boxm2_block_id> boxm2_scene::get_vis_blocks_opt(vpgl_perspective_camera<double>* cam,  unsigned int ni, unsigned int nj)
{
    std::vector<boxm2_block_id> vis_order;
    if (!cam) {
        std::cout << "null camera in boxm2_scene::get_vis_blocks(.)\n";
        return vis_order;
    }
    //grab visibility order from camera center
    vgl_point_3d<double> cam_center = cam->camera_center();
    //Map of distance, id
    std::vector<boxm2_dist_id_pair> distances;
    //get camera center and order blocks distance from the cam center
    //for non-projective cameras there may not be a single center of projection
    //so instead get the ray origin farthest from the scene origin.
    std::map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
    for (iter = blocks_.begin(); iter != blocks_.end(); ++iter)
    {
        if(!this->is_block_visible(iter->second,*cam,ni,nj))
            continue;
        vgl_point_3d<double>&    blk_o   = (iter->second).local_origin_;
        vgl_vector_3d<double>&   blk_dim = (iter->second).sub_block_dim_;
        vgl_vector_3d<unsigned>& blk_num = (iter->second).sub_block_num_;
        vgl_vector_3d<double>    length(blk_dim.x()*blk_num.x(),
                                        blk_dim.y()*blk_num.y(),
                                        blk_dim.z()*blk_num.z());

        //make sure there is a non empty intersection here
        vgl_box_2d<double> blkBox(blk_o.x(), blk_o.x()+length.x(),blk_o.y(), blk_o.y()+length.y());
        vgl_point_3d<double> blk_center = blk_o + length/2.0;

        double dist = vgl_distance( blk_center, cam_center);
        distances.emplace_back(dist, iter->first );

    }

    //sort distances
    std::sort(distances.begin(), distances.end());
    //put blocks in "vis_order"
    std::vector<boxm2_dist_id_pair>::iterator di;
    for (di = distances.begin(); di != distances.end(); ++di)
        vis_order.push_back(di->id_);
    return vis_order;
}
std::vector<boxm2_block_id>
boxm2_scene::get_vis_order_from_pt(vgl_point_3d<double> const& pt,
                                   vgl_box_2d<double> camBox, double distance)
{
  //Map of distance, id
  std::vector<boxm2_block_id> vis_order;
  std::vector<boxm2_dist_id_pair> distances;

  //get camera center and order blocks distance from the cam center
  //for non-projective cameras there may not be a single center of projection
  //so instead get the ray origin farthest from the scene origin.
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    vgl_point_3d<double>&    blk_o   = (iter->second).local_origin_;
    vgl_vector_3d<double>&   blk_dim = (iter->second).sub_block_dim_;
    vgl_vector_3d<unsigned>& blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double>    length(blk_dim.x()*blk_num.x(),
                                    blk_dim.y()*blk_num.y(),
                                    blk_dim.z()*blk_num.z());

    //make sure there is a non empty intersection here
    vgl_box_2d<double> blkBox(blk_o.x(), blk_o.x()+length.x(),
                              blk_o.y(), blk_o.y()+length.y());
    vgl_box_2d<double> intersect = vgl_intersection(camBox, blkBox);

      vgl_point_3d<double> blk_center = blk_o + length/2.0;

      double dist = vgl_distance( blk_center, pt);

      if (distance > 0 && dist < distance)
        distances.emplace_back(dist, iter->first );
      else
        distances.emplace_back(dist, iter->first );

  }

  //sort distances
  std::sort(distances.begin(), distances.end());

  //put blocks in "vis_order"
  std::vector<boxm2_dist_id_pair>::iterator di;
  for (di = distances.begin(); di != distances.end(); ++di)
    vis_order.push_back(di->id_);
  return vis_order;
}
//: return all blocks with center less than dist from the given point
std::vector<boxm2_block_id> boxm2_scene::get_vis_blocks(vgl_point_3d<double> const& pt, double distance)
{
  std::vector<boxm2_block_id> vis_order;
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    vgl_point_3d<double>&    blk_o   = (iter->second).local_origin_;
    vgl_vector_3d<double>&   blk_dim = (iter->second).sub_block_dim_;
    vgl_vector_3d<unsigned>& blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double>    length(blk_dim.x()*blk_num.x(),
                                    blk_dim.y()*blk_num.y(),
                                    blk_dim.z()*blk_num.z());
    vgl_point_3d<double> blk_center = blk_o + length/2.0;
    double dist = vgl_distance( blk_center, pt);
    if (dist <= distance)
      vis_order.push_back(iter->first);
  }
  return vis_order;
}


std::vector<boxm2_block_id>
boxm2_scene::get_vis_order_from_ray(vgl_point_3d<double> const& origin, vgl_vector_3d<double> const& dir, double distance)
{
  // Map of distance, id
  std::vector<boxm2_block_id> vis_order;
  std::vector<boxm2_dist_id_pair> distances;

  // get camera center and order blocks distance from the cam center
  // do not insert blocks if they are in front of the camera!
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    vgl_point_3d<double>&    blk_o   = (iter->second).local_origin_;
    vgl_vector_3d<double>&   blk_dim = (iter->second).sub_block_dim_;
    vgl_vector_3d<unsigned>& blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double>    length(blk_dim.x()*blk_num.x(),
                                    blk_dim.y()*blk_num.y(),
                                    blk_dim.z()*blk_num.z());

    vgl_point_3d<double> blk_center = blk_o + length/2.0;

    // ray from origin to blk center
    vgl_vector_3d<double> blk_ray = blk_center - origin;
    vgl_vector_3d<double> blk_ray_n = normalized(blk_ray);

    // check if the blk ray and camera ray are pointing to the same direction
    double cos = dot_product(dir, blk_ray_n);
    if (cos > 0) { // an angle in (-pi/2,pi/2)
      double dist = vgl_distance( blk_center, origin);
      if (distance > 0 && dist < distance)
        distances.emplace_back(dist, iter->first );
      else
        distances.emplace_back(dist, iter->first );
    }
  }

  //sort distances
  std::sort(distances.begin(), distances.end());

  //put blocks in "vis_order"
  std::vector<boxm2_dist_id_pair>::iterator di;
  for (di = distances.begin(); di != distances.end(); ++di)
    vis_order.push_back(di->id_);
  return vis_order;
}

//: If a block contains a 3-d point, set the local coordinates of the point
bool boxm2_scene::block_contains(vgl_point_3d<double> const& p, boxm2_block_id const& bid,
                                 vgl_point_3d<double>& local_coords) const
{
  boxm2_block_metadata md = this->get_block_metadata_const(bid);
  if (md.init_level_ == 0) // block does not exist
    return false;
  vgl_vector_3d<double> dims(md.sub_block_dim_.x()*md.sub_block_num_.x(),
                             md.sub_block_dim_.y()*md.sub_block_num_.y(),
                             md.sub_block_dim_.z()*md.sub_block_num_.z());

  vgl_point_3d<double> lorigin = md.local_origin_;
  vgl_box_3d<double> bbox(lorigin,lorigin+dims);
  if (p.x() >= bbox.min_x() && p.x() < bbox.max_x() &&
      p.y() >= bbox.min_y() && p.y() < bbox.max_y() &&
      p.z() >= bbox.min_z() && p.z() < bbox.max_z())
  {//Slightly different than bbox.contains, which was wrong on block boundary
    double local_x=(p.x()-md.local_origin_.x())/md.sub_block_dim_.x();
    double local_y=(p.y()-md.local_origin_.y())/md.sub_block_dim_.y();
    double local_z=(p.z()-md.local_origin_.z())/md.sub_block_dim_.z();
    local_coords.set(local_x, local_y, local_z);
    return true;
  }
  return false;
}

//: find the block containing the specified point, else return false
//  Local coordinates are also returned
bool boxm2_scene::contains(vgl_point_3d<double> const& p, boxm2_block_id& bid,
                           vgl_point_3d<double>& local_coords) const
{
  std::vector<boxm2_block_id> block_ids = this->get_block_ids();
  for (auto & block_id : block_ids)
  {
    if (this->block_contains(p, block_id, local_coords)) {
      bid = block_id;
      return true;
    }
  }
  return false;
}

//: save scene (xml file)
void boxm2_scene::save_scene()
{
  //write out to XML file
  std::ofstream xmlstrm(xml_path_.c_str());
  x_write(xmlstrm, (*this), "scene");
  xmlstrm.close();
}

//: return a heap pointer to a scene info
boxm2_scene_info* boxm2_scene::get_blk_metadata(boxm2_block_id const& id)
{
  if ( blocks_.find(id) == blocks_.end() )
  {
    std::cerr<<"\nboxm2_scene::get_blk_metadata: Block doesn't exist: "<<id<<"\n\n";
    return nullptr;
  }

  boxm2_block_metadata data = blocks_[id];
  auto* info = new boxm2_scene_info();

  info->scene_origin[0] = (float) data.local_origin_.x();
  info->scene_origin[1] = (float) data.local_origin_.y();
  info->scene_origin[2] = (float) data.local_origin_.z();
  info->scene_origin[3] = (float) 0.0f;

  info->scene_dims[0] = (int) data.sub_block_num_.x();  // number of blocks in each dimension
  info->scene_dims[1] = (int) data.sub_block_num_.y();
  info->scene_dims[2] = (int) data.sub_block_num_.z();
  info->scene_dims[3] = (int) 0;

  info->block_len = (float) data.sub_block_dim_.x();
  info->epsilon   = (float) (info->block_len / 100.0f);

  info->root_level = data.max_level_-1;
  info->num_buffer = 0;
  info->tree_buffer_length = 0;
  info->data_buffer_length = 0;
  info->pinit = data.p_init_;
  return info;
}


vgl_box_3d<double> boxm2_scene::bounding_box() const
{
  double xmin=10e10, xmax=-10e10;
  double ymin=10e10, ymax=-10e10;
  double zmin=10e10, zmax=-10e10;

  //iterate through each block
  std::map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter)
  {
    //determine xmin, ymin, zmin using block_o
    vgl_point_3d<double>  blk_o = (iter->second).local_origin_;
    if (blk_o.x() < xmin) xmin = blk_o.x();
    if (blk_o.y() < ymin) ymin = blk_o.y();
    if (blk_o.z() < zmin) zmin = blk_o.z();

    //get block max point
    vgl_vector_3d<double>   blk_dim = (iter->second).sub_block_dim_;
    vgl_vector_3d<unsigned> blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double>   length(blk_dim.x()*blk_num.x(),
                                   blk_dim.y()*blk_num.y(),
                                   blk_dim.z()*blk_num.z());
    vgl_point_3d<double> blk_max = blk_o + length;
    if (blk_max.x() > xmax) xmax = blk_max.x();
    if (blk_max.y() > ymax) ymax = blk_max.y();
    if (blk_max.z() > zmax) zmax = blk_max.z();
  }

  //: Construct from ranges in \a x,y,z (take care with order of inputs).
  //  The \a x range is given by the 1st and 4th coordinates,
  //  the \a y range is given by the 2nd and 5th coordinates,
  //  the \a z range is given by the 3rd and 6th coordinates.
  return {xmin, ymin, zmin,
                            xmax, ymax, zmax};
}

vgl_box_3d<int> boxm2_scene::bounding_box_blk_ids() const
{
  vgl_box_3d<int> bbox;
  //iterate through each block
  std::map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter)
    bbox.add(vgl_point_3d<int> ( iter->first.i(),iter->first.j(), iter->first.k()) ) ;

  return bbox;
}

vgl_vector_3d<unsigned int>  boxm2_scene::scene_dimensions() const
{
  std::vector<boxm2_block_id> ids = this->get_block_ids();

  if (ids.empty())
    return {0,0,0};

  int max_i=ids[0].i(),max_j=ids[0].j(),max_k=ids[0].k();
  int min_i=ids[0].i(),min_j=ids[0].j(),min_k=ids[0].k();

  for (auto & id : ids) {
    if (id.i() > max_i)
      max_i=id.i();
    if (id.j() > max_j)
      max_j=id.j();
    if (id.k() > max_k)
      max_k=id.k();

    if (id.i() < min_i)
      min_i=id.i();
    if (id.j() < min_j)
      min_j=id.j();
    if (id.k() < min_k)
      min_k=id.k();
  }
  max_i++; max_j++; max_k++;

  return {static_cast<unsigned int>((max_i-min_i)),static_cast<unsigned int>((max_j - min_j)),static_cast<unsigned int>((max_k-min_k))};
}

//: gets the smallest block index
void boxm2_scene::min_block_index (vgl_point_3d<int> &idx,
                                   vgl_point_3d<double> &local_origin) const
{
  auto iter= blocks_.begin();

  boxm2_block_id id = iter->first;
  boxm2_block_metadata data = iter->second;

  int min_i=id.i(),min_j=id.j(),min_k=id.k();
  double min_x = data.local_origin_.x(), min_y= data.local_origin_.y(), min_z= data.local_origin_.z();

  for (; iter != blocks_.end(); ++iter) {
    id = iter->first;
    data = iter->second;

    if (id.i() < min_i) {
      min_i=id.i();
      min_x = data.local_origin_.x();
    }
    if (id.j() < min_j) {
      min_j=id.j();
      min_y = data.local_origin_.y();
    }
    if (id.k() < min_k) {
      min_k=id.k();
      min_z = data.local_origin_.z();
    }
  }

  idx.set(min_i,min_j,min_k);
  local_origin.set(min_x, min_y, min_z);
}
float boxm2_scene::finest_resolution()
{
    std::map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter= blocks_.begin();
    boxm2_block_metadata data = iter->second;

    float final_level = data.max_level_;
    float block_dim = data.sub_block_dim_.x() ;

    return block_dim / std::pow(2.0f,final_level-1) ;
}
//: gets the smallest block index
void boxm2_scene::max_block_index (vgl_point_3d<int> &idx,
                                   vgl_point_3d<double> &local_origin) const
{
  auto iter= blocks_.begin();

  boxm2_block_id id = iter->first;
  boxm2_block_metadata data = iter->second;

  int max_i=id.i(),max_j=id.j(),max_k=id.k();
  double max_x = data.local_origin_.x(), max_y= data.local_origin_.y(), max_z= data.local_origin_.z();

  for (; iter != blocks_.end(); ++iter) {
    id = iter->first;
    data = iter->second;

    if (id.i() > max_i) {
      max_i=id.i();
      max_x = data.local_origin_.x();
    }
    if (id.j() > max_j) {
      max_j=id.j();
      max_y = data.local_origin_.y();
    }
    if (id.k() > max_k) {
      max_k=id.k();
      max_z = data.local_origin_.z();
    }
  }

  idx.set(max_i,max_j,max_k);
  local_origin.set(max_x, max_y, max_z);
}

unsigned& boxm2_scene::get_count()
{
  static unsigned count;
  return count;
}

//: returns true if the scene has specified data type (simple linear search)
bool boxm2_scene::has_data_type(std::string const& data_type)
{
  for (const auto & appearance : appearances_)
    if ( appearance == data_type )
      return true;
  return false;
}

bool boxm2_scene::is_block_visible(boxm2_block_metadata & data, vpgl_camera<double> const& cam, unsigned int ni, unsigned int nj )
{
    vgl_box_3d<double> bbox = data.bbox();
    std::vector<vgl_point_3d<double> > vertices =  bbox.vertices() ;
    vgl_box_2d<double> projectionbox;
    for(auto & vertice : vertices)
    {
        double u,v;
        cam.project(vertice.x(),vertice.y(),vertice.z(),u,v);
        projectionbox.add(vgl_point_2d<double>(u,v) );
    }
    vgl_box_2d<double> imagebbox(0,ni,0,nj);
    vgl_box_2d<double> ibox = vgl_intersection<double>(imagebbox,projectionbox);
    return !(ibox.is_empty());
}
//---------------------------------------------------------------------
// NON CLASS FUNCTIONS
//---------------------------------------------------------------------
//------------XML WRITE------------------------------------------------
void x_write(std::ostream &os, boxm2_scene& scene, std::string const& name)
{
  //open root tag
  vsl_basic_xml_element scene_elm(name);
  scene_elm.x_write_open(os);

  //write lvcs information
  vpgl_lvcs lvcs = scene.lvcs();
  x_write(os, lvcs, LVCS_TAG);
  x_write(os, scene.local_origin(), LOCAL_ORIGIN_TAG);

  //write scene path for (needs to know where blocks are)
  std::string path = scene.data_path();
  vsl_basic_xml_element paths(SCENE_PATHS_TAG);
  paths.add_attribute("path", path + '/');
  paths.x_write(os);

  vsl_basic_xml_element ver(VERSION_TAG);
  ver.add_attribute("number", scene.version());
  ver.x_write(os);

  //write list of appearance models

  std::vector<std::string> apps = scene.appearances();
  for (const auto & app : apps)
  {
    vsl_basic_xml_element apms(APM_TAG);
    apms.add_attribute("apm", app);
    apms.x_write(os);
  }
  if (scene.num_illumination_bins() > 0) {
    vsl_basic_xml_element apms(APM_TAG);
    std::stringstream ss; ss << scene.num_illumination_bins();
    apms.add_attribute("num_illumination_bins", ss.str());
    apms.x_write(os);
  }

  //write block information for each block
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene.blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  for (iter = blocks.begin(); iter != blocks.end(); ++iter) {
    boxm2_block_id id = iter->first;
    boxm2_block_metadata data = iter->second;
    vsl_basic_xml_element block(BLOCK_TAG);

    //add block id attribute
    block.add_attribute("id_i", id.i());
    block.add_attribute("id_j", id.j());
    block.add_attribute("id_k", id.k());

    //block local origin
    block.add_attribute("origin_x", data.local_origin_.x());
    block.add_attribute("origin_y", data.local_origin_.y());
    block.add_attribute("origin_z", data.local_origin_.z());

    //sub block dimensions
    block.add_attribute("dim_x", data.sub_block_dim_.x());
    block.add_attribute("dim_y", data.sub_block_dim_.y());
    block.add_attribute("dim_z", data.sub_block_dim_.z());

    //sub block numbers
    block.add_attribute("num_x", (int) data.sub_block_num_.x());
    block.add_attribute("num_y", (int) data.sub_block_num_.y());
    block.add_attribute("num_z", (int) data.sub_block_num_.z());

    //block init level
    block.add_attribute("init_level", data.init_level_);

    //block max level
    block.add_attribute("max_level", data.max_level_);

    //block max_mb
    block.add_attribute("max_mb", data.max_mb_);

    //block prob init
    block.add_attribute("p_init", data.p_init_);

          //block prob init
    block.add_attribute("random", 0);

    //write tag to stream
    block.x_write(os);
  }

  //clse up tag
  scene_elm.x_write_close(os);
}

//------------IO Stream------------------------------------------------
std::ostream& operator <<(std::ostream &s, boxm2_scene& scene)
{
  s <<"--- BOXM2_SCENE -----------------------------\n"
    <<"xml_path:         "<<scene.xml_path()<<'\n'
    <<"data_path:        "<<scene.data_path()<<'\n'
    <<"world origin:     "<<scene.rpc_origin()<<'\n'
    <<"list of APMs:     "<<'\n';

  //list appearance models for this scene
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

  //list of block ids for this scene....
  vgl_vector_3d<unsigned> dims = scene.scene_dimensions();
  s << "block array dims(" << dims.x() << ' ' << dims.y() << ' ' << dims.z() << ")\n";
  std::map<boxm2_block_id, boxm2_block_metadata>& blk = scene.blocks();
  s << " blocks:==>\n";
  for (auto & bit : blk) {
    s << bit.second.id_ << ' ';
    vgl_point_3d<double> org = bit.second.local_origin_;
    s << ", org( " << org.x() << ' ' << org.y() << ' ' << org.z() << ") ";
    vgl_vector_3d<double> dim = bit.second.sub_block_dim_;
    s << ", dim( " << dim.x() << ' ' << dim.y() << ' ' << dim.z() << ") ";
    vgl_vector_3d<unsigned> num = bit.second.sub_block_num_;
    s << ", num( " << num.x() << ' ' << num.y() << ' ' << num.z() << ")\n";
  }
  s << "<=====:end blocks\n";
  return s;
}

//: Binary write boxm2_scene to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene const& /*bit_scene*/) {}
//: Binary write boxm2_scene pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene* const& /*ph*/) {}
//: Binary write boxm2_scene smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene_sptr&) {}
//: Binary write boxm2_scene smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene_sptr const&) {}

//: Binary load boxm scene from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene& /*bit_scene*/) {}
//: Binary load boxm scene pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene* /*ph*/) {}
//: Binary load boxm scene smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_sptr&) {}
//: Binary load boxm scene smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_sptr const&) {}

//: Binary write boxm2_scene_info_wrapper to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene_info_wrapper const&) {}
//: Binary write boxm2_scene_info_wrapper pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, const boxm2_scene_info_wrapper* &) {}
//: Binary write boxm2_scene_info_wrapper smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene_info_wrapper_sptr&) {}
//: Binary write boxm2_scene_info_wrapper smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene_info_wrapper_sptr const&) {}

//: Binary load boxm2_scene_info_wrapper from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_info_wrapper &) {}
//: Binary load boxm2_scene_info_wrapper pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_info_wrapper*) {}
//: Binary load boxm2_scene_info_wrapper smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_info_wrapper_sptr&) {}
//: Binary load boxm2_scene_info_wrapper smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_info_wrapper_sptr const&) {}
